#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const byte sensor = 2;

volatile unsigned long t_pulse_started_volatile = 0;
volatile unsigned long t_pulse_duration_volatile = 0;
unsigned long t_pulse_started = 0;
unsigned long t_pulse_duration = 0;

long rpm_sum = 0;
long rpm_reading[100];
long rpm_average = 0;
byte n_max = 0;
byte n = 0;

volatile bool timeout = 1;
volatile bool newpulse = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.print("INITIALIZING");
  delay(1000);
  lcd.clear();
  
  pinMode(sensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensor), ISR_sensor, RISING);
}

void loop() { 
   
   noInterrupts();
   t_pulse_started = t_pulse_started_volatile;
   t_pulse_duration = t_pulse_duration_volatile;
   interrupts();
   
   if(((micros() - t_pulse_started) > 2000000) && timeout == 0 && newpulse == 0) {
           
     timeout = 1;
     rpm_average = 0;
     n = 0;

   };
   
   if(timeout == 0){
     
     if(newpulse){
       
       rpm_reading[n] = (60000000 / t_pulse_duration);
       n_max = constrain(map(rpm_reading[n], 60, 100000, 0, 100), 0, 100);
       n++;
       newpulse = 0;
       
       if(n > n_max){
       
         for (byte i = 0; i <= n_max; i++) {
           
           rpm_sum = rpm_sum + rpm_reading[i];
     
         };
         
         rpm_average = rpm_sum / (n_max + 1);     
         rpm_sum = 0;
         n = 0;
         
       }
       
     }
   
   }
updatedisplay();  
}

void updatedisplay() {

  byte x = 0;
  
  lcd.clear();
  //lcd.setTextSize(3);
  
  if(rpm_average < 10){
    x = 2;
  }   
  else if(rpm_average >= 10 && rpm_average < 100){
    x = 3;
  } 
  else if(rpm_average >= 100 && rpm_average < 1000){
    x = 4;
  } 
  else if(rpm_average >= 1000 && rpm_average < 10000){
    x = 5;
  } 
  //Ultima condicion, para 5 digitos
  else{
    x = 6;
  }
  
  if(rpm_average < 100000){
    // lcd.clear(); 
    // delay(100);
    lcd.setCursor(0,0);
    lcd.print(rpm_average);
    //delay(100);
    Serial.print(rpm_average);
    Serial.print("\n");
    //lcd.setTextSize(1);
    lcd.setCursor(x, 0);
    lcd.print("RPM");
    //delay(100);
    //lcd.display();
  
  } else {
      
      //lcd.setTextSize(2);
      lcd.clear();
      lcd.setCursor(2, 1); 
      lcd.print(F("MAX LIMIT"));
      delay(500);
      lcd.clear();
      //lcd.display();
  }
}

void ISR_sensor() {
 
  t_pulse_duration_volatile = micros() - t_pulse_started_volatile;
  t_pulse_started_volatile = micros();
  timeout = 0;
  newpulse = 1;
 
}
