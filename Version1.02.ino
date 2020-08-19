/*
 *** RFID controlled door management with single use restriction function - Written by Wallace Brown August 2020. ***
 
In this project you will need the following components:

                    ==========================================================================
                    NodeMCU module (https://www.banggood.com/custlink/DDmECtvrIk) 
                    RC522-RFID 13.56MHz reader (https://www.banggood.com/custlink/mG3YC5mcBc)
                    3 volt relay (https://www.banggood.com/custlink/D3mYrVmCgH)
                    ILI9341 compatible TFT (https://www.banggood.com/custlink/vmmypFmJ63)
                    Electronic dooor latch (https://www.banggood.com/custlink/DmGRpVmWeO) 
                    12 VDC power supply (https://www.banggood.com/custlink/3DKyp5GJ4q)
                    DC-DC buck converter (https://www.banggood.com/custlink/3v3YptGpbs) 
                    Reed Switch Door Sensor(https://www.banggood.com/custlink/m3DdWHGF2s)
                    ===========================================================================

Version 1.02 is a stand-alone RFID electric door latch controller (RFID Module) only. ** Later version will include increased security by adding serial communications, bluetooth and MQTT sub/pub libraries. **
The RFID MODULE contains an ESP8266, a RC522 RFID card reader, an ILI9341 compatible TFT display and a relay to trigger the door latch. It accepts 12VDC input power and the circuit can contain either a buck 
converter to step the voltage down to 3.3 volts for the electronics or a linear regulator. In some cases, you may want to put more than one linear regulator to step down the voltage in stages to reduce the heat 
for each regulator as most of the stepped down voltages can be converted to heat.

The ESP8266 offers a simulated EEPROM by using a dedicated 4MB flash RAM that can be writted / re-writted up to 10,000 times. The ESP8266 and flash memory should last indefinately if you are not regualarly adding or removing cards.

Future versions may include adding an SD card to replace the EEPROM writes or adding a cheap external EEPROM to increase the lifecycle of the ESP8266 that contains all the code.

Features found in the program (RFID MODULE) include:

Initial Setup mode:

When first powered on, the system will read the EEPROM and look for a value stored in the last memory location to see if it has been configured. If the value in memory location 4095 is not 42, the system will enter an initial configuration 
mode and the first card/fob tapped will become the new MASTER CARD.

Master Card:

This is the first card programmed into the system. The serial number of the card is stored in EEPROM memory location 0001-0004. When this card is used, the system will go into Program mode to allow the administrator to add additional cards.

Program mode:

After accepting the Master Card, the administrator is prompted to tap a new card. They system reads the number of cards stored from EEPROM memory location 0000 and will be assigned to a variable used to tell the system how many 4 byte memory 
locations to scan for the new card. If the card is already in the system, it will be removed and all subsiquent cards will be shifted down one 4 byte location. If the card is not in the system, it will be added after the last known card and 
EEPROM memory location 0000 will be incremented by 1.

Restricted mode:

As many buildings are having to introduce social distancing restrictions, this program has encorporated a restrited mode to allow for single person use for a room. The program reads the built in EEPROM at memory locaiton 4094 and stores that 
value as the number of minutes to restrict access to the room. If the memory locations contain 00, restricted mode is disabled. When enabled, the user taps in and the RFID MODULE sends a signal to the DOOR RELAY MODULE to open the door. It then 
awaits a reply for 30 seconds to know that door has been opened and its okay to start the timer and restrict access to the room. If the door is opened again the RFID MODULE assumes the user has left the room and sets the counter back to zero to 
allow someone else to use the room.

While in "restriced status" only the card that was last tapped in can access the room. This was added to avoid a possibiliy of the user accidentally opening and closing the door before entering and being locked out for the duration of the 
restriction timer.

EEPROM Memory Map:

  0000 - 0001: Number of cards stored in system 
  0002 - 0005: 4 byte location for Master Card ID
  0006 - 4006: storage location of user cards ( each card uses 4 bytes)
  4007 - 4093: Reserved
  4094 - Flag for restricted mode (00 = disabled otherwise DEC amount for number of minute to restrict access after user taps in
  4095 - Flag for unit setup (If = 42, the answer to the ultimate question of life, the universe and everything, the system is configured!)

Note: ** You may need to modify the toptext() function to center your text after chaning the company and room name variables below **

Wiring guide for this project:

      ILI9341 TFT with touch and SD slot               NodeMCU V3          RFID-RC522 (RFID reader)              
    +--------------------------------------+          +---+--+---+         +-----------------------+               
    | +---------------------------+  T_IRQ |          |3.3    Vin|         |                       |            
    | |  ILI9341   (ESP8266)      |  T_DO  |          |GND    GND|         |  RC522     (ESP8266)  |           
    | |  =======   =========      |  T_DIN |          |TX     RST|         |  -------   ---------  |                                                      
    | |  VCC     -   (3.3)        |  T_CS  |          |RX      EN|         |   3.3V   -   (3.3)    |             
    | |  GND     -   (GND)        |  T_CLK |          |D8     3.3|         |   RST    -   (SD2)    |             
    | |  CS      -   (D0)         |  MISO  |          |D7     GND|         |   GND    -   (GND)    |             
    | |  RESET   -   (D4)         |  LED   |          |D6     CLK|         |   IRQ    -            |      
    | |  DC      -   (D2)         |  SCK   |          |D5     SD0|         |   MISO   -    (D6)    |            
    | |  MOSI    -   (D7)         |  MOSI  |          |GND    CMD|         |   MOSI   -    (D7)    |      
    | |  SCK     -   (D5)         |  DC    |          |3.3    SD1|         |   SCK    -    (D5)    |      
    | |  LED     -   (3.3)        |  RESET |          |D4     SD2|         |   SDA    -   (SD3)    |      
    | | *no connection for MISO   |  CS    |          |D3     SD3|         |                       |      
    | |  Touch or SDSlot for this |  GND   |          |D2     RSV|         |    *|*|M|M|*|*|*|3|   |      
    | |  version.                 |  VCC   |          |D1     RSV|         |    S|S|O|I|I|G|R|.|   |      
    | +---------------------------+        |          |D0     A0 |         |    D|C|S|S|R|N|S|3|   |      
    +--------------------------------------+          +----------+         |    A|L|I|O|Q|D|T|V|   |      
                                                                           +-----------------------+      

 *** IMPORTANT *** - The ESP8266 and RC522 boards are NOT 5V tolerant. When connecting the DC to DC down converter, ALWAYS check the output voltage before connecting to the circuit or you may
                     damage your equipment.  As the DC to DC converter is adjustable, you might also want to add a 3.3 Volt Zener diode circuit accross the output of the converter to ensure the voltage 
                     never accidently goes above the maximum 3.3 Volts.  
                     
                     Although the ILI9341 is 5V tolerant but is still rated to run at 3.3 V so it's best if you step the 12 VDC down to 3.3 for everything and use a 3 VDC relay instead of the traditional
                     5VDC relays commonly found in msny ARDUINO development kits and electronic hobby shops.
 
 */ 
                                                                        
