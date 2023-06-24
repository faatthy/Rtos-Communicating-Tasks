/* RTOS Project */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/trace.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "timers.h"

#define CCM_RAM __attribute__((section(".ccmram")))

/* The timer callback function */
static void SenderTimerCallback1( TimerHandle_t xTimer ); //Sender 1 callback
static void SenderTimerCallback2( TimerHandle_t xTimer ); //Sender 2 callback
static void SenderTimerCallback3( TimerHandle_t xTimer ); //Sender 3 callback
static void ReciverTimerCallback( TimerHandle_t xTimer ); //Receiver callback

/* The software timer */
static TimerHandle_t xTimer1 = NULL; // Sender 1 TimerHandle
static TimerHandle_t xTimer2 = NULL; // Sender 2 TimerHandle
static TimerHandle_t xTimer3 = NULL; // Sender 3 TimerHandle
static TimerHandle_t xTimer4 = NULL; // Receiver TimerHandle
BaseType_t xTimer1Started, xTimer2Started, xTimer3Started,xTimer4Started;

/* Time of Timers */
const int Time_Reciever=100;   //Time of Receiver Callback function
int Time_Sender=100;           //Time of Sender Callback function

/* Counters*/
int Number_of_Blocked_Messages;      //total number of blocked messages counter
int Number_of_Sent_Messages;  //total number of transmitted messages counter
int Number_of_Received_Messages;     //total number of received messages counter


/*counters to calculate average sender time*/
int Sum_Timer_1=0;             /* this variables to summation the timer sender 1*/
int Sum_Timer_2=0;               /* this variables to summation the timer sender 2*/
int Sum_Timer_3=0;                /* this variables to summation the timer sender 3*/

int Counter_Timer_1=0;
int Counter_Timer_2=0;
int Counter_Timer_3=0;


/* Iteration */
int i=-1;

/* Arrays */
int lower_bound[6]={50, 80, 110, 140, 170, 200};
int upper_bound[6]={150, 200, 250, 300, 350, 400};

/* Queue */
xQueueHandle Queue=0;

/* Semaphore */
xSemaphoreHandle Sender_1=0;  // Sender 1 SemaphoreHandle
xSemaphoreHandle Sender_2=0;  // Sender 2 SemaphoreHandle
xSemaphoreHandle Sender_3=0;  // Sender 3 SemaphoreHandle
xSemaphoreHandle Receiver=0;  //Receiver SemaphoreHandle

/* Tasks */
void Sender_Task_1(void *p);  //Sender 1 Task
void Sender_Task_2(void *p);  //Sender 2 Task
void Sender_Task_3(void *p);  //Sender 3 Task
void Receiver_Task(void *p);  //Receiver Task

/* Uniform Distribution */
int uniform_distribution(int Range_Low, int Range_High)
{
    int Random_Value = (int)rand();
    int Range = Range_High - Range_Low + 1; //+1 makes it [rangeLow, rangeHigh], inclusive.
    int Scale = (Random_Value % Range) + Range_Low;
    return Scale;
}

