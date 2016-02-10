#include "diskio.h"		/* Common include file for FatFs and disk I/O layer */
#include "spi.h"



/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/

#include <avr/io.h>			/* Include device specific declareation file here */
#include <util/delay.h>

#define DDR_CS  DDRB
#define PORT_CS PORTB
#define DD_CS   DDB4

#define SET_CS()   PORT_CS |= _BV(DD_CS);
#define CLEAR_CS() PORT_CS &= ~_BV(DD_CS);

//#define	CS_H()		SET_CS()	// Set MMC CS "high" 
//#define CS_L()		CLEAR_CS()	// Set MMC CS "low" 
//#define	CS_H()		CLEAR_CS()	// Set MMC CS "high" 
//#define CS_L()		SET_CS()	// Set MMC CS "low" 

/* MMC/SD command (SPI mode) */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define CMD13	(13)		/* SEND_STATUS */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */


static
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static
BYTE CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */





//----------------------------------------------
//--------------------------------------------------------------
//-----------------------------------------------------------------------------




/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv			/* Drive number (always 0) */
)
{
	if (drv) return STA_NOINIT;

	return 0;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{

for (int i = 0; i < count; i++){
	mmc_read_block(sector + i, buff+(i*512));
}


	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	spi_init();
	mmc_init();

	return 0;
}
