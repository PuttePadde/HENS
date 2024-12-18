// 
// Project HENSIOT
// Author: Peter Hansen
// Date: 
// For comprehensive documentation and examples, please visit:
// https://github.com/PuttePadde/HENS/
//

// Include Particle Device OS APIs
#include "Particle.h"
#include "Nest.h"
#include "syssettings.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(MANUAL);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

Nest nestController(TRIG_PIN, ECHO_PIN, 
                   TRIGGER_DISTANCE, LOADCELL_DT_PIN, 
                   LOADCELL_CLK_PIN, DRIFT_THRS, EGG_MIN_THRS, 
                   EGG_MAX_THRS, HEN_THRS, LIGHT_PIN);

void setup() {
    pinMode(D2, OUTPUT);
    nestController.init();
    
}

void loop() {
    nestController.update();
}