/*

    ///////////////////////////////////////////////////////
   ///////////////// AVR Component Tester ////////////////
  //////////////////    Version 1 beta    ///////////////
 ///////////////////////////////////////////////////////

 AVR Component Tester
 Version 1.0 BETA

 This project is based on the original 'AVR Transistor Tester' and will be
 modified for a small group of people interested in expanding this project.
 The original project can be found:
 
 http://www.mikrocontroller.net/articles/AVR-Transistortester
 
 This new project (ACT for short) does not have its own website; but it does
 have its own google project page which includes news, wiki, bug tracking,
 and a publicly viewable SVN; where you can always find the latest source
 files and other resources such as: Schematics, PCB files; and even pre-
 compiled gerbers, if you want to get your own PCB(s) designed. The gerbers
 are already completed and designed to work with SEEED Studio's PCB rules and
 formats. See www.seeedstudios.com for more information on there 'Fusion PCB
 service'.

 Main Project Developers:
 BrentBXR (@Gmail.com)
 MickM
 
 We also have a related forum thread at DangerousPrototypes, there you can see
 where it all started, and where we are now with this project. Find it here:
 
 http://dangerousprototypes.com/forum/viewtopic.php?f=19&t=3260
 
 This is a open hardware project; which means everything about it from the source
 files, down to the schematics and pcb files are all open source and public domain
 you are welcome to use, modify, sell, or anything without issue. It is common
 curtosy to atleast let us know what your doing with our project if you use it.
 it is also common that if you decide to replicate and sell; that you atleast tell
 us... that said you dont have to tell us anything :P
 
                                                                   [BrentBXR]
*/

#include <avr/io.h>
#include "lcd.h"
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <math.h>
#include "settings.h"

// *########################################################################################

	// Initial Configuration
	
	uint8_t CapTestMode 			EEMEM 	= V_CAPTESTMODE;	
	
	unsigned int F_VER 				EEMEM 	= _FIRMWARE_VERSION_;
	unsigned int F_REV 				EEMEM 	= _FIRMWARE_REVISION_;
	unsigned int H_REV 				EEMEM 	= _HARDWARE_REVISION_;

	unsigned int R_L_VAL 			EEMEM 	= SMALL_R_VALUE;
	unsigned int R_H_VAL 			EEMEM 	= LARGE_R_VALUE;
	
	unsigned int H_CAPACITY_FACTOR 	EEMEM = SMALL_CAP_VALUE;
	unsigned int L_CAPACITY_FACTOR 	EEMEM = LARGE_CAP_VALUE;


// *########################################################################################

	// Words, messages, and strings:        
	
    unsigned char DeveloperName[]	EEMEM = "A.Grigoriev 2012";
unsigned char DeveloperContact[]	EEMEM = "+(972)0509468305";
		
	unsigned char StartupMessage[]	EEMEM = "ACTv1.6  ";//[7.01V]
	unsigned char BatMode[] 		EEMEM = "[BAT]";
	unsigned char PwrMode[]			EEMEM = "[PWR]";
	unsigned char TestRunning[]     EEMEM = "Testing   ->";
	unsigned char TestCapV[]		EEMEM = "Measuring ->";
	unsigned char Bat[]             EEMEM = "Battery ";
	unsigned char BatWeak[]         EEMEM = "weak";
	unsigned char BatEmpty[]        EEMEM = "empty!";

	unsigned char TestFailed1[]     EEMEM = "Unknown, Damaged";
	unsigned char TestFailed2[]     EEMEM = "or Nothing found";

	unsigned char BadResult1[]		EEMEM = " Part's Unknown";
	unsigned char BadResult2[]		EEMEM = " or Damaged: ";

	unsigned char TestTimedOut[] 	EEMEM = "Timeout!";
	
	// Components
	unsigned char Diode[]           EEMEM = "Diode: ";
	unsigned char DualDiode[]       EEMEM = "Double diode ";
	unsigned char TwoDiodes[]       EEMEM = "2 diodes";
	unsigned char Antiparallel[]    EEMEM = "anti-parallel";
	unsigned char Resistor[] 		EEMEM = "Resistor: ";
	unsigned char Capacitor[] 		EEMEM = "Capacitor: ";
	unsigned char InSeries[]        EEMEM = "serial A=";
	unsigned char Triac[]           EEMEM = "Triac";
	unsigned char Thyristor[]       EEMEM = "Thyristor";

	// Codes and Values
	unsigned char K1[]          EEMEM = ";C1=";
	unsigned char K2[]          EEMEM = ";C2=";
	unsigned char GAK[]         EEMEM = "GAC=";
	unsigned char NextK[]       EEMEM = ";C=";
	unsigned char K[]           EEMEM = "C=";
	unsigned char mosfet[]  	EEMEM = "-MOS";
	unsigned char emode[]   	EEMEM = "-E";
	unsigned char dmode[]   	EEMEM = "-D";
	unsigned char jfet[]    	EEMEM = "-JFET";
	unsigned char A1[]      	EEMEM = ";A1=";
	unsigned char A2[]      	EEMEM = ";A2=";
	unsigned char GateCap[] 	EEMEM = " C=";
	unsigned char hfestr[]  	EEMEM = "hFE=";
	unsigned char NPN[]     	EEMEM = "NPN";
	unsigned char PNP[]     	EEMEM = "PNP";
	unsigned char bstr[]    	EEMEM = " B=";
	unsigned char cstr[]    	EEMEM = ";C=";
	unsigned char estr[]    	EEMEM = ";E=";
	unsigned char gds[]     	EEMEM = "GDS=";
	unsigned char Uf[]      	EEMEM = "Uf=";
	unsigned char vt[]      	EEMEM = "Vt=";
	unsigned char mV[]      	EEMEM = "mV";
	unsigned char Anode[]   	EEMEM = "A=";
	unsigned char Gate[]    	EEMEM = "G=";
	unsigned char CA[]      	EEMEM = "CA";
	unsigned char CC[]     		EEMEM = "CC";

	// LCD Icons
	unsigned char DiodeIcon[] EEMEM = {4,31,31,14,14,4,31,4,0};	// Diode icon
		

struct Diode {
	uint8_t Anode;
	uint8_t Cathode;
	int Voltage;
};
									// Function prototypes

void 			CheckPins			(uint8_t HighPin, uint8_t LowPin, uint8_t TristatePin);
void 			DischargePin		(uint8_t PinToDischarge, uint8_t DischargeDirection);
unsigned int 	ReadADC				(uint8_t mux);
void 			lcd_show_format_cap	(char outval[], uint8_t strlength, uint8_t CommaPos);
void 			ReadCapacity		(uint8_t HighPin, uint8_t LowPin);		
void			ADC_init			(void);
unsigned int	ADC_result			(unsigned char adc_input);


volatile unsigned int PowerMode=PWR_5V;


struct 			Diode diodes[6];

uint8_t 		NumOfDiodes;
uint8_t 		b;						// pins of transistor
uint8_t 		c;						// pins of transistor
uint8_t 		e;						// pins of transistor
unsigned long 	lhfe;					// Amplification factor
uint8_t 		PartReady;				// Device recognized, Transistor, FET, Triac
unsigned int 	hfe[2];					// Amplification factors
unsigned int 	uBE[2];					// B-E Covering for transistors
uint8_t 		PartMode;				// See defines PART_MODE_
uint8_t 		tmpval;
uint8_t 		tmpval2;
uint8_t 		ra;						// Resistance pin
uint8_t 		rb;						// Resistance pin
unsigned int 	rv[2];					// Voltage drop at the resistance
unsigned int 	radcmax[2];	       		// Max ADC value (smaller than 1023, comes close but does not get to zero)
uint8_t 		ca;						// Condenser-Pins
uint8_t 		cb;						// Condenser-Pins
uint8_t 		cp1;					// Testing condenser pins, if measurement for individual pins selected
uint8_t 		cp2;					// Testing condenser pins, if measurement for individual pins sel
uint8_t 		ctmode;					// Condenser test mode
unsigned long 	cv;
uint8_t 		tmpPartFound;			// temp found Device, used for Resistor
uint8_t 		PartFound;				// the found Device numeric ID see defines PART_
char 			outval[8];
unsigned int 	adcv[4];
unsigned int 	gthvoltage;				// Gate threshold voltage
char 			outval2[6];

