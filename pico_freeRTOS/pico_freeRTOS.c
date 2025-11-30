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
#include <stdio.h>
#include "header/imu_driver.h"

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

// Task to test IMU I2C communication
void imu_test_task(void *params) {
    stdio_init_all();  // Initialize stdio for debugging output
    gpio_put(YELLOW_LED, 1); // Ensure LED is OFF initially
    vTaskDelay(pdMS_TO_TICKS(4000));  // Wait for USB to enumerate
    gpio_put(YELLOW_LED, 0); // Turn LED ON to indicate start

    // Step 1: Initialize I2C hardware
    printf("[1/3] Initializing I2C...\n");
    imu_i2c_init();
    gpio_put(YELLOW_LED, 1); // Turn LED OFF
    vTaskDelay(pdMS_TO_TICKS(2000));  // Brief delay after init
    gpio_put(YELLOW_LED, 0); // Turn LED ON
    
    // Step 2: Test if device responds
    printf("\n[2/3] Testing device connection...\n");
    if (!imu_test_connection()) {
        printf("\n❌ FAILED: Cannot detect IMU on I2C bus\n");
        printf("Check: GP4 (SDA) and GP5 (SCL) connections\n");
        gpio_put(YELLOW_LED, 1); // Turn LED OFF
        vTaskDelete(NULL);
    }
    
    // Step 3: Verify chip identity
    printf("\n[3/3] Verifying chip identity...\n");
    if (!imu_check_who_am_i()) {
        printf("\n❌ FAILED: Wrong chip ID\n");
        printf("Possible causes:\n");
        printf("  - Wrong I2C address (try 0x6A instead of 0x6B)\n");
        printf("  - Communication error\n");
        printf("  - Reading wrong sensor\n");
        gpio_put(YELLOW_LED, 1); // Turn LED OFF
        vTaskDelete(NULL);
    }
    
    // Success!
    printf("\n");
    printf("=====================================\n");
    printf("  ✓ IMU I2C Communication SUCCESS\n");
    printf("=====================================\n");
    printf("\nReady to configure and read sensor data!\n\n");

    // Test I2C write and read functions
    printf("\n[TESTING] I2C Write/Read Functions\n");
    printf("=====================================\n");

    // Test 1: Read WHO_AM_I register directly using imu_read_register
    printf("\nTest 1: Read WHO_AM_I register (0x0F)\n");
    uint8_t who_am_i = imu_read_register(0x0F);
    printf("  Result: 0x%02X (expected 0x6C)\n", who_am_i);

    // Test 2: Read STATUS_REG register (0x1E) - shows if new data is available
    printf("\nTest 2: Read STATUS_REG (0x1E)\n");
    uint8_t status = imu_read_register(0x1E);
    printf("  Result: 0x%02X\n", status);
    printf("  Bit 0 (XLDA): %d (Accel data ready)\n", (status & 0x01));
    printf("  Bit 1 (GDA): %d (Gyro data ready)\n", (status & 0x02) >> 1);

    // Test 3: Write to CTRL1_XL register (0x10) to configure accelerometer
    // Value: 0x60 = 416 Hz ODR, ±2g scale
    printf("\nTest 3: Write to CTRL1_XL (0x10) - Configure Accelerometer\n");
    printf("  Writing: 0x60 (416 Hz, ±2g scale)\n");
    imu_write_register(0x10, 0x60);

    // Read back to verify the write
    uint8_t ctrl1_xl = imu_read_register(0x10);
    printf("  Read back: 0x%02X\n", ctrl1_xl);
    if (ctrl1_xl == 0x60) {
        printf("  ✓ Write verified!\n");
    } else {
        printf("  ✗ Write failed (mismatch)\n");
    }

    // Test 4: Write to CTRL2_G register (0x11) to configure gyroscope
    // Value: 0x60 = 416 Hz ODR, 250 dps scale
    printf("\nTest 4: Write to CTRL2_G (0x11) - Configure Gyroscope\n");
    printf("  Writing: 0x60 (416 Hz, 250 dps scale)\n");
    imu_write_register(0x11, 0x60);

    // Read back to verify the write
    uint8_t ctrl2_g = imu_read_register(0x11);
    printf("  Read back: 0x%02X\n", ctrl2_g);
    if (ctrl2_g == 0x60) {
        printf("  ✓ Write verified!\n");
    } else {
        printf("  ✗ Write failed (mismatch)\n");
    }

    // Test 5: Read some accelerometer data (registers 0x28-0x2D)
    printf("\nTest 5: Read Accelerometer Raw Data\n");
    printf("  Reading 6 bytes from registers 0x28-0x2D...\n");
    uint8_t accel_x_l = imu_read_register(0x28);
    uint8_t accel_x_h = imu_read_register(0x29);
    uint8_t accel_y_l = imu_read_register(0x2A);
    uint8_t accel_y_h = imu_read_register(0x2B);
    uint8_t accel_z_l = imu_read_register(0x2C);
    uint8_t accel_z_h = imu_read_register(0x2D);

    int16_t accel_x = (int16_t)((accel_x_h << 8) | accel_x_l);
    int16_t accel_y = (int16_t)((accel_y_h << 8) | accel_y_l);
    int16_t accel_z = (int16_t)((accel_z_h << 8) | accel_z_l);

    printf("  X-axis: %d (raw)\n", accel_x);
    printf("  Y-axis: %d (raw)\n", accel_y);
    printf("  Z-axis: %d (raw)\n", accel_z);

    printf("\n=====================================\n");
    printf("  ✓ I2C READ/WRITE TESTS COMPLETE\n");
    printf("=====================================\n\n");

    gpio_put(YELLOW_LED, 1); // Turn LED OFF

    // Task complete - delete itself
    vTaskDelete(NULL);
}