#define CompanyName "Sample Company"                                     // Change this to your company name (16 char max) 
#define RoomName "Recreation Room"                                      // Change this to the room name (20 char max)
                                                                                                                                                
#include "Adafruit_GFX.h"                                               // Load the graphics library (version 1.9.0 was used to build this code) 
#include "Adafruit_ILI9341.h"                                           // Load the ILI9341 library (version 1.5.6 was used to build this code)
#include "Fonts/FreeSans9pt7b.h"                                        // Load FreeSans 9pt font library (included in the Adafruit_GFX library)
#include "Fonts/FreeSansBold18pt7b.h"                                   // Load FreeSansBold 18pt font library (included in the Adafruit_GFX library)

#include <SPI.h>                                                        // Load SPI bus for card reader
#include <MFRC522.h>                                                    // Load RFID reader library
#include <EEPROM.h>                                                     // Load EEProm Library

#define TFT_RST   2                                                     // Define reset pin for the ILI9341 compatible TFT screen
#define TFT_DC    4                                                     // Define data pin for the ILI9341 compatible TFT screen
#define TFT_CS    16                                                    // Define the chip select pin for the ILI9341 compatible TFT screen

#define RST_PIN   9                                                     // Define reset pin for the RC522 RFID card reader
#define SS_PIN    10                                                    // Define data pin for the RC522 RDIF card reader

#define RELAY     0                                                     // Define for LED/Relay option
#define Sensor    5                                                     // Door Sensor pin (optional)
#define AlarmPin  15

bool configured  = false;                                               // Set configureation flag to default false
bool programMode = false;                                               // Initialize programming mode to false
bool restricted = false;                                                // Set default restriction mode (will be sent to EEPROM on setup)
bool debug = false;                                                     // Set debug mode to default to false(change to enable serial communications)
bool doorOpened = false;                                                // Set doorOpened flag to false

uint8_t successRead;                                                    // Variable integer to keep if we have Successful Read from Reader
byte storedCard[4];                                                     // Stores an ID read from EEPROM
byte readCard[4];                                                       // Stores scanned ID read from RFID Module
byte masterCard[4];                                                     // Stores m/aster card's ID read from EEPROM

