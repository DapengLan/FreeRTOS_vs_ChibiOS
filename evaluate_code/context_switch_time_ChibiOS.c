// Connect a scope to pin 13
// Measure difference in time between first pulse with no context switch
// and second pulse started in thread 2 and ended in thread 1.
// Difference should be about 15-16 usec on a 16 MHz 328 Arduino.
// -----------------------------------------------------------------------------------------------------------------------------
// Context Switch Time Measurement
//
// Context switch time can also be measured using micros() but print statements & call to micros() may also consume time
// -----------------------------------------------------------------------------------------------------------------------------
#include <ChibiOS_AVR.h>

const uint8_t LED_PIN = 13;
int i,j;
// ----------------------- Semaphore to trigger context switch -----------------------
Semaphore sem;
//-----------------------------------------------------------------------------------------------------------------------
// ----------------------- Thread 1 where LED_PIN becomes LOW -----------------------
static WORKING_AREA(waThread1, 64);

static msg_t Thread1(void *arg) {

  while (i<3) {
    chSemWait(&sem); // decrements sem by 1 if sem < 0, the thread is queued
    digitalWrite(LED_PIN, LOW);
	i++;
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------------------------------
// ----------------------- Thread 2 where LED_PIN pulse HIGH and LOW are measured & LED_PIN goes HIGH before context switch
static WORKING_AREA(waThread2, 64);

static msg_t Thread2(void *arg) {
  pinMode(LED_PIN, OUTPUT);
  while (j<3) {
    // first pulse to get time with no context switch
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    // start second pulse
    digitalWrite(LED_PIN, HIGH);
    // ----------------------- Triggers context switch for task that ends pulse -----------------------
    chSemSignal(&sem); // increments sem by 1 
	
    // sleep until next tick (1024 microseconds tick on Arduino)
    chThdSleep(1);
	j++
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------------------------------
void setup() {
  chBegin(chSetup);
  while (1) {}
}
//-----------------------------------------------------------------------------------------------------------------------
void chSetup() {
  // initialize semaphore
  chSemInit(&sem, 0);

  // start high priority thread
  chThdCreateStatic(waThread1, sizeof(waThread1),
    NORMALPRIO+2, Thread1, NULL);

  // start lower priority thread
  chThdCreateStatic(waThread2, sizeof(waThread2),
    NORMALPRIO+1, Thread2, NULL);
}
//-----------------------------------------------------------------------------------------------------------------------
void loop() {
  // not used
}