/* CLASSIC NINTENDO UP IN THIS, CHILL TIME YO
 * http://www.youtube.com/watch?v=RsBtQCDbTWc
 */

#include <Wire.h>

const float nC3 = 130.813;
const float nA2 = 110;
const float nGs2 = 103.826;
const float nG2 = 97.9989;
const float nE2 = 82.4069;

float bpm = 128.5;
float msPerBeat = 60000 / bpm;

int songLength = 14; // notes
float notes[] =   {nA2, nA2, nC3, nA2,  nE2,  nE2, nE2, nG2, nGs2, nA2, nA2, nC3, nA2,  nE2};
float onTime[] =  {.25, .25, .25, .25, .125, .125, .25, .25,  .25, .25, .25, .25, .25,  .25};
float offTime[] = {.25, .25, .25, .25, .125, .125, .25, .25,  .25, .25, .25, .25, .25, 1.75};

byte addr = 01;

union freq_t {
	float freq;
	byte bytes[4];
} unionFreq;

void setup() {
	Serial.begin(115200);	// start serial comms
	Wire.begin();			// start I2C
	Serial.println("Ready to go!");
	delay(1000);

	Serial.println("Stopping all playback.");
	Wire.beginTransmission(0);
	Wire.write('0');
	Wire.endTransmission();
	delay(2000);

	Serial.println("Playback START!");
}

void loop() {
	for (int i = 0; i < songLength; i++) {
		play(notes[i], onTime[i] * msPerBeat);
		rest(offTime[i] * msPerBeat);
	}
}

void play(float playFreq, float duration) {
	Serial.print("Playing: freq ");
	Serial.print(playFreq);
	Serial.print(" Hz, time ");
	Serial.print(duration);
	Serial.println(" ms");

	unionFreq.freq = playFreq;

	Wire.beginTransmission(addr);
	Wire.write('1');
	Wire.endTransmission();
	
	Wire.beginTransmission(addr);
	Wire.write('F');
	for (int i = 0; i < 4; i++) {
		Wire.write(unionFreq.bytes[i]);
	}
	Wire.endTransmission();

	delay(duration);
}

void rest(float duration) {
	Serial.print("Resting: time ");
	Serial.print(duration);
	Serial.println(" ms");

	Wire.beginTransmission(addr);
	Wire.write('0');
	Wire.endTransmission();

	delay(duration);
}