int doorcode = 1;                                                       // Default code for serial communication to external module for door sensor (set to 0 - 255 to match external module)
int timeleft = 0;                                                       // Set timeleft on restriction counter (if in restrited mode)
int starttimer = 0;                                                     // Set strart timer default for restricted mode
int stoptimer = 0;                                                      // Set stop timer default for restricted mode
int t = 0;                                                              // Set variable t to 0 (used for xtimer function)
int x = t;                                                              // Set variable x to equal t (used for xtimer function)
int doorOpenTimer = 0;                                                  // Set default for door open time (used for doorOpentoLong function)
int doorOpenSeconds = 60;                                               // Set the number of seconds the door can be open before sounding alarm (for open too long state while in restricted mode)
int alarmTime = 15;                                                     // number of seconds to sound alarm before clearing (while in alarm state)

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);                // Set ILI9341 library parameters
MFRC522 mfrc522(SS_PIN, RST_PIN);                                       // Set MFCR522 library parameters

void setup() {                                                          // ** Begin setup() function **
  
  start_tft();                                                          // enable ILI9341 tft screen
  pinMode(RELAY,OUTPUT);                                                // set relay pin as output
  digitalWrite(RELAY,LOW);                                              // bring relay pin low by default
  pinMode(Sensor, INPUT_PULLUP);                                        // set sensor pin to default to high, reed switch will bring low when door closed
  pinMode(AlarmPin, OUTPUT);
  EEPROM.begin(4096);                                                   // enable EEPROM with 4096 byte buffer
  Serial.begin(9600);                                                   // enable serial port at 9600 baud
  SPI.begin();                                                          // enable SPI bus

  mfrc522.PCD_Init();  delay(5);                                        // initialize MFRC522 card reader and wait 5 ms for it to initialize
  if (EEPROM.read(4094) > 0) {restricted = true;}                       // check EEProm for for number of minutes to restrict access 0 = disabled
  if (EEPROM.read(4095) != 42) {                                        // go into initial setup mode if EEProm memory location 4096 is not 42
        tft.setCursor(0,130); tft.print(F("Inital Setup mode..."));     // print Initial Setup mode on LCD
        tft.setCursor(0,150); tft.print("Scan a new master card");      // prompt user to scan a card to be definded as Master Card

    do {successRead = getID(); }                                        // set successRead to 1 when we get read from reader otherwise 0
    while (!successRead);                                               // program will not go further while you not get a successful read

    for ( uint8_t j = 0; j < 4; j++ ) {                                 // loop 4 times to get each byte from card
      EEPROM.write( 2 + j, readCard[j] );  }                            // write scanned PICC's UID to EEPROM, start from address 2
      EEPROM.write(0000, 01);                                           // write a value of 1 card stored to EEPROM after scanning in the new master card
      EEPROM.write(4095, 42);                                           // write to EEPROM we defined Master Card.
      EEPROM.commit();                                                  // ESP8266 requires a commit to write to EEPROM
      clearscreen();                                                    // clear work area on screen    
      tft.setCursor(0,150);                                             // put cursor at position 0px, 150 px  (screen center)
      tft.print("New Master Card set");                                 // tell user master card has been defined
      delay(3000);                                                      // wait 3 seconds                                                    
  }                                                                     // * end if EEPROM.read(4095) *

 for (uint8_t i = 0; i < 4; i++) {masterCard[i] = EEPROM.read(2 + i);}  // read Master Card's UID from EEPROM and write it to masterCard string array
                                  
 if(debug == true) {Serial.print("System Initialized:  ");}             // announce device started for debugging purposes (if in debug mode)   
 mainscreen();                                                          // refresh screen and display main menu
 
 }                                                                      // ** End setup() routine **

