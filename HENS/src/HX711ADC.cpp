//    HCSR04.h
//
//    Hen In Nest sensor driver class.
//    Author: Particle libraries.

#include "HX711ADC.h"

HX711ADC::HX711ADC(int dout, int pd_sck, int gain) {
	begin(dout, pd_sck, gain);
}

HX711ADC::~HX711ADC() {
}

void HX711ADC::begin(int dout, int pd_sck, int gain) {
	PD_SCK = pd_sck;
	DOUT = dout;

	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);

	set_gain(gain);
}

void HX711ADC::set_gain(int gain) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

	digitalWrite(PD_SCK, LOW);
	//read();
}

long HX711ADC::read(time_t timeout) {

	
    if (!is_ready()) return -1;

	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	// pulse the clock pin 24 times to read the data
	data[2] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[1] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[0] = shiftIn(DOUT, PD_SCK, MSBFIRST);

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN; i++) {
		digitalWrite(PD_SCK, HIGH);
		digitalWrite(PD_SCK, LOW);
	}

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = static_cast<unsigned long>(filler)  << 24
		  | static_cast<unsigned long>(data[2]) << 16
		  | static_cast<unsigned long>(data[1]) << 8
		  | static_cast<unsigned long>(data[0]) ;

	return static_cast<long>(value);
}

long HX711ADC::read_average(int times) {
  if (times <= 0) return -1;
	long sum = 0;
	for (int i = 0; i < times; i++) {
		sum += read();
	}
	return sum / times;
}

double HX711ADC::get_value(int times) {
	return read_average(times) - OFFSET;
}

int HX711ADC::get_units(int times) {
	return get_value(times) / SCALE;
}
 
void HX711ADC::tare(int times) {
	double sum = read_average(times);
	set_offset(sum);
}

void HX711ADC::set_scale(float scale) {
  if (scale) {
	SCALE = scale;
  }
  else {
    SCALE = 1;
  }
}

float HX711ADC::get_scale() {
	return SCALE;
}

void HX711ADC::set_offset(long offset) {
	OFFSET = offset;
}

long HX711ADC::get_offset() {
	return OFFSET;
}

void HX711ADC::power_down() {
	digitalWrite(PD_SCK, LOW);
	digitalWrite(PD_SCK, HIGH);
}

void HX711ADC::power_up() {
	digitalWrite(PD_SCK, LOW);
}