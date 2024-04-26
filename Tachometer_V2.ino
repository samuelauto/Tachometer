#include <Button2.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define BUTTON_PIN D3

LiquidCrystal_I2C lcd(0x27,16,2);
Button2 button;

/* Dejar en el loop solamente el button.loop para que se este verificando constantemente en que modo se esta
 * desglosar los modos en diferentes funciones para llamar a estas luego de cada modo
 * 3 modos: VELOCIDAD, CONTEO DE OBJETOS, EL TERCERO ESPERAR A QUE VENGA ADRIANO
 * SETEAR EL MODO DE VELOCIDAD CUANDO SE OPRIME EL BOTON UNA VEZ
 * SETEAR EL MODO DE CONTEO DE OBJETOS CUANDO SE OPRIME EL BOTON DOS VECES
 * 
 */


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
  button.begin(BUTTON_PIN);
  button.setClickHandler(MODO_RPM);
  button.setDoubleClickHandler(MODO_COUNT);
  pinMode(sensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensor), ISR_sensor, RISING);
}



void loop() { 

  button.loop();
     
}

void MODO_RPM(Button2& b){

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
lcd_rpm();
  
  }

void MODO_COUNT(Button2& b){
//DESARROLLO DEL MODO CONTADOR
     


lcd_count();
  
}

void lcd_rpm() {

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
    lcd.setCursor(13, 0);
    lcd.print("RPM");
    delay(100);
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

void lcd_count(){

  //DESARROLLO DEL LCD CDO ESTA EN MODO CONTADOR

  
}

void ISR_sensor() {

    t_pulse_duration_volatile = micros() - t_pulse_started_volatile;
    t_pulse_started_volatile = micros();
    timeout = 0;
    newpulse = 1;

}