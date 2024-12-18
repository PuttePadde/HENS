
//    HCSR04.cpp
//    Hen In Nest sensor driver class.
//    Author: Peter Boye Hansenn
//    Version: 0.1
//    Based on original work by Richard Nash. 
//    https://github.com/rvnash/HC-SR04

#include "HCSR04.h"
#include "Particle.h"

HCSR04::HCSR04(int trg, int echo, float triggerDistance)
{
    this->trg = trg;
    this->echo = echo;
    this->triggerDistance = triggerDistance;
}

HCSR04::~HCSR04() {
}

void HCSR04::init()
{
    // Triggerpin.
    pinMode(trg, OUTPUT);

    // Use internal pulldown.
    pinMode(echo, INPUT_PULLDOWN);
}

float HCSR04::distToUs(float distanceInCm){
    // Calculate approximate roundtrip time in us. from distance.
    float t = distanceInCm * 58.31;
    return t;
}

int HCSR04::henPresent(){

    // Let's give the HR-SR04 up to 2.5 ms. to react.
    const float timeoutHigh = 2500;
    // Don't accept longer distances than expected distance.
    const float timeoutLow = distToUs(triggerDistance);

    float start, duration = 0;    

    // Timing is crucial here, so cannot allow other threads or interrupts.
    // Maximum amount of time in this block is limitted to 10 + timeout_high + timeout_low uS.
    // About 5.2 ms. with a trigger distance of 20. 
    // So keep timeouts low, to secure responsiveness!
    ATOMIC_BLOCK() {

        // Send the 10 uSec pulse
        pinSetFast(trg);
        delayMicroseconds(10);
        pinResetFast(trg);

        start = micros();

        // Fast = Skip safechecks and just read/write.
        while (pinReadFast(echo) != HIGH) {
            duration = micros() - start;
            if (duration >= timeoutHigh) {
                // Didn't recieve a pulse within HCSR04 timeout_high.
                return -1;
            }
        }

        start = micros();
        while (pinReadFast(echo) != LOW) {
            duration = micros() - start;
            if (duration >= timeoutLow) {
                // Pulse lasted longer than timeout_low.
                return 0;
            }
        }
    }

    // Return true if we believe there is a hen in the nest.
    return (duration < distToUs(this->triggerDistance)) ? 1 : 0;
}