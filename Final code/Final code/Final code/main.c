#define F_CPU 16000000UL // set the clock frequency

// Library Inputs
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <i2c.h>
#include <LCD_I2C.h>
#include <stdbool.h>
#include <stdio.h>
//Variable defined
char* Red_b, Green_b, Blue_b, Red_r, Green_r, Blue_r, Red_g, Green_g, Blue_g, Red_blue, Green_blue, Blue_blue, Green_w, Red_w, Blue_w; //for calibration values
//defining pins of LED bulbs
#define Red PORTC0
#define Green PORTC1
#define Blue PORTC2
// defining keypad
#define KEY_PRT   PORTD
#define KEY_DDR   DDRD
#define KEY_PIN   PIND
_Bool calibrated = false;
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
		KEY_DDR = 0xF0;           /* set the directions of the port */
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
			{   //codes for check whether which key is pressed
				_delay_ms(20);
				colloc = (KEY_PIN & 0x0F);
				} while (colloc == 0x0F);       /* check for any key press */

				_delay_ms (40);
				colloc = (KEY_PIN & 0x0F);
			} while (colloc == 0x0F);

			/* check the which row's key is pressed */
			KEY_PRT = 0xEF;            /* check 1st row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if (colloc != 0x0F)
			{
				rowloc = 0;
				break;
			}

			KEY_PRT = 0xDF;   /* check 2nd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if (colloc != 0x0F)
			{
				rowloc = 1;
				break;
			}

			KEY_PRT = 0xBF;   /* check 3rd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if (colloc != 0x0F)
			{
				rowloc = 2;
				break;
			}

			KEY_PRT = 0x7F;   /* check 4th row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if (colloc != 0x0F)
			{
				rowloc = 3;
				break;
			}
		}
		// check the column and return the value
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

		ADMUX = (1 << REFS0) | (1 << MUX0) | (1 << MUX1);// Enable the analog read

		//set the prescaler
		ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
	}
	uint16_t adc_read(uint8_t ch)
	{
		// activate the analog reading process
		ch &= 0b00000111;
		ADMUX = (ADMUX & 0xF8) | ch;

		
		ADCSRA |= (1 << ADSC);

		
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
				
				char* str_digit[2]; // convert "digit" to string to display
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
	// off the RGB light
	void RGB_off(){
		DDRC &= ~(1<<PORTC0 | 1<<PORTC1 | 1<<PORTC2);
	}
	// function to activate relavant pwm pin
	void pwm(int pin,int num){
		TCCR1B |= (1<<CS10)|(1<<CS11);//set prescalar /64
		TCCR2B |= (1<<CS20)|(1<<CS21);//set prescalar /64
		switch(pin){
			case 0:
			TCCR1A |= (1<<WGM12)|(1<<WGM10)|(1<<COM1A1);//to get fast pwm and non inverted mode
			DDRB |= (1<<PORTB1); //set the direction
			OCR1A=num;
			break;
			case 1:
			TCCR1A |= (1<<WGM12)|(1<<WGM10)|(1<<COM1B1);//to get fast pwm and non inverted mode
			DDRB |= (1<<PORTB2);
			OCR1B=num;
			break;
			case 2:
			TCCR2A |= (1<<WGM21)|(1<<WGM20)|(1<<COM2A1);//to get fast pwm and non inverted mode
			DDRB |= (1<<PORTB3);
			OCR2A=num;
			break;
			
			
		}
	}

	// function to sort an array
	int *sort(int arr[]) {
		int temp;
		for (int i = 0; i < 6; i++) {
			for (int j = i + 1; j < 6; j++) {
				if (arr[i] > arr[j]) {
					temp = arr[i];
					arr[i] = arr[j];
					arr[j] = temp;
				}
			}
		}
	}

	// Function to find a point using Linear Interpolation
	int LinearInterpolation(int y1, int y2, int x1, int x2, int x0) {

		int y0 = (int)(((y2 - y1) * (x0 - x1) / (x2 - x1)) + y1);
		return y0;
	}

	int indexOf(int arr[], int size, int val) {
		for (int i = 0; i <= size; i++) {
			if (val == arr[i]) {
				return i;
			}

		}
	}
	// check a value whether it is a element of an array
	int inList(int val,int arr[],int size){
		for(int i=0;i<size;i++){
			if(val==arr[i]){
				return 1;
				
				}else{
				return 0;
			}
		}
	}

	// function to get the best approximated point
	int bestApproximate(int x[], int y[], int x0) {
		if(inList(x0,x,5)){
			for(int i=0;i<5;i++){
				
				if(x0==x[i]){
					return y[i];
				}
			}
		}
		else{
			int z[6];
			for (int i = 0; i < 5; i++) {
				z[i] = x[i];
			}

			z[5] = x0;
			sort(z);
			int q = indexOf(z, 6, x0);
			int x1 = z[q - 1];
			int x2 = z[q + 1];
			int y1_pos = indexOf(x, 5, x1);
			int y2_pos = indexOf(x, 5, x2);
			int y1 = y[y1_pos];
			int y2 = y[y2_pos];
			int res = LinearInterpolation(y1, y2, x1, x2, x0);
			
			
			return res;
		}}
		// get the average of sensor readings
		int average(int list[]) {
			int sum = 0;
			for (int i = 0; i < sizeof(list); i++) {
				sum += list[i];
			}
			return sum / sizeof(list);
		}//get the sensor reading
		int* get_reading(int pin) {
			int list[10];
			for (int i = 0; i < 10; i++) {
				list[i] = adc_read(pin);
				delay(25);
			}
			return list;
		}
		int main(void){
			// initialize the i2c and lcd display
			i2c_init();
			i2c_start();
			i2c_write(0x70);
			lcd_init();
			// display modes
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
					// take the values of basic colors
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
											Red_r = displayWrite("RED","R-");
											if(Red_r){
												Green_r=displayWrite("RED","G-");
												if (Green_r){
													Blue_r = displayWrite("RED", "B-");
													if (Blue_r) {
														Red_g = displayWrite("Green", "R-");
														if (Red_g) {
															Green_g = displayWrite("Green", "G-");
															if (Green_g) {
																Blue_g = displayWrite("Green", "B-");
																if (Blue_g) {
																	Red_blue = displayWrite("Blue", "R-");
																	if (Red_blue) {
																		Green_blue = displayWrite("Blue", "G-");
																		if (Green_blue) {
																			Blue_blue = displayWrite("Blue", "B-");
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
											}

										}
									}
								}
							}

						}
					}


				}
				if (mode == '2') { //mode 2 - sensoring mode
					RGB_off();
					lcd_init();
					_Bool finish = false;
					while (! finish) {
						int bl_avg_nw,rd_avg_nw,gr_avg_nw;
						PORTC ^= (1 << Red); _delay_ms(500); int rd_avg = average(get_reading(pin));
						if (calibrated) {
							int Red_y[] = {0,255,235,70,50};
							int Red_x[] = {strtol(Red_b), strtol(Red_w), strtol(Red_r), strtol(Red_g), strtol(Red_blue)};
							int n = sizeof(Red_x) / sizeof(Red_x[0]);
							rd_avg_nw = bestApproximate(Red_x, Red_y, rd_avg);// final RED value
							
							
							
							
						}
						
						
						
						PORTC ^= (1 << Red); //light up red bulb
						PORTC ^= (1 << Green); _delay_ms(500); int gr_avg = average(get_reading(pin));
						if (calibrated) {
							int Green_y[] = {0,255,9,186,150};
							int Green_x[] = {strtol(Green_b), strtol(Green_w), strtol(Green_r), strtol(Green_g), strtol(Green_blue)};
							int n = sizeof(Green_x) / sizeof(Green_x[0]);
							gr_avg_nw= bestApproximate(Green_x, Green_y,gr_avg);//Final Green value
							
							
							
							
							
						}
						PORTC ^= (1 << Green); //light up green bulb
						PORTC ^= (1 << Blue); _delay_ms(500); int bl_avg  = average(get_reading(pin));
						if (calibrated) {
							int Blue_y[] = {0,255,9,40,232};
							int Blue_x[] = {strtol(Blue_b), strtol(Blue_w), strtol(Blue_r), strtol(Blue_g), strtol(Blue_blue)};
							int n = sizeof(Blue_x) / sizeof(Blue_x[0]);
							bl_avg_nw = bestApproximate(Blue_x, Blue_y,bl_avg);// final blue value
							
						}
						PORTC ^= (1 << Blue);   //light up blue bulb
						// convert uint16 to string
						char str_red [sizeof(rd_avg_nw) * 8 + 1];
						char str_green [sizeof(gr_avg_nw) * 8 + 1];
						char str_Blue [sizeof(bl_avg_nw) * 8 + 1];

						lcd_cmd(0x80);
						utoa(rd_avg_nw, str_red, 10);
						lcd_msg("R:"); lcd_cmd(0x83); lcd_msg(str_red);
						utoa(gr_avg_nw, str_green, 10);
						lcd_cmd(0x88); lcd_msg("G:"); lcd_cmd(0x8B); lcd_msg(str_green);
						utoa(bl_avg_nw, str_Blue, 10);
						lcd_cmd(0xC6); lcd_msg("B:"); lcd_cmd(0xCA); lcd_msg(str_Blue);
					}
				}
				if (mode == '3') { //mode 3 - light up RGB led for given R,G,B values
					RGB_off();
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
