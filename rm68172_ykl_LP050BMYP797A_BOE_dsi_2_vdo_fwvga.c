/*
 * Copyright (C) 2019, <raymond miracle>
 * 
 */

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"
#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
        #include <mt-plat/mt_gpio.h>
#endif

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define REGFLAG_DELAY (0xFD)
#define REGFLAG_END_OF_TABLE (0xFE)

/* Local Variables */
#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))
#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

/* Local Functions */
#define dsi_set_cmdq_V3(para_tbl,size,force_update)         lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define read_reg_v2(cmd, buffer, buffer_size)	            lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define write_regs(addr, pdata, byte_nums)	                lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)   lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define wrtie_cmd(cmd)	lcm_util.dsi_write_cmd(cmd)

/* LCM Driver Implementations */

static LCM_UTIL_FUNCS lcm_util = { 0 };

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] =
{
  {0xf0, 5, {0x52,0x08,0x02}},
  {0xf6, 2, {0x60,0x40}},
  {0xfe, 4, {0x01,0x80,0x09,0x09}},
  {0xeb, 4, {0x01,0x2e,0x03,0xe8}},
  {0xf0, 5, {0x55,0xaa,0x52,0x08,0x01}},
  {0xb0, 1, {0x0d}},
  {0xb1, 1, {0x0d}},
  {0xb2, 1, {0x00}},
  {0xb5, 1, {0x08}},
  {0xb6, 1, {0x54}},
  {0xb7, 1, {0x44}},
  {0xb8, 1, {0x24}},
  {0xb9, 1, {0x34}},
  {0xba, 1, {0x24}},
  {0xbc, 3, {0x00,0x60,0x00}},
  {0xbd, 3, {0x00,0x60,0x00}},
  {0xbe, 2, {0x00,0x65}},
  {0xd1, 52, {0x00,0x00,0x00,0x05,0x00,0x18,0x00,0x37,0x00,0x54,0x00,0x8f,0x00,0xc7,0x01,0x0f,0x01,0x43,0x01,0x8b,0x01,0xba,0x01,0xfd,0x02,0x30,0x02,0x31,0x02,0x5e,0x02,0x89,0x02,0xa1,0x02,0xbe,0x02,0xcf,0x02,0xe5,0x02,0xf5,0x03,0x0b,0x03,0x1d,0x03,0x3d,0x03,0x59,0x03,0xff}},
  {0xd2, 52, {0x00,0x00,0x00,0x05,0x00,0x18,0x00,0x37,0x00,0x54,0x00,0x8f,0x00,0xc7,0x01,0x0f,0x01,0x43,0x01,0x8b,0x01,0xba,0x01,0xfd,0x02,0x30,0x02,0x31,0x02,0x5e,0x02,0x89,0x02,0xa1,0x02,0xbe,0x02,0xcf,0x02,0xe5,0x02,0xf5,0x03,0x0b,0x03,0x1d,0x03,0x3d,0x03,0x59,0x03,0xff}},
  {0xd3, 52, {0x00,0x00,0x00,0x05,0x00,0x18,0x00,0x37,0x00,0x54,0x00,0x8f,0x00,0xc7,0x01,0x0f,0x01,0x43,0x01,0x8b,0x01,0xba,0x01,0xfd,0x02,0x30,0x02,0x31,0x02,0x5e,0x02,0x89,0x02,0xa1,0x02,0xbe,0x02,0xcf,0x02,0xe5,0x02,0xf5,0x03,0x0b,0x03,0x1d,0x03,0x3d,0x03,0x59,0x03,0xff}},
  {0xd4, 52, {0x00,0x00,0x00,0x05,0x00,0x18,0x00,0x37,0x00,0x54,0x00,0x8f,0x00,0xc7,0x01,0x0f,0x01,0x43,0x01,0x8b,0x01,0xba,0x01,0xfd,0x02,0x30,0x02,0x31,0x02,0x5e,0x02,0x89,0x02,0xa1,0x02,0xbe,0x02,0xcf,0x02,0xe5,0x02,0xf5,0x03,0x0b,0x03,0x1d,0x03,0x3d,0x03,0x59,0x03,0xff}},
  {0xd5, 52, {0x00,0x00,0x00,0x05,0x00,0x18,0x00,0x37,0x00,0x54,0x00,0x8f,0x00,0xc7,0x01,0x0f,0x01,0x43,0x01,0x8b,0x01,0xba,0x01,0xfd,0x02,0x30,0x02,0x31,0x02,0x5e,0x02,0x89,0x02,0xa1,0x02,0xbe,0x02,0xcf,0x02,0xe5,0x02,0xf5,0x03,0x0b,0x03,0x1d,0x03,0x3d,0x03,0x59,0x03,0xff}},
  {0xd6, 52, {0x00,0x00,0x00,0x05,0x00,0x18,0x00,0x37,0x00,0x54,0x00,0x8f,0x00,0xc7,0x01,0x0f,0x01,0x43,0x01,0x8b,0x01,0xba,0x01,0xfd,0x02,0x30,0x02,0x31,0x02,0x5e,0x02,0x89,0x02,0xa1,0x02,0xbe,0x02,0xcf,0x02,0xe5,0x02,0xf5,0x03,0x0b,0x03,0x1d,0x03,0x3d,0x03,0x59,0x03,0xff}},
  {0xf0, 5, {0x55,0xaa,0x52,0x08,0x03}},
  {0xb0, 7, {0x03,0x15,0xfa,0x00,0x20,0x00,0x00}},
  {0xb2, 9, {0xfb,0xfc,0xfd,0xfe,0xf0,0x20,0x00,0x83,0x04}},
  {0xb3, 6, {0x5b,0x00,0xfb,0x21,0x23,0x00}},
  {0xb4, 11, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
  {0xb5, 11, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44}},
  {0xb6, 7, {0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
  {0xb7, 8, {0x00,0x00,0x20,0x20,0x20,0x20,0x00,0x00}},
  {0xb8, 3, {0x00,0x00,0x00}},
  {0xb9, 1, {0x82}},
  {0xba, 16, {0x45,0x8a,0x04,0x5f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf5,0x41,0xb9,0x54}},
  {0xbb, 16, {0x54,0x9b,0x15,0x4f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf4,0x50,0xa8,0x45}},
  {0xbc, 4, {0xc7,0xff,0xff,0xe3}},
  {0xbd, 4, {0xc7,0xff,0xff,0xe3}},
  {0xf0, 5, {0x55,0xaa,0x52,0x08,0x00}},
  {0xb0, 2, {0x00,0x10}},
  {0xb1, 2, {0x78,0x00}},
  {0xb5, 1, {0x6b}},
  {0xba, 1, {0x01}},
  {0xbc, 1, {0x02}},
  {0x35, 1, {0x00}},
  {0x11, 0, {0x00,0x00}},
  {REGFLAG_DELAY,200,{}},
  {0x29, 0, {0x00,0x00}},
  {REGFLAG_DELAY,100,{}},
  {REGFLAG_END_OF_TABLE, 0, {}}		
};	

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
int i;
    for(i = 0; i < count; i++) {

        switch (table[i].cmd) {
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
            case REGFLAG_END_OF_TABLE :
                break;
            default:
                dsi_set_cmdq_V2(table[i].cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
}

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
  memcpy(&lcm_util, util, XXX);
}

static void lcm_get_params(LCM_PARAMS *params) {

  memset(params, 0, sizeof(LCM_PARAMS));
  
  params->dpi.dsc_params.nfl_bpg_offset = 256;
  params->dpi.dsc_params.final_offset = 14;
  params->dpi.dsc_params.flatness_minqp = 18;
  params->dpi.io_driving_current = 48;
  params->dpi.msb_io_driving_current = 46;
  params->dpi.lsb_io_driving_current = 44;
  params->dsi.horizontal_bllp = 185;
  params->type = 2;
  params->dpi.dsc_params.rct_on = 2;
  params->dpi.dsc_params.scale_value = 2;
  params->dpi.dsc_params.increment_interval = 2;
  params->dpi.dsc_params.line_bpg_offset = 2;
  params->dpi.dsc_params.initial_offset = 2;
  params->width = 480;
  params->dsi.mode = 480;
  params->height = 854;
  params->dpi.dsc_params.rc_mode1_size = 854;
  params->dbi.te_mode = 0;
  params->dbi.te_edge_polarity = 0;
  params->dpi.dsc_params.bp_enable = 0;
  params->dpi.dsc_params.dec_delay = 0;
  params->dpi.dsc_params.xmit_delay = 0;
  params->dpi.dsc_params.slice_mode = 1;
  params->dsi.horizontal_sync_active_byte = 1;
  
  }

static void lcm_init(void)
{

    SET_RESET_PIN(100);
    MDELAY(1);
    SET_RESET_PIN(0);
    MDELAY(50);
    SET_RESET_PIN(1);
    MDELAY(120);

 push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
 push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static unsigned int lcm_compare_id(void)
{
  return 1;
}

/* Get LCM Driver Hooks */
LCM_DRIVER rm68172_ykl_LP050BMYP797A_BOE_dsi_2_vdo_fwvga_lcm_drv =
{
    .name           = LCM_rm68172_ykl_LP050BMYP797A_BOE_dsi_2_vdo_fwvga,
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_init,
    .compare_id     = lcm_compare_id,
};