#ifdef ENABLE_PIN_ALIAS
uint8_t GetPinAlias(uint8_t nPin)				// GetPinAlias allows the user to define his own
{												// alias for each pin #; defined in 'settings.h'
   switch(nPin) {
      case (unsigned char)'1':
         nPin = PIN1_ALIAS;
         break;
      case (unsigned char)'2':
         nPin = PIN2_ALIAS;
         break;
      case (unsigned char)'3':
         nPin = PIN3_ALIAS;
         break;
   }
   return nPin;
}
#else
	#define GetPinAlias(x) (x)
#endif




int main(void) {
  POWER_ON();											// Turn the regulator ON
  PWRMODE_SETUP();										// Setup PWRMODE jumper input
  lcd_init(LCD_DISP_ON);								// init LCD
  lcd_eep_string(DeveloperName);
  Line2();
  lcd_eep_string(DeveloperContact);
  _delay_ms(1500);
  uint8_t tmp;
  ADC_init();
  //ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
  unsigned int rhval = eeprom_read_word(&R_H_VAL);		// R_H
  unsigned int rlval = eeprom_read_word(&R_L_VAL);		// R_L
  
  ctmode = eeprom_read_byte(&CapTestMode);				// Compile time choice of test modes (0x22)
  cp1 = (ctmode & 12) >> 2;							// Capacitor pin 1, DEFAULT 0
  cp2 = ctmode & 3;										// Capacitor pin 2, DEFAULT 2
  ctmode = (ctmode & 48) >> 4;							// Capacitor test mode, DEFAULT is 0x02 for all 6 cap tests.

    wdt_disable();										// Disable watch dog timer.
 
  if(MCU_STATUS_REG & (1<<WDRF)) {						// Examine for Watchdog RESETs That enters, if the Watchdog 2s were not put back Can occur, 
    lcd_clrscr();                                                    // if the program in a continuous loop " itself; tangled" has.
    lcd_eep_string(TestTimedOut);						// Message - "Timeout!"
    _delay_ms(10000);                                	// Wait 3 sec
	wdt_enable(WDTO_4S);								// Wait two seconds; if on power it will reset; on battery it will turn itself off
    while(1) {
lcd_string("byy..");
		POWER_OFF();									// Power down in BAT mode or RESET in PWR mode
	}
	
  }

 LCDLoadCustomChar();									// Custom indication Diode symbol into LCD load
 lcd_eep_string(DiodeIcon);							// Message - diode icon
 Line1();												// jump to start of first line

start:													// re-entry point, if button is re-pressed
  #ifdef WDT_enabled
    wdt_enable(WDTO_8S);								// Watchdog Timer on, 2 seconds?
  #endif

  PartFound 	= PART_NONE;							// Default all results
  tmpPartFound 	= PART_NONE;							//		  "    " 
  NumOfDiodes 	= 0;									//			||
  PartReady 	= 0;									//			||
  PartMode 		= 0;									//			||
  ca 			= 0;									//			||
  cb 			= 0;									//			\/

  lcd_init(LCD_DISP_ON);										//	->	// Startup Message ////////////////////////////////////////
  Line2();												// jump to start of first line
  lcd_clrscr();										// 
  lcd_eep_string(StartupMessage);					// LCD: ACT v#.#    [XXX]
  if(PWRMODE_GET()) {								// Get the PWRMODE jumper logic
	PowerMode = PWR_9V;								// Set powermode to PWR_9V
	_delay_us(250);
Line2();
lcd_string("readadc");
_delay_ms(2000);
 ReadADC(SETBITMASKL(5,REFS1));						// Measure the 9V battery Supply ( - diode drop)
float BATTERY_VOLT;
float RESISTOR1=3.3f;		//3.3
float RESISTOR2=21.2f;		//23.85
float Uref=1.1f; //For ATMEGA 328 Voltage Reference 1.1V
float Res_koef=(RESISTOR1/(RESISTOR1+RESISTOR2));
float Uop_koef=(1023.0f/1.1f);
float reftovolt=Uop_koef/Res_koef;
float dif_D1=0.64f; //������� ���������� �� ����� D1, ����� ���� ����� ���������.

//ADDRESS |= (1<<BIT))

hfe[0]=ReadADC(SETBITMASKL(5,REFS1)); // if in battery mode. Measure the 7.4 V battery Supply ( - diode drop)

BATTERY_VOLT= ((float)hfe[0]/Uop_koef/Res_koef)+dif_D1;		//converter to Volts;
	
lcd_data('[');
ftos(BATTERY_VOLT);
lcd_string("V]");
//ftos((float)hfe[0]/100.0f);
Line2();

	if (hfe[0] < BAT_WEAK) {						// Compare 9v reading with BAT_WEAK variable
		if(hfe[0] < BAT_DEAD) {					// If the batter is considered dead then
			lcd_eep_string(Bat);					
			lcd_eep_string(hfe[0]+BatEmpty);		// Tell the user battery is DEAD
			_delay_ms(2400);						// Wait a bit.
  			while(1) {								// Forever loop
				  lcd_string("bye..");
				  _delay_ms(5000);
				POWER_OFF();						// keep trying to kill the power forever.
			}
		}
		
		lcd_clrscr();			
		lcd_eep_string(Bat);						// Battery isnt dead; its just weak
		lcd_eep_string(BatWeak);					// tell the user; but keep testing...
		Line2();									// Start second line
	  }
  } else {
	PowerMode = -PWR_5V;								// Power mode is constent v5, skip battery check.
    lcd_eep_string(PwrMode);						// Tell user we are running in PWR mode.
	Line2();
  }
 
											//	->	// Begin testing sequince. ///////////////////////////////
  lcd_eep_string(TestRunning);						// Tell user the testing has begun...
 
  UpdateProgress("00%");							// Progress at 00% and Testing
  
  CheckPins(TP1, TP2, TP3);							//			||
  UpdateProgress("16%");							//			\/
  
  CheckPins(TP1, TP3, TP2);							//		TESTING...
  UpdateProgress("33%");							//

  CheckPins(TP2, TP1, TP3);							//			||
  UpdateProgress("50%");							//			\/
  
  CheckPins(TP2, TP3, TP1);							//		TESTING...
  UpdateProgress("66%");							//
  
  CheckPins(TP3, TP2, TP1);							//			||
  UpdateProgress("83%");							//			\/
  
  CheckPins(TP3, TP1, TP2);							//	   Almost there!
  UpdateProgress("99%");							// Testing Completed or 99%
  

//---------------------------------------------CAPACITOR---------------------------------------
									// Separate measurement to the test on condenser
  if(((PartFound == PART_NONE) || (PartFound == PART_RESISTOR) || (PartFound == PART_DIODE)) && (ctmode > 0)) {
									// Condenser unload; otherwise possibly no measurement is possible
    R_PORT = 0;
    R_DDR = (1<<(TP1 * 2)) | (1<<(TP2 * 2)) | (1<<(TP3 * 2));
    _delay_ms(10);
    R_DDR = 0;

    if(ctmode == NORMAL_CAP_TESTS) {					// see if we want to do all 6 Cap Tests
      ReadCapacity(cp1, cp2);						// No - just read the pins both ways.
      ReadCapacity(cp2, cp1);
    } else {								// DEFAULT ctmode == 0x02  to do all tests
	Line2();
	lcd_eep_string(TestCapV);
	UpdateProgress("00%");
	
	ReadCapacity(TP3, TP1);
	UpdateProgress("16%");
	
	ReadCapacity(TP3, TP2);
	UpdateProgress("33%");
	
	ReadCapacity(TP2, TP3);
	UpdateProgress("50%");
	
	ReadCapacity(TP2, TP1);
	UpdateProgress("66%");
	
	ReadCapacity(TP1, TP3);
	UpdateProgress("83%");
	
	ReadCapacity(TP1, TP2);
	UpdateProgress("99%");
      }
   }

  lcd_clrscr();								// Finished, now evaluate, the results

//---------------------------------------------DIODE------------------------------------------------  
  if(PartFound == PART_DIODE) {
    if(NumOfDiodes == 1) {						// Standard-Diode
      lcd_eep_string(Diode);						// Message - "Diode: "
      lcd_eep_string(Anode);						// Message - "A="
      lcd_data(GetPinAlias(diodes[0].Anode + ASCII_1));				// Display 1, 2, or 3
      lcd_eep_string(NextK);						// Message - ";C="
      lcd_data(GetPinAlias(diodes[0].Cathode + ASCII_1));				// Display 1, 2, or 3
      Line2();								// Start second line
      lcd_eep_string(Uf);						// Message - "Uf="
      lcd_string(itoa(diodes[0].Voltage, outval, 10));
      lcd_eep_string(mV);						// Message - "mV"
      goto end;
    } else if(NumOfDiodes == 2) {					// dual diode
	if(diodes[0].Anode == diodes[1].Anode) {			// Common Anode
	  lcd_eep_string(DualDiode);					// Message - "Double diode �"
	  lcd_eep_string(CA);						// Message - "CA"
	  Line2();							// Start second line
	  lcd_eep_string(Anode);					// Message - "A="
	  lcd_data(GetPinAlias(diodes[0].Anode + ASCII_1));				// Display 1, 2, or 3
	  lcd_eep_string(K1);						// Message - ";C1="
	  lcd_data(GetPinAlias(diodes[0].Cathode + ASCII_1));				// Display 1, 2, or 3
	  lcd_eep_string(K2);						// Message - ";C2="
	  lcd_data(GetPinAlias(diodes[1].Cathode + ASCII_1));			// Display 1, 2, or 3
	  goto end;
	} else if(diodes[0].Cathode == diodes[1].Cathode) {		// Common Cathode
	    lcd_eep_string(DualDiode);					// Message - "Double diode �"
	    lcd_eep_string(CC);						// Message - "CC"
	    Line2(); 							// Start second line
	    lcd_eep_string(K);						// Message - "C="
	    lcd_data(GetPinAlias(diodes[0].Cathode + ASCII_1));			// Display 1, 2, or 3
	    lcd_eep_string(A1);						// Message - ";A1="
	    lcd_data(GetPinAlias(diodes[0].Anode + ASCII_1));			// Display 1, 2, or 3
	    lcd_eep_string(A2);						// Message - ";A2="
	    lcd_data(GetPinAlias(diodes[1].Anode + ASCII_1));			// Display 1, 2, or 3
	    goto end;
	  } else if ((diodes[0].Cathode == diodes[1].Anode) && \
		     (diodes[1].Cathode == diodes[0].Anode)) {		// Antiparallel
	      lcd_eep_string(TwoDiodes);				// Message - "2 diodes"
	      Line2(); 							// Start second line
	      lcd_eep_string(Antiparallel);				// Message - "anti-parallel"
	      goto end;
	    }
    } else if(NumOfDiodes == 3) { 					// Series connection from 2 diodes; as 3 diodes one recognizes
	b = 3;
	c = 3;
									// Check to see if it is series connection of 2 diodes.
									// But 2 cathodes, and 2 anodes must agree.
									// Then the 2 diodes are a single dual-diode.
	if((diodes[0].Anode == diodes[1].Anode) || (diodes[0].Anode == diodes[2].Anode)) 
	  b = diodes[0].Anode;

	if(diodes[1].Anode == diodes[2].Anode) 
	  b = diodes[1].Anode;

	if((diodes[0].Cathode == diodes[1].Cathode) || (diodes[0].Cathode == diodes[2].Cathode)) 
	  c = diodes[0].Cathode;

	if(diodes[1].Cathode == diodes[2].Cathode) 
	  c = diodes[1].Cathode;

	if((b<3) && (c<3)) {
	  lcd_eep_string(TwoDiodes);					// Message - "2 diodes"
	  Line2();							// Start second line
	  lcd_eep_string(InSeries);					// Message - "serial A=��"
	  lcd_data(GetPinAlias(b + ASCII_1));					// Display 1, 2, or 3
	  lcd_eep_string(NextK);					// Message - ";C="
	  lcd_data(GetPinAlias(c + ASCII_1));					// Display 1, 2, or 3
	  goto end;
	}
      }
  } 
  	
//---------------------------------------------TRANSISTOR--------------------------------------------
    else if (PartFound == PART_TRANSISTOR) {
      if(PartReady == 0) {						// 2nd examination never made, e.g. a transistor with protection diode.
	hfe[1] = hfe[0];
	uBE[1] = uBE[0];
      }

      if((hfe[0]>hfe[1])) {						// If the amplification factor with the first test was higher: swap C and E
	hfe[1] = hfe[0];
	uBE[1] = uBE[0];
	tmp = c;
	c = e;
	e = tmp;
      }

      if(PartMode == PART_MODE_NPN) 
	lcd_eep_string(NPN);						// Message - "NPN"
      else 
	lcd_eep_string(PNP);						// Message - "PNP"

      lcd_eep_string(bstr);						// Message - " B="
      lcd_data(GetPinAlias(b + ASCII_1));						// Display 1, 2, or 3
      
      lcd_eep_string(cstr);						// Message - ";C="
      lcd_data(GetPinAlias(c + ASCII_1));						// Display 1, 2, or 3
      
      lcd_eep_string(estr);						// Message - ";E="
      lcd_data(GetPinAlias(e + ASCII_1));						// Display 1, 2, or 3
      
      Line2(); 								// Start second line
									// Amplification factor compute, hFE = Emitter current/base current
      lhfe = hfe[1];
      lhfe *= (((unsigned long)rhval * 100) / (unsigned long)rlval);	// 500000/750 = 666.666r
      

      if(uBE[1]<11) 
	uBE[1] = 11;

      lhfe /= uBE[1];
      hfe[1] = (unsigned int) lhfe;
      lcd_eep_string(hfestr);						// Message - "hFE="
      lcd_string(utoa(hfe[1], outval, 10));
      SetCursor(2,7);							// Cursor on line 2, character 7

      if(NumOfDiodes > 2) 						// Transistor with protection diode
	lcd_data(LCD_CHAR_DIODE);					// Diode indicate
      else
	lcd_data(' ');

      for(c=0;c<NumOfDiodes;c++) {
	if(( (diodes[c].Cathode == e) && (diodes[c].Anode == b) && \
	     (PartMode == PART_MODE_NPN)) || ((diodes[c].Anode == e) && \
	     (diodes[c].Cathode == b) && (PartMode == PART_MODE_PNP))) {
	  lcd_eep_string(Uf);						// Message - "Uf="
	  lcd_string(itoa(diodes[c].Voltage, outval, 10));
	  lcd_data('m');
	  goto end;
	}
      }

      goto end;
      } 

//---------------------------------------------FET---------------------------------------------------     
	else if (PartFound == PART_FET) {				// JFET or MOSFET
	  if(PartMode & 1)						// N-channel
	    lcd_data('N');
	  else 
	    lcd_data('P');						// P-channel

	  if((PartMode == PART_MODE_N_D_MOS) || (PartMode == PART_MODE_P_D_MOS)) {
	    lcd_eep_string(dmode);					// Message - "-D"
	    lcd_eep_string(mosfet);					// Message - "-MOS"
	    } else {
		if((PartMode == PART_MODE_N_JFET) || (PartMode == PART_MODE_P_JFET)) 
		  lcd_eep_string(jfet);					// Message - "-JFET"
		else {
		  lcd_eep_string(emode);				// Message - "-E"
		  lcd_eep_string(mosfet);				// Message - "-MOS"
		}
	    }
									// Gate capacity
	  if(PartMode < 3) {						// Enrichment MOSFET
	    lcd_eep_string(GateCap);					// Message - " C="
	    ReadCapacity(b,e);						// Measurement
	    hfe[0] = (unsigned int)cv;

	    if(hfe[0]>2) 
	      hfe[0] -= 3;

	    utoa(hfe[0], outval2, 10);
	    tmpval = strlen(outval2);
	    tmpval2 = tmpval;

	    if(tmpval>4) 
	      tmpval = 4;						// If capacity > 100nF drop fractional part to fit on the LCD

	    lcd_show_format_cap(outval2, tmpval, tmpval2);
	    lcd_data('n');
	  }

	  Line2();							// Start second line
	  lcd_eep_string(gds);						// Message - "GDS="
	  lcd_data(GetPinAlias(b + ASCII_1));					// Display 1, 2, or 3
	  lcd_data(GetPinAlias(c + ASCII_1));					// Display 1, 2, or 3
	  lcd_data(GetPinAlias(e + ASCII_1));					// Display 1, 2, or 3

	  if((NumOfDiodes > 0) && (PartMode < 3))			// MOSFET with protection diode; it gives only with enrichment FETs 
	    lcd_data(LCD_CHAR_DIODE);					// Diode indicate
	  else 
	    lcd_data(' ');						// Blank

	  if(PartMode < 3) {						// Enrichment MOSFET
	    gthvoltage=(gthvoltage/8);
	    lcd_eep_string(vt);						// Message - "Vt="
	    lcd_string(utoa(gthvoltage, outval, 10));			// Gate threshold voltage, was determined before
	    lcd_data('m');
	  }

	  goto end;


      } 

//---------------------------------------------THYRISTOR---------------------------------------------     
	else if (PartFound == PART_THYRISTOR) {
	  lcd_eep_string(Thyristor);			 		// Message - "Thyristor"
	  Line2();						 	// Start second line
	  lcd_eep_string(GAK);				 		// Message - "GAC="
	  lcd_data(GetPinAlias(b + ASCII_1));					// Display 1, 2, or 3
	  lcd_data(GetPinAlias(c + ASCII_1));					// Display 1, 2, or 3
	  lcd_data(GetPinAlias(e + ASCII_1));					// Display 1, 2, or 3
	  goto end;

	} 

//---------------------------------------------TRIAC-------------------------------------------------	
	  else if (PartFound == PART_TRIAC) {
	    lcd_eep_string(Triac);					// Message - "Triac"
	    Line2();							// Start second line
	    lcd_eep_string(Gate);					// Message - "G="
	    lcd_data(GetPinAlias(b + ASCII_1));					// Display 1, 2, or 3
	    lcd_eep_string(A1);						// Message - ";A1="
	    lcd_data(GetPinAlias(e + ASCII_1));					// Display 1, 2, or 3
	    lcd_eep_string(A2);						// Message - ";A2="
	    lcd_data(GetPinAlias(c + ASCII_1));					// Display 1, 2, or 3
	    goto end;

	  } 

//---------------------------------------------RESISTOR----------------------------------------------	  
	    else if(PartFound == PART_RESISTOR) {
	      lcd_eep_string(Resistor);					// Message - "Resistor: ��"
	      lcd_data(GetPinAlias(ra + ASCII_1));					// Display 1, 2, or 3 Pin data
	      lcd_data('-');
	      lcd_data(GetPinAlias(rb + ASCII_1));					// Display 1, 2, or 3
	      Line2();							// Start second line

	      if(rv[0] > HALF_ADC_RANGE) 				// Examine, how far the Voltages across the test resistances deviate from 512 
		hfe[0] = (rv[0] - HALF_ADC_RANGE);
	      else 
		hfe[0] = (HALF_ADC_RANGE - rv[0]);

	      if(rv[1] > HALF_ADC_RANGE) 
		hfe[1] = (rv[1] - HALF_ADC_RANGE);
	      else 
		hfe[1] = (HALF_ADC_RANGE - rv[1]);

	      if(hfe[0] > hfe[1])  {
		radcmax[0] = radcmax[1];
		rv[0] = rv[1];						// Result use, which is more near because of 512 (accuracy improves)
		rv[1] = rhval;						// High - Test resistance
	      } else 
		  rv[1] = rlval;					// Low - Test resistance

	      if(rv[0] == 0) 
		rv[0] = 1;

	      lhfe = (unsigned long)((unsigned long)((unsigned long)rv[1] * \
	                             (unsigned long)rv[0]) / (unsigned long)((unsigned long)radcmax[0] - (unsigned long)rv[0]));	// Resistance compute
	      ultoa(lhfe,outval,10);

	      if(rv[1] > (1000)) {	//rhval				// 470k- Resisted?
		ra = strlen(outval);					// Necessarily, in order to indicate comma

		for(rb=0;rb<ra;rb++) {
		  lcd_data(outval[rb]);

		  if(rb == (ra-2)) 
		    lcd_data(',');					// comma
		}
				
		lcd_data ('K');						// Kilo ohm, if 470k uses resistance
	      } else 
		  lcd_string(outval);
		    
	      lcd_data(LCD_CHAR_OMEGA);				// Omega for ohms 
	      goto end;

	    } 

//---------------------------------------------CAPACITOR---------------------------------------------	    
	      else if(PartFound == PART_CAPACITOR) {			// Capacitor measurement
		lcd_eep_string(Capacitor);				// Message - "Capacitor: ��"
		lcd_data(GetPinAlias(ca + ASCII_1));					// Display 1, 2, or 3 Pin - Data
		lcd_data('-');
		lcd_data(GetPinAlias(cb + ASCII_1));					// Display 1, 2, or 3
		Line2();						// Start second line
		tmpval2 = 'n';						// n for nF
		    
		if(cv > 99999) {					// Too big
		  cv /= 1000;						// convert to Micro Farads

		  tmpval2 = LCD_CHAR_U;		//			// change n to greek char for micro
		}

		ultoa(cv, outval, 10);					// outval now a string version of cv
		tmpval = strlen(outval); 
		lcd_show_format_cap(outval, tmpval, tmpval); 
		lcd_data(tmpval2);					// display the SI Suffix
		lcd_data('F');						// F for Farads
		goto end;
	      }

//---------------------------------------------NOT-FOUND-OR-DAMAGED---------------------------------------------------------	

		if(NumOfDiodes == 0) {						// Nothing found. Tell user.
				lcd_eep_string(TestFailed1);
				Line2();
				lcd_eep_string(TestFailed2);
			} else {								// Data found but bad result or no positive ident
				lcd_eep_string(BadResult1);
				Line2();
				lcd_eep_string(BadResult2);
				lcd_data(NumOfDiodes + ASCII_0);
				lcd_data(LCD_CHAR_DIODE);
		}

		end:

		while(!RESET_GET()) //RST_PIN_REG & (1<<RST_PIN)));			// wait, to tracers released
		  _delay_ms(200);

		for(hfe[0] = 0;hfe[0]<10000;hfe[0]++) {			// 10 Seconds untill power off.

		  if(!RESET_GET()) //(RST_PIN_REG & (1<<RST_PIN)))			// if the button is pressed, start all over
		    goto start;

		  wdt_reset();						// We want to wait the full 10 Seconds
		  _delay_ms(3);						// 1mS 10,000 times = 10 seconds
		}

	if(PowerMode==PWR_9V) {				// If in battery mode; try to turn off; otherwise wait for a reset
		POWER_OFF();
	}
	
	wdt_disable();						// Watchdog out
										// Continuous loop, no timer
  while(1) {
    if(!(RESET_GET()))					// only one reaches, if the automatic disconnection was not inserted
      goto start;
  }
  
  return 0;
}								// End of main()







