/*
 * PWM with DEMUX.c
 *
 * Created: 2021-08-08 09:01:14 AM
 * Author : PasinduManodara
 */ 

#define F_CPU 16000000
#define RAND_MAX 255
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>


void setPin(char port,int pin,int state){  //port=A,B,C,D. pin 0-7. state 0 or 1.
	switch(port){
		case 'B':
		if(state==1)
		DDRB |= (1<<pin);
		else
		DDRB &=	~(1<<pin);
		break;
		case 'C':
		if(state==1)
		DDRC |= (1<<pin);
		else
		DDRC &=	~(1<<pin);
		break;
		case 'D':
		if(state==1)
		DDRD |= (1<<pin);
		else
		DDRD &=	~(1<<pin);
		break;
		default:
		DDRB=0xFF;
		PORTB=0xFF;
	}
}

void pwm(int pin,int num){
	TCCR0B |= (1<<CS00)|(1<<CS01);//prescalar /64
	TCCR2B |= (1<<CS20)|(1<<CS21);//prescalar /64
	
	//PORTD = (1<<PIND0 | 1<<PIND1);
	switch(pin){
		case 0:
		TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0A1);//fast pwm, non inverted
		setPin('D',6,1);
		OCR0A=num;
		break;
		case 1:
		TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0B1);//fast pwm, non inverted
		setPin('D',5,1);
		OCR0B=num;
		break;
		case 2:
		TCCR2A |= (1<<WGM21)|(1<<WGM20)|(1<<COM2A1);//fast pwm, non inverted
		setPin('B',3,1);
		OCR2A=num;
		break;
		case 3:
		TCCR2A |= (1<<WGM21)|(1<<WGM20)|(1<<COM2B1);//fast pwm, non inverted
		setPin('D',3,1);
		OCR2B=num;
		break;
	}
}
int main(void)
{
	while(1)
	{DDRD = (1<<PIND0 | 1<<PIND1);
		int r=255;
		int g=0;
		int b=0;
		
		PORTD = (0<<PORTD0)| (0<<PORTD1);
		pwm(0,255-r);
		_delay_ms(2);
		
		PORTD = (1<<PORTD0)| (0<<PORTD1);
		pwm(0,255-g);
		_delay_ms(2);
		
		PORTD = (0<<PORTD0)| (1<<PORTD1);
		pwm(0,255-b);
		_delay_ms(2);
		
		
	}
	return 0;
}

