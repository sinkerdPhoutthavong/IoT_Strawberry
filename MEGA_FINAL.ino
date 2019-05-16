      #include <LiquidCrystal.h>
      #include<SoftwareSerial.h>
      #include "DHT.h"
      #include <Wire.h>
      #include <Keypad.h>
      #include <Password.h>   
      
      //LCD
      const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
      LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

      String MyinputString = "";
      char inChar;

      //DHT
      char inChar1;
      int Temp, Humi,Temp2,Humi2;
      #define DHTPIN A0
      #define DHTPIN2 A7
      #define DHTPIN3 A3
      #define DHTTYPE DHT22
      int AVG_TEMP;
      int AVG_HUMI;

      //FROM NODE_MCU
      int humi;
      int temp;
      int Hour;
      int Minutes;
      String PUMP;

      //SOIL
      #define SOILPin A6
      
      //LDR
      int LDRPin = A2; 
      int LDRValue = 0; 
      
      //Relay
      #define COOLER_PUMP 26
      #define WATER_PUMP 27
      #define COOLER 28
      #define FAN_IN_OUT 29
      
      //Buzzer
      #define Buzzer 22
    
      // CONDITION HIGH
      int TempHIGH = 30;
      int HumiHIGH = 80;
      
      //KEYPAD
      int STATE = HIGH;
      int locked = 1; 
      int passinput = 0;
      int lockedled = 14;
      int unlockedled = 15;
      long ledflashvar = 0;
      long ledflashtime = 300;  
      const byte ROWS = 4;
      const byte COLS = 4;
      char key;
      char keys[ROWS][COLS] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
      byte rowPins[ROWS] = {46, 47, 48, 49};
      byte colPins[COLS] = {50, 51, 52, 53};
      Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
      Password password = Password("0000");   // รหัสของการปลดล็อค

      boolean OFF =  HIGH;
      boolean ON = LOW;
        
      DHT dht(DHTPIN, DHTTYPE);
      DHT dht2(DHTPIN2, DHTTYPE);
      
void setup() {
      SET_SENSOR();
      SET_LCD();
      SET_SERIAL();
      SET_RELAY();
      SET_BUZZER();
      SETFIRST_RELAY();
      delay(1000);
}

