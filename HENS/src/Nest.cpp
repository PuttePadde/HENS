#include "particle.h"
#include "HX711ADC.h"
#include "HCSR04.h"
#include "Nest.h"

Nest * nestInstance = nullptr;

bool LIGHT_ON = false;


Nest::Nest(int triggerPin, int echoPin, float triggerDistance,
        int loadCellDataPin, int loadCellClockPin,
        int driftThreshold, int eggMinThreshold, 
        int eggMaxThreshold, int henThreshold, int lightPin): 
        hinSensor(triggerPin, echoPin, triggerDistance),
        weightSensor(loadCellDataPin, loadCellClockPin, 128),
        lightActuator(lightPin){
    
    this->currentState = CHECK_HEN;
    this->nextState = CHECK_HEN;
    this->prevState = CHECK_HEN;
    this->prevWeight = 0;
    this->weight = 0;
    this->eggWeight = 0;
    this->driftThreshold = driftThreshold;
    this->eggMinThreshold = eggMinThreshold;
    this->eggMaxThreshold = eggMaxThreshold;
    this->henThreshold = henThreshold;
    this->henEnterTime = 0;

    // Global pointer to self.
    nestInstance = this;
    
    // Configure sleep mode
    sleepConfig.mode(SystemSleepMode::STOP).duration(15s); // 15 seconds.

}

Nest::~Nest() {
}

int Nest::init() {
    hinSensor.init();
    lightActuator.init();
    weightSensor.set_scale(-99.80);
    weightSensor.tare();
    return 0;
}

// Statemachine, most system logic is here.
int Nest::update() {

    switch (currentState) {

        case IDLE: {
           
            Log.info("State: IDLE");

            // Power down HX711.
            //weightSensor.power_down();

            // Only sleep if we are not talking to cloud.
            if (WiFi.ready()) {
                HAL_Delay_Milliseconds(15000);
            }else{
                System.sleep(sleepConfig);
            }
            // Wake up HX711.
            //weightSensor.power_up();

            // Goto next state.
            nextState = SET_LIGHT;
            break;
        }

        case SET_LIGHT: {
            Log.info("State: CHECK_LIGHT");

            if(lightActuator.light()){
                lightActuator.on();
                Log.info("Light: light is on.");
            }else{
                lightActuator.off();
                Log.info("Light: is off.");
            }

            // Check if hen in nest.
            nextState = CHECK_HEN;
            break;
        }

        case CHECK_HEN: {
            //DEBUG.
            Log.info("State: CHECK_HEN");

            // Check if a hen is detected.
            int henPresent = hinSensor.henPresent();

            if (henPresent == 1) {
                // Hen detected, update state.
                nextState = HEN_PRESENT;
                break;
            } else if (henPresent == 0 && (prevWeight > henThreshold)) {
                // Hen left.
                nextState = HEN_LEFT;
                break;
            } else if (henPresent == -1) {
                // Error detected in sensor.
                currentError = HIN;
                nextState = ERROR;
                break;
            }

            // Default to IDLE if no condition is met.
            nextState = IDLE;
            break;
        }

        case HEN_PRESENT: {
            // Log current state for debugging.
            Log.info("State: HEN_PRESENT");
            henEnterTime = Time.now();  // UNIX timestamp.
            Log.info("Hen entered nest at: %lu", henEnterTime);

            if (prevState == CHECK_HEN) {
                // Update weight measurement.
                prevWeight = weightSensor.get_units(1);

                if (prevWeight < 0) {
                    // Handle invalid weight reading.
                    currentError = WEIGHT;
                    nextState = ERROR;
                    break;
                }
            }

            // Return to idle.
            nextState = IDLE;
            break;
        }

        case HEN_LEFT: {
            Log.info("State: HEN_LEFT");

            // Measure the current weight.
            weight = weightSensor.get_units(1);
            if (weight < 0) {
                // Handle invalid weight reading.
                currentError = WEIGHT;
                nextState = ERROR;
                break;
            }
           
            // Check for significant weight changes, trying to rule out hen movement.
            henWeight = abs(prevWeight - weight); // save henweight.

            if (henWeight > driftThreshold) {
                // Possible egg detected.
                henLeaveTime = Time.now();
                Log.info("Hen was left nest at: %lu seconds", henLeaveTime);
                nextState = CHECK_EGG;
                break;
            }
             
            // No egg, return to idle.
            nextState = IDLE;
            break;
        }

        case CHECK_EGG: {
            //DEBUG
            Log.info("State: CHECK_EGG");
            // Save the current state and weight.
            prevState = CHECK_EGG;
            eggWeight = weight;

            if (eggWeight > eggMinThreshold) {
                //Egg layed.
                nextState = EGG_LAYED;
                break;
            }

            // No egg detected, return to idle.
            prevWeight = 0;
            nextState = IDLE;

            break;
        }

        case EGG_LAYED: {
            //DEBUG
            Log.info("State: EGG_LAYED");
            Log.info("Egg Layed");

            // Tare weight for another round.  
            while(!weightSensor.is_ready());
            weightSensor.tare(); 

            // Update cloud update.
            nextState = CLOUD_EXCHANGE;
            break;
        }

        case CLOUD_EXCHANGE: {
            // Calculate time hen was in nest this time.
            int layTime = henLeaveTime - henEnterTime;

            Log.info("State: CLOUD_EXCHANGE"); 

            WiFi.on();
            WiFi.connect();
            Log.info("Connecting to WIFI.");
            
            if(waitFor(WiFi.ready, 20000)) {
                Log.info("WIFI OK.");
            } else {
                currentError = CLOUD;
                nextState = ERROR;  
                break;  
            }
            Particle.connect();
            Log.info("Connecting to CLOUD.");

            if(waitFor(Particle.connected, 30000)) {
                Log.info("CLOUD OK.");
            } else {
                currentError = CLOUD;
                nextState = ERROR;   
                break; 
            }
            
            // Call webhook and report egg layed, hen weight and lay_time in JSON.
            char data[512]; 
            sprintf(data, "{\"egg_weight\":%d,\"hen_weight\":%d,\"lay_time\":%d}", eggWeight, henWeight, layTime);
            
            Particle.subscribe("0a10aced202194944a059c4c/hook-response/eggLayed", webHandlerNest);

            Particle.publish("eggLayed", data);
            Log.info("Published eggLayed event.");

            // Clear variables for another cycle.
            prevWeight = 0;
            weight = 0;
            eggWeight = 0;

            // Return to idle state.
            nextState = IDLE;
            break;
        }


        case ERROR: {
            //DEBUG.
            Log.error("An error occured: ");
            
            Log.error(errToString(currentError));
            
            currentError = NONE;

            // Return to idle.
            nextState = IDLE;
            break;
        }
        return 0;
    }

    // Save the current state and transition to the next.
    prevState = currentState;
    currentState = nextState;
    return 0;
}

void Nest::webHandlerNest(const char *event, const char *data){
    Log.info("EGG Reported2");
    if(!strcmp(data, "OK")){
        Log.info("EGG Reported");
        Log.info(data);
        nestInstance->nextState = IDLE;
    }else{
        nestInstance->currentError = CLOUD;
        nestInstance->nextState = ERROR;
    }
    Particle.disconnect();
    waitFor(Particle.disconnected, 1000);
    WiFi.off();
}
   