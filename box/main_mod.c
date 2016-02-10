#include <avr/io.h>
#include <avr/delay.h>
#include "rf73_spi.h"

#include "ff.h"		/* Declarations of FatFs API */

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */



#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));



UINT8 rx_buf[MAX_PACKET_LEN];

#define COLORYELOWDDRPORT 	DDRA
#define COLORYELOWPORT 		PORTA
#define COLORYELOW 			0xF0

#define COLORGREENDDRPORT 	DDRC
#define COLORGREENPORT 		PORTC
#define COLORGREEN 			0xF0

#define COLORBLUEDDRPORT 	DDRC
#define COLORBLUEPORT 		PORTC
#define COLORBLUE			0x0F

#define ALLLEDON	0xFF
#define ALLLEDOFF	0x00

inline void shutDownLeds(){
	COLORYELOWPORT 		&= (~COLORYELOW);
	COLORGREENPORT 		&= (~COLORGREEN);
	COLORBLUEPORT 		&= (~COLORBLUE);
}

char fitPowerInByte(char power){
	return ( ( (power << 4) & 0xF0 ) | power );
}

#define POINTNUMBER 1
#define POINTPOWER 2

int main ()
{

	COLORYELOWDDRPORT 	|= COLORYELOW;
	COLORYELOWPORT 		|= COLORYELOW;
	COLORGREENDDRPORT 	|= COLORGREEN;
	COLORGREENPORT 		|= COLORGREEN;
	COLORBLUEDDRPORT 	|= COLORBLUE;
	COLORBLUEPORT 		|= COLORBLUE;

_delay_ms(1000);


	UINT bw;


if(	f_mount(&FatFs, "", 1) == FR_OK)	/* Give a work area to the default drive */
{
			PORTA = 0b10000000;
			_delay_ms(1000);
}

PORTC = 0b00000001;
_delay_ms(1000);

	if (f_open(&Fil, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {	/* Create a file */
//	if (f_open(&Fil, "1.txt", FA_READ) == FR_OK) {	/* Create a file */


PORTC = 0b00000011;
_delay_ms(1000);

		f_write(&Fil, "It works!\r\n", 11, &bw);	/* Write data to the file */

PORTC = 0b00000111;
_delay_ms(1000);

		f_close(&Fil);								/* Close the file */

PORTC = 0b00001111;
_delay_ms(1000);

		if (bw == 11) {		/* Lights green LED if data written well */
			PORTC = 0b01010101;	
		}
	}

	PORTC = 0b10000001;
_delay_ms(1000);

	for (;;) ;


	return 0;

}


