#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"
#include "morze_structs.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

#define POINPAUSE			PointTime * 50
#define LINEPAUSE			POINPAUSE * 4
#define WAITPAUSE			POINPAUSE
#define SEPARATEPAUSE 		POINPAUSE * 3
#define UNKNOWNSIMBOLPAUSE 	POINPAUSE * 7 // pause for space or unknown simbol

#define MESSLEN 128
uint8_t message[MESSLEN];

#define MMC_CMD_SIZE 6
uint8_t mmc_cmd[MMC_CMD_SIZE];

#define MMC_TIMEOUT  0xFF
#define MMC_BLOCK_SIZE 512

#define DDR_CS  DDRB
#define PORT_CS PORTB
#define DD_CS   DDB4

#define SET_CS()   PORT_CS |= _BV(DD_CS);
#define CLEAR_CS() PORT_CS &= ~_BV(DD_CS);

#define STARTREADEDBLOCK 66

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//
//		USED FUNCTIONS
//
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

uint8_t mmc_response(uint8_t response);
void mmc_init(void);
void mmc_read_block(uint16_t block_number, uint8_t* block_address);

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

char init_abc(uint8_t *block_address, uint16_t length)
{
	PointTime = block_address[0] - 49; // atoi "char-49=digit"
	for (int i = 1; i < TOTALLEN; i++){
		abc[i].code = block_address[i];
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

char init_message(uint8_t *block_address, uint16_t length)
{
	memset(message,0x00,MESSLEN);
	for (int i = 0; i < length; i++){
					if (block_address[i] == 'e' && 
						block_address[i+1] == 'o' && 
						block_address[i+2] == 'f' &&
						block_address[i+3] == 'f')
					{
						message[0]=i;
						int d = 3;
						while(d--){
						PORTA = 0b00010000;
						_delay_ms(300);
						PORTA = 0b00100000;
						_delay_ms(300);
						PORTA = 0b01000000;
						_delay_ms(300);
						PORTA = 0b10000000;
						_delay_ms(300);
						}
						return 1;
					}
		//PORTC = block_address[i];
		//_delay_ms(450);
		message[i+1] = block_address[i];
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

char check_block(uint8_t *block_address, uint16_t length)
{
 	for(int i = 0; i < length - 3 ; i++){
		if(block_address[i]){
			
			// DATA Part started
			if (block_address[i] == 't' && 
				block_address[i+1] == 'e' && 
				block_address[i+2] == 'x' &&
				block_address[i+3] == 't')
			{
				int d = 3;
				while(d--){
					PORTA = 0b11110000;
					_delay_ms(300);
					PORTA = 0b10010000;
					_delay_ms(300);
					PORTA = 0b01100000;
					_delay_ms(300);
				}
				//_delay_ms(1000);
				
				init_abc(block_address + i + 4, length - i - 4); // munus bytes "text"

				for (int j=i+4; j< length - 3; j++)
				{
					if (block_address[j] == 'm' && 
						block_address[j+1] == 'e' && 
						block_address[j+2] == 's' &&
						block_address[j+3] == 's')
					{
						d = 3;
						while(d--){
						PORTA = 0b00110000;
						_delay_ms(300);
						PORTA = 0b11000000;
						_delay_ms(300);
						PORTA = 0b01100000;
						_delay_ms(300);
						}

						init_message(block_address + j + 4, length - j - 4);

						return 1;
					}

					//PORTC = block_address[j];
					//_delay_ms(450);	

						
				}
				

			}
			//_delay_ms(20);
		}
		INVBIT(PORTA, 6);
	}
	return 0;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void morze_char(MORZE_SYMBOL simbol)
{
	char pos = 8; 
	for (int i = 0; i < simbol.morsecodlen; i++){
		pos--;
		if (simbol.morsecod & (1 << pos)){ // long 	
			PORTC = 0xff;
			_delay_ms(LINEPAUSE);		
		}else{	// short
			PORTC = 0xff;
			_delay_ms(POINPAUSE);		
		} 
		PORTC = 0x00;
		_delay_ms(WAITPAUSE);		
	}
}

void morze_message(void)
{
	for (char j = 1; j <= message[0]; j++)
	{
		for (int i = 0; i < TOTALLEN; i++){
			if (abc[i].code == message[j])
				morze_char(abc[i]);
			if (i == TOTALLEN)
				_delay_ms(UNKNOWNSIMBOLPAUSE);
		}
		_delay_ms(SEPARATEPAUSE);		
	}
 INVBIT(PORTA, 7);
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//
//		MAIN PROGRAMM
//
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

uint8_t mmc_block[MMC_BLOCK_SIZE];

int main (void)
{

	DDRC = 0xff;
	DDRA = 0xf0;

  	// Init the SPI 
	spi_init();
  	
  	// Init the MMC 
  	mmc_init();
 

  	// Read the blocks 
  	int blockNumber = STARTREADEDBLOCK;

	while (blockNumber++ >= 0){

		// Reset the MMC buffer 
	  	memset(mmc_block, 0x11, MMC_BLOCK_SIZE);
		// Read block
		mmc_read_block(blockNumber, mmc_block);
		// Check block for my data
		if(check_block(mmc_block, MMC_BLOCK_SIZE))
			break;
	}

	PORTA = 0b10010000;
  	// Infinite loop 
  	for(;;)
  		morze_message();

  return(0);
}

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
    PORTC |= 0x10;
    return;
  }

  /* Send some dummy clocks after GO_IDLE_STATE */
  SET_CS();
  spi_receive_byte();
  CLEAR_CS();


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
    return;
  }


  /* Send some dummy clocks after GO_IDLE_STATE */
  SET_CS();
  spi_receive_byte();
  CLEAR_CS();


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
    ;
  }

  SET_CS();
  spi_receive_byte();


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


