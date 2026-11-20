#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Task configuration parameters */
#define HIGH_TASK_PRIO     3
#define MEDIUM_TASK_PRIO   2
#define LOW_TASK_PRIO      1

#define HIGH_TASK_DELAY_MS    500
#define MEDIUM_TASK_DELAY_MS  1000
#define LOW_TASK_DELAY_MS     1500

#define TASK_STACK_SIZE    2048

/* Execution counters for each task */
static volatile uint32_t highCount   = 0;
static volatile uint32_t mediumCount = 0;
static volatile uint32_t lowCount    = 0;

/* Task handles */
static TaskHandle_t xHighTaskHandle   = NULL;
static TaskHandle_t xMediumTaskHandle = NULL;
static TaskHandle_t xLowTaskHandle    = NULL;

void HighTask(void *pvParameters)
{
  (void)pvParameters;
  for (;;)
  {
    highCount++;
    UBaseType_t prio = uxTaskPriorityGet(NULL);

    /* RUNNING state */
    Serial.printf("HIGH   | Priority: %u | Count: %lu | RUNNING -> BLOCKED (%d ms)\n",
                  (unsigned int)prio,
                  (unsigned long)highCount,
                  HIGH_TASK_DELAY_MS);

    /* Transitions task from RUNNING -> BLOCKED for 500 ms;
       after delay expires, FreeRTOS moves it to READY -> RUNNING */
    vTaskDelay(pdMS_TO_TICKS(HIGH_TASK_DELAY_MS));
  }
}

void MediumTask(void *pvParameters)
{
  (void)pvParameters;
  for (;;)
  {
    mediumCount++;
    UBaseType_t prio = uxTaskPriorityGet(NULL);

    Serial.printf("MEDIUM | Priority: %u | Count: %lu | RUNNING -> BLOCKED (%d ms)\n",
                  (unsigned int)prio,
                  (unsigned long)mediumCount,
                  MEDIUM_TASK_DELAY_MS);

    vTaskDelay(pdMS_TO_TICKS(MEDIUM_TASK_DELAY_MS));
  }
}

void LowTask(void *pvParameters)
{
  (void)pvParameters;
  for (;;)
  {
    lowCount++;
    UBaseType_t prio = uxTaskPriorityGet(NULL);

    Serial.printf("LOW    | Priority: %u | Count: %lu | RUNNING -> BLOCKED (%d ms)\n",
                  (unsigned int)prio,
                  (unsigned long)lowCount,
                  LOW_TASK_DELAY_MS);

    vTaskDelay(pdMS_TO_TICKS(LOW_TASK_DELAY_MS));
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("========================================");
  Serial.println(" FreeRTOS TASK PRIORITY EXPERIMENT");
  Serial.println("========================================");

  BaseType_t r1 = xTaskCreate(
      HighTask,
      "HighTask",
      TASK_STACK_SIZE,
      NULL,
      HIGH_TASK_PRIO,
      &xHighTaskHandle);

  BaseType_t r2 = xTaskCreate(
      MediumTask,
      "MediumTask",
      TASK_STACK_SIZE,
      NULL,
      MEDIUM_TASK_PRIO,
      &xMediumTaskHandle);

  BaseType_t r3 = xTaskCreate(
      LowTask,
      "LowTask",
      TASK_STACK_SIZE,
      NULL,
      LOW_TASK_PRIO,
      &xLowTaskHandle);

  if (r1 == pdPASS && r2 == pdPASS && r3 == pdPASS)
  {
    Serial.println("Three tasks created successfully.");
  }
  else
  {
    Serial.println("Error: One or more tasks failed to create!");
  }

  Serial.printf("Priority: HIGH=%d, MEDIUM=%d, LOW=%d\n",
                HIGH_TASK_PRIO, MEDIUM_TASK_PRIO, LOW_TASK_PRIO);
  Serial.println("State: RUNNING -> BLOCKED -> READY -> RUNNING");
  Serial.println("========================================");
}

void loop()
{
  /* In ESP32 Arduino, FreeRTOS tasks run independently;
     keep loop() idle so it does not interfere with task timing. */
  vTaskDelay(pdMS_TO_TICKS(1000));
}
