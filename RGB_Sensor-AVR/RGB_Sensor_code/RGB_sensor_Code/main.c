#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <i2c.h>
#include <LCD_I2C.h>

#define Red PORTB0
#define Green PORTB1
#define Blue PORTB2
uint8_t pin = 3;
uint16_t Red_val,Green_val,Blue_val;
uint16_t adc_result0;
void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0) | (1<<MUX0) | (1<<MUX1);
	
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);
}
uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with ’7? will always keep the value
	// of ‘ch’ between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single convertion
	// write ’1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}

int main(void)
{ i2c_init();
	i2c_start();
	i2c_write(0x70);
	lcd_init();
	
	/* Replace with your application code */
	DDRB = 0b11111111;
	// 	lcd_cmd(0x80);
	// 	lcd_msg("Pasindu");
	// 	_delay_ms(1000);
	// 	lcd_cmd(0x01);
	//
	// 	lcd_cmd(0x80);
	// 	lcd_msg("Manodara");
	// 	_delay_ms(1000);
	// 	lcd_cmd(0x01);
	adc_init();
	
	
	while (1)
	
	{
		adc_result0 = adc_read(3);
		PORTB ^= (1<<Red); _delay_ms(500); Red_val = adc_read(pin); PORTB ^= (1<<Red);
		
		
		PORTB ^= (1<<Green); _delay_ms(500); Green_val = adc_read(pin);PORTB ^= (1<<Green);
		PORTB ^= (1<<Blue); _delay_ms(500); Blue_val = adc_read(pin);PORTB ^= (1<<Blue);
		char str_red [sizeof(Red_val)*8+1];
		char str_green [sizeof(Green_val)*8+1];
		char str_Blue [sizeof(Blue_val)*8+1];
		lcd_cmd(0x80);
		utoa(Red_val,str_red,10);
		lcd_msg("R:");lcd_cmd(0x83);lcd_msg(str_red);
		utoa(Green_val,str_green,10);
		lcd_cmd(0x88);lcd_msg("G:");lcd_cmd(0x8B);lcd_msg(str_green);
		utoa(Blue_val,str_Blue,10);
		lcd_cmd(0xC6);lcd_msg("B:");lcd_cmd(0xCA);lcd_msg(str_Blue);
	}
}