/* Reset Function */
void Reset_Function(){
	if(i!=-1){
			printf("\nTotal Number Of Sent Messages= %d \n",Number_of_Sent_Messages);
			printf("Total Number Of Blocked Messages= %d \n\n",Number_of_Blocked_Messages);
			printf("Total Number Of Received Messages= %d \n\n",Number_of_Received_Messages);
	}
	int Average_Time=(Sum_Timer_1+Sum_Timer_2+Sum_Timer_3)/(Counter_Timer_1+Counter_Timer_2+Counter_Timer_3);
		printf("The Average Time is %d \r\n",Average_Time);
	if(i==5){
		xTimerStop(xTimer1,0);
		xTimerDelete(xTimer1, 0);
		xTimerStop(xTimer2, 0);
		xTimerDelete(xTimer2, 0);
		xTimerStop(xTimer3, 0);
		xTimerDelete(xTimer3, 0);
		xTimerStop(xTimer4, 0);
	    xTimerDelete(xTimer4, 0);
		puts("Game Over\n");
		vTaskEndScheduler();
	}
	i++;                                      								  //Increment Iterator
	if(i==0)
	{
		Time_Sender=uniform_distribution(lower_bound[i],upper_bound[i]);
		xTimerChangePeriod(xTimer1,Time_Sender,0);
		xTimerChangePeriod(xTimer2,Time_Sender,0);
		xTimerChangePeriod(xTimer3,Time_Sender,0);
	}

	// reset
	Average_Time=0;
	Number_of_Sent_Messages=0;
	Number_of_Blocked_Messages=0;
	Number_of_Received_Messages=0;
	Sum_Timer_1=0;
	Sum_Timer_2=0;
	Sum_Timer_3=0;
	Counter_Timer_1=0;
	Counter_Timer_2=0;
	Counter_Timer_3=0;
	xQueueReset(Queue);
}

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[])
{

	vSemaphoreCreateBinary(Sender_1);  //Sender 1 Binary Semaphore
	vSemaphoreCreateBinary(Sender_2);  //Sender 2 Binary Semaphore
	vSemaphoreCreateBinary(Sender_3);  //Sender 3 Binary Semaphore
	vSemaphoreCreateBinary(Receiver);  //Receiver Binary Semaphore

	Queue=xQueueCreate(3,sizeof(char[20]));  //Global Queue that store char variables

	xTaskCreate(Sender_Task_1,(signed char*) "Sender_Task_1",1024,NULL,1,NULL); //Sender 1 Task Creation
	xTaskCreate(Sender_Task_2,(signed char*) "Sender_Task_2",1024,NULL,1,NULL); //Sender 2 Task Creation
	xTaskCreate(Sender_Task_3,(signed char*) "Sender_Task_3",1024,NULL,2,NULL); //Sender 3 Task Creation
	xTaskCreate(Receiver_Task,(signed char*) "Receiver_Task",1024,NULL,3,NULL); //Receiver Task Creation

	xTimer1 = xTimerCreate( "Timer1", ( pdMS_TO_TICKS(Time_Sender) ), pdTRUE, ( void * ) 0, SenderTimerCallback1); //Sender 1 Timer creation
	xTimer2 = xTimerCreate( "Timer2", ( pdMS_TO_TICKS(Time_Sender) ), pdTRUE, ( void * ) 0, SenderTimerCallback2); //Sender 2 Timer creation
	xTimer3 = xTimerCreate( "Timer3", ( pdMS_TO_TICKS(Time_Sender) ), pdTRUE, ( void * ) 0, SenderTimerCallback3); //Sender 3 Timer creation
	xTimer4 = xTimerCreate( "Timer4", ( pdMS_TO_TICKS(Time_Reciever) ), pdTRUE, ( void * ) 0, ReciverTimerCallback); //Receiver Timer creation

	Reset_Function();

	if( ( xTimer1 != NULL ) && ( xTimer2 != NULL ) && ( xTimer3 != NULL )&&( xTimer4 != NULL ) )
	{
		xTimer1Started = xTimerStart( xTimer1, 0 ); //Sender 1 time started at t=0
		xTimer2Started = xTimerStart( xTimer2, 0 ); //Sender 2 time started at t=0
		xTimer3Started = xTimerStart( xTimer3, 0 ); //Sender 2 time started at t=0
		xTimer4Started = xTimerStart( xTimer4, 0 ); //Receiver time started at t=0
	}

	if( xTimer1Started == pdPASS && xTimer2Started == pdPASS && xTimer3Started == pdPASS&& xTimer4Started == pdPASS)
	{
		vTaskStartScheduler();
	}

	return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

void Sender_Task_1(void *p)  //Sender 2 Task
{
	TickType_t Current_Time;  //Variable to store current time
	char Data_Sent[20];      //Variable to store data that will be sent to Queue
	while(1){
		if(xSemaphoreTake(Sender_1, portMAX_DELAY)){
			Current_Time = xTaskGetTickCount();
			sprintf(Data_Sent, "Time is %d", Current_Time);
			if(xQueueSend(Queue,Data_Sent,0)){
				Number_of_Sent_Messages++;
				puts("Sender 1");
			}
			else{
				puts("Sender 1 Blocked");
				Number_of_Blocked_Messages++;          //Increment Blocked message
			}
		}
	}
}

void Sender_Task_2(void *p)  //Sender 2 Task
{
	TickType_t Current_Time;  //Variable to store current time
	char Data_Sent[20];      //Variable to store data that will be sent to Queue
	while(1){
		if(xSemaphoreTake(Sender_2, portMAX_DELAY)){
			Current_Time = xTaskGetTickCount();
			sprintf(Data_Sent, "Time is %d", Current_Time);
			if(xQueueSend(Queue,Data_Sent,0)){
				Number_of_Sent_Messages++;
				puts("Sender 2");
			}
			else{
				puts("Sender 2 Blocked");
				Number_of_Blocked_Messages++;          //Increment Blocked message
			}
		}
	}
}
void Sender_Task_3(void *p)  //Sender 2 Task
{
	TickType_t Current_Time;  //Variable to store current time
	char Data_Sent[20];      //Variable to store data that will be sent to Queue
	while(1){
		if(xSemaphoreTake(Sender_3, portMAX_DELAY)){
			Current_Time = xTaskGetTickCount();
			sprintf(Data_Sent, "Time is %d", Current_Time);
			if(xQueueSend(Queue,Data_Sent,0)){
				Number_of_Sent_Messages++;
				puts("Sender 3");
			}
			else{
				puts("Sender 3 Blocked");
				Number_of_Blocked_Messages++;          //Increment Blocked message
			}
		}
	}
}


void Receiver_Task(void *p)  //Received Task
{
	char Data_Received[20]; //variable to store data that will be received to Queue
	while(1){
		if(xSemaphoreTake(Receiver, portMAX_DELAY)){                     //Semaphore waiting for signal from Timer Callback function
			if(xQueueReceive(Queue,Data_Received,0)){     //Check if receiving messages was successful
				Number_of_Received_Messages++;                     //Increment successful received messages
				printf("Received %s \n",Data_Received);
			}
			else{
				puts("Didn't Receive");
			}
		}
	}
}

static void SenderTimerCallback1( TimerHandle_t xTimer )
{
	xSemaphoreGive(Sender_1); //release semaphore to act like a signal for Sender Task 1
	Time_Sender=uniform_distribution(lower_bound[i],upper_bound[i]);  //Change Time of Sender Callback function
	xTimerChangePeriod(xTimer1,Time_Sender,0);   //Function that changes Callback function time
	Sum_Timer_1+=Time_Sender;
	Counter_Timer_1++;
}

static void SenderTimerCallback2( TimerHandle_t xTimer )
{

	xSemaphoreGive(Sender_2); //release semaphore to act like a signal for Sender Task 2
	Time_Sender=uniform_distribution(lower_bound[i],upper_bound[i]);  //Change Time of Sender Callback function
	xTimerChangePeriod(xTimer2,Time_Sender,0);    //Function that changes Callback function time
	Sum_Timer_2+=Time_Sender;
		Counter_Timer_2++;
}
static void SenderTimerCallback3( TimerHandle_t xTimer )
{

	xSemaphoreGive(Sender_3); //release semaphore to act like a signal for Sender Task 2
	Time_Sender=uniform_distribution(lower_bound[i],upper_bound[i]);  //Change Time of Sender Callback function
	xTimerChangePeriod(xTimer3,Time_Sender,0);    //Function that changes Callback function time
	Sum_Timer_3+=Time_Sender;
		Counter_Timer_3++;
}

static void ReciverTimerCallback( TimerHandle_t xTimer )
{
	xSemaphoreGive(Receiver); //release semaphore to act like a signal for Receiver Task 1
	if(Number_of_Received_Messages == 1000){
		Reset_Function();     //Call reset function if messages reached 500 message
	}
}


void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

void vApplicationTickHook(void) {
}

StaticTask_t xIdleTaskTCB CCM_RAM;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] CCM_RAM;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
  state will be stored. */
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

  /* Pass out the array that will be used as the Idle task's stack. */
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;

  /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
  Note that, as the array is necessarily of type StackType_t,
  configMINIMAL_STACK_SIZE is specified in words, not bytes. */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB CCM_RAM;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] CCM_RAM;

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}