void loop() {
                KEYPAD_CONTROL();s
                Get_DHT();
                Get_Soil();
                Get_LDR();
                delay(50);  
                 if(key == NO_KEY){
                    //AUTO CODE
                    int state = 0;
                    if(AVG_HUMI >= 80 && AVG_TEMP > 31 ){
                            digitalWrite(FAN_IN_OUT,ON);
                            delay(5000);
                            digitalWrite(WATER_PUMP,OFF);
                            digitalWrite(COOLER_PUMP, ON);
                            state = 1;
                    }
                    else if(AVG_HUMI >= 80 && AVG_TEMP <= 31 ){
                             if(state!=1 && AVG_HUMI >= 80 ){
                                digitalWrite(FAN_IN_OUT,ON);//TEST
                             }
                             digitalWrite(FAN_IN_OUT,OFF);
                             digitalWrite(COOLER_PUMP, OFF);
                             digitalWrite(WATER_PUMP,OFF);
                             digitalWrite(COOLER,ON);       
                    }
                    else if(AVG_HUMI <= 80 && AVG_TEMP > 31){
                            digitalWrite(FAN_IN_OUT,ON);
                            delay(5000);
                            digitalWrite(COOLER_PUMP, ON);
                            digitalWrite(FAN_IN_OUT,OFF);
                            digitalWrite(WATER_PUMP,OFF);
                            digitalWrite(COOLER,ON);
                             state = 1;
                    }
                    else if(AVG_HUMI <= 80 && AVG_TEMP <= 31){
                            digitalWrite(FAN_IN_OUT,OFF);
                            digitalWrite(COOLER_PUMP, OFF);
                            digitalWrite(WATER_PUMP,OFF);
                            digitalWrite(COOLER,ON);
                    }else{
                          Serial.print("LOWW");
                          digitalWrite(FAN_IN_OUT,OFF);
                          digitalWrite(COOLER_PUMP, OFF);
                          digitalWrite(WATER_PUMP,OFF);
                          digitalWrite(COOLER,ON);
                    }
                 }
                 
                 if (key != NO_KEY){
                      //MANUAL
                      Serial.println(key);   
                      password.append(key); 
                      passinput = 1;  
                      if(key == '*'){  
                        password.reset();     
                        passinput = 0;  
                        locked = 1;
                      }
                      if(password.evaluate()) {       
                        locked = !locked; 
                        password.reset();            
                        passinput = 0;
                      }
                      if(!locked){
                         while(key != '9'){
                          key = keypad.getKey();
                          password.append(key);
                          passinput = 1;  
                          lcd.setCursor(0,0);
                          lcd.print(" CONTROL PROGRAM ");  
                          if(key== '1' || key=='2' || key=='3' || key=='4'|| key=='7' || key=='8'){
                                            lcd.setCursor(0,0);
                                            lcd.print("CONTROL PROGRAM");
                                            lcd.setCursor(0,1);
                                            lcd.print(" PLEASE  SELECT ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print(" A: FAN , 9 OUT ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Water_Pump: B  ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print("   Cooler:  C   ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Cooler_Pump: D  ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print(" SET_Humi: 6  ");
                                            delay(1500);    
                                            lcd.setCursor(0,1);
                                            lcd.print(" SET_Temp: 5  ");
                                            delay(1500);    
                          }
                          if(key == '5'){
                               key = keypad.getKey();
                                password.append(key);
                                passinput = 1;  
                                lcd.setCursor(0,0);
                                lcd.print("SET TempHigh");
                                SET_TEMP();
                          }
                          if(key == '6'){
                                key = keypad.getKey();
                                password.append(key);
                                passinput = 1;  
                                lcd.setCursor(0,0);
                                lcd.print("SET HumiHigh");
                                SET_HUMI();
                          }
                             switch (key) {             
                                case 'A':
                                      digitalWrite(FAN_IN_OUT,  !digitalRead(FAN_IN_OUT));
                                        if(digitalRead(FAN_IN_OUT)==HIGH){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("FAN: OFF");
                                        }
                                        if(digitalRead(FAN_IN_OUT)==LOW){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("FAN: OPEN");
                                        }
                                      break;
                                case 'B':
                                      digitalWrite(WATER_PUMP, !digitalRead(WATER_PUMP));
                                      if(digitalRead(WATER_PUMP)==HIGH){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("WATER_PUMP: OFF");
                                        }
                                        if(digitalRead(WATER_PUMP)==LOW){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("WATER_PUMP:OPEN");
                                        }
                                      break;
                                case 'C':
                                      digitalWrite(COOLER, !digitalRead(COOLER));
                                      if(digitalRead(COOLER)==HIGH){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("SYSTEM_COOL:OFF");
                                        }
                                        if(digitalRead(COOLER)==LOW){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("SYSTEM_COOL:OPEN");
                                        }
                                      break;
                                case 'D':
                                      digitalWrite(COOLER_PUMP, !digitalRead(COOLER_PUMP));
                                      if(digitalRead(COOLER_PUMP)==HIGH){
                                              lcd.clear();
                                              lcd.setCursor(0,1);
                                              lcd.print("COOLER_PUMP:OFF");
                                        }
                                      if(digitalRead(COOLER_PUMP)==LOW){
                                              lcd.clear();
                                              lcd.setCursor(0,1);
                                              lcd.print("COOLER_PUMP:OPEN");
                                         }
                                       break; 
                                 case '#':
                                            lcd.setCursor(0,0);
                                            lcd.print("CONTROL PROGRAM");
                                            lcd.setCursor(0,1);
                                            lcd.print(" A: FAN , 9 OUT ");
                                            delay(1000);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Water_Pump: B  ");
                                            delay(1000);
                                            lcd.setCursor(0,1);
                                            lcd.print("   Cooler:  C   ");
                                            delay(1000);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Cooler_Pump: D  ");
                                            delay(1000);
                                       break;
                        }
                            delay(100);
                      }
                      password.reset();
                   }
                     
           }  
}
void Get_DHT(){ 
      Humi = dht.readHumidity();
      Temp = dht.readTemperature();
      Humi2 = dht2.readHumidity();
      Temp2 = dht2.readTemperature();
      if (isnan(Humi) || isnan(Temp) || isnan(Humi2) || isnan(Temp2) ) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }
      AVG_TEMP = (Temp + Temp2)/2;
      AVG_HUMI = (Humi + Humi2)/2;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("T: ");
      lcd.print(AVG_TEMP);
      lcd.print((char)223);
      lcd.print("C");
      lcd.print(" ");
      lcd.print(" H: ");
      lcd.print(AVG_HUMI);
      lcd.print(" %"); 
 }
void Get_Soil(){
      int sensorValue;
      sensorValue = analogRead(SOILPin);
      lcd.setCursor(0,1);
      lcd.print("S: ");
      lcd.print(sensorValue);
      lcd.print(" ");
      lcd.print(" ");
      lcd.print("L: ");
      lcd.print(LDRValue);
}
void Get_LDR(){
      LDRValue = analogRead(LDRPin); // Divides input 0-1023 to resemble to 0-255
}
void SET_RELAY(){
      pinMode(COOLER_PUMP,OUTPUT);
      pinMode(WATER_PUMP,OUTPUT);
      pinMode(COOLER,OUTPUT);
      pinMode(FAN_IN_OUT,OUTPUT);
      digitalWrite(COOLER,HIGH);
}
void SET_SERIAL(){
      Serial.begin(115200);
      Serial.print(char(169));
}
void SET_LCD(){
      lcd.begin(16, 2);
      lcd.setCursor(0,0);
      lcd.print("     WELCOME    ");
      lcd.setCursor(0,1);
      lcd.print("   STRAWBERRY   ");
      delay(1000);
      lcd.setCursor(0,1);
      lcd.print("System Boot ");
      delay(200);
      lcd.print(".");
      delay(600);
      lcd.print(".");
      delay(500);
      lcd.print(".");
      delay(500);
      lcd.print(".");
      delay(500);
      lcd.clear();
}
void SET_SENSOR(){
      dht.begin();
      pinMode(SOILPin,INPUT);
      pinMode(LDRPin,INPUT);
}
void SET_BUZZER(){
      pinMode(Buzzer,OUTPUT);
}
void KEYPAD_CONTROL(){
      key = keypad.getKey();
}
void SETFIRST_RELAY(){
      digitalWrite(WATER_PUMP,OFF);
      digitalWrite(COOLER_PUMP,OFF);
      digitalWrite(COOLER,ON);
      digitalWrite(FAN_IN_OUT,OFF);
}
void SET_TEMP(){
        while(key != '5'){
                          if(key=='3'){
                                  if(key=='0'){
                                      TempHigh = 30;
                                       lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 30");
                                       delay(700);
                                  }else if(key=='3'){
                                  if(key=='1'){
                                      TempHigh = 31;
                                       lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 31");
                                       delay(700);
                                  }
                                }else if(key=='3'){
                                  if(key=='2'){
                                      TempHigh = 32;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 32");
                                       delay(700);
                                  }
                                }else if(key=='3'){
                                  if(key=='3'){
                                      TempHigh = 33;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 32");
                                       delay(700);
                                  }
                                }
                                else if(key=='3'){
                                  if(key=='4'){
                                      TempHigh = 34;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 34");
                                       delay(700);
                                  }
                                }
                                else if(key=='3'){
                                  if(key=='5'){
                                      TempHigh = 35;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 35");
                                       delay(700);
                                  }
                                }
                                else if(key=='3'){
                                  if(key=='6'){
                                      TempHigh = 36;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 36");
                                       delay(700);
                                  }
                                }
                                else if(key=='3'){
                                  if(key=='7'){
                                      TempHigh = 37;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 37");
                                       delay(700);
                                  }
                                }
                                else if(key=='3'){
                                  if(key=='8'){
                                      TempHigh = 38;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 38");
                                       delay(700);
                                  }
                                }
                                else if(key=='3'){
                                  if(key=='9'){
                                      TempHigh = 39;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 39");
                                       delay(700);
                                  }
                                }
                                else {
                                  TempHigh = TempHigh;
                                }
                                if(key=='2'){
                                  if(key=='5'){
                                      TempHigh = 25;
                                       lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 25");
                                       delay(700);
                                  }
                                }else if(key=='2'){
                                  if(key=='6'){
                                      TempHigh = 26;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 26");
                                       delay(700);
                                  }
                                }else if(key=='2'){
                                  if(key=='7'){
                                      TempHigh = 27;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 27");
                                       delay(700);
                                  }
                                }
                                else if(key=='2'){
                                  if(key=='8'){
                                      TempHigh = 28;
                                      lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 28");
                                       delay(700);
                                  }
                                }
                                else if(key=='2'){
                                  if(key=='9'){
                                      TempHigh = 29;
                                       lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 29");
                                       delay(700);
                                  }
                                }
                                else {
                                  TempHigh = 31;
                                   lcd.setCursor(0,0);
                                       lcd.print("SET TempHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("TempHigh = 31");
                                       delay(700);
                                }
                           }
        }
}
void SET_HUMI(){
         while(key != '6'){
                          if(key=='7'){
                                  if(key=='0'){
                                      HumiHigh = 70;
                                       lcd.setCursor(0,0);
                                       lcd.print("SET HumiHigh");
                                       lcd.setCursor(0,1);
                                       lcd.print("HumiHigh = 70");
                                       delay(700);
                                  }else if(key=='7'){
                                    if(key=='5'){
                                         TempHigh = 75;
                                         lcd.setCursor(0,0);
                                         lcd.print("SET HumiHigh");
                                         lcd.setCursor(0,1);
                                         lcd.print("HumiHigh = 75");
                                    }
                                  }else if(key=='8'){
                                    if(key=='0'){
                                         TempHigh = 80;
                                         lcd.setCursor(0,0);
                                         lcd.print("SET HumiHigh");
                                         lcd.setCursor(0,1);
                                         lcd.print("HumiHigh = 80");
                                    }
                                  }
                                  else if(key=='8'){
                                    if(key=='5'){
                                         TempHigh = 85;
                                         lcd.setCursor(0,0);
                                         lcd.print("SET HumiHigh");
                                         lcd.setCursor(0,1);
                                         lcd.print("HumiHigh = 85");
                                    }
                                  }
                                  else if(key=='9'){
                                    if(key=='0'){
                                         TempHigh = 90;
                                         lcd.setCursor(0,0);
                                         lcd.print("SET HumiHigh");
                                         lcd.setCursor(0,1);
                                         lcd.print("HumiHigh = 90");
                                    }
                                  }else{
                                     HumiHigh = 80;
                                     lcd.print("SET HumiHigh");
                                     lcd.setCursor(0,1);
                                     lcd.print("HumiHigh = 80");
                                  }
                          }
         }
}
  
