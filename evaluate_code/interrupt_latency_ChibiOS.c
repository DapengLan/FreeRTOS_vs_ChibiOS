//----------------------------------------
// Interrupt latency Measurement
//----------------------------------------
#include <ChibiOS_AVR.h>

// pin to trigger interrupt
const uint8_t INTERRUPT_PIN = 2;
const uint8_t LED_PIN = 13;
int i,j;
// initialize a binary semaphore as taken
BSEMAPHORE_DECL(isrSem, 1);

// ISR entry time
volatile uint32_t tIsr = 0;
//------------------------------------------------------------------------------

void isr_Test() {
  // On AVR this forces compiler to save registers r18-r31.
  CH_IRQ_PROLOGUE();
  /* IRQ handling code, preemptable if the architecture supports it.*/
  
  // Only ISR processing is to save time
  //tIsr = micros();
  
  chSysLockFromIsr();
 
  //-----------------------------------------------------------------------------
  // Time taken by LED_PIN to go LOW from HIGH gives the interrupt latency
  digitalWrite(LED_PIN, LOW);
  //-----------------------------------------------------------------------------
  tIsr = micros();
  Serial.print("Time in ISR : ");
  Serial.println(tIsr);
  
  // ----------------------- Give control to ISR handler -----------------------
  chBSemSignalI(&isrSem); // decrements by 1 
  chSysUnlockFromIsr();
 
  // Perform rescheduling if required.
  CH_IRQ_EPILOGUE();  
}
//------------------------------------------------------------------------------
// ---------------------- Handler task for interrupt ----------------------
static WORKING_AREA(waThd1, 200);
 msg_t handler(void *arg) {
	 
  while (i<2) {
	  
    // wait for event
    chBSemWait(&isrSem); // if isrSem = 1, queues the thread 
    
    // print elapsed time
    tIsr = micros();
    Serial.print("Handler : ");
    Serial.println(tIsr);
	i++;
  }
}
//------------------------------------------------------------------------------
void setup() {
  Serial.flush();
  Serial.begin(9600);
  while (!Serial) {}
  
  pinMode(INTERRUPT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  chBegin(mainThread);
  while (1);
}
//------------------------------------------------------------------------------

void mainThread() {
  // ---------------------- start handler task ----------------------
  chThdCreateStatic(waThd1, sizeof(waThd1), NORMALPRIO + 1, handler, NULL);
  
  // ---------------------- Interrupt occurs on falling edge of INTERRUPT_PIN ----------------------
  attachInterrupt(0, isr_Test, FALLING); // attach interrupt function  
  while (j<2) {
    // ---------------------- Software generated interrupt ----------------------
    Serial.println("Interrupt occurs....");
    digitalWrite(INTERRUPT_PIN, HIGH);
   // first pulse to get time with no context switch
    digitalWrite(LED_PIN, HIGH);
	// ---------------------- Interrupt occurs now ----------------------
    digitalWrite(LED_PIN, LOW); 
    // start second pulse
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(INTERRUPT_PIN, LOW);
    Serial.println();
	
	j++;
  }
}
//------------------------------------------------------------------------------
void loop() {
  // not used
}
