/*
 * test1.c
 *
 * Created: 2021-05-18 01:01:59 AM
 * Author : PasinduManodara
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <i2c.h>
#include <LCD_I2C.h>

#define KEY_PRT 	PORTD
#define KEY_DDR		DDRD
#define KEY_PIN		PIND

unsigned char keypad[4][4] = {	{'7','8','9','/'},
{'4','5','6','*'},
{'1','2','3','-'},
{' ','0','=','+'}};
char keyfind();
unsigned char colloc, rowloc;
 char keyfind()
 {
	 while(1)
	 {
		 KEY_DDR = 0xF0;           /* set port direction as input-output */
		 KEY_PRT = 0xFF;

		 do
		 {
			 KEY_PRT &= 0x0F;      /* mask PORT for column read only */
			 asm("NOP");
			 colloc = (KEY_PIN & 0x0F); /* read status of column */
		 }while(colloc != 0x0F);
		 
		 do
		 {
			 do
			 {
				 _delay_ms(20);             /* 20ms key debounce time */
				 colloc = (KEY_PIN & 0x0F); /* read status of column */
				 }while(colloc == 0x0F);        /* check for any key press */
				 
				 _delay_ms (40);	            /* 20 ms key debounce time */
				 colloc = (KEY_PIN & 0x0F);
			 }while(colloc == 0x0F);

			 /* now check for rows */
			 KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
			 asm("NOP");
			 colloc = (KEY_PIN & 0x0F);
			 if(colloc != 0x0F)
			 {
				 rowloc = 0;
				 break;
			 }

			 KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
			 asm("NOP");
			 colloc = (KEY_PIN & 0x0F);
			 if(colloc != 0x0F)
			 {
				 rowloc = 1;
				 break;
			 }
			 
			 KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
			 asm("NOP");
			 colloc = (KEY_PIN & 0x0F);
			 if(colloc != 0x0F)
			 {
				 rowloc = 2;
				 break;
			 }

			 KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
			 asm("NOP");
			 colloc = (KEY_PIN & 0x0F);
			 if(colloc != 0x0F)
			 {
				 rowloc = 3;
				 break;
			 }
		 }

		 if(colloc == 0x0E){
		 return(keypad[rowloc][0]);}
		 else if(colloc == 0x0D){
		 return(keypad[rowloc][1]);}
		 else if(colloc == 0x0B){
		 return(keypad[rowloc][2]);}
		 else{
		 return(keypad[rowloc][3]);}
	 }


int main(void)
{i2c_init();
	i2c_start();
	i2c_write(0x70);
	lcd_init();
	
    /* Replace with your application code */
    while (1) 
    {
lcd_cmd(0x80);
		lcd_msg((char*) keyfind());
		_delay_ms(2000);
		
    }
}

