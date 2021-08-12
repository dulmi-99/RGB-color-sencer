#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
String Red_b, Green_b, Blue_b, Red_r, Green_r, Blue_r, Red_g, Green_g, Blue_g, Red_blue, Green_blue, Blue_blue, Green_w, Red_w, Blue_w;
//String Green_w, Red_w, Blue_w = "1023";
float Red_m,Red_c,Green_m,Green_c,Blue_m,Blue_c;
boolean calibrated = false;

const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
char deciKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(deciKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x3F for a 16 chars and 2 line display
int green = A1;
int red = A2;
int blue = A0;

int green_ = 10;
int red_ = 9;
int blue_ = 11;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red_, OUTPUT);
  pinMode(green_, OUTPUT);
  pinMode(blue_, OUTPUT);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("1)Mode1");
  lcd.setCursor(8, 0);
  lcd.print("2)Mode2");
  lcd.setCursor(4, 1);
  lcd.print("3)Mode3");

}

String get_3_digit() {
  int count = 0;
  String val;

  while (count < 3) {
    char digit = customKeypad.getKey();
    if (digit) {

      lcd.setCursor(9 + count, 0);
      lcd.print(digit);
      val += digit;
      count += 1;
    }
  }
  return val;
}
 typedef struct Tuple{float m,c;};Tuple;
