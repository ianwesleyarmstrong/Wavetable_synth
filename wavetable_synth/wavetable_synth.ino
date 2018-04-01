#include <LiquidCrystal.h>
#include <waveforms.h>
LiquidCrystal LCDDriver(11,9,5,6,7,8);


///******** Load AVR timer interrupt macros ********/
//#include <avr/interrupt.h>
//
///******** Sine wave parameters ********/
//#define PI2     6.283185 // 2 * PI - saves calculating it later
//#define AMP     127      // Multiplication factor for the sine wave
//#define OFFSET  128      // Offset shifts wave to just positive values
//
///******** Lookup table ********/
//#define LENGTH  256  // The length of the waveform lookup table
//byte wave[LENGTH];   // Storage for the waveform


/*** Encoder ***/
volatile unsigned int EncoderPosition;

unsigned long timer;

int incomingByte = 0;
int notes[] = {478,451,426,402,379,358,338,319,301,284,268,253,239,225,213,201,190,179,169,159,151,142,134,127,119,113,106,100,95,
             89,84,80,75,71,67,63,60,56,53,50,47,45,42,40,38,36,34,32,20,28,27,25,24,22,21,20,19,18,17,16,15};

void setup() {

  Serial.begin(9600);

  LCDDriver.begin(16,2);
  LCDDriver.clear();

  timer = millis();

  /**** Setup pins for encoder ***/
  pinMode(2, INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);

  attachInterrupt(0,MonitorA, CHANGE);
  attachInterrupt(0, MonitorB, CHANGE);

  EncoderPosition = 0;
  EncoderPosition = constrain(EncoderPosition, 0, 61);
  //notes = {478,451,426,402,379,358,338,319,301,284,268,253,239,225,213,201,190,179,169,159,151,142,134,127,119,113,106,100,95,
    //         89,84,80,75,71,67,63,60,56,53,50,47,45,42,40,38,36,34,32,20,28,27,25,24,22,21,20,19,18,17,16,15};


  
  /******** Populate the waveform lookup table with a sine wave ********/
  for (int i=0; i<LENGTH; i++) {
    float v = (AMP*sin((PI2/LENGTH)*i));  // Calculate current entry
    wave[i] = int(v+OFFSET);              // Store value as integer
  }
 
  /******** Set timer1 for 8-bit fast PWM output ********/
  pinMode(9, OUTPUT);       // Make timer's PWM pin an output
  TCCR1B  = (1 << CS10);    // Set prescaler to full 16MHz
  TCCR1A |= (1 << COM1A1);  // PWM pin to go low when TCNT1=OCR1A
  TCCR1A |= (1 << WGM10);   // Put timer into 8-bit fast PWM mode
  TCCR1B |= (1 << WGM12); 

  /******** Set up timer 2 to call ISR ********/
  TCCR2A = 0;               // We need no options in control register A
  TCCR2B = (1 << CS21);     // Set prescaller to divide by 8
  TIMSK2 = (1 << OCIE2A);   // Set timer to call ISR when TCNT2 = OCRA2
  OCR2A = 15;               // sets the frequency of the generated wave
  sei();                    // Enable interrupts to generate waveform!
}

void MonitorA() {
  if (digitalRead(2) == digitalRead(3)) {
    EncoderPosition -= 1;
  }
  else {
    EncoderPosition += 1;
  }
}

void MonitorB() {
  if (digitalRead(2) == digitalRead(3)) {
    EncoderPosition += 1;
  }
  else {
    
    EncoderPosition -= 1;
  }
}



void loop() {  // Nothing to do!
  LCDDriver.clear();
  if (millis()- timer >= 1000){
    Serial.print(EncoderPosition);
    Serial.print('\n');
    Serial.print(incomingByte,DEC);
    Serial.print('\n');
    Serial.print(OCR2A);
    Serial.print('\n');
    Serial.print('\n');
  
    timer += 1000;
  }
  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    OCR2A = notes[incomingByte];
    
  }
  LCDDriver.clear();
  
  //LCDDriver.print(EncoderPosition);
  //OCR2A = EncoderPosition;
  OCR2A = notes[EncoderPosition];
  //OCR2A = notes[EncoderPosition/4];
  
  
}

/******** Called every time TCNT2 = OCR2A ********/
ISR(TIMER2_COMPA_vect) {  // Called each time TCNT2 == OCR2A
  static byte index=0;    // Points to successive entries in the wavetable
  OCR1AL = waveformsTable[wave0][0]]; // Update the PWM output
  asm("NOP;NOP");         // Fine tuning
  TCNT2 = 6;              // Timing to compensate for time spent in ISR
}
