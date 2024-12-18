//    light.c
//
//    Light actuator driver class.
//    Author: Peter Boye Hansenn
//    Version: 0.1

#include "Particle.h"
#include "LIGHT.h"
#include <string>  

LIGHT * lightInstance = nullptr;

LIGHT::LIGHT(int pin) : checkTimeTimer(3600*24*7*1000, setTimeFlag)
{
    this->pin = pin;
    lightInstance = this;
}

LIGHT::~LIGHT() {
}

void LIGHT::init()
{
    // lightpin.
    pinMode(pin, OUTPUT);


    // Connect and get both interval and set RTC time.
    getCloudTime();

    checkTimeTimer.start();

}
void LIGHT::setTimeFlag(){
    lightInstance->timeFlag = true;
}

void LIGHT::getCloudTime(){

    // Wifi on
    WiFi.on();
    WiFi.connect();
    Log.info("Connecting to WIFI.");
    
    if(waitFor(WiFi.ready, 10000)) {
        Log.info("WIFI OK.");
    } else {
        Log.error("WIFI NOT OK - Could not get timeinterval.");
    }
    
    // Particle cloud on.
    Particle.connect();
    Log.info("Connecting to CLOUD.");

    if(waitFor(Particle.connected, 10000)) {
        Log.info("CLOUD OK.");
    } else {
        Log.error("CLOUD NOT OK - Could not get timeinterval.");
    }

    Particle.subscribe(System.deviceID() + "/hook-response/gettimeinterval/", webHandler, MY_DEVICES);

    // Call webhook and get time interval.
    Particle.publish("gettimeinterval", NULL); 


}

bool LIGHT::light()
{
    // If time flag set then get time interval from cloud.
    if(timeFlag){
        getCloudTime();
        timeFlag = false;
    }

    if((Time.hour() + 1) >= timeOn && (Time.hour() + 1)  < timeOff){
        return true;
    }

    return false;
}

void LIGHT::on()
{
    // lightpin on.
    digitalWrite(pin, HIGH);
}

void LIGHT::off()
{
    // lightpin off.
    digitalWrite(pin, LOW);
}

void LIGHT::webHandler(const char *event, const char *data) {
    Log.info("Interval data recieved");

    // Convert data to std::string for use later.
    std::string input(data);

    // Parse the values from the input string.
    size_t pos = input.find(';');
    if (pos != std::string::npos) {
        // Update the interval.
        lightInstance->timeOn = std::stoi(input.substr(0, pos));
        lightInstance->timeOff = std::stoi(input.substr(pos + 1));
        Log.info("Light on: %d, Light off: %d", lightInstance->timeOn, lightInstance->timeOff);
    } else {
        // Handle invalid input format (missing delimiter).
        Log.error("Invalid data format! Expected: 'xx;xx'.");
    }
        // Disconnect from Particle Cloud and turn off WiFi.
        Particle.disconnect();
        waitFor(Particle.disconnected, 1000);
        WiFi.off();
}