void CheckPins(uint8_t HighPin, uint8_t LowPin, uint8_t TristatePin) {	// Do the tests on the Probe pins, get device characteristics
									// HighPin   - Starts at H, logic 1.
									// LowPin    - Starts at L, logic 0.
									// TriState  - Starts Hi-Z, is put both H and L during the tests.
unsigned int adcv[6];
uint8_t tmpval, tmpval2;
									// HighPin is Vcc, LowPin is Gnd, TriState is HiZ
  wdt_reset();
									// Pin set
  tmpval = (LowPin * 2);						// necessarily because of the arrangement of the resistances
  R_DDR = (1<<tmpval);							// Low pin on exit and over R_L to ground
  R_PORT = 0;
  ADC_DDR = (1<<HighPin);					// High pin on exit
  ADC_PORT = (1<<HighPin);					// High pin to Vcc
  _delay_ms(5);
									// With some MOSFETs the gate (TriState act pin) must be unloaded first
									// N-channel:
  DischargePin(TristatePin,0);						
  adcv[0] = ReadADC(LowPin);						// Read the Voltage at the Low pin
  
  if(adcv[0] < 200) 
    goto next;								// Does the Device close now?
									
  DischargePin(TristatePin,1);						// otherwise: Unloaded for p-channel (gate on pluses)
  adcv[0] = ReadADC(LowPin);						// Read the Voltage at the Low pin

  next:

  if(adcv[0] > 19) {							// Device leads something without control current
									// Test on N-JFET or leading N-MOSFET
    R_DDR |= (2<<(TristatePin*2));					// Tristate pin (assumed gate) over R_H to ground
    _delay_ms(20);
    adcv[1] = ReadADC(LowPin);						// Voltage at the assumed SOURCE measure
    R_PORT |= (2<<(TristatePin*2));					// Tristate pin (assumed gate) over R_H on pluses
    _delay_ms(20);
    adcv[2] = ReadADC(LowPin);						// Voltage at the assumed SOURCE measure again
									// If it concerns a leading MOSFET or JFET, would have adcv [1] > adcv [0] its
    if(adcv[2]>(adcv[1]+100)) {						// Voltage at the gate measure, to the distinction between MOSFET and JFET
      ADC_PORT = 0x00;
      ADC_DDR = (1<<LowPin);					// Low pin to ground
      tmpval = (HighPin * 2);						// necessarily because of the arrangement of the resistances
      R_DDR |= (1<<tmpval);						// High pin on exit
      R_PORT |= (1<<tmpval);						// High pin over R_L on Vcc
      _delay_ms(20);
      adcv[2] = ReadADC(TristatePin);					// Voltage at the assumed gate measure

      if(adcv[2]>800) {							// MOSFET
	PartFound = PART_FET;						// N-channel-MOSFET
	PartMode = PART_MODE_N_D_MOS;					// Depletion MOSFET
      } else {								// JFET (pn transition between G and S leads)
	PartFound = PART_FET;						// N-channel-JFET
	PartMode = PART_MODE_N_JFET;
      }
      
      b = TristatePin;
      c = HighPin;
      e = LowPin;
    }
    
    ADC_PORT = 0x00;

									// Test on P-JFET or leading P-MOSFET
    ADC_DDR = (1<<LowPin);					// Low pin (assumed drain) to ground, Tristate pin (assumed gate) still is over R_H on pluses
    tmpval = (HighPin * 2);						// necessarily because of the arrangement of the resistances
									
    R_DDR |= (1<<tmpval);						// High pin on exit
    R_PORT |= (1<<tmpval);						// High pin over R_L on Vcc
    _delay_ms(20);
    adcv[1] = ReadADC(HighPin);						// Voltage at the assumed SOURCE measure
									
    R_PORT = (1<<tmpval);						// Tristate pin (assumed gate) over R_H to ground
    _delay_ms(20);
    adcv[2] = ReadADC(HighPin);						// Voltage at the assumed SOURCE measure again
									// - If it concerns a leading P-MOSFET or P-JFET, would have adcv [0] > adcv [1] its
    if(adcv[1]>(adcv[2]+100)) {
									// - Voltage at the gate measure, for distinction between MOSFET and JFET
      ADC_PORT = (1<<HighPin);				// High pin firmly on pluses
      ADC_DDR = (1<<HighPin);				// High pin on exit
      _delay_ms(20);
      adcv[2] = ReadADC(TristatePin);					// Voltage at the assumed gate measure
      
      if(adcv[2]<200) {							// MOSFET
	PartFound = PART_FET;						// P-channel-MOSFET 
	PartMode = PART_MODE_P_D_MOS;					// Depletion MOSFET
      } else {								// JFET (pn transition between G and S leads)
	PartFound = PART_FET;						// P-channel-JFET
	PartMode = PART_MODE_P_JFET;
      }

      b = TristatePin;
      c = LowPin;
      e = HighPin;
    }
  }
									// Pins erneut setzen - Pin set again
  tmpval = (LowPin * 2);						// necessarily because of the arrangement of the resistances
  R_DDR = (1<<tmpval);							// Low pin on exit and over R_L to ground
  R_PORT = 0;
  ADC_DDR = (1<<HighPin);					// High pin on exit
  ADC_PORT = (1<<HighPin);					// High pin to Vcc
  _delay_ms(5);	

  if(adcv[0] < 200) {							// If the Device does not have a passage between HighPin and LowPin
									// Test auf pnp
    tmpval2 = (TristatePin * 2);					// necessarily because of the arrangement of the resistances
    R_DDR |= (1<<tmpval2);						// Tristate pin over R_L to ground, to the test on pnp
    _delay_ms(2);
    adcv[1] = ReadADC(LowPin);						// Voltage measure
    
    if(adcv[1] > 700) {
									// Device leads => Pnp transistor or the like.
									// Amplification factor in both directions measure
      R_DDR = (1<<tmpval);						// Tristate pin (basis) high impedance
      tmpval2++;
      R_DDR |= (1<<tmpval2);						// Tristate pin (basis) over R_H to ground

      _delay_ms(10);
      adcv[1] = ReadADC(LowPin);					// Voltage at the Low pin (assumed collector) measure
      adcv[2] = ReadADC(TristatePin);					// Base voltage measure
									// Examine whether test already times run
      if((PartFound == PART_TRANSISTOR) || (PartFound == PART_FET)) 
	PartReady = 1;

      hfe[PartReady] = adcv[1];
	uBE[PartReady] = adcv[2];

      if(PartFound != PART_THYRISTOR) {
	if(adcv[2] > 200) {
	  PartFound = PART_TRANSISTOR;					// PNP transistor found (basis is " " upward; pulled)
	  PartMode = PART_MODE_PNP;
	} else {
	    if(adcv[0] < 20) {						// - Durchlassspannung in the closed condition small enough? (otherwise D-mode-FETs are falsely recognized as E-mode)
	      PartFound = PART_FET;					// P-channel-MOSFET found (basis/gate is not " " upward; pulled)
	      PartMode = PART_MODE_P_E_MOS;
									// Measurement of the gate threshold voltage
	      tmpval = (1<<LowPin);
	      tmpval2 = R_DDR;
	      ADMUX = TristatePin | (1<<REFS0);
	      gthvoltage = 0;

	      for(b=0;b<13;b++) {
		wdt_reset();
		DischargePin(TristatePin,1);

		while (!(ADC_PIN&tmpval));				// Control rooms, until the MOSFET scolded and on high goes to drain

		R_DDR = 0;
		ADCSRA |= (1<<ADSC);

		while (ADCSRA&(1<<ADSC));
		
		gthvoltage += (MAX_ADC - ADCW);
		R_DDR = tmpval2;
	      }

	      gthvoltage *= 3;						// Conversion in mV, together with the division by 8 (with the LCD announcement)
	    }
	  }
	  b = TristatePin;
	  c = LowPin;
	  e = HighPin;
	}
      }

									// Tristate (basis assumed) on pluses, to the test on npn
      ADC_PORT = 0x00;						// Low pin to ground
      tmpval = (TristatePin * 2);					// necessarily because of the arrangement of the resistances
      tmpval2 = (HighPin * 2);						// necessarily because of the arrangement of the resistances
      R_DDR = (1<<tmpval) | (1<<tmpval2);				// High pin and Tristate pin on exit
      R_PORT = (1<<tmpval) | (1<<tmpval2);				// High pin and Tristate pin over R_L on Vcc
      ADC_DDR = (1<<LowPin);					// Low pin on exit
      _delay_ms(10);
      adcv[1] = ReadADC(HighPin);					// Voltage at the High pin measure
      
      if(adcv[1] < 500) {
	if(PartReady == 1) goto testend;
									// Device leads => NPN transistor or the like.
									// Test on thyristor:
									// Gate unload
	R_PORT = (1<<tmpval2);						// Tristate pin (gate) over R_L to ground
	_delay_ms(10);
	R_DDR = (1<<tmpval2);						// Tristate pin (gate) high impedance
									
	_delay_ms(5);
	adcv[3] = ReadADC(HighPin);					// Again Voltage at the High pin (anode assumed) measure

	R_PORT = 0;							// High pin (anode assumed) to ground
	_delay_ms(5);
	R_PORT = (1<<tmpval2);						// High pin (anode assumed) pluses
	_delay_ms(5);
	adcv[2] = ReadADC(HighPin);					// Again Voltage at the High pin (anode assumed) measure

	if((adcv[3] < 500) && (adcv[2] > 900)) {			// After switching the holding current off the thyristor must close
									// was switched before disconnection of triggering Rome and is still switched although gate out => Thyristor
	  PartFound = PART_THYRISTOR;
									//  Test on Triac
	  R_DDR = 0;
	  R_PORT = 0;
	  ADC_PORT = (1<<LowPin);				// Low-Pin fest auf Plus - Low pin firmly on pluses
	  _delay_ms(5);
	  R_DDR = (1<<tmpval2);						// HighPin over R_L to ground
	  _delay_ms(5);

	  if(ReadADC(HighPin) > 50) 
	    goto savenresult;						// - Voltage at the High pin (more assumed a2) measure; if too highly: Device leads now => no triac

	  R_DDR |= (1<<tmpval);						// Gate also over R_L to ground => Triac would have to ignite
	  _delay_ms(5);

	  if(ReadADC(TristatePin) < 200) 
	    goto savenresult;						// - Voltage at the Tristate pin (assumed gate) measure; Abort if Voltage too small

	  if(ReadADC(HighPin) < 150) 
	    goto savenresult;						// Device does not lead now => no triac => Abort

	  R_DDR = (1<<tmpval2);						// TriState act pin (gate) again high impedance
	  _delay_ms(5);

	  if(ReadADC(HighPin) < 150) 
	    goto savenresult;    					// - Device does not lead after switching the gate stream off any longer => no triac => Abort

	  R_PORT = (1<<tmpval2);					// HighPin over R_L on pluses => Holding current out
	  _delay_ms(5);
	  R_PORT = 0;							// HighPin again over R_L to ground; Triac would have to now close
	  _delay_ms(5);

	  if(ReadADC(HighPin) > 50) 
	    goto savenresult;						// - Voltage at the High pin (more assumed a2) measure; if too highly: Device leads now => no triac

	  PartFound = PART_TRIAC;
	  PartReady = 1;
	  goto savenresult;
	}
									// Test on transistor or MOSFET
	tmpval++;
	R_DDR |= (1<<tmpval);						// Tristate pin (basis) on exit
	R_PORT |= (1<<tmpval);						// Tristate pin (basis) over R_H on pluses
	_delay_ms(50);
	adcv[1] = ReadADC(HighPin);					// Voltage at the High pin (assumed collector) measure
	adcv[2] = ReadADC(TristatePin);					// Base voltage measure
  
	if((PartFound == PART_TRANSISTOR) || (PartFound == PART_FET)) 
	  PartReady = 1;						// examine whether test already times run

	hfe[PartReady] = MAX_ADC - adcv[1];
	uBE[PartReady] = MAX_ADC - adcv[2];

	if(adcv[2] < 500) {
	  PartFound = PART_TRANSISTOR;					// NPN transistor found (basis is " " downward; pulled)
	  PartMode = PART_MODE_NPN;
	} else {
	    if(adcv[0] < 20) {						// - Durchlassspannung in the closed condition small enough? (otherwise D-mode-FETs are falsely recognized as E-mode)
	      PartFound = PART_FET;					// N-channel-MOSFET found (basis/gate is not " " downward; pulled)
	      PartMode = PART_MODE_N_E_MOS;
	      tmpval2 = R_DDR;						// Gate threshold voltage measure
	      tmpval=(1<<HighPin);
	      ADMUX = TristatePin | (1<<REFS0);
	      gthvoltage = 0;

	      for(b=0;b<13;b++) {
		wdt_reset();
		DischargePin(TristatePin,0);

		while ((ADC_PIN&tmpval));	 			// Control rooms, until the MOSFET scolded and on low goes to drain 

		R_DDR = 0;
		R_PORT = 0;
		ADCSRA |= (1<<ADSC);

		while (ADCSRA&(1<<ADSC));

		gthvoltage += ADCW;

		R_DDR = tmpval2;
		R_PORT = tmpval2;
	      }

	      gthvoltage *= 3;						// Conversion in mV, together with the division by 8 (with the LCD announcement)
	    }
	  }

	  savenresult:
	  b = TristatePin;
	  c = HighPin;
	  e = LowPin;
	}

	ADC_DDR = 0x00;
	ADC_PORT = 0x00;
									// Finished
	} else {							// Passage
									// Test auf Diode
	    tmpval2 = (2<<(2*HighPin));					// R_H
	    tmpval = (1<<(2*HighPin));					// R_L
	    ADC_PORT = 0x00;
	    ADC_DDR = (1<<LowPin);				// Low pin to ground, High pin still is over R_L on Vcc
	    DischargePin(TristatePin,1);				// Unloaded for P-channel-MOSFET
	    _delay_ms(5);
	    adcv[0] = ReadADC(HighPin) - ReadADC(LowPin);
	    R_DDR = tmpval2;						// High pin over R_H on pluses
	    R_PORT = tmpval2;
	    _delay_ms(5);
	    adcv[2] = ReadADC(HighPin) - ReadADC(LowPin);
	    R_DDR = tmpval;						// High pin over R_L on pluses
	    R_PORT = tmpval;
	    DischargePin(TristatePin,0);				// Unloaded for N-channel-MOSFET
	    _delay_ms(5);
	    adcv[1] = ReadADC(HighPin) - ReadADC(LowPin);
	    R_DDR = tmpval2;						// High pin over R_H on pluses
	    R_PORT = tmpval2;
	    _delay_ms(5);
	    adcv[3] = ReadADC(HighPin) - ReadADC(LowPin);
		/*      Without unloading it can come to wrong identifications, since the gate of a MOSFETs can be still loaded. 
		        The additional measurement with " gro�en" R_H resisted is accomplished, around antiparallel diodes of 
		        Resistances differentiate to be able.
		        A diode has a Durchlassspg relatively independent of the passage stream. 
		        With a resistance the voltage drop changes strongly (linear) with the river. 
		*/
	    if(adcv[0] > adcv[1]) {
	      adcv[1] = adcv[0];					// the higher value wins
	      adcv[3] = adcv[2];
	    }

	    if((adcv[1] > 30) && (adcv[1] < 950)) {			// Voltage is over 0,15V and under 4,64V => Ok one 
	      if((PartFound == PART_NONE) || (PartFound == PART_RESISTOR)) 
		PartFound = PART_DIODE;					// Diode by default.
									// Otherwise there would be problems with transistors with protection diode 
	      diodes[NumOfDiodes].Anode = HighPin;
	      diodes[NumOfDiodes].Cathode = LowPin;
	      diodes[NumOfDiodes].Voltage = (adcv[1]*54/11);		// - Multiply by approx 4,9, in order to receive from the ADC the Voltage in millivolts
	      NumOfDiodes++;

	      for(uint8_t i=0;i<NumOfDiodes;i++) {
		if((diodes[i].Anode == LowPin) && (diodes[i].Cathode == HighPin)) {	// two antiparallel diodes: Defectively or duo LED 
		  if((adcv[3]*64) < (adcv[1] / 5)) {			// Durchlassspannung falls with smaller test stream strongly off => Defectively
		    if(i<NumOfDiodes) {
		      for(uint8_t j=i;j<(NumOfDiodes-1);j++) {diodes[j].Anode = diodes[j+1].Anode;\
			                                      diodes[j].Cathode = diodes[j+1].Cathode;\
			                                      diodes[j].Voltage = diodes[j+1].Voltage;
		      }
		    }

		    NumOfDiodes -= 2;
	          }
	        }
	      }
	    }
	}

									// Test on resistance
	tmpval2 = (2<<(2*HighPin));					// R_H
	tmpval = (1<<(2*HighPin));					// R_L
	ADC_PORT = 0x00;
	ADC_DDR = (1<<LowPin);				// Low pin to ground
	R_DDR = tmpval;							// High pin over R_L on pluses
	R_PORT = tmpval;
	adcv[2] = ReadADC(LowPin);
	adcv[0] = ReadADC(HighPin) - adcv[2];
	R_DDR = tmpval2;						// High pin over R_H on pluses
	R_PORT = tmpval2;
	adcv[3] = ReadADC(LowPin);
	adcv[1] = ReadADC(HighPin) - adcv[3];

									// Measurement of the voltage difference between the positive terminal of R_L and R_H and Vcc
	tmpval2 = (2<<(2*LowPin));					// R_H
	tmpval = (1<<(2*LowPin));					// R_L
	ADC_DDR = (1<<HighPin);				// High pin on exit
	ADC_PORT = (1<<HighPin);				// High pin firmly on pluses
	R_PORT = 0;
	R_DDR = tmpval;							// Low pin over R_L to ground
	adcv[2] += (MAX_ADC - ReadADC(HighPin));
	R_DDR = tmpval2;						// Low pin over R_H to ground
	adcv[3] += (MAX_ADC - ReadADC(HighPin));

	if(((adcv[0] - adcv[2]) < 900) && ((adcv[1] - adcv[3]) > 20)) goto testend; 	// Voltage drops with small test stream not far enough

	if(((adcv[1] * 32) / 31) < adcv[0]) {				// - Sloping Voltage does not drop with smaller test stream strongly and it exists " Almost Kurzschluss" => Resistance
	  if((PartFound == PART_DIODE) || (PartFound == PART_NONE) || (PartFound == PART_RESISTOR)) {
	    if((tmpPartFound == PART_RESISTOR) && (ra == LowPin) && (rb == HighPin)) {
					/* The Device was tested already once with reverse polarity. Now compare both results with one another.
					If they are quite similar, it concerns (in all probability) a resistance. 
					*/
	      if(!((((adcv[0] + 100) * 6) >= ((rv[0] + 100) * 5)) &&  \
		   (((rv[0] + 100) * 6) >= ((adcv[0] + 100) * 5)) &&  \
		   (((adcv[1] + 100) * 6) >= ((rv[1] + 100) * 5)) &&  \
		   (((rv[1] + 100) * 6) >= ((adcv[1] + 100) * 5)))) {	// min. 20% deviation => no resistance
		tmpPartFound = PART_NONE;
		goto testend;
	      }

	      PartFound = PART_RESISTOR;
	    }

	    rv[0] = adcv[0];
	    rv[1] = adcv[1];

	    radcmax[0] = MAX_ADC - adcv[2];				// - V at the Low pin is not completely zero, but approximately 0,1V (however one measures).
	    radcmax[1] = MAX_ADC - adcv[3];
	    ra = HighPin;
	    rb = LowPin;
	    tmpPartFound = PART_RESISTOR;
	  }
	}

	testend:
	ADC_DDR = 0x00;
	ADC_PORT = 0x00;
	R_DDR = 0;
	R_PORT = 0;
}									// End of CheckPins()


