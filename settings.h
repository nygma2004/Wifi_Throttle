
const char *ssid = "xxxx";                     // Wifi SSID
const char *password = "xxxx";              // Wifi password


//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "192.168.4.1";              // If the LokRemote is in Access Point (AP) mode, this is always 192.168.4.1
const int udpPort = 4444;                               // This has to be the same with what is configured in LokRemote
const int udpListenPort = 4445;                         // This has to be the same with what is configured in LokRemote

/*

ESP 32 pinout

                                                  +-------------------+
                                                  |       |USB|       |
                                              3V3 |o      +---+      o| Vin
                                              GND |o                 o| GND
                   Keypad Row 1               D15 |o                 o| D13             Keypad Col 1
                   OLED_RESET                 D2  |o                 o| D12             Keypad Col 2
                   OLED_DC                    D4  |o                 o| D14             Keypad Col 3
                                              D16 |o                 o| D27             Keypad Col 4
                                              D17 |o                 o| D26             Vibro motor
                   OLED_CS                    D5  |o                 o| D25             Rotary Encoder Button
                   OLED_CLK                   D18 |o                 o| D33             Rotary Encoder B
                   Keypad Row 2               D19 |o                 o| D32             Rotary Encoder A
                   KeyPad Row 3               D21 |o                 o| D35
                                              D3  |o                 o| D34
                                              D1  |o                 o| D39             Direction Switch
                   Keypad Row 4               D22 |o                 o| D36             Power Switch
                   OLED_MOSI                  D23 |o                 o| EN
                                                  |                   |
                                                  +-------------------+













 */
