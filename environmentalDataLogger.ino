#include <SoftwareSerial.h>
#include <TimerThree.h>
SoftwareSerial LCD(3, 9);

// variables
const int SWITCH = 8, TIMER = 2;
const int LEDPIN[] = {5, 6, 7}; // red, yellow, green
int TESTMODE, time = 0;
volatile int redLEDstate = 1, LCDsetting = 1; 

float windReadings[6];
float windAvg1, FullWindAvg1, windAvg2, FullWindAvg2;
float lightReading, FullLightReading, airReading, FullAirReading;

//---------------------------------------------------------------------------------------------------------------------------------//

void setup() {
  LCD.begin(9600);   delay(500);
  LCDclear();  // clear display
  
  Serial.begin(9600);
  analogRead(A2); // potentiometer
  analogRead(A3); // photo resistor
  analogRead(A4); // temp sensor
  pinMode(SWITCH, INPUT);  
    
  Timer3.initialize(100000); 
  Timer3.attachInterrupt(blinkLED);
  attachInterrupt(digitalPinToInterrupt(2), Press, FALLING);

  for (int i = 0; i < 3; i++){ // initalize each LED pin
    pinMode(LEDPIN[i], OUTPUT);
    digitalWrite(LEDPIN[i], LOW);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------//

void loop() {
  
  for (int j = 0; j < 10; j++){ //FULL MODE - loop to store readings every 10 mins
  TESTMODE = digitalRead(SWITCH);
 
    for (int i = 0; i < 6; i++){ //TEST MODE - loop to store all readings in 1 min
      noInterrupts();
      windReadings[i] = analogRead(A2)/64;  
      lightReading += analogRead(A3);
      airReading += celsius(analogRead(A4)); 
      windAvg1 += windReadings[i];
      interrupts();      
      delay(TIMER*1000); // take reading every 10s
      }
    
    // 1 min data avg over 6 ten second readings
    windAvg1/=6; 
    lightReading/=6;
    airReading/=6;
    
    LCDdisplay();
    
    if (TESTMODE == HIGH){
      printHeader();
      WindSpeed(windAvg1, windAvg2);
      ambient(lightReading, LEDPIN[1]);
      airTemp(airReading, LEDPIN[2]);
    }
     
    FullWindAvg1 += windAvg1;
    FullLightReading += lightReading; 
    FullAirReading += airReading; 
    
    windAvg2 = windAvg1; // set second to first
    windAvg1 = 0;
    lightReading = 0;
    airReading = 0;
  }
  
  // 10 min data avg over 10 one min readings
  FullWindAvg1/=10;
  FullLightReading/=10;
  FullAirReading/=10;
  
  if (TESTMODE == LOW){ 
    printHeader();
    WindSpeed(FullWindAvg1, FullWindAvg2);
    ambient(FullLightReading, LEDPIN[1]);
    airTemp(FullAirReading, LEDPIN[2]);
  }
      
  FullWindAvg2 = FullWindAvg1; //set second to first
  FullLightReading = 0;
  FullAirReading = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------//

void Press(){ 
  if (LCDsetting < 3)
    LCDsetting++;
  else 
    LCDsetting = 1;
   delay(200);
}

void LCDdisplay(){
  LCDclear();
  switch(LCDsetting){
    case 1:
      LCDtemperature(airReading); 
       break;
    case 2:
      LCDlight(lightReading);
       break;
    case 3:
      LCDwind(windAvg1, windAvg2); break;
  }
}
void LCDclear(){
  LCD.write(254); // move cursor to beginning of first line
  LCD.write(128);
  LCD.write("                                                "); // clear display
  LCD.write(254); // move cursor to beginning of first line
  LCD.write(128);
}

void LCDtemperature(int temp){
 char tempstring[10], tempstring2[10];

  LCD.write("Temperature:");  
  LCD.write("    "); 
  LCD.write("                ");
  sprintf(tempstring,"%4d",temp);
  sprintf(tempstring2,"%4d",(temp*9/5+32));
  LCD.write(tempstring);  LCD.write("C ");  
  LCD.write(tempstring2); LCD.write("F");
}

void LCDlight(int lux){
  
  const int lightLimit[] = {100, 350, 650, 1023};
  const String lightReading[] = {"dark", "overcast","brght","sunny"};
  
  LCD.write("Light:");  
  LCD.write("          "); 
  LCD.write("                ");
  if (lux < 100)        { LCD.write("Dark");     }
  else if (lux < 350)   { LCD.write("Overcast"); }
  else if (lux < 650)   { LCD.write("Bright");   }
  else if (lux < 1023)  { LCD.write("Sunny");    } 
}        

void LCDwind(int wind1, int wind2){
  LCD.write("Wind:");  
  LCD.write("           "); 
  LCD.write("                ");
      if      (wind1==wind2 && wind1==7 || wind1==8)    {   LCD.write("Still");     }
      else if (wind1==wind2 && wind1>=3 && wind1<=6)    {   LCD.write("Windy");     }
      else if (wind1==wind2 && wind1>=9 && wind1<=12)   {   LCD.write("Windy");     }
      else if (wind1==wind2 && wind1>=0 && wind1<=2)    {   LCD.write("Stormy");    }
      else if (wind1==wind2 && wind1>=13 && wind1<=15)  {   LCD.write("Stormy");    }
  
      else if (wind1!=wind2 && wind1>=6 && wind1<=9)    {   LCD.write("Breezy");    }
      else if (wind1!=wind2 && wind1>=0 && wind1<=5)    {   LCD.write("Gusty");     }
      else if (wind1!=wind2 && wind1>=10 && wind1<=15)  {   LCD.write("Gusty");     }
      else                                              {   LCD.write("No Wind Data"); }
      LCD.write("           ");
}

void WindSpeed(float wind1, float wind2){
  Serial.print("Wind:                      ");

  if (wind1==wind2 && wind1==7 || wind1==8){   
    Serial.println("Still");     
    redLEDstate = 1; 
    }
  else if (wind1==wind2 && wind1>=3 && wind1<=6){
    Serial.println("Windy");     
    redLEDstate = 3; 
  }
  else if (wind1==wind2 && wind1>=9 && wind1<=12){
    Serial.println("Windy");
    redLEDstate = 3;
  }
  else if (wind1==wind2 && wind1>=0 && wind1<=2){
    Serial.println("Stormy");
    redLEDstate = 2; 
    }
  else if (wind1==wind2 && wind1>=13 && wind1<=15){
    Serial.println("Stormy");    
    redLEDstate = 2; 
  }
  else if (wind1!=wind2 && wind1>=6 && wind1<=9){
    Serial.println("Breezy");
    redLEDstate = 3;
  }
  else if (wind1!=wind2 && wind1>=0 && wind1<=5){
    Serial.println("Gusty");     
    redLEDstate = 3; 
  }
  else if (wind1!=wind2 && wind1>=10 && wind1<=15){
    Serial.println("Gusty");
    redLEDstate = 3; }
  }

void ambient(int lux, int LEDpin){
  int luxLimit[] = {100, 350, 650, 1023};
  String luxReading[] = {"less than 100 lux   dark", 
                         "100-1000 lux        overcast", 
                         "1000-10000 lux      bright", 
                         "over 10000 lux      sunny"};
  digitalWrite(LEDpin,LOW); 
  Serial.print("Outside ambient light:   "); 
  
  for (int i = 0; i < 4; i++){
    if (lux < luxLimit[i]){
      Serial.print("  ");
      Serial.println(luxReading[i]);
        if (luxReading[i] == "sunny")
          digitalWrite(LEDpin, HIGH);
    break;    
    }
  }
}

void airTemp(int temp, int LEDpin){
  int tempLimit[] = {0, 15, 25, 1000}; //celsius
  String tempReading[] = {"below freezing", "cool", "warm", "hot"};
  Serial.print("Outside air temperature:   "); 
  Serial.print(temp);
  Serial.print("C ");
  Serial.print(temp*9/5+32);
  Serial.print("F             ");
  
  for (int i = 0; i < 4; i++){
    if (temp < tempLimit[i]){
      Serial.println(tempReading[i]);
        if (tempReading[i] == "warm" or tempReading[i] == "hot")
          digitalWrite(LEDpin, HIGH);
    break;    
    }
  }
}

void blinkLED(){ 
   switch (redLEDstate){
    case 1: // off
      digitalWrite(LEDPIN[0], LOW); break; // off
    case 2:
      digitalWrite(LEDPIN[0], HIGH); break; // on
    case 3:
      digitalWrite(LEDPIN[0], digitalRead(LEDPIN[0])^1); break; // blink
   } 
}

void printHeader(){
  Serial.println("-------------------------------------------------------");
  Serial.print("Mount Lake Resort. Date: 25 Feb 2020. Time: 10:");
  Serial.println(time);
  Serial.println("Location: Peak Lake");
  Serial.println("-------------------------------------------------------");
  time++;
}

float celsius(float reading){ //convert the voltage reading to temperature
  float voltage = reading*5.0;
  voltage /= 1024.0;
  float celsius  = (voltage - 0.5) * 100;                                      
  return celsius;
}
