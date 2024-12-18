//    HCSR04.h
//
//    Hen In Nest sensor driver class.
//    Author: Peter Boye Hansenn
//    Version: 0.1
//    Based on original work by Richard Nash. 
//    https://github.com/rvnash/HC-SR04

#pragma once

class HCSR04 {
//
//    Takes triggerpin, echopin and triggerdistance.
//    HCSR04::hen_present() returns true if hen is present, false otherwise. 
//

    private:
        int trg;
        int echo;
        float triggerDistance;
        float distToUs(float distanceInCm);
        
    public:
        // No default Constructor, need to specify the pins.
        HCSR04() = delete;

        // Constructor takes pins and HCSR04 distance.
        HCSR04(int trg, int echo, float triggerDistance);

        // Destructor.
        virtual ~HCSR04();

        // Protos.

        // Inits module.
        void init();

        // Returns true if hen closer than distance, -1 on error.
        int henPresent();

};