void loop () {                                                          // ** Begin loop() function **
  
  do {successRead = getID();                                            // set successRead to 1 when we get read from reader otherwise 0 
      if(restricted == true) {                                          // check if in restricted mode
       if(digitalRead(Sensor) == HIGH) {Alarm();}                       // go into alarm mode if door opens outside of timer event when in restricted mode
      }                                                                 // * end if int restricted mode check *
  }                                                                     // * end do getID() commands in loop *
  while (!successRead);                                                 // the program will not go further while you are not getting a successful read
  
  if (programMode) { if ( isMaster(readCard) ) {                        // when in program mode check first if Master card scanned again to exit program mode
          
          clearscreen();                                                // clear work area on screen 
          tft.setCursor(30,170);                                        // move to position 30px, 170px 
          tft.setTextColor(ILI9341_GREEN);                              // set text color to Green
          tft.setFont(&FreeSans9pt7b);                                  // use FreeSans 9pt font
          tft.print("Master Card Detected");                            // tell user master card was detected
          clearmsg();                                                   // clear bottom line of text on screen
          tft.setTextColor(ILI9341_WHITE);                              // set text color to White
          tft.setFont(&FreeSans9pt7b);                                  // use FreeSans 9pt font
          tft.setCursor(0,230);                                         // move to bottom text line on screen
          tft.print("Exiting...");                                      // tell user program is returning to run mode
          programMode = false;                                          // set programmode flag to false
          delay(3000);                                                  // wait for 3 seconds
          mainscreen();                                                 // refresh screen and display main menu
  }                                                                     // * end  isMaster(readCard) condition *
    
  else {                                                                // * Start Program moded else contition *
      if ( findID(readCard) ) {                                         // if scanned card is known delete it      
        deleteID(readCard);                                             // execute delete card from EEProm and
        programscreen();                                                // display program mode screen and     
      }                                                                 // end if condition
       
      else {                                                            // if scanned card is not known add it
        writeID(readCard);                                              // write new card id to EEProm and 
        programscreen();                                                // display program mode screen and 
            
      }                                                                 // * end writeID(readCard) else condition *
    }                                                                   // * end findID(readCard) else if condition *               
  }                                                                     // * end if ProgramMode condition *
  
  else {                                                                // if not ProgramMode do the following    
    if ( isMaster(readCard)) {                                          // check scanned card's ID matches Master Card's ID 
      programMode = true;                                               // set program mode flag to true and
      programscreen();                                                  // display program mode screen      
      }                                                                 // * end checking for Master Card *    
    else {
      if ( findID(readCard) ) {                                         // if not, see if the card is in the EEPROM
        if(timeleft == 0) {                                             // check to see if timer is active in restricted mode
           doorUnlocked();                                              // if not call function to open/unlock the door
           mainscreen();                                                // refresh screen and display main menu
           timeleft = 0;                                                // clear the timer to ensure no time left
        }                                                               // * end if timeleft == 0 if statement *
      }                                                                 // * end findID(readCard) if statement *
      
      else {                                                            // if card not found 
        clearscreen();                                                  // clear work area on screen
        denied();                                                       // call denied function to tell user access is denied.
     }                                                                  // * end denied() else condition *
     
   }                                                                    // * end findID(readCard) else condition *
 }                                                                      // * end isMaster(readCard) else condition *
 
}                                                                       // ** end loop() function **

void granted () {                                                       // ** Start granted function **
  
  if(debug == true) {                                                   // do the following if in debug mode:                                                                        
    Serial.print("OD");                                                 // - send open door command over serial to unlock the door
    Serial.print(":");                                                  // - send colon to identify the rest is for tracking and not a command
    Serial.print(SERIAL);                                               // - send serial number of this unit
    Serial.print("-");                                                  // - send dash to separate serial number from card id
    for(int i=0; i < 4; i++){Serial.print(readCard[i],HEX);}            // - send card id for tracking purposes
  }                                                                     // * end if debug statement *
  
  int doorcheck = millis() + 30000;                                   // set serialcheck variable to current runtime plus 30 seconds 
  digitalWrite(RELAY,HIGH);                                             // bring the LED/relay pin high
  
  do{                                                                   // loop for to look for door open response from sensor
 
    if(digitalRead(Sensor) == HIGH) {
     if(debug == true) {Serial.println("Door Open detected");}          // send door open detected if in debug mode
      doorcheck = millis();                                           // set timer to timeout
      doorOpened = true;                                                // set doorOpened flag to true
      delay(100);                                                       // wait 100 ms
     }                                                                  // * end if Sensor == HIGH statement  *      
 }                                                                      // * end do while serialcheck statement *
  while (doorcheck > millis());                                       // above do runs while serialcheck variable is greater than current run time
  digitalWrite(RELAY,LOW);                                              // bring the LED/relay pin low
  if(doorOpened == true) {                                              // check if doorOpened flag was set to true  
      int y = EEPROM.read(4094);                                        // get number of minutes to restrict the room from EEPROM and assign int value to y
        if(restricted == true) {                                        // check to see if restricted mode is enabled and set timer if true
           starttimer = millis() ;                                      // to set timer, get current running time of processor
           stoptimer = (millis() + (y*60000));                          // define stop time by adding the value stored in EEPROM(4094) and multiply by 60,000 to get minutes
           roomInUse();                                                 // call roomInUse() function to tell the user the room is now in use and reserved for # of minutes
           digitalWrite(RELAY,LOW);                                     // set LED/relay pin to low again to release the door latch
           xtimer();                                                    // call xtimer function 
        }                                                               // * end if restricted statement *
  }                                                                     // * end if doorOpened statement *
        
}                                                                       // ** end granted() function **

