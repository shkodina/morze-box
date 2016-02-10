/*
  Copyright 2009  Mathieu SONET (contact [at] elasticsheep [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/* Adapted from AN10406 - Accessing SD/MMC card using SPI on LPC2000
   http://www.standardics.nxp.com/support/documents/microcontrollers/pdf/an10406.pdf
*/

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"



/*****************************************************************************
* MMC functions
******************************************************************************/

#define MMC_CMD_SIZE 6
uint8_t mmc_cmd[MMC_CMD_SIZE];

#define MMC_TIMEOUT  0xFF
#define MMC_BLOCK_SIZE 512

#define DDR_CS  DDRB
#define PORT_CS PORTB
#define DD_CS   DDB4

#define SET_CS()   PORT_CS |= _BV(DD_CS);
#define CLEAR_CS() PORT_CS &= ~_BV(DD_CS);

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

uint8_t mmc_response(uint8_t response)
{
  uint16_t count = 0xFFF;
  uint8_t result;

  while(count > 0)
  {
    result = spi_receive_byte();
    if (result == response)
      break;
    count--;
  }

  if (count == 0){
  	//PORTA = 0xf0;
    return 1; /* Failure, loop was exited due to timeout */
  }

  return 0; /* Normal, loop was exited before timeout */
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------


void mmc_init(void)
{
  uint8_t i;
  uint8_t byte;

  /* Init the CS output */
  DDR_CS |= _BV(DD_CS);

  SET_CS();
  {
    /* Init the card in SPI mode by sending 80 clks */
    for(i = 0; i < 10; i++)
      byte = spi_receive_byte();
  }
  CLEAR_CS();



  /* Send CMD0 GO_IDLE_STATE */
  mmc_cmd[0] = 0x40;
  mmc_cmd[1] = 0x00;
  mmc_cmd[2] = 0x00;
  mmc_cmd[3] = 0x00;
  mmc_cmd[4] = 0x00;
  mmc_cmd[5] = 0x95;
  spi_send(mmc_cmd, MMC_CMD_SIZE);

  if (mmc_response(0x01))
  {
//    PORTC |= 0x10;
    return;
  }

//PORTC |= 0x02;

  /* Send some dummy clocks after GO_IDLE_STATE */
  SET_CS();
  spi_receive_byte();
  CLEAR_CS();

//PORTC |= 0x04;

  /* Send CMD1 SEND_OP_COND until response is 0 */
  i = MMC_TIMEOUT;
  do
  {
    mmc_cmd[0] = 0x41;
    mmc_cmd[1] = 0x00;
    mmc_cmd[2] = 0x00;
    mmc_cmd[3] = 0x00;
    mmc_cmd[4] = 0x00;
    mmc_cmd[5] = 0xFF;
    spi_send(mmc_cmd, MMC_CMD_SIZE);
    i--;
  } while((mmc_response(0x00) != 0) && (i > 0));

  if (i == 0)
  {
 //   PORTC |= 0x20;
    return;
  }

//PORTC |= 0x08;

  /* Send some dummy clocks after GO_IDLE_STATE */
  SET_CS();
  spi_receive_byte();
  CLEAR_CS();

//  PORTC &= 0b11011111;

  /* Send CMD16 SET_BLOCKLEN to set the block length */
  mmc_cmd[0] = 0x50;
  mmc_cmd[1] = 0x00;   /* 4 bytes from here is the block length */
                       /* LSB is first */
                       /* 00 00 00 10 set to 16 bytes */
                       /* 00 00 02 00 set to 512 bytes */
  mmc_cmd[2] = 0x00;
  /* high block length bits - 512 bytes */
  mmc_cmd[3] = 0x02;
  /* low block length bits */
  mmc_cmd[4] = 0x00;
  mmc_cmd[5] = 0xFF; /* checksum is no longer required but we always send 0xFF */
  spi_send(mmc_cmd, MMC_CMD_SIZE);

  if ((mmc_response(0x00)) == 1)
  {
//    PORTC |= 0x40;
  }

  SET_CS();
  spi_receive_byte();

//  PORTA |= 0x20;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

void mmc_read_block(uint16_t block_number, uint8_t* block_address)
{
  uint16_t checksum;
  uint16_t varh, varl;

  varl = ((block_number & 0x003F) << 9);
  varh = ((block_number & 0xFFC0) >> 7);

 

  CLEAR_CS();
  {
    /* send MMC CMD17(READ_SINGLE_BLOCK) to read the data from MMC card */
    mmc_cmd[0] = 0x51;
    /* high block address bits, varh HIGH and LOW */
    mmc_cmd[1] = varh >> 0x08;
    mmc_cmd[2] = varh & 0xFF;
    /* low block address bits, varl HIGH and LOW */
    mmc_cmd[3] = varl >> 0x08;
    mmc_cmd[4] = varl & 0xFF;
    /* checksum is no longer required but we always send 0xFF */
    mmc_cmd[5] = 0xFF;
    spi_send(mmc_cmd, MMC_CMD_SIZE);

    /* if mmc_response returns 1 then we failed to get a 0x00 response */
    if ((mmc_response(0x00)) == 1)
    {
      return;
    }

    /* wait for data token */
    if ((mmc_response(0xFE)) == 1)
    {
     return;
    }

 

    /* Get the block of data based on the length */
    spi_receive(block_address, MMC_BLOCK_SIZE);

    /* CRC bytes that are not needed */
    checksum = spi_receive_byte();
    checksum = checksum << 0x08 | spi_receive_byte();
  }
  SET_CS();
  
  spi_receive_byte();
  
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

void print_block(uint8_t *block_address, uint16_t length)
{
  uint16_t i, j;
  uint8_t byte;
/*  
  for(i = 0; i < length / 16; i++)
  {
    printf("%04x: ", i * 16);
    
    for(j = 0; j < 16; j++)
    {
      printf("%02x", block_address[i * 16 + j]);
    }
    
    printf(" ");
    for(j = 0; j < 16; j++)
    {
      byte = block_address[i * 16 + j];
      
      if ((byte >= 32) && byte <= 126)
        printf("%c", byte);
      else
        printf(".");
    }

    printf("\n");

  }
*/

	for(i = 0; i < length ; i++){
		PORTC = block_address[i];
		_delay_ms(20);
	}
}

/*****************************************************************************
* Main program
******************************************************************************/
uint8_t mmc_block[MMC_BLOCK_SIZE];

int mains (void)
{

	DDRC = 0xff;
	DDRA = 0xf0;



  /* Init the SPI */

  spi_init();
  	PORTC |= 0x01;

  /* Init the MMC */

  mmc_init();
 
PORTA |= 0xF0;

  // Reset the MMC buffer 
  memset(mmc_block, 0xCA, MMC_BLOCK_SIZE);

_delay_ms(1000);
PORTC = 0;

  // Read the first block 
  for(int t=63; t<90; t++){
//  mmc_read_block(63, mmc_block);
  mmc_read_block(t, mmc_block);
  print_block(mmc_block, MMC_BLOCK_SIZE);
  }

PORTA &= 0x0f;
PORTA = 0b10010000;
  // Infinite loop 
  for(;;);

  return(0);
}