/**
 * Main Function
 * 
 * Initializes hardware and creates FreeRTOS tasks
 */
int main() {
    // stdio_init_all();  // Initialize stdio (for debugging if needed)

    // CRITICAL: Wait for USB to enumerate
    // vTaskDelay(pdMS_TO_TICKS(2000));    
    
    // printf("\n\n");
    // printf("*************************************\n");
    // printf("  Pico FreeRTOS - IMU Driver Test\n");
    // printf("*************************************\n");
    
    // Create IMU test task
    xTaskCreate(
        imu_test_task,      // Task function
        "IMU_Test",         // Task name (for debugging)
        512,                // Stack size (words, not bytes)
        NULL,               // Parameters
        1,                  // Priority
        NULL                // Task handle
    );

    // Initialize the LED pin and toggle it ON
    gpio_init(YELLOW_LED);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);

    gpio_put(YELLOW_LED, 0); // drive LED OFF (3pi+ LED is active-low)

    // Start FreeRTOS scheduler
    vTaskStartScheduler();

    gpio_put(YELLOW_LED, 1);

    while(1) {}

    // while(1) {
    //     gpio_put(YELLOW_LED, 0);  // ON
    //     sleep_ms(1000);
    //     gpio_put(YELLOW_LED, 1);  // OFF
    //     sleep_ms(1000);
    // }

    // Test message - you should see this in your serial terminal!
    // while (1) {
    //     printf("Hello, FreeRTOS on 3pi+ 2040!\n");
    //     sleep_ms(1000);
    // }
    
    return 0;

    // // Initialize yellow LED
    // gpio_init(YELLOW_LED);
    // gpio_set_dir(YELLOW_LED, GPIO_OUT);
    // gpio_put(YELLOW_LED, 1);  // Start OFF
    
    // // Startup signal: 10 rapid blinks to show program loaded
    // for (int i = 0; i < 10; i++) {
    //     gpio_put(YELLOW_LED, 0);
    //     sleep_ms(50);
    //     gpio_put(YELLOW_LED, 1);
    //     sleep_ms(50);
    // }
    
    // sleep_ms(1000);  // Pause before starting FreeRTOS
    
    // /**
    //  * Create Task 1: Fast Blink (Priority 3)
    //  * Stack: 256 words = 1024 bytes
    //  */
    // xTaskCreate(
    //     task_fast_blink,
    //     "FastBlink",
    //     256,
    //     NULL,
    //     3,  // Highest priority
    //     NULL
    // );
    
    // /**
    //  * Create Task 2: Slow Blink (Priority 1)
    //  * This will only run when fast blink is sleeping
    //  */
    // xTaskCreate(
    //     task_slow_blink,
    //     "SlowBlink",
    //     256,
    //     NULL,
    //     1,  // Lowest priority
    //     NULL
    // );
    
    // /**
    //  * Create Task 3: Core 1 Task (Priority 2, Core Affinity = Core 1)
    //  */
    // TaskHandle_t core1_handle = NULL;
    // xTaskCreate(
    //     task_core1_exclusive,
    //     "Core1Task",
    //     256,
    //     NULL,
    //     2,  // Medium priority
    //     &core1_handle
    // );
    
    // // Pin this task to Core 1 only
    // // vTaskCoreAffinitySet(core1_handle, (1 << 1));  // Bit 1 = Core 1
    
    // /**
    //  * Create Task 4: Heartbeat (Priority 2)
    //  */
    // xTaskCreate(
    //     task_heartbeat,
    //     "Heartbeat",
    //     256,
    //     NULL,
    //     2,
    //     NULL
    // );
    
    // /**
    //  * Start the FreeRTOS scheduler
    //  * This function NEVER returns
    //  * CPU control transfers to FreeRTOS
    //  */
    // vTaskStartScheduler();
    
    // // Should never reach here
    // // If we do, blink SOS pattern (3 short, 3 long, 3 short)
    // while (1) {
    //         // Double blink
    //         gpio_put(YELLOW_LED, 0);
    //         sleep_ms(100);
    //         gpio_put(YELLOW_LED, 1);
    //         sleep_ms(100);
    //         gpio_put(YELLOW_LED, 0);
    //         sleep_ms(100);
    //         gpio_put(YELLOW_LED, 1);
            
    //         // Long pause
    //         sleep_ms(2000);
    // }
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