void denied() {                                                         // ** Start denied function **
  
  tft.setCursor(40,140);                                                // move to position 40px, 140px on tft screen
  tft.setTextColor(ILI9341_BLACK);                                      // set text color to black
  tft.fillRect(0,67,320,132,ILI9341_RED);                               // fill work area red
  tft.setFont(&FreeSansBold18pt7b);                                     // set fon to FreeSansBold 18pt
  tft.print("Not Authorized");                                          // tell user card is not authorized
  if (debug == true){                                                   // do the following if in debug mode:
    Serial.print("NA");                                                 //  - send NA over serial bus for tracking
    Serial.print(":");                                                  //  - put a colon as separator
    for(int i=0; i < 4; i++){Serial.print(readCard[i],HEX);}            //  - write card id read 
    Serial.println();                                                   //  - send carriage return to serial port  
  }                                                                     // * end if debug() condition *
  
  clearmsg();                                                           // clear bottom line of text on screen
  tft.setTextColor(ILI9341_RED);                                        // set text color to red
  tft.setFont(&FreeSans9pt7b);                                          // set font to FreeSans 9pt
  tft.setCursor(30,230);                                                // move to bottom text line on screen
  tft.print("Please contact Strata for access");                        // send message to user
  delay(5000);                                                          // wait 5 seconds
  mainscreen();                                                         // return to main screen
  
}                                                                       // ** end denied() function **  

void xtimer(){                                                          // ** Start xtime function **
  
  doorOpenTimer = millis();                                             // start door open timer
  
do {                                                                    // do the following
   doorOpentoLong();                                                    // and call doorOpentoLong() function
   delay(100);      }                                                   // wait 100ms before looping
while(digitalRead(Sensor) == HIGH);                                     // while the door is opened
   doorOpened = false;                                                  // set doorOpened flag to false after detecting door shut
   
do {                                                                    // do the following while stoptimer is greater than starttimer
    timeleft = (stoptimer - millis());                                  // calculate time remaining
    x = t;                                                              // set x to equal t before setting t to = minutes left
    t = (timeleft/60000)+1;                                             // set t to equal minutes remaining
    if (x != t) {clearmsg();}                                           // if x is not equal to t, clear the bottom text line
    tft.setTextColor(ILI9341_WHITE);                                    // set text color to white
    tft.setFont(&FreeSans9pt7b);                                        // set font to FreeSans 9pt
    tft.setCursor(0,230);                                               // got to bottom text line
    tft.print("Room reserved for ");                                    // write message on bottom text line
    tft.print(t);                                                       // write number of minutes remaing on timer
    if(t > 1) {tft.print(" Minutes");}                                  // write minutes if time left is greater that 1
    else { tft.print(" Minute"); }                                      // else write minute if time left is 1 or less  
    delay(100);                                                         // delay to not overload the processor
    
    if(digitalRead(Sensor) == HIGH) {                                   // check if Sensor pin was brought low
       if(debug == true) {Serial.print("Door Open");}                   // send Door Open to serial port when in debug mode
         do{doorOpentoLong(); delay(150);}                              // run the doorOpentoLong timer and delay for 150ms to avoid crashing CPU
         while(digitalRead(Sensor) == HIGH);                            // while the door is open
         stoptimer = 0;                                                 // clear stoptimer value
    }                                                                   // * end if Sensor is low statement *   
  }                                                                     // * end do loop *
while (stoptimer > starttimer);                                         // do this while there is still time remaining on timer

timeleft = 0;                                                           // ensure time left is now set to zero

}                                                                       // ** end xtimer() function **

uint8_t getID() {                                                       // ** Start getID from Card function **
                                                        
  if ( ! mfrc522.PICC_IsNewCardPresent()) {                             // If a new PICC placed to RFID reader continue
    delay(30);                                                          // wait 30 ms 
    return 0;                                                           // return back with code 0
  }                                                                     // * end if PICC_IsNewCardPresent() *
  
  if ( ! mfrc522.PICC_ReadCardSerial()) {                               // Since a PICC placed get Serial and continue
    return 0;                                                           // return back with code 0  
  }                                                                     // * end if PICC_ReadCardSerial() *  

  for ( uint8_t i = 0; i < 4; i++) {                                    // loop 4 times to get the 4 Bytes for the serial number of card
    readCard[i] = mfrc522.uid.uidByte[i];                               // get byte from card
  }                                                                     // * end for loop *

  mfrc522.PICC_HaltA();                                                 // Stop reading
  return 1;                                                             // return back with code 1
}                                                                       // ** end GetID() function **

void readID( uint8_t number ) {                                         // ** Start readID from EEPROM function **
  
  uint8_t start = (number * 4 )+2 ;                                     // figure out starting position
  for ( uint8_t i = 0; i < 4; i++ ) {                                   // loop 4 times to get the 4 Bytes of the serial number for each card
    storedCard[i] = EEPROM.read(start + i);                             // assign values read from EEPROM to storedCard array
  }                                                                     // * end for loop *

}                                                                       // ** end readID function **