void ReadCapacity(uint8_t HighPin, uint8_t LowPin) {
  if(PartFound == PART_CAPACITOR) 
    goto end;								// Already a condenser found
  
  unsigned long gcval = 0;
  unsigned int tmpint = 0;
  uint8_t extcnt = 0;
  uint8_t tmpx = 0;

  tmpval2 = (2<<(2*HighPin));						// R_H
  tmpval = (1<<(2*HighPin));						// R_L
  ADC_PORT = 0x00;
  R_PORT = 0;
  R_DDR = 0;
  ADC_DDR = (1<<LowPin);					// Low pin to ground
  R_DDR = tmpval2;							// HighPin over R_H to ground
  _delay_ms(5);
  adcv[0] = ReadADC(HighPin);
  DischargePin(HighPin,1);
  adcv[2] = ReadADC(HighPin);
  _delay_ms(5);
  adcv[1] = ReadADC(HighPin);
  wdt_reset();

  if((adcv[1] > (adcv[0] + 1)) || (adcv[2] > (adcv[0] + 1))) {		// Voltage rose
    R_DDR = tmpval;							// High pin over R_L to gnd
    
    while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
      wdt_reset();
      tmpint++;

      if(tmpint == 0) {
	extcnt++;

	if(extcnt == 30) 
	  break;							// High pin over R_L to gnd
      }
    }
    
    tmpint = 0;
    extcnt = 0;
    R_PORT = tmpval;							// High pin over R_L on pluses
    _delay_ms(5);
    adcv[2] = ReadADC(HighPin);
    _delay_ms(80);
    adcv[3] = ReadADC(HighPin);
    
    if((adcv[3] < (adcv[2] + 3)) && (adcv[3] < 850)) 
      goto end;								// Voltage is not considerably increasedly => Abort
    
    if((NumOfDiodes > 0) && (adcv[3] > 950) && (PartFound != PART_FET)) 
      goto end; 							// in all probability (or several) a diode (n) in check direction, which is otherwise falsely recognized as condenser
    	
    R_PORT = 0;
    
    while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
      wdt_reset();
      tmpint++;
      if(tmpint == 0) {
	extcnt++;

	if(extcnt == 30) 
	  break;							// Timeout for unloading
      }
    }
    
    tmpint = 0;
    extcnt = 0;
    ADC_DDR = 7;						// all pins on exit and from gnd
    R_PORT = tmpval;							// HighPin over R_L on pluses
    tmpval=(1<<HighPin);
    _delay_ms(2);
    ADC_DDR = (1<<LowPin);					// Condenser over R_L slowly load
    
    while (!(ADC_PIN & tmpval)) {					// Control rooms, until HighPin goes on High; Loop lasts 7 cycles
      wdt_reset();
      tmpint++;
      
      if(tmpint == 0) {
	extcnt++;

	if(extcnt == 30) 
	  break;							// Timeout for load
      }
    }

    if((extcnt == 0) && (tmpint<256)) {					// Low capacity
      ADC_DDR = (1<<LowPin);	
									// with R_H measure again
      R_PORT = 0;
      tmpint = 0;
      extcnt = 0;

      while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
	wdt_reset();
	tmpint++;

	if(tmpint == 0) {
	  extcnt++;

	  if(extcnt == 30) 
	    break;							// Timeout for unloading
	}
      }

      tmpint = 0;
      extcnt = 0;
      ADC_DDR = 7;						// all pins on exit
      ADC_PORT = 0x00;						// all pins to ground
      R_DDR = tmpval2;							// HighPin over R_H on exit
      R_PORT = tmpval2;							// HighPin over R_H on pluses
      _delay_ms(2);

      if(PartFound == PART_FET) 
	ADC_DDR = (7 & ~tmpval);				// - Condenser over R_H slowly load, free pin (drain) for gate capacity measurement on gnd
      else 
	ADC_DDR = (1<<LowPin);				// Condenser over R_H slowly load

      while (!(ADC_PIN & tmpval)) {					// Control rooms, until HighPin goes on High; Loop lasts 7 cycles
	wdt_reset();
	tmpint++;

	if(tmpint == 0) {
	  extcnt++;

	  if(extcnt == 30) 
	    break;							// Timeout for capacity measurement
	}
      }

      tmpx = 1;
    }

    if(tmpx) {
      gcval = eeprom_read_word(&H_CAPACITY_FACTOR);

      if((extcnt == 0) && (tmpint < 5)) 
	goto end;							// Capacity too small

      cv = 1;
    } else {
	gcval = eeprom_read_word(&L_CAPACITY_FACTOR);
	cv = 1000;
      }

    gcval *= (unsigned long)(((unsigned long)extcnt * 65536) + (unsigned long)tmpint);	// Unrechnen worth and store
    gcval /= 100;
    cv *= gcval;

    PartFound = PART_CAPACITOR;						// Condenser found

    ca = HighPin;
    cb = LowPin;
									// Condenser again unload
    tmpint = 0;
    extcnt = 0;
    R_DDR = (1<<(2*HighPin));						// High pin over R_L to ground
    R_PORT = 0;

    while(ReadADC(HighPin) > (ReadADC(LowPin) + 10)) {
      wdt_reset();
      tmpint++;

      if(tmpint == 0) {
	extcnt++;

	if(extcnt == 30) break;						// Timeout for unloading
      }
    }

    ADC_DDR = 7;						// Timeout for unloading
    ADC_PORT = 7;
    _delay_ms(10);
									// Finished
  }
  
  end:
  ADC_DDR = 0x00;
  ADC_PORT = 0x00;
  R_DDR = 0;
  R_PORT = 0; 
}									// End of ReadCapacity()


