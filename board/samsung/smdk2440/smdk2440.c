/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002, 2010
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <video_fb.h>

DECLARE_GLOBAL_DATA_PTR;

#define FCLK_SPEED 1

#if FCLK_SPEED==0		/* Fout = 203MHz, Fin = 12MHz for Audio */
#define M_MDIV	0xC3
#define M_PDIV	0x4
#define M_SDIV	0x1
#elif FCLK_SPEED==1		/* Fout = 202.8MHz */
#define M_MDIV	0xA1
#define M_PDIV	0x3
#define M_SDIV	0x1
#endif

#define USB_CLOCK 1

#if USB_CLOCK==0
#define U_M_MDIV	0xA1
#define U_M_PDIV	0x3
#define U_M_SDIV	0x1
#elif USB_CLOCK==1
#define U_M_MDIV	0x48
#define U_M_PDIV	0x3
#define U_M_SDIV	0x2
#endif

static inline void pll_delay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_early_init_f(void)
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	//writel(0xFFFFFF, &clk_power->locktime);

	/* configure MPLL */
	//writel((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV,
	       //&clk_power->mpllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(4000);

	/* configure UPLL */
	writel((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV,
	       &clk_power->upllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(8000);

	/* set up the I/O ports */
	writel(0x007FFFFF, &gpio->gpacon);
	writel(0x00044555, &gpio->gpbcon);
	writel(0x000007FF, &gpio->gpbup);
	writel(0xAAAAAAAA, &gpio->gpccon);
	writel(0x0000FFFF, &gpio->gpcup);
	writel(0xAAAAAAAA, &gpio->gpdcon);
	writel(0x0000FFFF, &gpio->gpdup);
	writel(0xAAAAAAAA, &gpio->gpecon);
	writel(0x0000FFFF, &gpio->gpeup);
	writel(0x000055AA, &gpio->gpfcon);
	writel(0x000000FF, &gpio->gpfup);
	writel(0xFF95FFBA, &gpio->gpgcon);
	writel(0x0000FFFF, &gpio->gpgup);
	writel(0x002AFAAA, &gpio->gphcon);
	writel(0x000007FF, &gpio->gphup);

	return 0;
}

int board_init(void)
{
	/* arch number of SMDK2410-Board */
	gd->bd->bi_arch_number = MACH_TYPE_SMDK2410;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	 int rc = 0;	
#ifdef CONFIG_DRIVER_DM9000	
	 rc=dm9000_initialize(bis);	
#endif
	 return rc;

}
#endif

/*
 * Hardcoded flash setup:
 * Flash 0 is a non-CFI AMD AM29LV800BB flash.
 */
ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	info->portwidth = FLASH_CFI_16BIT;
	info->chipwidth = FLASH_CFI_BY16;
	info->interface = FLASH_CFI_X16;
	return 1;
}


#define MVAL		(0)
#define MVAL_USED 	(0)		//0=each frame   1=rate by MVAL
#define INVVDEN		(1)		//0=normal       1=inverted
#define BSWP		(0)		//Byte swap control
#define HWSWP		(1)		//Half word swap control

# if 1
//TFT 480x272
#define LCD_XSIZE_TFT_480x272 	(480)	
#define LCD_YSIZE_TFT_480x272 	(272)

//TFT480x272
#define HOZVAL_TFT_480x272	(LCD_XSIZE_TFT_480x272-1)
#define LINEVAL_TFT_480x272	(LCD_YSIZE_TFT_480x272-1)

//Timing parameter for NEC3.5"
#define VBPD_480x272		(40)		
#define VFPD_480x272		(50)
#define VSPW_480x272		(1)

#define HBPD_480x272		(50)
#define HFPD_480x272		(50)
#define HSPW_480x272_NEC	(1)  //Adjust the horizontal displacement of the screen :tekkamanninja@163.com
#define HSPW_480x272_TD		(1)  //64MB nand mini2440 is 36 ,128MB is 23
#define CLKVAL_TFT_480x272	(3) 	
//FCLK=101.25MHz,HCLK=50.625MHz,VCLK=6.33MHz
#else
//TFT 480x272
#define LCD_XSIZE_TFT_480x272 	(272)	
#define LCD_YSIZE_TFT_480x272 	(480)

//TFT480x272
#define HOZVAL_TFT_480x272	(288)
#define LINEVAL_TFT_480x272	(525)

//Timing parameter for NEC3.5"
#define VBPD_480x272		(40)		
#define VFPD_480x272		(5)
#define VSPW_480x272		(1)

#define HBPD_480x272		(8)
#define HFPD_480x272		(8)
#define HSPW_480x272_NEC	(1)  //Adjust the horizontal displacement of the screen :tekkamanninja@163.com
#define HSPW_480x272_TD		(1)  //64MB nand mini2440 is 36 ,128MB is 23
#define CLKVAL_TFT_480x272	(3) 	
//FCLK=101.25MHz,HCLK=50.625MHz,VCLK=6.33MHz

#endif

void board_video_init(GraphicDevice *pGD) 
{ 
	struct s3c24x0_lcd * const lcd	 = s3c24x0_get_base_lcd(); 
	struct s3c2440_nand * const nand = s3c2440_get_base_nand();
    /* FIXME: select LCM type by env variable */ 
	 
	/* Configuration for GTA01 LCM on QT2410 */ 
	lcd->lcdcon1 = 0x00000378; /* CLKVAL=4, BPPMODE=16bpp, TFT, ENVID=0 */ 
	lcd->lcdcon2 = (VBPD_480x272<<24)|(LINEVAL_TFT_480x272<<14)|(VFPD_480x272<<6)|(VSPW_480x272); 
	lcd->lcdcon3 = (HBPD_480x272<<19)|(HOZVAL_TFT_480x272<<8)|(HFPD_480x272); 

	if ( (nand->nfconf) & 0x08 )	{ 
		lcd->lcdcon4 = (MVAL<<8)|(HSPW_480x272_TD);
	}
	else{
	  	lcd->lcdcon4 = (MVAL<<8)|(HSPW_480x272_NEC);
	}
	
	lcd->lcdcon5 = 0x00000f09; 
	lcd->lpcsel  = 0x00000000; 
} 


