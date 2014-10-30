
/* Autonomous Guitar - MIDI Controlled
   Thomas Karagianes
   April 22, 2014
   ARPEGGIO (Automatic Robot Playing Excellent Guitar Guided by mIdi Outputs) 
   [I recognize that this is quite a stretch for an acronym - deal with it]
   This sketch controls the Autonomous Guitar Playing Robot via MIDI commands.
   8-bit serial shift registers control the fret solenoids and strumming lock-
   actuators, and are updated with bit-specific addressing of a byte array 
   (acts like the port registers).
*/
//********************************************************************************************************
// Library Initializations
#include <MIDI.h>

// Pin Definitions
const byte latchPin = 7;
const byte clockPin = 13;
const byte dataPin = 11;
const byte reset = 8;
const byte row[6] = {2,3,4,8,9,10};
// const byte strumLatchPin = 6;  //As far as I can tell, the strumLatchPin isn't used.

// Global Variables
/*Contains 6 bytes that act as the "registers" for the 74HC595. Each of these bits map to a fret solenoid
or a strumming actuator. Writing a one to the bit activates the associated hardware. Each strummer has 2 
control bits, mapping to the 2 inputs of the H-bridge; one activates a forward strum, the other a reverse
strum.

*** Bit Mapping:
              7      6      5      4      3      2       1        0  
  shift[0]  NULL   Fret5  Fret4  Fret3  Fret2  Fret1  REVStrum FORStrum    // Maps to String 1
  shift[1]  NULL   Fret5  Fret4  Fret3  Fret2  Fret1  REVStrum FORStrum    // Maps to String 2
  shift[2]  NULL   Fret5  Fret4  Fret3  Fret2  Fret1  REVStrum FORStrum    // Maps to String 3
  shift[3]  NULL   Fret5  Fret4  Fret3  Fret2  Fret1  REVStrum FORStrum    // Maps to String 4
  shift[4]  NULL   Fret5  Fret4  Fret3  Fret2  Fret1  REVStrum FORStrum    // Maps to String 5
  shift[5]  NULL   Fret5  Fret4  Fret3  Fret2  Fret1  REVStrum FORStrum    // Maps to String 6
*/
byte shift[6] = {0x00,0x00,0x00,0x00,0x00,0x00};      // Shift register array
byte adjusted[6] = {0x00,0x00,0x00,0x00,0x00,0x00};   // Shift register array adjusted for current pins

