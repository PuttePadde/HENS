//    syssettings.h
//
//    SYSTEM SETTING.
//    Author: Peter Boye Hansenn
//    Version: 0.1
#pragma once 

#include <string.h>
// SYSTEM SETTINGS //

//**  HCSR04 **//
const int TRIG_PIN = D1; // Pin to triggger measurement.
const int ECHO_PIN = D0; // Pin to detect echo.
const float TRIGGER_DISTANCE = 12.00; // Max. distance in cm. to look for objects.

//**  HX711 **//
const int LOADCELL_DT_PIN = A0; // HX711 data pin.
const int LOADCELL_CLK_PIN = A1; // HX711 clock pin.

//**  LIGHT **//
const int LIGHT_PIN = D2; // Pin to triggger measurement.


//**  Tresholds **//
const int DRIFT_THRS = 100; // Min. Weight change to trigger hen left.
const int EGG_MIN_THRS = 40; // Min. egg weight to register.
const int EGG_MAX_THRS = 100; // Max. egg weight to register.
const int HEN_THRS = 200; // Min. hen weight to register.

String UUID = "4569b4bb-eaf4-451d-b054-37f359a6d6b"; 