      #include <LiquidCrystal.h>
      #include<SoftwareSerial.h>
      #include "DHT.h"
      #include <Wire.h>
      #include <Keypad.h>
      #include <Password.h>   
      //LCD
      const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
      LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

      //DERIAL COMUNICATION
      SoftwareSerial MegaSerial(18, 19); // RX, TX

      String MyinputString = "";
      char inChar;

      //DHT
      char inChar1;
      int Temp, Humi,Temp2,Humi2,Temp3,Humi3;
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
      int TempHIGH = 33;
      int HumiHIGH = 70;
      
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

      boolean CONTROL_WATER_PUMP = 0;
      boolean CONTROL_COOLER = 0;
      boolean CONTROL_COOLER_PUMP = 0;
      boolean CONTROL_FAN_IN_OUT = 0;

      boolean STATE_CONTROL_WATER_PUMP   = 0;
      boolean STATE_CONTROL_COOLER = 0;
      boolean STATE_CONTROL_COOLER_PUMP = 0;
      boolean STATE_CONTROL_FAN_IN_OUT = 0;
        
      
      
      DHT dht(DHTPIN, DHTTYPE);
      DHT dht2(DHTPIN2, DHTTYPE);
      DHT dht3(DHTPIN3, DHTTYPE);
void setup() {
      SET_SENSOR();
      SET_LCD();
      SET_SERIAL();
      SET_RELAY();
      SET_BUZZER();
      set_relay();
      delay(1000);
}

