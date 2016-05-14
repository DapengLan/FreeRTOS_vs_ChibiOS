
//.......interruptPin (pin 3) from Hign to low is the interrupt time

#include "FreeRTOS_AVR.h"
//#include "task.h"
//#include "semphr.h"
//#include "portasm.h"

/* Demo includes. */
#include "basic_io_avr.h"

/* Compiler includes. */
//#include <dos.h>

/* The tasks to be created. */
static void vHandlerTask( void *pvParameters );
static void vPeriodicTask( void *pvParameters );

/* The service routine for the interrupt.  This is the interrupt that the task
will be synchronized with. */
static void vExampleInterruptHandler( void );

/*-----------------------------------------------------------*/

/* Declare a variable of type SemaphoreHandle_t.  This is used to reference the
semaphore that is used to synchronize a task with an interrupt. */
SemaphoreHandle_t xBinarySemaphore;

// pin to generate interrupts
const uint8_t interruptPin = 3;


void setup( void )
{
  Serial.begin(9600);
    /* Before a semaphore is used it must be explicitly created.  In this example
  a binary semaphore is created. */
    vSemaphoreCreateBinary( xBinarySemaphore );

  /* Install the interrupt handler. */
//  _dos_setvect( 0x82, vExampleInterruptHandler );

   pinMode(interruptPin, OUTPUT);
   attachInterrupt(0, vExampleInterruptHandler,FALLING ); //RISING


  /* Check the semaphore was created successfully. */
  if( xBinarySemaphore != NULL )
  {
    /* Create the 'handler' task.  This is the task that will be synchronized
    with the interrupt.  The handler task is created with a high priority to
    ensure it runs immediately after the interrupt exits.  In this case a
    priority of 3 is chosen. */
    xTaskCreate( vHandlerTask, "Handler", 200, NULL, 3, NULL );

    /* Create the task that will periodically generate a software interrupt.
    This is created with a priority below the handler task to ensure it will
    get preempted each time the handler task exist the Blocked state. */
    xTaskCreate( vPeriodicTask, "Periodic", 200, NULL, 1, NULL );

    /* Start the scheduler so the created tasks start executing. */
    vTaskStartScheduler();
  }

    /* If all is well we will never reach here as the scheduler will now be
    running the tasks.  If we do reach here then it is likely that there was
    insufficient heap memory available for a resource to be created. */
  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

static void vHandlerTask( void *pvParameters )
{
  /* Note that when you create a binary semaphore in FreeRTOS, it is ready
  to be taken, so you may want to take the semaphore after you create it
  so that the task waiting on this semaphore will block until given by
  another task. */
  xSemaphoreTake( xBinarySemaphore, 0);

  /* As per most tasks, this task is implemented within an infinite loop. */
  for( ;; )
  {
    /* Use the semaphore to wait for the event.  The semaphore was created
    before the scheduler was started so before this task ran for the first
    time.  The task blocks indefinitely meaning this function call will only
    return once the semaphore has been successfully obtained - so there is no
    need to check the returned value. */
    xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );

    /* To get here the event must have occurred.  Process the event (in this
    case we just print out a message). */
    vPrintString( "Handler task - Processing event.\r\n" );
  }
}
/*-----------------------------------------------------------*/

static void vPeriodicTask( void *pvParameters )
{

  /* As per most tasks, this task is implemented within an infinite loop. */
  for( ;; )
  {
    /* This task is just used to 'simulate' an interrupt.  This is done by
    periodically generating a software interrupt. */
    vTaskDelay( 500 / portTICK_PERIOD_MS );

    /* Generate the interrupt, printing a message both before hand and
    afterwards so the sequence of execution is evident from the output. */
    vPrintString( "Perodic task - About to generate an interrupt.\r\n" );
//    __asm{ int 0x82 }

//........................................
//.......interruptPin from Hign to low is the interrupt time
    digitalWrite(interruptPin, HIGH);
    digitalWrite(interruptPin, LOW);
//........................................

    vPrintString( "Periodic task - Interrupt generated.\r\n\r\n\r\n" );
  }
}
/*-----------------------------------------------------------*/

static void  vExampleInterruptHandler( void )
{
static signed portBASE_TYPE xHigherPriorityTaskWoken;

  xHigherPriorityTaskWoken = pdFALSE;

  /* 'Give' the semaphore to unblock the task. */
  xSemaphoreGiveFromISR( xBinarySemaphore, (signed portBASE_TYPE*)&xHigherPriorityTaskWoken );

  if( xHigherPriorityTaskWoken == pdTRUE )
  {
    /* Giving the semaphore unblocked a task, and the priority of the
    unblocked task is higher than the currently running task - force
    a context switch to ensure that the interrupt returns directly to
    the unblocked (higher priority) task.

    NOTE: The syntax for forcing a context switch is different depending
    on the port being used.  Refer to the examples for the port you are
    using for the correct method to use! */
    // portSWITCH_CONTEXT();
    vPortYield();
  }
}
//------------------------------------------------------------------------------
void loop() {}






