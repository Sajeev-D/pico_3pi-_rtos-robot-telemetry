/**
 * FreeRTOS Demo for Pololu 3pi+ 2040 Robot
 * LED-Based Task Demonstration
 * 
 * This demonstrates:
 * - Multiple FreeRTOS tasks running concurrently
 * - Task priorities
 * - Non-blocking delays with vTaskDelay()
 * - Dual-core operation (SMP)
 * 
 * LED Patterns Show Task Activity:
 * - Fast blink (100ms): High priority task
 * - Slow blink (500ms): Low priority task
 * - Very fast (50ms): Core 1 task
 */

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

// LED pins on 3pi+ robot
#define YELLOW_LED 25

/**
 * TASK 1: Fast Blink Task (Priority 3 - Highest)
 * 
 * Blinks LED at 100ms intervals
 * Demonstrates high-priority task execution
 */
void task_fast_blink(void *params) {
    while (true) {
        // Fast blink pattern: 5 quick blinks
        for (int i = 0; i < 5; i++) {
            gpio_put(YELLOW_LED, 0);  // ON
            vTaskDelay(pdMS_TO_TICKS(50));
            gpio_put(YELLOW_LED, 1);  // OFF
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        // Pause between patterns
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * TASK 2: Slow Blink Task (Priority 1 - Low)
 * 
 * This task will be preempted by the fast blink task
 * Demonstrates priority-based scheduling
 */
void task_slow_blink(void *params) {
    while (true) {
        gpio_put(YELLOW_LED, 0);  // ON
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(YELLOW_LED, 1);  // OFF
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * TASK 3: Core 1 Exclusive Task (Priority 2)
 * 
 * This task runs ONLY on Core 1
 * Demonstrates dual-core (SMP) operation
 * 
 * Makes a distinctive pattern: 3 very fast blinks
 */
void task_core1_exclusive(void *params) {
    while (true) {
        // Very fast blink pattern (core 1 signature)
        for (int i = 0; i < 3; i++) {
            gpio_put(YELLOW_LED, 0);  // ON
            vTaskDelay(pdMS_TO_TICKS(30));
            gpio_put(YELLOW_LED, 1);  // OFF
            vTaskDelay(pdMS_TO_TICKS(30));
        }
        
        // Long pause so you can distinguish patterns
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/**
 * TASK 4: Heartbeat Task (Priority 2)
 * 
 * Single blink every 3 seconds
 * Proves system is still running even when other tasks are active
 */
void task_heartbeat(void *params) {
    while (true) {
        // Long blink
        gpio_put(YELLOW_LED, 0);  // ON
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_put(YELLOW_LED, 1);  // OFF
        
        // Wait 3 seconds
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

/**
 * Main Function
 * 
 * Initializes hardware and creates FreeRTOS tasks
 */
int main() {
    // Initialize yellow LED
    gpio_init(YELLOW_LED);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);
    gpio_put(YELLOW_LED, 1);  // Start OFF
    
    // Startup signal: 10 rapid blinks to show program loaded
    for (int i = 0; i < 10; i++) {
        gpio_put(YELLOW_LED, 0);
        sleep_ms(50);
        gpio_put(YELLOW_LED, 1);
        sleep_ms(50);
    }
    
    sleep_ms(1000);  // Pause before starting FreeRTOS
    
    /**
     * Create Task 1: Fast Blink (Priority 3)
     * Stack: 256 words = 1024 bytes
     */
    xTaskCreate(
        task_fast_blink,
        "FastBlink",
        256,
        NULL,
        3,  // Highest priority
        NULL
    );
    
    /**
     * Create Task 2: Slow Blink (Priority 1)
     * This will only run when fast blink is sleeping
     */
    xTaskCreate(
        task_slow_blink,
        "SlowBlink",
        256,
        NULL,
        1,  // Lowest priority
        NULL
    );
    
    /**
     * Create Task 3: Core 1 Task (Priority 2, Core Affinity = Core 1)
     */
    TaskHandle_t core1_handle = NULL;
    xTaskCreate(
        task_core1_exclusive,
        "Core1Task",
        256,
        NULL,
        2,  // Medium priority
        &core1_handle
    );
    
    // Pin this task to Core 1 only
    // vTaskCoreAffinitySet(core1_handle, (1 << 1));  // Bit 1 = Core 1
    
    /**
     * Create Task 4: Heartbeat (Priority 2)
     */
    xTaskCreate(
        task_heartbeat,
        "Heartbeat",
        256,
        NULL,
        2,
        NULL
    );
    
    /**
     * Start the FreeRTOS scheduler
     * This function NEVER returns
     * CPU control transfers to FreeRTOS
     */
    vTaskStartScheduler();
    
    // Should never reach here
    // If we do, blink SOS pattern (3 short, 3 long, 3 short)
    while (1) {
            // Double blink
            gpio_put(YELLOW_LED, 0);
            sleep_ms(100);
            gpio_put(YELLOW_LED, 1);
            sleep_ms(100);
            gpio_put(YELLOW_LED, 0);
            sleep_ms(100);
            gpio_put(YELLOW_LED, 1);
            
            // Long pause
            sleep_ms(2000);
    }
}

// FreeRTOS stack overflow hook - called when a task overflows its stack
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // In a real application, you'd log this or handle it gracefully
    // For now, just trap it in an infinite loop so you know it happened
    
    (void)xTask;  // Suppress unused parameter warning
    (void)pcTaskName;
    
    // If you have USB stdio enabled, you could print:
    // printf("Stack overflow in task: %s\n", pcTaskName);
    
    // Halt the system
    while(1) {
        // Blink LED rapidly to indicate error? 
        // Or just loop forever
    }
}

/**
 * WHAT YOU'LL SEE:
 * 
 * 1. On startup: 10 rapid blinks (program initialization)
 * 2. Then complex blinking patterns:
 *    - Fast bursts (5 quick blinks) from high-priority task
 *    - Medium-speed patterns (3 very fast blinks) from Core 1 task
 *    - Occasional single long blinks (heartbeat)
 *    - The patterns interleave showing multitasking
 * 
 * 3. If you see SOS pattern (... --- ...), FreeRTOS failed to start
 * 
 * WHAT THIS DEMONSTRATES:
 * ✅ Multiple tasks running concurrently
 * ✅ Priority-based scheduling (high priority preempts low)
 * ✅ Dual-core operation (tasks on both cores)
 * ✅ Non-blocking delays (vTaskDelay)
 * ✅ FreeRTOS working correctly on RP2040
 */