/*
 * FreeRTOS Configuration for RP2040 (Pololu 3pi+ Robot)
 * Simplified for LED-based demonstration
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* ============================================================================
 * CORE CONFIGURATION
 * ============================================================================ */

#define configUSE_PREEMPTION                    1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      125000000
#define configTICK_RATE_HZ                      1000
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                128
#define configTOTAL_HEAP_SIZE                   (128 * 1024)
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configUSE_TIME_SLICING                  1

/* ============================================================================
 * DUAL-CORE (SMP) SUPPORT
 * ============================================================================ */

#define configNUM_CORES                         2
#define configUSE_CORE_AFFINITY                 0

/* ============================================================================
 * MEMORY ALLOCATION
 * ============================================================================ */

#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         0

/* ============================================================================
 * HOOK FUNCTIONS
 * ============================================================================ */

#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            0
#define configCHECK_FOR_STACK_OVERFLOW          2

/* ============================================================================
 * RUNTIME STATS
 * ============================================================================ */

#define configGENERATE_RUN_TIME_STATS           0

/* ============================================================================
 * CO-ROUTINES (disabled)
 * ============================================================================ */

#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         2

/* ============================================================================
 * SOFTWARE TIMERS
 * ============================================================================ */

#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE

/* ============================================================================
 * API INCLUDES
 * ============================================================================ */

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetHandle                  1

/* ============================================================================
 * QUEUE CONFIGURATION
 * ============================================================================ */

#define configUSE_QUEUE_SETS                    0
#define configQUEUE_REGISTRY_SIZE               10

/* ============================================================================
 * INTERRUPT PRIORITIES
 * ============================================================================ */

#define configMAX_SYSCALL_INTERRUPT_PRIORITY    5

/* ============================================================================
 * ASSERTION
 * ============================================================================ */

#define configASSERT(x) if((x) == 0) { portDISABLE_INTERRUPTS(); for(;;); }

/* ============================================================================
 * RP2040 SPECIFIC
 * ============================================================================ */

#include <stdint.h>
extern uint32_t SystemCoreClock;

#define configSUPPORT_PICO_SYNC_INTEROP         1
#define configSUPPORT_PICO_TIME_INTEROP         1

#endif /* FREERTOS_CONFIG_H */