void DischargePin(uint8_t PinToDischarge, uint8_t DischargeDirection) {
	/* - Connection of an element briefly (10ms) on a certain potential put 
		 This function is intended to unloading of MOSFET Gates, 
		 in order to be able to recognize protection diodes etc. in MOSFETs Parameter: 
		 PinToDischarge: unloading pin DischargeDirection: 
		 0 =  gnd  (N-channel-FET), 
		 1 =  plus (P-channel-FET)
	*/
uint8_t tmpval;
  
  tmpval = (PinToDischarge * 2);					// necessarily because of the arrangement of the resistances

  if(DischargeDirection) 
    R_PORT |= (1<<tmpval);						// R_L out

  R_DDR |= (1<<tmpval);							// Pin on exit and over R_L to ground
  _delay_ms(10);
  R_DDR &= ~(1<<tmpval);						// Pin entrance
  
  if(DischargeDirection) 
    R_PORT &= ~(1<<tmpval);						// R_L out
}


void lcd_show_format_cap(char outval[], uint8_t strlength, uint8_t CommaPos) {
  if(strlength < 3) {
    if(strlength == 1) {
      lcd_string("0.");
      lcd_data('0');
      lcd_data(outval[0]);
    } else {								// strlength == 0, or strlength == 2
	lcd_string("0.");
	lcd_data(outval[0]);
	lcd_data(outval[1]);
      }
  } else {								// strlength >= 3
      for(PartReady = 0; PartReady < strlength; PartReady++) {
	if((PartReady + 2) == CommaPos) 
	  lcd_data('.');						// Should this be a comma?
	
	lcd_data(outval[PartReady]);
      }
    }
}