void writeID( byte a[] ) {                                              // ** Start writeID to EEPROM function **
  
  if ( !findID( a ) ) {                                                 // check to see if we have not seen this card before
    uint8_t num = EEPROM.read(0);                                       // get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = ( num * 4 ) + 2;                                    // figure out where the next slot starts
    num++;                                                              // increment the counter by one
    EEPROM.write( 0, num );                                             // write the new count to the counter
    EEPROM.commit();                                                    // commit the changes to EEPROM for ESP8266
    for ( uint8_t j = 0; j < 4; j++ ) {                                 // loop 4 times to get the 4 Bytes
      EEPROM.write( start + j, a[j] );                                  // write the array values to EEPROM in the correct position
    }                                                                   // * end for loop *    
    EEPROM.commit();                                                    // commit the changes to EEPROM for ESP8266
    successWrite();                                                     // call successWrite() function
  }                                                                     // ** end if not findID(a) function **
  else {                                                                // else (if not successful)  
    failedWrite();                                                      // call failedWrite() function
  }                                                                     // * end else statement *
  
}                                                                       // ** end writeID( byte a[]) function

void deleteID( byte a[] ) {                                             // ** Start deleteID function **
  
  if ( !findID( a ) ) {                                                 // check to see if we have this card!
    failedWrite();                                                      // call failedWrite() functionif not found on EEPROM
    tft.setCursor(0,180);                                               // return to home position on lcd
    tft.print("READ FAILURE!");                                         // tell the user we couldn't read the card
  }                                                                     // * end not findID(a) function *
  else                                                                  // else (if found)
  {                                                                     // do the following...                                                                     
    uint8_t num = EEPROM.read(0);                                       // get the number of used spaces, position 0 stores the number of ID cards
    uint8_t slot;                                                       // figure out the slot number of the card
    uint8_t start;                                                      // figure out where the next slot starts
    uint8_t looping;                                                    // the number of times the loop repeats
    uint8_t j;                                                          // define j as an 8 bit integer for loop
    uint8_t count = EEPROM.read(0);                                     // read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );                                             // figure out the slot number of the card to delete
    start = (slot * 4) + 2;                                             // set the start poistion
    looping = ((num - slot) * 4);                                       // define the number of slots to move
    num--;                                                              // decrement the counter by one
    EEPROM.write( 0, num );                                             // write the new count to the counter
    EEPROM.commit();                                                    // commit the change to the EEPROM for ESP8266
    for ( j = 0; j < looping; j++ ) {                                   // loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));}            // shift the array values to 4 places earlier in the EEPROM
      EEPROM.commit();                                                  // commit the change to the EEPROM for ESP8266  
    for ( uint8_t k = 0; k < 4; k++ ) {                                 // shifting loop
      EEPROM.write( start + j + k, 0); }                                // write to the EEPROM at location  
      EEPROM.commit();                                                  // commit to change to the EEPROM for ESP8266
    successDelete();                                                    // call successDelete() function
  }                                                                     // * end else (if found) loop *
  
}                                                                       // ** end deleteID() function **


bool checkTwo ( byte a[], byte b[] ) {                                  // ** Start checkTwo bytes function **
  
  for ( uint8_t k = 0; k < 4; k++ ) {                                   // loop 4 times to get the 4 bytes
    if ( a[k] != b[k] ) {                                               // if a is not = to b then false, because: one fails, all fail
       return false;                                                    // return with code false
    }                                                                   // * end if statement *
  }                                                                     // * end for loop *  
  return true;                                                          // otherwise return with code true
  
}                                                                       // ** end checkTwo() function  **


uint8_t findIDSLOT( byte find[] ) {                                     // ** Start findIDSLOT() function **
  
  uint8_t count = EEPROM.read(0);                                       // read the first Byte of EEPROM that
  for ( uint8_t i = 1; i <= count; i++ ) {                              // loop once for each EEPROM entry
    readID(i);                                                          // read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {                               // check to see if the storedCard read from EEPROM
                                                                        // is the same as the find[] ID card passed
      return i;                                                         // return the slot number of the card
    }                                                                   // * end if statement *
  }                                                                     // * end for loop *
  
}                                                                       // ** end findIDSLOT() function **


bool findID( byte find[] ) {                                            // ** Start findID function (from EEPROM) **
  
  uint8_t count = EEPROM.read(0);                                       // read the first Byte of EEPROM that
  for ( uint8_t i = 1; i < count; i++ ) {                               // loop once for each EEPROM entry
    readID(i);                                                          // read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {                               // check to see if the storedCard read from EEPROM
      return true;                                                      // return with code true    
    }                                                                   // * end if statement *  
      else {       }                                                    // reserved for option to add additional checking                                        
      }                                                                 // * end for loop *
  return false;                                                         // return with result 'false'
  
}                                                                       // ** end findID function *

