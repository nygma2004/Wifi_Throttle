
#define OLED_WIFI_X 0
#define OLED_WIFI_Y 0

#define OLED_RSSI_X 18
#define OLED_RSSI_Y 5

#define OLED_ENCODER_VAL_X 25
#define OLED_ENCODER_VAL_Y 110

#define OLED_SPEED_X 10
#define OLED_SPEED_Y 126
#define OLED_SPEED_W 108
#define OLED_SPEED_H 25

#define OLED_LEFTDIR_X 0
#define OLED_LEFTDIR_Y 20
#define OLED_RIGHTDIR_X 113
#define OLED_RIGHTDIR_Y 20

#define OLED_UPDATE_ALL 0xFF
#define OLED_UPDATE_SPEED     0b00000001
#define OLED_UPDATE_SABER     0b00000010
#define OLED_UPDATE_DIRECTION 0b00000100
#define OLED_UPDATE_INTERVAL  0b00001000

#define OLED_SMALLFONT_W 6
#define OLED_SMALLFONT_H 8
#define OLED_LARGEFONT_W 22
#define OLED_LARGEFONT_H 25

#define OLED_INTERVAL_X 45
#define OLED_INTERVAL_Y 5

#define OLED_ALL_BAT_X 20
#define OLED_ALL_BAT_Y 40
#define OLED_ALL_BAT_W 9

#define OLED_CURTEXT_X 40
#define OLED_CURTEXT_Y 28
#define OLED_CURM1_X 20
#define OLED_CURM1_Y 38
#define OLED_CUR_W 4
#define OLED_CURM2_X 20
#define OLED_CURM2_Y 70

#define OLED_TEMPTEXT_X 30
#define OLED_TEMPTEXT_Y 28
#define OLED_TEMPM1_X 20
#define OLED_TEMPM1_Y 38
#define OLED_TEMP_W 4
#define OLED_TEMPM2_X 20
#define OLED_TEMPM2_Y 70

#define OLED_BATTEXT_X 40
#define OLED_BATTEXT_Y 35
#define OLED_BAT_X 20
#define OLED_BAT_Y 45
#define OLED_BAT_W 3
#define OLED_BATMIN_X 45
#define OLED_BATMIN_Y 85

#define OLED_ALL_M1_X 20
#define OLED_ALL_M1_Y 55
#define OLED_ALL_M1_W 14

#define OLED_ALL_M2_X 20
#define OLED_ALL_M2_Y 70
#define OLED_ALL_M2_W 14

#define OLED_CLEAR_X 20
#define OLED_CLEAR_Y 25
#define OLED_CLEAR_W 92
#define OLED_CLEAR_H 78

#define SW_POWER 36
#define SW_DIRECTION 39
#define VIBROMOTOR 26

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 
#define OLED_MOSI  23  // SDA
#define OLED_CLK   18  // SCL
#define OLED_DC    4
#define OLED_CS    5
#define OLED_RESET 2

// SSD1331 color definitions
const uint16_t  OLED_Color_Black        = 0x0000;
const uint16_t  OLED_Color_Blue         = 0x001F;
const uint16_t  OLED_Color_Red          = 0xF800;
const uint16_t  OLED_Color_Green        = 0x07E0;
const uint16_t  OLED_Color_Cyan         = 0x07FF;
const uint16_t  OLED_Color_Magenta      = 0xF81F;
const uint16_t  OLED_Color_Yellow       = 0xFFE0;
const uint16_t  OLED_Color_White        = 0xFFFF;

// The colors we actually want to use
uint16_t        OLED_Text_Color         = OLED_Color_Black;
uint16_t        OLED_Backround_Color    = OLED_Color_Blue;


const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte colPins[ROWS] = {15, 19, 21, 22}; 
byte rowPins[COLS] = {13, 12, 14, 27};

/*
connecting Rotary encoder
Rotary encoder side    MICROCONTROLLER side  
-------------------    ---------------------------------------------------------------------
CLK (A pin)            any microcontroler intput pin with interrupt -> in this example pin 32
DT (B pin)             any microcontroler intput pin with interrupt -> in this example pin 21
SW (button pin)        any microcontroler intput pin with interrupt -> in this example pin 25
GND - to microcontroler GND
VCC                    microcontroler VCC (then set ROTARY_ENCODER_VCC_PIN -1) 
***OR in case VCC pin is not free you can cheat and connect:***
VCC                    any microcontroler output pin - but set also ROTARY_ENCODER_VCC_PIN 25 
                        in this example pin 25
*/
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 33
#define ROTARY_ENCODER_BUTTON_PIN 25
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4

unsigned int lastUDP, lastSound;
char packetBuffer[255]; //buffer to hold incoming packet
byte ThrottleBuffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00 };


int motorspeed, m1temp, m2temp, soundid;
bool lightState, light3State, light4State, speedtick;
float battery, m1current, m2current;
int displaymode = 0;
float battmin = 1000.0;

unsigned int lastRSSI, lastSaber, lastResponse, lastResponseInterval, lastResponseUpdate, lastWifiCheck;
int direction_sw, power_sw;
bool stopped;