void ADC_init()
{
	//������������� ���������� ���������, ������������� �����
    //ADMUX = 0xB00011111;
	//ADCSRA =0x8D;
	//	ADCSRA = 0xB10001110;
//ADCSRA=(1<<ADEN)|(1<<ADPS1)|(1<<ADPS0); 	// Enable ADC, set Prescale to 8
//ADMUX = 0xB0101 | (1<<REFS0);
// ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Rrescalar div factor =128
//ADMUX=(0<<REFS1) | (0<<REFS0);  // For Internal=2.56V;
//ADMUX = 0xB0101 | (1<<REFS0);
// ADMUX = ADMUX |0xB01111111;
//  �������� ������� ADPS2:0, ADEN-���������� ���, ADIE-���������� ���������� �� ���, ADATE-0-1������� ��������������. 1-����������� ��������������. 
ADCSRA=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN)|(1<<ADIE)|(0<<ADATE); //Rrescalar div factor =128

// ADC initialization
// ADC Clock frequency: 125,000 kHz
// ADC Voltage Reference: AREF pin
// ADC High Speed Mode: Off
// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
// ADC4: On, ADC5: On, ADC6: On, ADC7: On
//ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);		// Enable ADC, set Prescale to 8
//DIDR0=0x00;
//ADMUX=0xB1111110;   //ADC_VREF_TYPE & 0xff; 1.1V
//ADCSRA=0x86;
//ADCSRB&=0x7F;

}

unsigned int ReadADC(uint8_t mux) {					// - ADC value of the indicated channel pick out and as unsigned int back against
  unsigned int adcx = 0;
ADMUX = mux | (1<<REFS1);
for(uint8_t j=0;j<20;j++) {						// 20 measurements; for better accuracy
ADCSRA |= (1<<ADSC);
while (ADCSRA&(1<<ADSC));
adcx += ADCW;
  }
  adcx /= 20;
  return adcx;
}

/* eof */
void ftos(float volts)
{
int f1,f2,f3;
float f2f;
f1=truncf(volts); //4
f2f=(volts-(float)f1)*10; //(4.25-4)*10=2.5
f2=truncf(f2f); //2
f3=truncf((f2f-f2)*10); //(2.5-2)*10=0.5*10=5
lcd_data(f1+ASCII_0);
lcd_data('.');
lcd_data(f2+ASCII_0);
lcd_data(f3+ASCII_0);
}