void successWrite() {                                                   // ** Start successWrite() function **  
  
  if (debug == true) {Serial.println("Successfully added new card");}   // send card added over serial port if debug mode enabled
  clearscreen();                                                        // clear work area on screen
  tft.setCursor(40,140);                                                // move to position 40px, 140px on tft screen
  tft.setTextColor(ILI9341_GREEN);                                      // set text color to green  
  tft.setFont(&FreeSansBold18pt7b);                                     // set font to FreeSansBold 18pt
  tft.print("1 card added");                                            // tell user 1 card was added
  delay(3000);                                                          // wait 3 seconds  
  
}                                                                       // ** end successWrite() function **


void failedWrite() {                                                    // ** Start failedWrite() function **  
  
  if (debug == true) {Serial.println("Failed to write to EEPROM");}     // send failed to write to serial port if debug mode enabled
  tft.setCursor(10,180);                                                // go to position 10px, 180px
  tft.setFont(&FreeSans9pt7b);                                          // set font to FreeSans 9pt
  tft.print("WRITE FAILURE!");                                          // tell user card failed to write
      
}                                                                       // ** end failedWrite() function

void successDelete() {                                                  // ** Start successDelete() **

  if(debug == true) {Serial.println("Successfully removed card");}     // send card removed to the serial port if debug mode enabled
  clearscreen();                                                       // clear work area on screen
  tft.setCursor(30,140);                                               // move to position 30px, 140px on screen
  tft.setTextColor(ILI9341_RED);                                       // set text color to red
  tft.setFont(&FreeSansBold18pt7b);                                    // set font to FreeSansBold 18pt
  tft.print("1 card removed");                                         // tell user 1 card was removed successfully
  delay(3000);                                                         // wait 3 second
  
}                                                                      // ** end successDelete() function **

bool isMaster( byte test[] ) {  return checkTwo(test, masterCard);}    // Check to see if the ID passed is the master programing card

void start_tft() {                                                     // ** Start start_tft() function **
  
  tft.begin();                                                         // enable tft library
  tft.setRotation(1);                                                  // set rotation to 1 (320x240 with pins on right side of board)
  tft.fillScreen(ILI9341_BLACK);                                       // set background to black and fill screen
  tft.drawLine(0,65,320,65,ILI9341_CYAN);                              // draw a border line below company and room name lines
  tft.drawLine(0,200,320,200,ILI9341_CYAN);                            // draw a border line above bottom text line for fram off workspace
  topText();                                                           // call topText() function to populate top 2 lines of text on screen
  
}                                                                      // ** end start_tft() function **

void clearscreen() {tft.fillRect(0,67,320,132,ILI9341_BLACK);}         // ** Call this function to clear the workspace area of the screen **
void clearmsg() {tft.fillRect(0,202,320,118,ILI9341_BLACK);}           // ** Call this function to clear the bottom text area of the screen **

void mainscreen(){                                                     // ** Start mainscreen() function ** 
  
  clearscreen();                                                       // clear work area on screen
  topText();                                                           // call topText() function to display company name and room name    
  if((starttimer >= timeleft)) {  roomAvailable();tapcard();}          // prompt user to tap card if xtimer function not running
  return;                                                              // return back to loop
  
}                                                                      // ** end mainscreen() function **   

void programscreen() {                                                 // ** Start programscreen() function **
 
  uint8_t count = EEPROM.read(0);                                      // read the first Byte of EEPROM to get number of cards stored
  clearscreen();                                                       // clear work area on screen       
  tft.setTextColor(ILI9341_MAGENTA);                                   // set text color to magenta
  tft.setCursor(0,105);                                                // move to position 0px, 105px on screen
  tft.setFont(&FreeSansBold18pt7b);                                    // set font to FreeSansBold 18pt
  tft.print("Program mode:");                                          // tell user they are in program mode
  tft.setCursor(50,150);                                               // move to position 50px, 150px on screen
  tft.setTextColor(ILI9341_CYAN);                                      // set text color to cyan 
  tft.setFont(&FreeSansBold18pt7b);                                    // set font to FreeSansBold 18pt   
  tft.print(count);                                                    // print the number of cards found on EEPROM
  tft.print(" cards found");                                           // print cards found following the number
  clearmsg();                                                          // clear bottom text line
  tft.setCursor(0,230);                                                // move to position 0px, 230px (bottom text line)
  tft.setFont(&FreeSans9pt7b);                                         // set font to FreeSans 9pt
  tft.setTextColor(ILI9341_YELLOW);                                    // set text color to yellow
  tft.print("Scan another card to add/remove cards");                  // tell user to scan another card 
  
  }                                                                    // ** end programscreen() function **

