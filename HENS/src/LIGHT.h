#pragma once
//    light.h
//
//    Light actuator driver class.
//    Author: Peter Boye Hansenn
//    Version: 0.1

class LIGHT {
    private:
        int pin;
        int timeOn = 0;
        int timeOff = 0;
        Timer checkTimeTimer;
        bool timeFlag;

    public:
        // No default Constructor, need to specify the pins.
        LIGHT() = delete;

        // Constructor takes pins and HCSR04 distance.
        LIGHT(int lightPin);

        // Destructor.
        virtual ~LIGHT();

        // Protos.

        // Inits module.
        void init();

        // Inits module.
        void on();

        // Inits module.
        void off();
        
        // Sets flag for cloud connection.
        static void setTimeFlag();

        // Get time interval from cloud.
        void getCloudTime();

        // Returns true if light should be on now.
        bool light();

        static void webHandler(const char *event, const char *data);   

};
