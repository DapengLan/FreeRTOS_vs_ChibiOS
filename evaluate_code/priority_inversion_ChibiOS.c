// -----------------------------------------------------------------------------------------------------------------------------
// Priority inheritance test using priority inversion scenario
//
//Measurement of time taken to resolve priority inversion is done using oscilloscope
//
// RAM Usage : 1,273 of 2,048 bytes - approx 62% of Memory
// -----------------------------------------------------------------------------------------------------------------------------
#include <ChibiOS_AVR.h>
volatile int data;
const uint8_t LED_PIN = 13;
// declare and initialize a mutex for shared resource access to threads
MUTEX_DECL(pMutex);
//-----------------------------------------------------------------------------------------------------------------------------------
// ----------------------- Thread 1 - The lowest priority thread which starts first -----------------------
static WORKING_AREA(waThread1, 100);

static msg_t Thread1(void *arg) {
  
 while(1)
 {
  chMtxLock(&pMutex);
  //Serial.print("\nMUTEX locked by Thread 1!");
  data = 1;
  //Serial.print("\nData in Memory : ");
    //Serial.print(data);
    // ----------------------- To lock the mutex and trigger a context switch -----------------------
    chThdSleepMilliseconds(500);
    //Serial.print("\nMUTEX released by Thread 1!");
  chMtxUnlock();
 } 
  return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------
// ----------------------- Thread 2 - The meidum priority thread which starts after Thread 1 -----------------------
static WORKING_AREA(waThread2, 100);

static msg_t Thread2(void *arg) {
while(1)
{
    Serial.print("\nThread 2 execution started....");
    Serial.print("\nThread 2 execution complete....");
  // first pulse to get time with no context switch
    ///digitalWrite(LED_PIN, HIGH);
    //digitalWrite(LED_PIN, LOW);
  chThdYield();
}
  return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------
// ----------------------- Thread 3 - The highest priority thread supposedly blocked by Thread1 -----------------------
static WORKING_AREA(waThread3, 100);

static msg_t Thread3(void *arg) {
while(1)
{
 //Serial.print("\nMUTEX requested by Thread 3!");
 digitalWrite(LED_PIN, HIGH);
 digitalWrite(LED_PIN, LOW);
 digitalWrite(LED_PIN, HIGH);
 chMtxLock(&pMutex);
  // ----------- The time between LED_PIN set to LOW from HIGH gives the time taken to resolve priority inversion -----------
  digitalWrite(LED_PIN, LOW); 
  data = 3;
    Serial.print("\nData in Memory : ");
    Serial.print(data);
  Serial.print("\nMUTEX released by Thread 3!");
 chMtxUnlock(); 
}
  return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  // wait for USB Serial
 while (!Serial) {}
  chBegin(chSetup);
  while (1) {}
}
//-----------------------------------------------------------------------------------------------------------------------------------
void chSetup() {
  
  Serial.print("\nThread 1 started........");
 // start lowest priority thread
  chThdCreateStatic(waThread1, sizeof(waThread1),
    NORMALPRIO+1, Thread1, NULL);
    
  
  Serial.print("\nThread 2 started........");
  
  // start medium priority thread
  chThdCreateStatic(waThread2, sizeof(waThread2),
    NORMALPRIO+2, Thread2, NULL);

  Serial.print("\nThread 3 started........");
 // start highest priority thread
  chThdCreateStatic(waThread3, sizeof(waThread3),
    NORMALPRIO+3, Thread3, NULL);
    
  
}
//-----------------------------------------------------------------------------------------------------------------------
void loop() {
  // not used
}