void topText() {                                                       // ** Start topText() function **
  
  tft.setFont(&FreeSansBold18pt7b);                                    // set font to FreeSansBold 18pt (allows for 16 char max for Company Name)
  tft.setCursor(15,30);                                                // move to location 15px, 30px (first text line, modify to match length of your variable)
  tft.setTextColor(ILI9341_WHITE);                                     // set text color white
  tft.print(CompanyName);                                              // display company name on first line
  tft.setFont(&FreeSans9pt7b);                                         // set font to FreeSans 9pt (allows for 32 char max for room name but < 20 is recommended)
  tft.setCursor(90, 55);                                               // move to location 90px, 55px (second text line, modify to match length of your variable)
  tft.setTextColor(ILI9341_YELLOW);                                    // set text color to yellow
  tft.println(RoomName);                                               // display room name on second text line 
  
}                                                                      // ** end topText() function **

void roomAvailable() {                                                 // ** Start roomAvailable() function **
  
  clearscreen();                                                       // clear work area on screen
  tft.setFont(&FreeSansBold18pt7b);                                    // set font to FreeSansBold 18pt
  tft.setCursor(30,140);                                               // move to position 30px, 140px
  tft.setTextColor(ILI9341_GREEN);                                     // set text color green
  tft.print("Room Available");                                         // display Room Available
   
}                                                                      // ** end roomAvailale() function **

void roomInUse() {                                                     // ** Start roomInUse() function **
  
  clearscreen();                                                       // clear work area on screen
  tft.setFont(&FreeSansBold18pt7b);                                    // set font to FreeSansBold 18pt   
  tft.setCursor(50,140);                                               // move to position 50px, 140px (screen center)
  tft.setTextColor(ILI9341_RED);                                       // set text color red 
  tft.println("Room in Use");                                          // display Room in Use 
  clearmsg();                                                          // clear bottom text line
  
}                                                                      // ** end roomInUse() function ** 

void tapcard() {                                                       // ** Start tapcard() function
  clearmsg();                                                          // clear bottom text line
  tft.setTextColor(ILI9341_WHITE);                                     // set text color white
  tft.setFont(&FreeSans9pt7b);                                         // set font ot FreeSans 9pt
  tft.setCursor(35,230);                                               // move to position 35px, 230px (bottom text line)
  tft.println("Tap RFID Card below to Enter");                         // prompt user to tap card     
  
}                                                                      // ** end tapcard() function ** 

void doorUnlocked(){                                                   // ** Start doorUnlocked() function ** 
  
  clearscreen();                                                       // clear work area on screen 
  tft.setCursor(30,140);                                               // move to position 30px, 140px (screen center)
  tft.setTextColor(ILI9341_GREEN);                                     // set text color green
  tft.setFont(&FreeSansBold18pt7b);                                    // set font to FreeSansBold 18pt
  tft.print("Door unlocked!");                                         // tell user the door is unlocked
  clearmsg();                                                          // clear the bottom text line
  tft.setTextColor(ILI9341_WHITE);                                     // set text color white   
  tft.setFont(&FreeSans9pt7b);                                         // set font to FreeSans 9pt
  tft.setCursor(0,230);                                                // move to position 0px, 230px (bottom text line)
  tft.println("Please open the door now...");                          // ask user to open the door now
  granted();                                                           // call granted() function to open the door lock for 30 seconds
  
}                                                                      // ** end doorUnlocked() function

void Alarm(){                                                          // ** Start Alarm() function ** 
  
  clearmsg();                                                          // clear the bottom text line
  tft.setCursor(90,140);                                               // move to position 60px, 140px on tft screen
  tft.setTextColor(ILI9341_BLACK);                                     // set text color to black
  tft.fillRect(0,67,320,132,ILI9341_RED);                              // fill work area red
  tft.setFont(&FreeSansBold18pt7b);                                    // set fon to FreeSansBold 18pt
  tft.print("Alarm!!!");                                               // display ALARM! on the screen
  for(int a = 0; a < (alarmTime *2); a++){                             // start loop for calculated seconds from declared value at top of code
     digitalWrite(AlarmPin, HIGH);                                     // set pin high to sounds buzzer
     delay(250);                                                       // hold for 250 ms
     digitalWrite(AlarmPin, LOW);                                      // set pin low to silence buzzer
     delay(250);                                                       // hold for 250 ms
  }                                                                    // * end of for loop *
  clearscreen();                                                       // clear the screen
  if(doorOpened == false) { mainscreen(); }                            // return to main screen if the door opened flag is false    
  else {roomInUse(); xtimer();}                                                       // otherwise assume xtimer function sent you here.
}                                                                      // ** end Alarm() function 

void doorOpentoLong() {                                                // ** Start doorOpentoLong() function
 
  if((millis()-doorOpenTimer >= (doorOpenSeconds * 1000))){ Alarm();}  // go into alarm if door open for number of seconds defined by doorOpenSeconds
      
}                                                                      // ** end doorOpentoLong function