void loop() {
                KEYPAD_CONTROL();
                Get_DHT();
                Get_Soil();
                Get_LDR();
                delay(50);  
                Serial.print("GII");
                 if(key == NO_KEY){
                    Condition_SENSOR();
                    Input();
                    if(AVG_HUMI >= 80 && AVG_TEMP > 31 ){
//                            digitalWrite(FAN_IN_OUT,ON);
                           // digitalWrite(WATER_PUMP,OFF);
                            digitalWrite(COOLER_PUMP, ON);
                    }
                    else if(AVG_HUMI >= 80 && AVG_TEMP <= 31 ){
                             digitalWrite(FAN_IN_OUT,OFF);//TEST
                             digitalWrite(COOLER_PUMP, OFF);
                             //digitalWrite(WATER_PUMP,OFF);
                             digitalWrite(COOLER,ON);
                    }
                    else if(AVG_HUMI <= 80 && AVG_TEMP > 31){
                            digitalWrite(FAN_IN_OUT,ON);
                            delay(5000);
                            digitalWrite(COOLER_PUMP, ON);
                            digitalWrite(FAN_IN_OUT,OFF);
                         //   digitalWrite(WATER_PUMP,OFF);
                            digitalWrite(COOLER,ON);
                    }
                    else if(AVG_HUMI <= 80 && AVG_TEMP <= 31){
                            digitalWrite(FAN_IN_OUT,OFF);
                            digitalWrite(COOLER_PUMP, OFF);
                         //   digitalWrite(WATER_PUMP,OFF);
                            digitalWrite(COOLER,ON);
                    }else{
                          Serial.print("LOWW");
                          digitalWrite(FAN_IN_OUT,OFF);
                          digitalWrite(COOLER_PUMP, OFF);
                         // digitalWrite(WATER_PUMP,OFF);
                          digitalWrite(COOLER,ON);
                    }
                 }
                 
                 if (key != NO_KEY){
                      Serial.println(key);   
                      password.append(key);  // บันทึกค่ารหัสที่กดและเปรียบเทียบกับรหัสที่ถูกต้อง
                      passinput = 1;  
                      if(key == '*'){  
                        password.reset();     // เริ่มต้นใส่รหัสใหม่ตั้งแต่ตัวแรก
                        passinput = 0;        // ไม่อยู่ระหว่างการกดรหัส  
                        locked = 1;
                      }
                      
                      if(password.evaluate()) {       //  ถ้าใส่ Password ถูกต้องให้ปลดล็อค 
                        locked = !locked; 
                        password.reset();             // เริ่มต้นใส่รหัสใหม่ตั้งแต่ตัวแรก
                        passinput = 0;
                      }
                      if(!locked){
                         while(key != '9'){
                          key = keypad.getKey();
                          password.append(key);  // บันทึกค่ารหัสที่กดและเปรียบเทียบกับรหัสที่ถูกต้อง
                          passinput = 1;  
                          lcd.setCursor(0,0);
                          lcd.print(" CONTROL PROGRAM ");  
                          if(key== '1' || key=='2' || key=='3' || key=='4' || key=='6' || key=='7' || key=='8'){
                                            lcd.setCursor(0,0);
                                            lcd.print("CONTROL PROGRAM");
                                            lcd.setCursor(0,1);
                                            lcd.print(" PLEASE  SELECT ");
                                            delay(700);
                                            lcd.setCursor(0,1);
                                            lcd.print(" A: FAN , 9 OUT ");
                                            delay(700);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Water_Pump: B  ");
                                            delay(700);
                                            lcd.setCursor(0,1);
                                            lcd.print("   Cooler:  C   ");
                                            delay(1000);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Cooler_Pump: D  ");
                                            delay(1000);   
                          }
                              switch (key) {                //  และเลือก Relay ที่ต้องการควบคุม
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
                                          lcd.print("WATER_PUMP:  OPEN");
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
                                          lcd.print("SYSTEM_COOL: OPEN");
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
                                              lcd.print("COOLER_PUMP: OPEN");
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
      Humi3 = dht3.readHumidity();
      Temp3 = dht3.readTemperature();
      if (isnan(Humi) || isnan(Temp) || isnan(Humi2) || isnan(Temp2) || isnan(Humi3) || isnan(Temp3) ) {
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
      lcd.print("H: ");
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
void Condition_SENSOR(){
      if(AVG_TEMP>31){
        STATE_CONTROL_COOLER_PUMP = 1;
        STATE_CONTROL_FAN_IN_OUT = 1;
      }
      if(AVG_HUMI> 70){
         STATE_CONTROL_FAN_IN_OUT = 1;
      }
      STATE_CONTROL_WATER_PUMP = 1;
      STATE_CONTROL_COOLER_PUMP = 1;

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
//      MegaSerial.begin(115200);
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
      
      // กรณีที่ Lock อยู่  LED สีแดงจะติด  และหากอยู่ระหว่างการใส่รหัส LED สีแดงจะกระพริบ
      if(locked){
        if(passinput){
          unsigned long ledcurrentvar = millis();
          if(ledcurrentvar - ledflashvar > ledflashtime) {
            ledflashvar = ledcurrentvar;
            digitalWrite(lockedled, !digitalRead(lockedled));
          }
        }
        else{
          digitalWrite(lockedled, 255);   // LED สีแดงจะติด
        }
        digitalWrite(unlockedled, 0);     // LED สีเขียวดับ
      }
      }

void SET_LED(){
      pinMode(lockedled, OUTPUT);
      digitalWrite(lockedled, 255);
      pinMode(unlockedled, OUTPUT);
      digitalWrite(unlockedled, 0);
}
void set_relay(){
      digitalWrite(WATER_PUMP,OFF);
      digitalWrite(COOLER_PUMP,OFF);
      digitalWrite(COOLER,ON);
      digitalWrite(FAN_IN_OUT,OFF);
}
void Input(){
     if (Serial.available()){
    char inChar = (char)Serial.read();
    MyinputString += inChar;  // add it to the inputString:
//    Serial.print(MyinputString);
    if (MyinputString[0] == 'H') // check array [0] is H
    {
      if (inChar == 0x0D)  // check received 'enter' (0x0D)
      {

          int Level = ((MyinputString[1] - 48)*10) + (MyinputString[2] - 48); // change Char to Integer

          Serial.print("Humidity: ");
          Serial.println(Level);
          humi = Level;

          MyinputString = "";
      }
    }else if (MyinputString[0] == 'T') // check array [0] is T
    {

        if (inChar == 0x0D)  // check received 'enter' (0x0D)
      {

          int Level = ((MyinputString[1] - 48)*10) + (MyinputString[2] - 48);

          Serial.print("Temperature: ");
          Serial.println(Level);
          temp = Level;

          MyinputString = "";
      }


    }
    else if (MyinputString[0] == 'P') // check array [0] is T
    {

        if (inChar == 0x0D)  // check received 'enter' (0x0D)
      {

          int Level = ((MyinputString[1] - 48)*10) + (MyinputString[2] - 48);

          Serial.print("Hour: ");
          Serial.println(Level);
          Hour = Level;

          MyinputString = "";
      }


    }else if (MyinputString[0] == 'M') // check array [0] is T
    {

        if (inChar == 0x0D)  // check received 'enter' (0x0D)
      {

          int Level = ((MyinputString[1] - 48)*10) + (MyinputString[2] - 48);

          Serial.print("Minutes: ");
          Serial.println(Level);
          Minutes = Level;

          MyinputString = "";
      }


    }else
    {
      MyinputString = "";
    }
    int State =0;
    if((Hour == 23 && Minutes > 1) && (Hour == 23 && Minutes <= 60)){
                            digitalWrite(WATER_PUMP,ON);
        }else{
                    digitalWrite(WATER_PUMP,OFF);
        }
        delay(500);
  }
}
