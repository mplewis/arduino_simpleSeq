/* CLASSIC NINTENDO UP IN THIS, CHILL TIME YO
 * http://www.youtube.com/watch?v=RsBtQCDbTWc
 */

// Wire.h is for I2C communication with ATtiny slaves
#include <Wire.h>
// pianoFreqs.c holds the frequency floats in Hz of each key on a piano keyboard
#include "pianoFreqs.c"

// beats per minute: sets the tempo of the song
float bpm = 128.5;

// msPerBeat: the amount of time each beat takes in milliseconds, given the above bpm
float msPerBeat = 60000 / bpm;

/* song data
 * in the future, this will be moved to an external file so songs can be dropped in without changing source
 * songLength: the number of notes in the song. tells the arduino when to loop
 * notes[]: a matrix that holds each note in the order it will be played. notes are pulled from pianoFreqs.c
 * onTime[]: the amount of time each note is held, in fractions of a beat
 * offTime[]: the amout of rest that follows each note, in fractions of a beat
 */
int songLength = 14; // notes
float notes[] =   {nA2, nA2, nC3, nA2,  nE2,  nE2, nE2, nG2, nGs2, nA2, nA2, nC3, nA2,  nE2};
float onTime[] =  {.25, .25, .25, .25, .125, .125, .25, .25,  .25, .25, .25, .25, .25,  .25};
float offTime[] = {.25, .25, .25, .25, .125, .125, .25, .25,  .25, .25, .25, .25, .25, 1.75};

// currently this sketch sends frequency data to one ATtiny, address 01
byte addr = 01;

// the union unionFreq is used to convert a frequency in float form to bytes to send via I2C
union freq_t {
	float freq;
	byte bytes[4];
} unionFreq;

void setup() {
	Serial.begin(115200);	// start serial comms
	Wire.begin();			// start I2C
	Serial.println("Ready to go!");

	// Send '0' to all attached I2C devices, stopping playback
	Serial.println("Stopping all playback.");
	Wire.beginTransmission(0);
	Wire.write('0');
	Wire.endTransmission();
	delay(1000); // wait a second

	Serial.println("Playback START!");
}

void loop() {
	// play each note in order for the specified time, then rest for a specified time after each note
	for (int i = 0; i < songLength; i++) {
		play(notes[i], onTime[i] * msPerBeat);
		rest(offTime[i] * msPerBeat);
	}
}

void play(float playFreq, float duration) {
	// debug playing data to serial
	Serial.print("Playing: freq ");
	Serial.print(playFreq);
	Serial.print(" Hz, time ");
	Serial.print(duration);
	Serial.println(" ms");

	// use unionFreq to convert the float to 4 bytes
	unionFreq.freq = playFreq;

	// write '1' to enable waveform output from the chip
	Wire.beginTransmission(addr);
	Wire.write('1');
	Wire.endTransmission();
	
	// send the frequency to be played
	Wire.beginTransmission(addr);
	Wire.write('F');
	for (int i = 0; i < 4; i++) {
		Wire.write(unionFreq.bytes[i]);
	}
	Wire.endTransmission();

	// wait for the next rest period
	delay(duration);
}

void rest(float duration) {
	Serial.print("Resting: time ");
	Serial.print(duration);
	Serial.println(" ms");

	// write '0' to disable waveform output from the chip
	Wire.beginTransmission(addr);
	Wire.write('0');
	Wire.endTransmission();

	// wait for the next play period
	delay(duration);
}