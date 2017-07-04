#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

Adafruit_MPR121 touchSensor = Adafruit_MPR121();

uint16_t touchVal = 0;
uint16_t lastTVal = 0;
int pot1 = A0;
int pot2 = A2;
int ledPin = 13;
int button1 = 2;
boolean bState1 = LOW;
int button2 = 3;
boolean bState2 = LOW;
int octaveLEDPins [10] = {13, 12, 11, 10, 9, 8, 7, 6, 5, 4};
int velocity = 0;
int octave = 0;
int midiNotes [12] = {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71};

void ShutOffOtherLEDs();
void blinkLED(int led = 0);
void DebugMessageFromAdafruit();
void ChangeOctave(int notes[12], int oct);
boolean DebounceButton(int bPin, boolean bState);


void setup() 
{
  
  Serial.begin(9600);
  DebugMessageFromAdafruit();
  Serial.print("Octave value "); Serial.println(octave);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(ledPin, OUTPUT);

  for(int i = 0; i < 10; i++)
  {
    pinMode(octaveLEDPins[i], OUTPUT);  
  }
}

void loop() 
{
  touchVal = touchSensor.touched();
  velocity = map(analogRead(pot1), 0, 1023, 0, 127);
  //Serial.print("Touch value "); Serial.println(touchVal); //Debug Output
  Serial.print("Value on Velocity pot: "); Serial.println(velocity);
  //Serial.print("Value on pot 2: "); Serial.println(analogRead(pot2));
  
  
  if(DebounceButton(button2,bState2) == HIGH && bState2 == LOW)
  {
    Serial.println("BUTTON PUSHED!!!! WHOOO!!!");
    octave--;
    Serial.print("Octave value "); Serial.println(octave);
    ChangeOctave(midiNotes, octave);
    bState2 = HIGH;
  }
  else if(DebounceButton(button2,bState2) == LOW && bState2 == HIGH)
  {
    bState2 = LOW;  
  }
  
  if(DebounceButton(button1,bState1) == HIGH && bState1 == LOW)
  {
    Serial.println("BUTTON PUSHED!!!! YEAH!!!");
    octave++;
    Serial.print("Octave value "); Serial.println(octave);
    ChangeOctave(midiNotes, octave);
    bState1 = HIGH;
  }
  else if(DebounceButton(button1,bState1) == LOW && bState1 == HIGH)
  {
    bState1 = LOW;  
  }
  
  for(int i = 0; i < 12; i++)
  {
    if((bitRead(touchVal, i)) && !(bitRead(lastTVal, i)))
    {
      //Serial.print("Touched Pin Number "); Serial.println(i); 
      Serial.print("Playing Note with Value: "); Serial.println(midiNotes[i]);
      MIDI.sendNoteOn(midiNotes[i], velocity, 1);
      blinkLED();
      
    }

    if(!(bitRead(touchVal, i)) && (bitRead(lastTVal, i)))
    {
      Serial.print("Released Pin Number "); Serial.println(i);
      MIDI.sendNoteOff(midiNotes[i], velocity, 1);
      Serial.print("Stop Playing Note with Value: "); Serial.println(midiNotes[i]);
      blinkLED(); 
    }
  } 
  
  
  lastTVal = touchVal;
}

void blinkLED(int led = 0)
{
  if(led == 0)
  { 
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);   
    return;
  }
}


void ChangeOctave(int notes[12], int oct)
{
  if(oct > 5 || oct < -5 || oct == 0)
  {
    if(oct == 0)
    {
      ShutOffOtherLEDs();
      return;
    }
    
    Serial.println("ERROR!!! ERROR!!! BLLELELELELE!!! OUT OF OCTAVE RANGE");
    return;  
  }
  
  if(oct > 0)
  {
    for(int i = 0; i < 12; i++)
    {
      notes[i] += 12; 
    }

    ShutOffOtherLEDs();
    digitalWrite(octaveLEDPins[oct+4], HIGH);
  }
  else if(oct < 0)
  {
    for(int i = 0; i < 12; i++)
    {
      notes[i] -= 12; 
    }

    ShutOffOtherLEDs();
    digitalWrite(octaveLEDPins[oct+5], HIGH);
  }

}

boolean DebounceButton(int bPin, boolean bState)
{
  boolean currState = digitalRead(bPin);
  if(bState != currState)
  {
    delay(10);
    currState = digitalRead(bPin);  
  }
  return currState;
}

void ShutOffOtherLEDs()
{
    for(int i = 0; i < 10; i++)
    {
      if(digitalRead(octaveLEDPins[i]) == HIGH)
        digitalWrite(octaveLEDPins[i], LOW);
    }
}

void DebugMessageFromAdafruit()
{
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!touchSensor.begin(0x5A)) 
  {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");  
  
}