void temp() {

  Serial.println("temp");
  char mode = customKeypad.getKey();
  //mode 1
  boolean flag = false;
  if (mode) {
    flag = true;
  }
  else {
    flag = false;
  }
  while (flag) {
    if (mode == '1') {
      Serial.println("1");
      RGB_off();
      Red_b = displayWrite("Black", "R-");
      if (Red_b) {
        Green_b = displayWrite("Black", "G-");
        if (Green_b) {
          Blue_b = displayWrite("Black", "B-");
          if (Blue_b) {
            Red_w = displayWrite("White", "R-");
            if (Red_w) {
              Green_w = displayWrite("White", "G-");
              if (Green_w) {
                Blue_w = displayWrite("White", "B-");
                if (Blue_w) {
                  Red_r = displayWrite("RED", "R-");
                  if (Red_r) {
                    Green_r = displayWrite("RED", "G-");
                    if (Green_r) {
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
                                  flag = false;
                                  calibrated = true;
                                  lcd.clear();
                                  lcd.setCursor(2, 0);
                                  lcd.print("Calibration");
                                  lcd.setCursor(6, 1);
                                  lcd.print("Done!");
                                  delay(1000);
                                  setup();
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

    //mode2
    else if (mode == '2') {
     
      Serial.println("2");
      RGB_off();
      lcd.clear();
      boolean finish = false;
      while (not finish) {
        lcd.setCursor(2, 0);
        lcd.print("   ");
        lcd.setCursor(12, 0);
        lcd.print("   ");
        lcd.setCursor(8, 1);
        lcd.print("   ");
        digitalWrite(green, HIGH);
        delay(300);
        int bl_avg_nw,rd_avg_nw,gr_avg_nw;
        int gr_avg = average(get_reading(A3));
        if (calibrated) {
          int Green_y[] = {0,255,9,186,150};
          int Green_x[] = {Green_b.toInt(), Green_w.toInt(), Green_r.toInt(), Green_g.toInt(), Green_blue.toInt()};
          int n = sizeof(Green_x) / sizeof(Green_x[0]);
           gr_avg_nw= bestApproximate(Green_x, Green_y,gr_avg);
           
         
         
        
         
        }



        digitalWrite(green, LOW);

        digitalWrite(red, HIGH);
        delay(300);
        int rd_avg = average(get_reading(A3));
        if (calibrated) {
          int Red_y[] = {0,255,235,70,50};
          int Red_x[] = {Red_b.toInt(), Red_w.toInt(), Red_r.toInt(), Red_g.toInt(), Red_blue.toInt()};
          int n = sizeof(Red_x) / sizeof(Red_x[0]);
          rd_avg_nw = bestApproximate(Red_x, Red_y, rd_avg);
          
          
         
         
        }


        digitalWrite(red, LOW);

        digitalWrite(blue, HIGH);
        delay(300);
        int bl_avg  = average(get_reading(A3));
        if (calibrated) {
         int Blue_y[] = {0,255,9,40,232};
          int Blue_x[] = {Blue_b.toInt(), Blue_w.toInt(), Blue_r.toInt(), Blue_g.toInt(), Blue_blue.toInt()};
          int n = sizeof(Blue_x) / sizeof(Blue_x[0]);
           bl_avg_nw = bestApproximate(Blue_x, Blue_y,bl_avg);
          
        }


        digitalWrite(blue, LOW);

        //byte rd_avg_new,gr_avg_new, bl_avg_new = byte(develop(rd_avg,gr_avg,bl_avg)); //if want some pushup for values...!
        
        Serial.println("");
        if (calibrated) {
          analogWrite(red_, 255 - rd_avg_nw);
          analogWrite(green_, 255 - gr_avg_nw);
          analogWrite(blue_, 255 - bl_avg_nw);
        lcd.setCursor(0, 0);
        lcd.print("R-");
        lcd.setCursor(2, 0);
        lcd.print(rd_avg_nw);

        lcd.setCursor(10, 0);
        lcd.print("G-");
        lcd.setCursor(12, 0);
        lcd.print(gr_avg_nw);

        lcd.setCursor(6, 1);
        lcd.print("B-");
        lcd.setCursor(8, 1);
        lcd.print(bl_avg_nw);


        Serial.print("Green :");
        Serial.print(gr_avg_nw);
        Serial.print(" RED :");
        Serial.print(rd_avg_nw);
        Serial.print(" Blue :");
        Serial.print(bl_avg_nw);
        }
        else{
          lcd.setCursor(0, 0);
        lcd.print("R-");
        lcd.setCursor(2, 0);
        lcd.print(rd_avg);

        lcd.setCursor(10, 0);
        lcd.print("G-");
        lcd.setCursor(12, 0);
        lcd.print(gr_avg);

        lcd.setCursor(6, 1);
        lcd.print("B-");
        lcd.setCursor(8, 1);
        lcd.print(bl_avg);


        }
        delay(500);



      }
    }
    //mode3
    else if (mode == '3') {
      Serial.println("3");
      RGB_off();
      lcd.clear();
      lcd.setCursor(0, 0);
      String R = displayWrite("RGB", "R-");
      if (R) {
        String G = displayWrite("RGB", "G-");
        if (G) {
          String B = displayWrite("RGB", "B-");
          if (B) {
            lcd.clear();
            lcd.setCursor(6, 0);
            lcd.print("Done!");
            flag = false;
            analogWrite(red_, 255 - int (R.toInt()));
            analogWrite(green_,  255 - int (G.toInt()));
            analogWrite(blue_, 255 - int (B.toInt()));

            delay(900);
            setup();
          }
        }
      }

    }

  }
}

void loop() {
  temp();
  char chr = customKeypad.getKey();
  if (chr == '#') {
    setup();
  }
}


String displayWrite(String clr_mode, String clr ) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(clr_mode);
  lcd.setCursor(7, 0);
  lcd.print(clr);
  lcd.setCursor(0, 1);
  lcd.print("*-clear");
  lcd.setCursor(9, 1);
  lcd.print("Enter-#");
  String val = get_3_digit();
  if (val) {
    boolean flag1 = true;
    while (flag1) {
      char stat = customKeypad.getKey();
      if (stat == '#') {
        return  val;
      }
      else if (stat == '*') {
        displayWrite(clr_mode, clr );
      }
    }
  }
}
int average(int list[]) {
  int sum = 0;
  for (int i = 0; i < sizeof(list); i++) {
    sum += list[i];
  }
  return sum / sizeof(list);
}
int* get_reading(int pin) {
  int list[10];
  for (int i = 0; i < 10; i++) {
    list[i] = analogRead(pin);
    delay(25);
  }
  return list;
}
//int* develop(byte r, byte g, byte b) {
//  if (r < g and b < g) {
//    r = r - 50;
//    b = b - 50;
//    g = g + 50;
//
//  }
//  else if (b < r and g < r) {
//    r = r + 50;
//    b = b - 50;
//    g = g - 50;
//
//  }
//  else if (r < b and g < b) {
//    r = r - 50;
//    b = b + 50;
//    g = g - 50;
//
//  }
//  return r, g, b;
//}
byte  int_to_byte(int val) {
  if (val < 255) {
    return byte(val);
  }
  else {
    return 255;
  }
}
void RGB_off() {
  analogWrite(red_, 255);
  analogWrite(green_, 255 );
  analogWrite(blue_, 255);
}


// function to calculate m and c that best fit points
// represented by x[] and y[]
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
int inList(int val,int arr[],int size){
    for(int i=0;i<size;i++){
        if(val==arr[i]){
            return 1;
            
        }else{
            return 0;
        }
    }
}


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








