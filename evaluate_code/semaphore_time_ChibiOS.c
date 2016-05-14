// -----------------------------------------------------------------------------------------------------------------------------
// Semaphore Acquire/Release Time Measurement
//
//Measure by using oscilloscope
// -----------------------------------------------------------------------------------------------------------------------------
#include <ChibiOS_AVR.h>

const uint8_t LED_PIN = 13;
int i,j;
// ----------------------- Declare a Counting Semaphore  -----------------------
Semaphore sem;
//-----------------------------------------------------------------------------------------------------------------------
// ----------------------- Thread 1 where LED_PIN pulse HIGH and LOW are measured & LED_PIN goes HIGH before context switch
static WORKING_AREA(waThread1, 64);

static msg_t Thread1(void *arg) {
  pinMode(LED_PIN, OUTPUT);
  while (j<3) {
    // first pulse to get time with no context switch
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    // start second pulse
    digitalWrite(LED_PIN, HIGH);
    // ----------------------- Releases the semaphore -----------------------
    chSemSignal(&sem);  // decrements sem by 1 
	
    // sleep until next tick (1024 microseconds tick on Arduino)
    chThdSleep(1);
	j++
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------------------------------
// ----------------------- Thread 2 where LED_PIN becomes LOW -----------------------
static WORKING_AREA(waThread2, 64);

static msg_t Thread2(void *arg) {

  while (i<3) {
    chSemWait(&sem); // acquires the semaphore after it is released by thread 1
	// ----------------------- This gives the time to acquire the semaphore -----------------------
    digitalWrite(LED_PIN, LOW);
	i++;
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
  // initialize the counting semaphore to zero i.e.TAKEN
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