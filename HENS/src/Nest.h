//    Nest.h
//
//    Nest controller class.
//    Author: Peter Boye Hansenn
//    Version: 0.1
#pragma once 

#include <string>

#include "Particle.h"
#include "HX711ADC.h"
#include "HCSR04.h"
#include "LIGHT.h"
#include <map>

class Nest {
    private:
        // Sensors and actuators.
        HCSR04 hinSensor;
        HX711ADC weightSensor;
        LIGHT lightActuator;

        // New sleep API Init.
        SystemSleepConfiguration sleepConfig;

        enum State {
            IDLE,
            SET_LIGHT,
            CHECK_HEN,
            HEN_PRESENT,
            HEN_LEFT,
            CHECK_EGG,
            EGG_LAYED,
            CLOUD_EXCHANGE,
            ERROR,
        };

        enum Err {
            NONE,
            WEIGHT,
            HIN,
            CLOUD,
        };

        const char* errStrings[2] = {
            "WEIGHT ERROR",
            "HIN ERROR"
        };

        int driftThreshold;
        int eggMinThreshold;
        int eggMaxThreshold;
        int henThreshold;
        int prevWeight;
        int weight;
        int eggWeight;
        int henWeight;
        unsigned long henEnterTime; 
        unsigned long henLeaveTime; 
        Err currentError;   
        State currentState;
        State nextState;
        State prevState;
        bool eggReported = false;

        // Function to get the string for an Err value.
        const char* errToString(Err error) {
            return errStrings[error];
        }

    public:

        // No default Constructor, need to specify the pins.
        Nest() = delete;

        Nest(int triggerPin, int echoPin, float triggerDistance,
         int loadCellDataPin, int loadCellClockPin,
         int driftThreshold, int eggMinThreshold, 
         int eggMaxThreshold, int henThreshold, int lightPin);
 

        // Destructor.
        virtual ~Nest();

        // ** Protos **.
        
        // Init nest.
        int init();

        // Update statemachine.
        int update();

        // API callback handler.
        static void webHandlerNest(const char *event, const char *data);   

};

