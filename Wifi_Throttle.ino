#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include "Keypad.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Fonts/FreeSerifBold18pt7b.h>
#include "icons.h"
#include "globals.h"
#include "settings.h"
#include <jled.h>

#include <WiFi.h>
#include "WiFiUdp.h"
WiFiUDP udp;

Adafruit_SSD1351 display = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS, OLED_DC,  OLED_RESET);

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

JLed VibroMotor = JLed(VIBROMOTOR).Off();

void rotary_onButtonClick()
{
  static unsigned long lastTimePressed = 0;
  //ignore multiple press in that time milliseconds
  if (millis() - lastTimePressed < 500)
  {
    return;
  }
  lastTimePressed = millis();
  Serial.print("button pressed at ");
  Serial.println(millis());
}

void rotary_loop()
{
  // ignore rotary changes if the power switch is off
  int encodervalue;
  if (power_sw == 0) {
    encodervalue = rotaryEncoder.readEncoder();
  } else {
    return;
  }

  // do not do anything if the value did not change
  if (!rotaryEncoder.encoderChanged())
  {
    return;
  }
  if (!stopped && (encodervalue==0)) {
    stopped = true;
    VibroMotor = JLed(VIBROMOTOR).Blink(500, 100).Repeat(1);
  }
  if (stopped && (encodervalue>0)) {
    stopped = false;
  }

  // simulate the speed tick
  if (!stopped && (encodervalue>0) && speedtick) {
    VibroMotor = JLed(VIBROMOTOR).Blink(25,20*(255-encodervalue)).Forever();
  }
  Serial.print("Value: ");
  Serial.println(encodervalue);
  UpdateDisplay(OLED_UPDATE_SPEED);

}

void DisplaySplashScreen() {
  for (int x=0; x<128; x++) {
    for (int y=0; y<128; y++) {
      display.drawPixel(x, y, splash[y*128+x]);
    }
  }
}

void DrawBackgroundScreen() {
  for (int x=0; x<128; x++) {
    for (int y=0; y<128; y++) {
      display.drawPixel(x, y, background[y*128+x]);
    }
  }
}

