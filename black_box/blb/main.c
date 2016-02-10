/*----------------------------------------------------------------------*/
/* Foolproof FatFs sample project for AVR              (C)ChaN, 2013    */
/*----------------------------------------------------------------------*/

#include <avr/io.h>	/* Device specific declarations */
#include "ff.h"		/* Declarations of FatFs API */

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */


int main (void)
{
	UINT bw;


	DDRC = 0xff;
	DDRA = 0xf0;



  /* Init the SPI */

//  spi_init();
  	PORTC |= 0x01;

  /* Init the MMC */

 // mmc_init();
 
PORTA |= 0xF0;

#define MMC_BLOCK_SIZE 512
char mmc_block[MMC_BLOCK_SIZE];



	f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */

PORTC |= 0x22;

	if (f_open(&Fil, "txt.txt", FA_READ | FA_OPEN_EXISTING) == FR_OK) {	/* Open a file */

PORTC |= 0x04;

		f_read(&Fil, mmc_block, MMC_BLOCK_SIZE, &bw);	/* read data */

PORTC |= 0x08;

		f_close(&Fil);								/* Close the file */

PORTC |= 0x10;

		if (bw == 11) {		/* Lights green LED if data written well */
			PORTA = 0xf0;	/* Set PB4 high */
		}
	}

PORTA = 0b10010000;

	for (;;) ;
}



/*---------------------------------------------------------*/
/* User Provided RTC Function called by FatFs module       */

DWORD get_fattime (void)
{
	/* Returns current time packed into a DWORD variable */
	return	  ((DWORD)(2013 - 1980) << 25)	/* Year 2013 */
			| ((DWORD)7 << 21)				/* Month 7 */
			| ((DWORD)28 << 16)				/* Mday 28 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				/* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

