#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <i2c.h>
#include <LCD_I2C.h>
#include <stdbool.h>
#include <stdio.h>
char* Red_b, Blue_b, Green_b, Red_w, Blue_w, Green_w; //for calibration values
//defining pins of LED bulbs
#define Red PORTC0
#define Green PORTC1
#define Blue PORTC2
// defining keypad
#define KEY_PRT   PORTB
#define KEY_DDR   DDRB
#define KEY_PIN   PINB

unsigned char keypad[4][4] = {
	{' ', '0', '=', '+'},
	{'1', '2', '3', '-'},
	{'4', '5', '6', '*'},
	{'7', '8', '9', '/'},
};

unsigned char colloc, rowloc;
char keyfind(){

	while (1)
	{
		KEY_DDR = 0xF0;           /* set port direction as input-output */
		KEY_PRT = 0xFF;

		do
		{
			KEY_PRT &= 0x0F;      /* mask PORT for column read only */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F); /* read status of column */
		} while (colloc != 0x0F);

		do
		{
			do
			{
				_delay_ms(20);             /* 20ms key debounce time */
				colloc = (KEY_PIN & 0x0F); /* read status of column */
				} while (colloc == 0x0F);       /* check for any key press */

				_delay_ms (40);             /* 20 ms key debounce time */
				colloc = (KEY_PIN & 0x0F);
			} while (colloc == 0x0F);

			/* now check for rows */
			KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if (colloc != 0x0F)
			{
				rowloc = 0;
				break;
			}

			KEY_PRT = 0xDF;   /* check for pressed key in 2nd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if (colloc != 0x0F)
			{
				rowloc = 1;
				break;
			}

			KEY_PRT = 0xBF;   /* check for pressed key in 3rd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if (colloc != 0x0F)
			{
				rowloc = 2;
				break;
			}

			KEY_PRT = 0x7F;   /* check for pressed key in 4th row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if (colloc != 0x0F)
			{
				rowloc = 3;
				break;
			}
		}

		if (colloc == 0x0E) {
			return (keypad[rowloc][0]);
		}
		else if (colloc == 0x0D) {
			return (keypad[rowloc][1]);
		}
		else if (colloc == 0x0B) {
			return (keypad[rowloc][2]);
		}
		else {
			return (keypad[rowloc][3]);
		}
	}

	uint8_t pin = 3;
	uint16_t Red_val, Green_val, Blue_val;
	
	void adc_init()
	{

		ADMUX = (1 << REFS0) | (1 << MUX0) | (1 << MUX1);

		// ADC Enable and prescaler of 128
		// 16000000/128 = 125000
		ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
	}
	uint16_t adc_read(uint8_t ch)
	{
		// select the corresponding channel 0~7
		// ANDing with ’7? will always keep the value
		// of ‘ch’ between 0 and 7
		ch &= 0b00000111;  // AND operation with 7
		ADMUX = (ADMUX & 0xF8) | ch; // clears the bottom 3 bits before ORing

		// start single convertion
		// write ’1? to ADSC
		ADCSRA |= (1 << ADSC);

		// wait for conversion to complete
		// ADSC becomes ’0? again
		// till then, run loop continuously
		while (ADCSRA & (1 << ADSC));

		return (ADC);
	}
	
	//function for get 3 digit from keypad
	char* get_3_digit() {
		int count = 0;
		char* val;

		while (count < 3) {
			char digit = keyfind();
			if (digit) {

				lcd_cmd(0xC9 + count);
				//lcd_msg("hey");
				char* str_digit[2];
				str_digit[0] = digit;
				str_digit[1] = '\0';
				lcd_msg(str_digit);
				val += digit;
				count += 1;
			}
		}
		return val;
	}
	
	//function for get 3 digits from keypad in the form of integers
	int get_3_int_digits() {
		int count = 0;
		int val=0;

		while (count < 3) {
			char digit = keyfind();
			int num=digit-'0';
			lcd_cmd(0xC9 + count);
			char* str_digit[1];
			str_digit[0] = digit;
			lcd_msg(str_digit);
			if(count == 2){
				val += num;
			}
			else{
				val = (val + num) * 10;
			}
			count += 1;
			
		}
		return val;
	}
	// function for display details on lcd display
	char* displayWrite(char* clr_mode, char* clr ) {
		lcd_init();
		lcd_cmd(0xC0);
		lcd_msg(clr_mode);
		lcd_cmd(0xC7);
		lcd_msg(clr);
		lcd_cmd(0x80);
		lcd_msg("- clear");
		lcd_cmd(0x89);
		lcd_msg("Enter +");
		char* val = get_3_digit();

		if (val) {

			_Bool flag1 = true;
			while (flag1) {

				char stat = keyfind();
				if (stat == '+') {
					return  val;
				}
				else if (stat == '-') {
					displayWrite(clr_mode, clr );
				}
			}
		}
	}
	void RGB_off(){
		DDRC &= ~(1<<PORTC0 | 1<<PORTC1 | 1<<PORTC2);
	}
	
	void pwm(int pin,int num){
		TCCR0B |= (1<<CS00)|(1<<CS01);//prescalar /64
		TCCR2B |= (1<<CS20)|(1<<CS21);//prescalar /64
		switch(pin){
			case 0:
			TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0A1);//fast pwm, non inverted
			DDRD |= (1<<PORTD6); //set the direction
			OCR0A=num;
			break;
			case 1:
			TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0B1);//fast pwm, non inverted
			DDRD |= (1<<PORTD5);
			OCR0B=num;
			break;
			case 2:
			TCCR2A |= (1<<WGM21)|(1<<WGM20)|(1<<COM2B1);//fast pwm, non inverted
			DDRD |= (1<<PORTD3);
			OCR2B=num;
			break;
		}
	}



	int main(void){
		i2c_init();
		i2c_start();
		i2c_write(0x70);
		lcd_init();
		lcd_cmd(0x80); lcd_msg("mode 1");
		lcd_cmd(0x88); lcd_msg("mode 2");
		lcd_cmd(0xc5); lcd_msg("mode 3");

		DDRC = 0b00000111;

		adc_init();


		while (1)

		{
			//mode selection
			char mode = keyfind();
			if (mode == '1') { //mode 1 - calibration mode
				RGB_off();
				Red_b = displayWrite("Min", "R-");
				if (Red_b) {
					Green_b = displayWrite("Min", "G-");
					if (Green_b) {
						Blue_b = displayWrite("Min", "B-");
						if (Blue_b) {
							Red_w = displayWrite("Max", "R-");
							if (Red_w) {
								Green_w = displayWrite("Max", "G-");
								if (Green_w) {
									Blue_w = displayWrite("Max", "B-");
									if (Blue_w) {
										_Bool flag = false;
										_Bool calibrated = true;
										lcd_init();
										lcd_cmd(0x82);
										lcd_msg("Calibration");
										lcd_cmd(0xC6);
										lcd_msg("Done!");
										_delay_ms(1000);
										main();
									}
								}
							}
						}

					}
				}
			}
			if (mode == '2') { //mode 2 - sensoring mode
				lcd_init();
				while (1) {
					
					PORTC ^= (1 << Red); _delay_ms(500); Red_val = adc_read(pin); PORTC ^= (1 << Red); //light up red bulb
					PORTC ^= (1 << Green); _delay_ms(500); Green_val = adc_read(pin); PORTC ^= (1 << Green); //light up green bulb
					PORTC ^= (1 << Blue); _delay_ms(500); Blue_val = adc_read(pin); PORTC ^= (1 << Blue);   //light up blue bulb
					// convert uint16 to string
					char str_red [sizeof(Red_val) * 8 + 1];
					char str_green [sizeof(Green_val) * 8 + 1];
					char str_Blue [sizeof(Blue_val) * 8 + 1];

					lcd_cmd(0x80);
					utoa(Red_val, str_red, 10);
					lcd_msg("R:"); lcd_cmd(0x83); lcd_msg(str_red);
					utoa(Green_val, str_green, 10);
					lcd_cmd(0x88); lcd_msg("G:"); lcd_cmd(0x8B); lcd_msg(str_green);
					utoa(Blue_val, str_Blue, 10);
					lcd_cmd(0xC6); lcd_msg("B:"); lcd_cmd(0xCA); lcd_msg(str_Blue);
				}
			}
			if (mode == '3') { //mode 3 - light up RGB led for given R,G,B values
				lcd_init();
				lcd_cmd(0x80);
				lcd_msg("mode 3 selected");
				_delay_ms(200);
				lcd_init();
				lcd_cmd(0x80);
				lcd_msg("Enter RED value");
				int Red_Val= get_3_int_digits();
				_delay_ms(100);
				lcd_init();
				lcd_cmd(0x80);
				lcd_msg("Enter GREEN");
				lcd_cmd(0xC0);
				lcd_msg("value");
				int Green_Val=get_3_int_digits();
				_delay_ms(100);
				lcd_init();
				lcd_cmd(0x80);
				lcd_msg("Enter BLUE value");
				int Blue_Val= get_3_int_digits();
				pwm(0,Red_Val);
				pwm(1,Green_Val);
				pwm(2,Blue_Val);
				_delay_ms(1000);
				pwm(0,0);
				pwm(1,0);
				pwm(2,0);
				main();
			}



		}
	}