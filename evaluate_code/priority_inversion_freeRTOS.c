
#include <FreeRTOS_AVR.h>

#define LED_PIN 13
volatile int data;

SemaphoreHandle_t xSemaphore = NULL;

  void Thread1(void *pvParameters) {

  for (;;) {
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    data = 1;
    vTaskDelay( 500 / portTICK_PERIOD_MS );
    xSemaphoreGive( xSemaphore );   
  }
}


void Thread2(void *pvParameters) {

  for (;;) {
    Serial.print("\nThread 2 execution started....");
    Serial.print("\nThread 2 execution complete....");
    xSemaphoreGive( xSemaphore );  
  }
}
//------------------------------------------------------------------------------
void Thread3(void *pvParameters) {

  for (;;) {
     digitalWrite(LED_PIN, HIGH);
     digitalWrite(LED_PIN, LOW);
     digitalWrite(LED_PIN, HIGH);
     xSemaphoreTake(xSemaphore, portMAX_DELAY);
  digitalWrite(LED_PIN, LOW); 
  data = 3;
    Serial.print("\nData in Memory : ");
    Serial.print(data);
  Serial.print("\nMUTEX released by Thread 3!");
      xSemaphoreGive( xSemaphore );  
  }

}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  while (!Serial) {}
    // create high priority thread
  xTaskCreate(Thread1,
    "Task1",
    configMINIMAL_STACK_SIZE,
    NULL,
    tskIDLE_PRIORITY + 1,
    NULL);

  // create lower priority thread
  xTaskCreate(Thread2,
    "Task2",
    configMINIMAL_STACK_SIZE,
    NULL,
    tskIDLE_PRIORITY + 2,
    NULL);  

    xTaskCreate(Thread3,
    "Task3",
    configMINIMAL_STACK_SIZE,
    NULL,
    tskIDLE_PRIORITY + 3,
    NULL);  

  // create semaphore
  // vSemaphoreCreateBinary(xSemaphore);
    xSemaphore = xSemaphoreCreateMutex();
  // start FreeRTOS
  vTaskStartScheduler();
  
  // should never return
  Serial.println("Die");
  while(1);
}
//------------------------------------------------------------------------------
void loop() {
  // Not used - idle loop has a very small, configMINIMAL_STACK_SIZE, stack
  // loop must never block
}
