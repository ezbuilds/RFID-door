# RFID-door Version 1.02
Version 1.02 is a stand-alone RFID electric door latch controller (RFID Module) only. 
** Later version will include increased security by adding serial communications, bluetooth and MQTT sub/pub libraries. **
 
The RFID MODULE contains an ESP8266, a RC522 RFID card reader, an ILI9341 compatible TFT display and a relay to trigger the door latch.
It accepts 12VDC input power and the circuit can contain either a buck converter to step the voltage down to 3.3 volts for the electronics
or a linear regulator.  In some cases, you may want to put more than one linear regulator to step down the voltage in stages to reduce the 
heat for each regulator as most of the stepped down voltages can be converted to heat. 

The ESP8266 offers a simulated EEPROM by using a dedicated 4MB flash RAM that can be writted / re-writted up to 10,000 times. The ESP8266 and flash
memory should last indefinately if you are not regualarly adding or removing cards.

Future versions may include adding an SD card to replace the EEPROM writes or adding a cheap external EEPROM to increase the lifecycle of the 
ESP8266 that contains all the code.

----------------------------------------------------------------------------------------------------------------------------------------------

Features found in the program (RFID MODULE) include:

Initial Setup mode: 

  When first powered on, the system will read the EEPROM and look for a value stored in the last memory location to see if
  it has been configured.   If the value in memory location 4095 is not 42, the system will enter an initial configuration mode and the first 
  card/fob tapped will become the new MASTER CARD.   

Master Card: 

  This is the first card programmed into the system.  The serial number of the card is stored in EEPROM memory location 0001-0004.
  When this card is used, the system will go into Program mode to allow the administrator to add additional cards.

Program mode:  

  After accepting the Master Card, the administrator is prompted to tap a new card.  They system reads the number of cards stored
  from EEPROM memory location 0000 and will be assigned to a variable used to tell the system how many 4 byte memory locations to scan for the 
  new card.   If the card is already in the system, it will be removed and all subsiquent cards will be shifted down one 4 byte location.  If the
  card is not in the system, it will be added after the last known card and EEPROM memory location 0000 will be incremented by 1.

Restricted mode: 

  As many buildings are having to introduce social distancing restrictions, this program has encorporated a restrited mode to 
  allow for single person use for a room. The program reads the built in EEPROM at memory locaiton 4094 and stores that value as the number of 
  minutes to restrict access to the room.  If the memory locations contain 00, restricted mode is disabled. When enabled, the user taps in and 
  the RFID MODULE sends a signal to the DOOR RELAY MODULE to open the door. It then awaits a reply for 30 seconds to know that door has been opened 
  and its okay to start the timer and restrict access to the room.  If the door is opened again the RFID MODULE assumes the user has left the room 
  and sets the counter back to zero to allow someone else to use the room.

  While in "restriced status" only the card that was last tapped in can access the room.  This was added to avoid a possibiliy of the user 
  accidentally opening and closing the door before entering and being locked out for the duration of the restriction timer.  


EEPROM Memory Map:
      
      0000 - 0001: Number of cards stored in system 
      0002 - 0005: 4 byte location for Master Card ID
      0006 - 4006: storage location of user cards ( each card uses 4 bytes)
      4007 - 4093: Reserved
      4094 - Flag for restricted mode (00 = disabled otherwise DEC amount for number of minute to restrict access after user taps in
      4095 - Flag for unit setup (If = 42, the answer to the ultimate question of life, the universe and everything, the system is configured!)


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


You will need the following libraries for this code:

Adafruit_GFX:  https://github.com/adafruit/Adafruit-GFX-Library 
Adafruit_ILI9341: https://github.com/adafruit/Adafruit_ILI9341 
MFRC522: https://github.com/ljos/MFRC522
