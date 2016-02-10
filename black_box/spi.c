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

#include <stdio.h>
#include <avr/io.h>

#define DDR_SPI DDRB
#define DD_MOSI DDB5
#define DD_MISO DDB6
#define DD_SCK  DDB7

void spi_init(void)
{
  /* Set MOSI and SCK output */
  DDR_SPI |= _BV(DD_MOSI) | _BV(DD_SCK);
  DDR_SPI &= ~_BV(DD_MISO);
  
  /* Enable SPI, Master, set clock rate fck/128 */
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0) | _BV(SPR1);
  SPSR = _BV(SPI2X);




PORTB=0x00;
DDRB=0xB0;
SPCR=0x50;
SPSR=0x01;

}

void spi_send_byte(uint8_t byte)
{
  /* Start transmission */
  SPDR = byte;
  
  /* Wait for transmission complete */
  while (!(SPSR & _BV(SPIF)));
}

uint8_t spi_receive_byte(void)
{
  uint8_t byte = 0;
  
  /* Write dummy byte to generate clock */
  SPDR = 0xFF;
  
  /* Wait for transmission complete */
  while (!(SPSR & _BV(SPIF)));
  
  /* Read the received byte */
  byte = SPDR;
  
  return byte;
}

uint8_t spi_send_receive_byte(uint8_t byte)
{
  /* Start transmission */
  SPDR = byte;
  
  /* Wait for transmission complete */
  while (!(SPSR & _BV(SPIF)));
  
  /* Read the received byte */
  byte = SPDR;
  
  return byte;
}

void spi_send(uint8_t* data, uint16_t length)
{
  for(uint16_t i = 0; i < length; i++)
    spi_send_byte(data[i]);
}

void spi_receive(uint8_t* data, uint16_t length)
{
  for(uint16_t i = 0; i < length; i++)
  {
    data[i] = spi_receive_byte();
  }
}
