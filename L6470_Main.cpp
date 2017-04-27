#include <SPI.h>    // include the SPI library:
#include "SparkFunAutoDriver.h"
#include "SparkFundSPINConstants.h"


AutoDriver boardA(0, 10, 8);


// Support functions.

#define NOTE_DIVISOR 2  // My cheesy way of reducing the note frequencies to a range
                        //  that doesn't cause the motor to slip. I *could* rewrite
                        //  the wantYouGone() function to change the notes, but that
                        //  would be a lot of work.

int stepDir = 1;        // Direction flipping bit. Rather than all going one way,
                        //  they change directions. It looks cooler.

// To play a note, we start the motor spinning at the note's frequency in steps/s.
//  The run() function automagically calculates the appropriate value to feed to the
//  dSPIN part based on the desired steps/s.
void playNote(int note, int duration)
{
  if (stepDir == 1)  boardA.run(FWD, note/NOTE_DIVISOR);
  else               boardA.run(REV, note/NOTE_DIVISOR);

  delay(duration);
  stepDir*=-1;
  boardA.softStop();

  while (boardA.busyCheck());
}

// This is the configuration function for the two dSPIN parts. Read the inline
//  comments for more info.
void dSPINConfig(void)
{
  boardA.SPIPortConnect(&SPI);      // Before doing anything else, we need to


  boardA.configSyncPin(BUSY_PIN, 0);// BUSY pin low during operations;
                                    //  second paramter ignored.
  boardA.configStepMode(STEP_FS);   // 0 microsteps per step
  boardA.setMaxSpeed(10000);        // 10000 steps/s max
  boardA.setFullSpeed(10000);       // microstep below 10000 steps/s
  boardA.setAcc(10000);             // accelerate at 10000 steps/s/s
  boardA.setDec(10000);
  boardA.setSlewRate(SR_530V_us);   // Upping the edge speed increases torque.
  boardA.setOCThreshold(OC_750mA);  // OC threshold 750mA
  boardA.setPWMFreq(PWM_DIV_2, PWM_MUL_2); // 31.25kHz PWM freq
  boardA.setOCShutdown(OC_SD_DISABLE); // don't shutdown on OC
  boardA.setVoltageComp(VS_COMP_DISABLE); // don't compensate for motor V
  boardA.setSwitchMode(SW_USER);    // Switch is not hard stop
  boardA.setOscMode(EXT_16MHZ_OSCOUT_INVERT); // for boardA, we want 16MHz
                                    //  external osc, 16MHz out. boardB
                                    //  will be the same in all respects
                                    //  but this, as it will generate the
                                    //  clock.
  boardA.setAccKVAL(128);           // We'll tinker with these later, if needed.
  boardA.setDecKVAL(128);
  boardA.setRunKVAL(128);
  boardA.setHoldKVAL(32);           // This controls the holding current; keep it low.

}


void setup()
{
  Serial.begin(9600);
  Serial.println("Hello world");
  // Start by setting up the SPI port and pins. The
  //  Autodriver library does not do this for you!
  pinMode(8, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(13, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  digitalWrite(8, LOW);       // This low/high is a reset of the L6470 chip on the
  digitalWrite(8, HIGH);      //  Autodriver board, and is a good thing to do at
                              //  the start of any Autodriver sketch, to be sure
                              //  you're starting the Autodriver from a known state.
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  dSPINConfig();
}

void loop()
{
  if (Serial.available() !=0)
  {
    Serial.read();
    Serial.println("Play it!");
    //wantYouGone();
    Serial.println("Done playing!");
  }
}