byte debug[8] = {2,3,4,5,6,8,9,10};
//********************************************************************************************************
void setup() {
  // Initiate MIDI communications, channel 1
  MIDI.begin(1); 
  Serial.begin(115200);  
  MIDI.setHandleNoteOn(HandleNoteOn);    // Set function called upon receipt of NOTE_ON Command
  MIDI.setHandleNoteOff(HandleNoteOff);  // Set function called upon receipt of NOTE_OFF command
  // Set pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(reset, OUTPUT);
  //digitalWrite(reset,HIGH);
  //pinMode(strumlatchPin, OUTPUT);
  
  // ***DEBUG LEDS
  for(byte i = 0; i<6; i++) {
    pinMode(row[i],OUTPUT); }
} 
//********************************************************************************************************
void loop() {
    // Call MIDI.read the fastest you can for real-time performance.
    MIDI.read();
    while(Serial.available() == 0) {updateDisplay();}
    // There is no need to check if there are messages incoming 
    // if they are bound to a Callback function.
    // The attached method will be called automatically 
    // when the corresponding message has been received.
}
//********************************************************************************************************
void HandleNoteOn(byte channel, byte pitch, byte velocity) {
    // Callback function that calls below code when Arduino receives NOTE_ON command
  if(channel == 1) {
    pitch = octaveShift(pitch);
    translate(pitch, velocity);    // Convert MIDI bytes into correlated shift bytes
    adjustMatrix();
    
//    digitalWrite(latchPin, LOW);                         // Prepares shift registers to receive data
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[0]);  // Send out byte to control 1st string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[1]);  // Send out byte to control 2nd string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[2]);  // Send out byte to control 3rd string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[3]);  // Send out byte to control 4th string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[4]);  // Send out byte to control 5th string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[5]);  // Send out byte to control 6th string
//    digitalWrite(latchPin, HIGH);                        // Signals end of data, registers send out values
//
//    digitalWrite(latchPin, LOW);                         // Prepares shift registers to receive data
//    shiftOut(dataPin, clockPin, MSBFIRST, 0);            // Send out byte to control 1st string
//    shiftOut(dataPin, clockPin, MSBFIRST, 0);            // Send out byte to control 2nd string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[2]);  // Send out byte to control 3rd string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[3]);  // Send out byte to control 4th string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[4]);  // Send out byte to control 5th string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[5]);  // Send out byte to control 6th string
//    digitalWrite(latchPin, HIGH);                        // Signals end of data, registers send out values
  }
} 
//********************************************************************************************************
void HandleNoteOff(byte channel, byte pitch, byte velocity) {
    // Callback function that calls below code when Arduino receives NOTE_OFF command
  if(channel == 1) {
    pitch = octaveShift(pitch);
    translate(pitch, 0);    // Convert MIDI bytes into correlated shift bytes
    adjustMatrix();
    
//    digitalWrite(latchPin, LOW);                         // Prepares shift registers to receive data
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[0]);  // Send out byte to control 1st string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[1]);  // Send out byte to control 2nd string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[2]);  // Send out byte to control 3rd string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[3]);  // Send out byte to control 4th string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[4]);  // Send out byte to control 5th string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[5]);  // Send out byte to control 6th string
//    digitalWrite(latchPin, HIGH);                        // Signals end of data, registers send out values
//    
//    digitalWrite(latchPin, LOW);                         // Prepares shift registers to receive data
//    shiftOut(dataPin, clockPin, MSBFIRST, 0);            // Send out byte to control 1st string
//    shiftOut(dataPin, clockPin, MSBFIRST, 0);            // Send out byte to control 2nd string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[2]);  // Send out byte to control 3rd string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[3]);  // Send out byte to control 4th string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[4]);  // Send out byte to control 5th string
//    shiftOut(dataPin, clockPin, MSBFIRST, adjusted[5]);  // Send out byte to control 6th string
//    digitalWrite(latchPin, HIGH);                        // Signals end of data, registers send out values
  }
}
//********************************************************************************************************
void pushDownFret(byte fret, byte string) { 
  /* Function uses bit-specific addressing in shift array to write 1's on given string byte.
     Bit-specific addressing allows for changing only the specific bit, leaving other bits,
     like the strum bits untouched. 1 activates bit-associated fret solenoid.
  */
  byte x = string-1;                // Shift array is zero indexed; thus string 6 refers to shift[5]
  shift[x] = shift[x] & 0x03;       // Clear shift array; release any frets currently active
  if(fret == 0) {
    shift[x] = shift[x] | 0x00;}    // Write 1 to fret 0 bit (no register bit)
  if(fret == 1) {
    shift[x] = shift[x] | 0x04;}    // Write 1 to fret 1 bit (register bit 2)
  if(fret == 2) {
    shift[x] = shift[x] | 0x08;}    // Write 1 to fret 2 bit (register bit 3)
  if(fret == 3) {
    shift[x] = shift[x] | 0x10;}    // Write 1 to fret 3 bit (register bit 4)
  if(fret == 4) {
    shift[x] = shift[x] | 0x20;}    // Write 1 to fret 4 bit (register bit 5)
  if(fret == 5) {
    shift[x] = shift[x] | 0x40;}    // Write 1 to fret 5 bit (register bit 6)
}
//********************************************************************************************************
void releaseFret(byte fret, byte string) {
  /* Function uses bit-specific addressing in shift array to write 0's on given string byte.
     Bit-specific addressing allows for changing only the specific bit, leaving other bits,
     like the strum bits untouched. 0 deactivates bit-associated fret solenoid.
  */
  byte x = string-1;                 // Shift array is zero indexed; thus string 6 refers to shift[5]
  if(fret == 0) {
    shift[x] = shift[x] & ~(0x00);}  // Write 0 to fret 0 bit (no register bit)
  if(fret == 1) {
    shift[x] = shift[x] & ~(0x04);}  // Write 0 to fret 1 bit (register bit 2)
  if(fret == 2) {
    shift[x] = shift[x] & ~(0x08);}  // Write 0 to fret 2 bit (register bit 3)
  if(fret == 3) {
    shift[x] = shift[x] & ~(0x10);}  // Write 0 to fret 3 bit (register bit 4)
  if(fret == 4) {
    shift[x] = shift[x] & ~(0x20);}  // Write 0 to fret 4 bit (register bit 5)
  if(fret == 5) {
    shift[x] = shift[x] & ~(0x40);}  // Write 0 to fret 5 bit (register bit 6)
}
//********************************************************************************************************
void strum(byte string) {
     /* Function uses bit-specific addressing in shift array to write 1's on given string byte.
     Bit-specific addressing allows for changing only the specific bit, leaving other bits,
     like the strum bits untouched. 1 activates bit-associated strumming lock actuators
     */
  byte x = string-1;                  // Shift array is zero indexed; thus string 6 refers to shift[5]
  byte forward = bitRead(shift[x],0); // Read shift forward strum bit for given string
  if (forward == 0) {                 // If guitar hasn't strummed, can strum forward
    shift[x] = shift[x] | 0x01;       // Write 1 to strum forward bit (register bit 0)
    shift[x] = shift[x] & ~(0x02);    // Clear strum reverse bit (register bit 1)
  }
  else {                              // If guitar has strummed, need to reset by reverse strum
    shift[x] = shift[x] & ~(0x01);    // Clear strum forward bit (register bit 0)
    shift[x] = shift[x] | 0x02;       // Write 1 to strum reverse bit (register bit 1)
  }
}
//********************************************************************************************************
void translate(byte pitch, byte velocity) {
  switch (pitch) {
    case 40: //E2 (0th Fret, 6th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(0,6); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(0,6);
        strum(6); }
      break;
    case 41: //F2 (1st Fret, 6th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(1,6); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(1,6);
        strum(6); }
      break;
    case 42: //F#2 (2nd Fret, 6th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(2,6); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(2,6);
        strum(6); }
      break;
    case 43: //G2 (3rd Fret, 6th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(3,6); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(3,6);
        strum(6); }
      break;
    case 44: //G#2 (4th Fret, 6th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(4,6); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(4,6);
        strum(6); }
      break;
    case 45: //A2 (0th Fret, 5th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(0,5); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(0,5);
        strum(5); }
      break;
    case 46: //A#2 (1st Fret, 5th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(1,5); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(1,5);
        strum(5); }
      break;
    case 47: //B2 (2nd Fret, 5th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(2,5); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(2,5);
        strum(5); }
      break;
    case 48: //C2 (3rd Fret, 5th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(3,5); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(3,5);
        strum(5); }
      break;
    case 49: //C#3 (4th Fret, 5th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(4,5); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(4,5);
        strum(5); }
      break;
    case 50: //D3 (0th Fret, 4th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(0,4); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(0,4);
        strum(4); }
      break;
    case 51: //D#3 (1st Fret, 4th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(1,4); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(1,4);
        strum(4); }
      break;
    case 52: //E3 (2nd Fret, 4th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(2,4); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(2,4);
        strum(4); }
      break;
    case 53: //F3 (3rd Fret, 4th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(3,4); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(3,4);
        strum(4); }
      break;
    case 54: //F#3 (4th Fret, 4th String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(4,4); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(4,4);
        strum(4); }
      break;
    case 55: //G3 (0th Fret, 3rd String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(0,3); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(0,3);
        strum(3); }
      break;
    case 56: //G#3 (1st Fret, 3rd String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(1,3); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(1,3);
        strum(3); }
      break;
    case 57: //A3 (2nd Fret, 3rd String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(2,3); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(2,3);
        strum(3); }
      break;
    case 58: //A#3 (3rd Fret, 3rd String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(3,3); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(3,3);
        strum(3); }
      break;
    case 59: //B3 (0th Fret, 2nd String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(0,2); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(0,2);
        strum(2); }
      break;
    case 60: //C4 (1st Fret, 2nd String) (Middle C)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(1,2); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(1,2);
        strum(2); }
      break;
    case 61: //C#4 (2nd Fret, 2nd String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(2,2); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(2,2);
        strum(2); }
      break;
    case 62: //D4 (3rd Fret, 2nd String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(3,2); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(3,2);
        strum(2); }
      break;
    case 63: //D#4 (4th Fret, 2nd String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(4,2); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(4,2);
        strum(2); }
      break;
    case 64: //E4 (0th Fret, 1st String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(0,1); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(0,1);
        strum(1); }
      break;
    case 65: //F4 (1st Fret, 1st String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(1,1); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(1,1);
        strum(1); }
      break;
    case 66: //F#4 (2nd Fret, 1st String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(2,1); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(2,1);
        strum(1); }
      break;
    case 67: //G4 (3rd Fret, 1st String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(3,1); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(3,1);
        strum(1); }
    case 68: //G#4 (4th Fret, 1st String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(4,1); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(4,1);
        strum(1); }
      break;
    case 69: //A4 (5th Fret, 1st String)
      if(velocity == 0) {    // Velocity = 0 acts as a Note Off message
        releaseFret(5,1); }
      else {                 // No velocity sensitivity currently; all velocities act as Note On
        pushDownFret(5,1);
        strum(1); }
      break;
    default:
      break;
  }
}
//********************************************************************************************************
byte octaveShift(byte pitch) {
  pitch -= 12;
  //if(pitch>=0 && pitch<=15) {pitch += 48;}
  //if(pitch>15 && pitch<=39) {pitch += 24;}
  //if(pitch>=70 && pitch<94) {pitch -= 24;}
  //if(pitch>=94 && pitch<118) {pitch -= 48;}
  //if(pitch>=118 && pitch<=127) {pitch -=72;}
  return pitch;
}
//********************************************************************************************************
void adjustMatrix(void) {
  /* Function adjusts bit mapping of shift registers to match current electrical setup. The bit 
  mapping of the shift array is setup for ease in coding the fretting and strumming functions.
  The real shift registers are connected via the following pin mapping: 
  *** Pin Mapping:
                7      6       5     4      3      2       1      0  
  adjusted[0]  FORW2  REV2   FORW3  REV3   DEAD   DEAD   FORW1   REV1    // Maps to Strummers 1,2,3
  adjusted[1]  DEAD   DEAD   FORW6  REV6   FORW5  REV5   FORW4   REV4    // Maps to Strummers 4,5,6
  adjusted[2]  S2,F3  S2,F2  S2,F1  S1,F5  S1,F4  S1,F3  S1,F2   S1,F1   // Maps to String 1,2 frets
  adjusted[3]  S4,F1  S3,F5  S3,F4  S3,F3  S3,F2  S3,F1  S2,F5   S2,F4   // Maps to String 2,3,4 frets
  adjusted[4]  S5,F4  S5,F3  S5,F2  S5,F1  S4,F5  S4,F4  S4,F3   S4,F2   // Maps to String 4,5 frets
  adjusted[5]  DEAD   DEAD   S6,F5  S6,F4  S6,F3  S6,F2  S6,F1   S5,F5   // Maps to String 5,6 frets
  */
  
  for(byte i=0; i<6; i++) {
    for(byte j=0; j<7; j++) {
    boolean x = bitRead(shift[i],j); 
    if(i==0 && j==0) {bitWrite(adjusted[0],1,x);}  // Map S[0]bit0 to A[0]bit1 (String 1 Forward Strum)
    if(i==0 && j==1) {bitWrite(adjusted[0],0,x);}  // Map S[0]bit1 to A[0]bit0 (String 1 Reverse Strum)
    if(i==0 && j==2) {bitWrite(adjusted[2],0,x);}  // Map S[0]bit2 to A[2]bit0 (String 1 Fret 1)
    if(i==0 && j==3) {bitWrite(adjusted[2],1,x);}  // Map S[0]bit3 to A[2]bit1 (String 1 Fret 2)
    if(i==0 && j==4) {bitWrite(adjusted[2],2,x);}  // Map S[0]bit4 to A[2]bit2 (String 1 Fret 3)
    if(i==0 && j==5) {bitWrite(adjusted[2],3,x);}  // Map S[0]bit5 to A[2]bit3 (String 1 Fret 4)
    if(i==0 && j==6) {bitWrite(adjusted[2],4,x);}  // Map S[0]bit6 to A[2]bit4 (String 1 Fret 5)

    if(i==1 && j==0) {bitWrite(adjusted[0],7,x);}  // Map S[1]bit0 to A[0]bit7 (String 2 Forward Strum)
    if(i==1 && j==1) {bitWrite(adjusted[0],6,x);}  // Map S[1]bit1 to A[0]bit6 (String 2 Reverse Strum)
    if(i==1 && j==2) {bitWrite(adjusted[2],5,x);}  // Map S[1]bit2 to A[2]bit5 (String 2 Fret 1)
    if(i==1 && j==3) {bitWrite(adjusted[2],6,x);}  // Map S[1]bit3 to A[2]bit6 (String 2 Fret 2)
    if(i==1 && j==4) {bitWrite(adjusted[2],7,x);}  // Map S[1]bit4 to A[2]bit7 (String 2 Fret 3)
    if(i==1 && j==5) {bitWrite(adjusted[3],0,x);}  // Map S[1]bit5 to A[3]bit0 (String 2 Fret 4)
    if(i==1 && j==6) {bitWrite(adjusted[3],1,x);}  // Map S[1]bit6 to A[3]bit1 (String 2 Fret 5)

    if(i==2 && j==0) {bitWrite(adjusted[0],5,x);}  // Map S[2]bit0 to A[0]bit5 (String 3 Forward Strum)
    if(i==2 && j==1) {bitWrite(adjusted[0],4,x);}  // Map S[2]bit1 to A[0]bit4 (String 3 Reverse Strum)
    if(i==2 && j==2) {bitWrite(adjusted[3],2,x);}  // Map S[2]bit2 to A[3]bit2 (String 3 Fret 1)
    if(i==2 && j==3) {bitWrite(adjusted[3],3,x);}  // Map S[2]bit3 to A[3]bit3 (String 3 Fret 2)
    if(i==2 && j==4) {bitWrite(adjusted[3],4,x);}  // Map S[2]bit4 to A[3]bit4 (String 3 Fret 3)
    if(i==2 && j==5) {bitWrite(adjusted[3],5,x);}  // Map S[2]bit5 to A[3]bit5 (String 3 Fret 4)
    if(i==2 && j==6) {bitWrite(adjusted[3],6,x);}  // Map S[2]bit6 to A[3]bit6 (String 3 Fret 5)

    if(i==3 && j==0) {bitWrite(adjusted[1],1,x);}  // Map S[3]bit0 to A[1]bit1 (String 4 Forward Strum)
    if(i==3 && j==1) {bitWrite(adjusted[1],0,x);}  // Map S[3]bit1 to A[1]bit0 (String 4 Reverse Strum)
    if(i==3 && j==2) {bitWrite(adjusted[3],7,x);}  // Map S[3]bit2 to A[3]bit7 (String 4 Fret 1)
    if(i==3 && j==3) {bitWrite(adjusted[4],0,x);}  // Map S[3]bit3 to A[4]bit0 (String 4 Fret 2)
    if(i==3 && j==4) {bitWrite(adjusted[4],1,x);}  // Map S[3]bit4 to A[4]bit1 (String 4 Fret 3)
    if(i==3 && j==5) {bitWrite(adjusted[4],2,x);}  // Map S[3]bit5 to A[4]bit2 (String 4 Fret 4)
    if(i==3 && j==6) {bitWrite(adjusted[4],3,x);}  // Map S[3]bit6 to A[4]bit3 (String 4 Fret 5)
   
    if(i==4 && j==0) {bitWrite(adjusted[1],3,x);}  // Map S[4]bit0 to A[1]bit3 (String 5 Forward Strum)
    if(i==4 && j==1) {bitWrite(adjusted[1],2,x);}  // Map S[4]bit1 to A[1]bit2 (String 5 Reverse Strum)
    if(i==4 && j==2) {bitWrite(adjusted[4],4,x);}  // Map S[4]bit2 to A[4]bit4 (String 5 Fret 1)
    if(i==4 && j==3) {bitWrite(adjusted[4],5,x);}  // Map S[4]bit3 to A[4]bit5 (String 5 Fret 2)
    if(i==4 && j==4) {bitWrite(adjusted[4],6,x);}  // Map S[4]bit4 to A[4]bit6 (String 5 Fret 3)
    if(i==4 && j==5) {bitWrite(adjusted[4],7,x);}  // Map S[4]bit5 to A[4]bit7 (String 5 Fret 4)
    if(i==4 && j==6) {bitWrite(adjusted[5],0,x);}  // Map S[4]bit6 to A[5]bit0 (String 5 Fret 5)
    
    if(i==5 && j==0) {bitWrite(adjusted[1],5,x);}  // Map S[5]bit0 to A[1]bit5 (String 6 Forward Strum)
    if(i==5 && j==1) {bitWrite(adjusted[1],4,x);}  // Map S[5]bit1 to A[1]bit4 (String 6 Reverse Strum)
    if(i==5 && j==2) {bitWrite(adjusted[5],1,x);}  // Map S[5]bit2 to A[5]bit1 (String 6 Fret 1)
    if(i==5 && j==3) {bitWrite(adjusted[5],2,x);}  // Map S[5]bit3 to A[5]bit2 (String 6 Fret 2)
    if(i==5 && j==4) {bitWrite(adjusted[5],3,x);}  // Map S[5]bit4 to A[5]bit3 (String 6 Fret 3)
    if(i==5 && j==5) {bitWrite(adjusted[5],4,x);}  // Map S[5]bit5 to A[5]bit4 (String 6 Fret 4)
    if(i==5 && j==6) {bitWrite(adjusted[5],5,x);}  // Map S[5]bit6 to A[5]bit5 (String 6 Fret 5)
    }
  }
}
//********************************************************************************************************
void updateDisplay(void) {
  //Repeat for every row of LED matrix
  for (byte thisRow = 0; thisRow < 6; thisRow++) {
    //Turn row on (5V to common anode for given row)
    digitalWrite(row[thisRow], HIGH);
    
    digitalWrite(latchPin, LOW);                         
    shiftOut(dataPin, clockPin, MSBFIRST, ~(shift[thisRow]));
    digitalWrite(latchPin, HIGH);
    delay(1);
    
    digitalWrite(row[thisRow], LOW);
  }
}
//********************************************************************************************************
/*
void strum(byte string) {
     Function uses bit-specific addressing in shift array to write 1's on given string byte.
     Bit-specific addressing allows for changing only the specific bit, leaving other bits,
     like the strum bits untouched. 1 activates bit-associated strumming lock actuators

  byte x = string-1;                // Shift array is zero indexed; thus string 6 refers to shift[5]
  if (strumStatus[x] == false) {    // If guitar hasn't strummed, can strum forward
    bitClear(shift[x],1);           // Clear strum reverse bit (register bit 1)
    bitSet(shift[x],0);             // Write 1 to strum forward bit (register bit 0)
    strumStatus[x] = true;          // Set strumStatus for string to 1 (forward strum) 
  }
  if (strumStatus[x] == true) {     // If guitar has strummed, need to reset by reverse strum
    bitClear(shift[x],0);           // Clear strum forward bit (register bit 0)
    bitSet(shift[x],1)   ;          // Write 1 to strum reverse bit (register bit 1)
    strumStatus[x] = false;         // Set strumStatus for string to 0 (reset/reverse strum)
  }
}

void pushDownFret(byte fret, byte string) {
     Function uses bit-specific addressing in shift array to write 1's on given string byte.
     Bit-specific addressing allows for changing only the specific bit, leaving other bits,
     like the strum bits untouched. 1 activates bit-associated fret solenoid.

  byte x = string-1;                // Shift array is zero indexed; thus string 6 refers to shift[5]
  shift[x] = shift[x] & 0x00;       // Clear shift array; release any frets currently active
  if(fret == 0) {
    shift[x] = shift[x] | 0x00;}    // Write 1 to fret 0 bit (no register bit)
  else {
    bitSet(shift[x],fret+1);}
}

void releaseFret(byte fret, byte string) {
     Function uses bit-specific addressing in shift array to write 0's on given string byte.
     Bit-specific addressing allows for changing only the specific bit, leaving other bits,
     like the strum bits untouched. 0 deactivates bit-associated fret solenoid.

  byte x = string-1;                 // Shift array is zero indexed; thus string 6 refers to shift[5]
  if(fret == 0) {
    shift[x] = shift[x] & ~(0x00);}  // Write 0 to fret 0 bit (no register bit)
  else {
    bitClear(shift[x],fret+1);}
}

void pushDownFret(byte fret, byte string) {
     Function uses bit-specific addressing in shift array to write 1's on given string byte.
     Bit-specific addressing allows for changing only the specific bit, leaving other bits,
     like the strum bits untouched. 1 activates bit-associated fret solenoid.

  byte x = string-1;                // Shift array is zero indexed; thus string 6 refers to shift[5]
  shift[x] &= 0x00;       // Clear shift array; release any frets currently active
  if(fret == 0) {
    shift[x] |= 0x00;}    // Write 1 to fret 0 bit (no register bit)
  if(fret == 1) {
    shift[x] |= 0x04;}    // Write 1 to fret 1 bit (register bit 2)
  if(fret == 2) {
    shift[x] |= 0x08;}    // Write 1 to fret 2 bit (register bit 3)
  if(fret == 3) {
    shift[x] |= 0x10;}    // Write 1 to fret 3 bit (register bit 4)
  if(fret == 4) {
    shift[x] |= 0x20;}    // Write 1 to fret 4 bit (register bit 5)
  if(fret == 5) {
    shift[x] |= 0x40;}    // Write 1 to fret 5 bit (register bit 6)
}

void releaseFret(byte fret, byte string) {
     Function uses bit-specific addressing in shift array to write 0's on given string byte.
     Bit-specific addressing allows for changing only the specific bit, leaving other bits,
     like the strum bits untouched. 0 deactivates bit-associated fret solenoid.

  byte x = string-1;                 // Shift array is zero indexed; thus string 6 refers to shift[5]
  if(fret == 0) {
    shift[x] &= ~(0x00);}  // Write 0 to fret 0 bit (no register bit)
  if(fret == 1) {
    shift[x] &= ~(0x04);}  // Write 0 to fret 1 bit (register bit 2)
  if(fret == 2) {
    shift[x] &= ~(0x08);}  // Write 0 to fret 2 bit (register bit 3)
  if(fret == 3) {
    shift[x] &= ~(0x10);}  // Write 0 to fret 3 bit (register bit 4)
  if(fret == 4) {
    shift[x] &= ~(0x20);}  // Write 0 to fret 4 bit (register bit 5)
  if(fret == 5) {
    shift[x] &= ~(0x40);}  // Write 0 to fret 5 bit (register bit 6)
}

*/
//********************************************************************************************************
//--------------------------------------------------------------------------------------------------------