void UpdateDisplay(byte UpdateReason) {

  //Serial.print("Screen update: ");
  //Serial.println(UpdateReason);
  // RSSI update

  display.setFont();
  if (millis()-lastRSSI>500) {
    display.setTextSize(1);
    int rssi = WiFi.RSSI();
    display.drawBitmap(OLED_WIFI_X, OLED_WIFI_Y, icon_blank, 16, 16, OLED_Color_Black);
    if (rssi<-80) {
      display.drawBitmap(OLED_WIFI_X, OLED_WIFI_Y, icon_wifi[0], 16, 16, OLED_Color_Red);
      display.setTextColor(OLED_Color_Red);
    } else {
       if (rssi<-70) {
          display.drawBitmap(OLED_WIFI_X, OLED_WIFI_Y, icon_wifi[1], 16, 16, OLED_Color_Yellow); 
          display.setTextColor(OLED_Color_Yellow);
       } else {
          if (rssi<-60) {
            display.drawBitmap(OLED_WIFI_X, OLED_WIFI_Y, icon_wifi[2], 16, 16, OLED_Color_White); 
            display.setTextColor(OLED_Color_White);
          } else {
            display.drawBitmap(OLED_WIFI_X, OLED_WIFI_Y, icon_wifi[3], 16, 16, OLED_Color_White); 
            display.setTextColor(OLED_Color_White);
          }
       }
    }
    display.fillRect(OLED_RSSI_X,OLED_RSSI_Y, OLED_SMALLFONT_W*3, OLED_SMALLFONT_H, OLED_Color_Black);
    display.setCursor(OLED_RSSI_X,OLED_RSSI_Y);
    display.println(rssi);
    lastRSSI = millis();
  }

  // Speed update
  if (UpdateReason & OLED_UPDATE_SPEED) {
    int encoder = rotaryEncoder.readEncoder();
    display.fillTriangle(OLED_SPEED_X,OLED_SPEED_Y, OLED_SPEED_X+OLED_SPEED_W,OLED_SPEED_Y, OLED_SPEED_X+OLED_SPEED_W,OLED_SPEED_Y-OLED_SPEED_H,OLED_Color_Black);
    display.drawTriangle(OLED_SPEED_X,OLED_SPEED_Y, OLED_SPEED_X+OLED_SPEED_W,OLED_SPEED_Y, OLED_SPEED_X+OLED_SPEED_W,OLED_SPEED_Y-OLED_SPEED_H,OLED_Color_Yellow);
    display.fillTriangle(OLED_SPEED_X,OLED_SPEED_Y, OLED_SPEED_X+(int)(OLED_SPEED_W*encoder/255),OLED_SPEED_Y, OLED_SPEED_X+(int)(OLED_SPEED_W*encoder/255),OLED_SPEED_Y-(int)(OLED_SPEED_H*encoder/255),OLED_Color_Yellow);
    
    display.setTextColor(OLED_Color_White);
    display.setCursor(OLED_ENCODER_VAL_X,OLED_ENCODER_VAL_Y);
    display.fillRect(OLED_ENCODER_VAL_X,OLED_ENCODER_VAL_Y, OLED_SMALLFONT_W*3, OLED_SMALLFONT_H, OLED_Color_Black);
    display.print(rotaryEncoder.readEncoder());
  }

  // Direction
  if (UpdateReason & OLED_UPDATE_DIRECTION) {
    //Serial.println("Direction");
    if (power_sw == 1) {
      // both arrow red
      //Serial.println("Red arrows");
      display.drawBitmap(OLED_LEFTDIR_X, OLED_LEFTDIR_Y, dir_left, 15, 80, OLED_Color_Red);
      display.drawBitmap(OLED_RIGHTDIR_X, OLED_RIGHTDIR_Y, dir_right, 15, 80, OLED_Color_Red);
    } else {
      if (direction_sw == 0) {
        // forward
        //Serial.println("Right green");
        display.drawBitmap(OLED_LEFTDIR_X, OLED_LEFTDIR_Y, dir_left, 15, 80, OLED_Color_Black);
        display.drawBitmap(OLED_RIGHTDIR_X, OLED_RIGHTDIR_Y, dir_right, 15, 80, OLED_Color_Green);
      } else {
        // reverse
        //Serial.println("Left green");
        display.drawBitmap(OLED_LEFTDIR_X, OLED_LEFTDIR_Y, dir_left, 15, 80, OLED_Color_Green);
        display.drawBitmap(OLED_RIGHTDIR_X, OLED_RIGHTDIR_Y, dir_right, 15, 80, OLED_Color_Black);
      }
    }
    
  }

  // Sabertooth info update
  if (UpdateReason & OLED_UPDATE_SABER) {
    if (millis()-lastSaber>500) {

      if (displaymode==0) {
        // All Sabertooth data on the screen
        display.fillRect(OLED_ALL_BAT_X,OLED_ALL_BAT_Y, OLED_SMALLFONT_W*OLED_ALL_BAT_W, OLED_SMALLFONT_H, OLED_Color_Black);
        display.setTextColor(OLED_Color_White);
        display.setCursor(OLED_ALL_BAT_X,OLED_ALL_BAT_Y);
        display.print("B: ");
        display.print(battery);
        display.print("V");
  
        display.fillRect(OLED_ALL_M1_X,OLED_ALL_M1_Y, OLED_SMALLFONT_W*OLED_ALL_M1_W, OLED_SMALLFONT_H, OLED_Color_Black);
        display.setCursor(OLED_ALL_M1_X,OLED_ALL_M1_Y);
        display.print("M1: ");
        display.print(m1current);
        display.print("A, ");
        display.print(m1temp);
        display.print("C");
        
        display.fillRect(OLED_ALL_M2_X,OLED_ALL_M2_Y, OLED_SMALLFONT_W*OLED_ALL_M2_W, OLED_SMALLFONT_H, OLED_Color_Black);
        display.setCursor(OLED_ALL_M2_X,OLED_ALL_M2_Y);
        display.print("M2: ");
        display.print(m2current);
        display.print("A, ");
        display.print(m2temp);
        display.print("C");
      }
      if (displaymode==1) {
        // Battery information
        display.setTextColor(OLED_Color_White);
        display.setCursor(OLED_BATTEXT_X,OLED_BATTEXT_Y);
        display.print("Battery");
        display.setFont(&FreeSerifBold18pt7b);
        display.fillRect(OLED_BAT_X,OLED_BAT_Y, OLED_LARGEFONT_W*OLED_BAT_W, OLED_LARGEFONT_H, OLED_Color_Black);
        display.setTextColor(OLED_Color_White);
        display.setCursor(OLED_BAT_X,OLED_BAT_Y+OLED_LARGEFONT_H);
        display.print(battery);
        display.setFont();
        display.fillRect(OLED_BATMIN_X,OLED_BATMIN_Y, OLED_SMALLFONT_W*OLED_BAT_W, OLED_SMALLFONT_H, OLED_Color_Black);
        display.setCursor(OLED_BATMIN_X,OLED_BATMIN_Y);
        display.print(battmin);
      }
      if (displaymode==2) {
        // Current information
        display.setTextColor(OLED_Color_White);
        display.setCursor(OLED_CURTEXT_X,OLED_CURTEXT_Y);
        display.print("Current");
        display.setFont(&FreeSerifBold18pt7b);
        display.fillRect(OLED_CURM1_X,OLED_CURM1_Y, OLED_LARGEFONT_W*OLED_CUR_W, OLED_LARGEFONT_H, OLED_Color_Black);
        display.setTextColor(OLED_Color_White);
        display.setCursor(OLED_CURM1_X,OLED_CURM1_Y+OLED_LARGEFONT_H);
        display.print(m1current);
        display.fillRect(OLED_CURM2_X,OLED_CURM2_Y, OLED_LARGEFONT_W*OLED_CUR_W, OLED_LARGEFONT_H, OLED_Color_Black);
        display.setCursor(OLED_CURM2_X,OLED_CURM2_Y+OLED_LARGEFONT_H);
        display.print(m2current);
        display.setFont();
      }
      if (displaymode==3) {
        // Temperature information
        display.setTextColor(OLED_Color_White);
        display.setCursor(OLED_TEMPTEXT_X,OLED_TEMPTEXT_Y);
        display.print("Temperature");
        display.setFont(&FreeSerifBold18pt7b);
        display.fillRect(OLED_TEMPM1_X,OLED_TEMPM1_Y, OLED_LARGEFONT_W*OLED_TEMP_W, OLED_LARGEFONT_H, OLED_Color_Black);
        display.setTextColor(OLED_Color_White);
        display.setCursor(OLED_TEMPM1_X,OLED_TEMPM1_Y+OLED_LARGEFONT_H);
        display.print(m1temp);
        display.print("C");
        display.fillRect(OLED_TEMPM2_X,OLED_TEMPM2_Y, OLED_LARGEFONT_W*OLED_TEMP_W, OLED_LARGEFONT_H, OLED_Color_Black);
        display.setCursor(OLED_TEMPM2_X,OLED_TEMPM2_Y+OLED_LARGEFONT_H);
        display.print(m2temp);
        display.print("C");
        display.setFont();
      }
      
      lastSaber = millis();
    }
  }

  // Controller response interval
  if (UpdateReason & OLED_UPDATE_INTERVAL) {
    if (millis()-lastResponseUpdate>500) {

      String text = String(lastResponseInterval);
      if (lastResponseInterval>1000) {
        display.setTextColor(OLED_Color_Red);
        text = "----";
      } else {
        if (lastResponseInterval>500) {
          display.setTextColor(OLED_Color_Yellow);
        } else {
          display.setTextColor(OLED_Color_White);
        }
      }

      display.setCursor(OLED_INTERVAL_X,OLED_INTERVAL_Y);
      display.fillRect(OLED_INTERVAL_X,OLED_INTERVAL_Y, OLED_SMALLFONT_W*4, OLED_SMALLFONT_H, OLED_Color_Black);
      display.print(text);

      lastResponseUpdate = millis();
    }
  }  
  
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("Rotary Encoder test");

    display.begin();
    display.setFont();
    display.fillScreen(OLED_Backround_Color);
    DisplaySplashScreen();
    display.setTextColor(OLED_Text_Color);
    display.setTextSize(1);


  pinMode(SW_POWER, INPUT);
  pinMode(SW_DIRECTION, INPUT);
  pinMode(VIBROMOTOR, OUTPUT);

  //we must initialize rotary encoder
  rotaryEncoder.begin();

  rotaryEncoder.setup(
    [] { rotaryEncoder.readEncoder_ISR(); },
    [] { rotary_onButtonClick(); });

  //set boundaries and if values should cycle or not
  //in this example we will set possible values between 0 and 1000;
  bool circleValues = false;
  rotaryEncoder.setBoundaries(0, 255, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

  /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
  //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
  rotaryEncoder.setAcceleration(100); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration

    Serial.print("Connecting to wifi network");
  
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal [RSSI]: ");
    Serial.println(WiFi.RSSI());

  if (udp.begin(4445)) {
    Serial.print("UDP running at IP: ");
    Serial.print(WiFi.localIP());
    Serial.println(" port: 4445");
  }
  VibroMotor = JLed(VIBROMOTOR).Blink(50, 50).Repeat(3);

  DrawBackgroundScreen();
  UpdateDisplay(OLED_UPDATE_ALL);
}

void loop()
{
  //in loop call your custom function which will process rotary encoder values
  rotary_loop();

  VibroMotor.Update();

  if (millis() - lastWifiCheck >= 250) {
    // reconnect to the wifi network if connection is lost
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Reconnecting to wifi...");
      WiFi.reconnect();
    }
    lastWifiCheck = millis();
  }

  char key = keypad.getKey();

  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    switch (key) {
      case '1':
        lightState = !lightState;
        Serial.print("Light 1: ");
        Serial.println(lightState);
        break;
      case '2':
        light3State = !light3State;
        break;
      case '3':
        light4State = !light4State;
        break;
      case '4':
        soundid = 1;
        lastSound = millis();
        break;
      case '5':
        soundid = 2;
        lastSound = millis();
        break;
      case '6':
        soundid = 3;
        lastSound = millis();
        break;
      case '7':
        soundid = 4;
        lastSound = millis();
        break;
      case '8':
        soundid = 5;
        lastSound = millis();
        break;
      case '9':
        soundid = 6;
        lastSound = millis();
        break;
      case 'A':
        displaymode = 0;
        display.fillRect(OLED_CLEAR_X,OLED_CLEAR_Y, OLED_CLEAR_W, OLED_CLEAR_H, OLED_Color_Black);
        break;
      case 'B':
        displaymode = 1;
        display.fillRect(OLED_CLEAR_X,OLED_CLEAR_Y, OLED_CLEAR_W, OLED_CLEAR_H, OLED_Color_Black);
        break;
      case 'C':
        displaymode = 2;
        display.fillRect(OLED_CLEAR_X,OLED_CLEAR_Y, OLED_CLEAR_W, OLED_CLEAR_H, OLED_Color_Black);
        break;
      case 'D':
        displaymode = 3;
        display.fillRect(OLED_CLEAR_X,OLED_CLEAR_Y, OLED_CLEAR_W, OLED_CLEAR_H, OLED_Color_Black);
        break;
      case '#':
        speedtick = !speedtick;
        if (!speedtick) VibroMotor = JLed(VIBROMOTOR).Off();
        break;

    }
  } else {
    if (millis()-lastSound > 500) {
      soundid = 0;
    }
  }

  if (millis()-lastUDP > 250) {
    int direction_temp = digitalRead(SW_DIRECTION);
    if (direction_temp!=direction_sw) {
      direction_sw = direction_temp;
      UpdateDisplay(OLED_UPDATE_DIRECTION);
    }
    int power_temp = digitalRead(SW_POWER);
    if (power_temp!=power_sw) {
      power_sw = power_temp;
      UpdateDisplay(OLED_UPDATE_DIRECTION);
    }
    ThrottleBuffer[0] = direction_sw ? 1 : 0;
    if (power_sw == 0) {
      ThrottleBuffer[1] = rotaryEncoder.readEncoder();
    } else {
      ThrottleBuffer[1] = 0;
      rotaryEncoder.setEncoderValue(0);
      if (!stopped) {
        VibroMotor = JLed(VIBROMOTOR).Blink(500, 100).Repeat(1);
        stopped = true;
      }
      UpdateDisplay(OLED_UPDATE_SPEED + OLED_UPDATE_DIRECTION);
    }
    ThrottleBuffer[2] = 0;
    ThrottleBuffer[2] += lightState << 7;
    ThrottleBuffer[2] += light3State << 6;
    ThrottleBuffer[2] += light4State << 5;
    ThrottleBuffer[3] = soundid;
    ThrottleBuffer[4] = 0;
    udp.beginPacket(udpAddress,udpPort);
    udp.write(ThrottleBuffer, 5);
    udp.endPacket();
    //Serial.print("UDP packet sent : ");
    //Serial.print(ThrottleBuffer[0]);
    //Serial.print(", ");
    //Serial.print(ThrottleBuffer[1]);
    //Serial.print(", ");
    //Serial.print(ThrottleBuffer[2]);
    //Serial.print(", ");
    //Serial.print(ThrottleBuffer[3]);
    //Serial.print(", ");
    //Serial.print(ThrottleBuffer[4]);
    //Serial.println(" <");

    lastUDP=millis();
  }


  int packetSize = udp.parsePacket();
  if (packetSize) {
    //Serial.print("Received packet of size ");
    //Serial.println(packetSize);
    //Serial.print("From ");
    //IPAddress remoteIp = udp.remoteIP();
    //Serial.print(remoteIp);
    //Serial.print(", port ");
    //Serial.println(udp.remotePort());
    // read the packet into packetBufffer
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    //Serial.println("Contents:");
    //Serial.println(packetBuffer);
    battery = ((float)packetBuffer[0]*256+packetBuffer[1])/10;
    if (battery<battmin) {
      battmin = battery;
    }
    m1current = ((float)packetBuffer[2]*256+packetBuffer[3])/10;
    m1temp = packetBuffer[4];
    m2current = ((float)packetBuffer[5]*256+packetBuffer[6])/10;
    m2temp = packetBuffer[7];
    lastResponseInterval=millis()-lastResponse;
    lastResponse=millis();
    UpdateDisplay(OLED_UPDATE_SABER+OLED_UPDATE_INTERVAL);
  }

  if (millis()-lastResponse > 500) {
    lastResponseInterval=millis()-lastResponse;  
    UpdateDisplay(OLED_UPDATE_INTERVAL);
  }


}
