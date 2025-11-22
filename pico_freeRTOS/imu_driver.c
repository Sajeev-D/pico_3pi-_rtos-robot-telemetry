#include "imu_driver.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

// ============================================================================
// Hardware Configuration
// ============================================================================

// I2C Configuration (from 3pi+ 2040 pin assignments)
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4      // GP4 - Inertial sensors SDA
#define I2C_SCL_PIN 5      // GP5 - Inertial sensors SCL
#define I2C_SPEED 400000   // 400kHz - LSM6DSO supports up to 1MHz

// LSM6DSO I2C Address (7-bit address)
#define LSM6DSO_ADDR 0x6B  // Alternative: 0x6A (depends on SA0 pin)

// LSM6DSO Register Map
#define LSM6DSO_WHO_AM_I 0x0F
#define LSM6DSO_WHO_AM_I_VALUE 0x6C  // Expected chip ID

// ============================================================================
// Public Functions
// ============================================================================

/**
 * Initialize I2C communication for IMU sensor
 * Must be called before any other IMU functions
 */
void imu_i2c_init(void) {
    // Initialize I2C peripheral at 400kHz
    i2c_init(I2C_PORT, I2C_SPEED);
    
    // Configure GPIO pins for I2C function
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    
    // Enable internal pull-ups (board has external 10kΩ, but doesn't hurt)
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    printf("[IMU] I2C initialized: GP%d (SDA), GP%d (SCL) @ %dkHz\n", 
           I2C_SDA_PIN, I2C_SCL_PIN, I2C_SPEED / 1000);
}

/**
 * Test if LSM6DSO responds on I2C bus
 * Returns: true if device acknowledges, false otherwise
 */
bool imu_test_connection(void) {
    uint8_t dummy;
    
    // Attempt to read 1 byte - just testing for ACK
    int result = i2c_read_blocking(I2C_PORT, LSM6DSO_ADDR, &dummy, 1, false);
    
    if (result < 0) {
        printf("[IMU] ERROR: No device at address 0x%02X\n", LSM6DSO_ADDR);
        return false;
    }
    
    printf("[IMU] Device detected at address 0x%02X\n", LSM6DSO_ADDR);
    return true;
}

/**
 * Read WHO_AM_I register to verify chip identity
 * Returns: true if correct chip ID detected
 */
bool imu_check_who_am_i(void) {
    uint8_t chip_id = imu_read_register(LSM6DSO_WHO_AM_I);
    
    printf("[IMU] WHO_AM_I: 0x%02X (expected 0x%02X)\n", 
           chip_id, LSM6DSO_WHO_AM_I_VALUE);
    
    if (chip_id == LSM6DSO_WHO_AM_I_VALUE) {
        printf("[IMU] ✓ LSM6DSO chip verified\n");
        return true;
    } else {
        printf("[IMU] ✗ Unexpected chip ID (wrong sensor or communication error)\n");
        return false;
    }
}

/**
 * Read a single register from LSM6DSO
 * 
 * reg_addr: Register address to read
 * Returns: Register value (8-bit)
 */
uint8_t imu_read_register(uint8_t reg_addr) {
    uint8_t data;
    
    // I2C register read sequence:
    // 1. Write register address (with repeated start)
    // 2. Read data byte
    i2c_write_blocking(I2C_PORT, LSM6DSO_ADDR, &reg_addr, 1, true);  // keep control
    i2c_read_blocking(I2C_PORT, LSM6DSO_ADDR, &data, 1, false);      // release bus
    
    return data;
}

/**
 * Write a single register to LSM6DSO
 * 
 * reg_addr: Register address to write
 * data: Value to write (8-bit)
 */
void imu_write_register(uint8_t reg_addr, uint8_t data) {
    uint8_t buffer[2];
    buffer[0] = reg_addr;
    buffer[1] = data;
    
    // I2C register write sequence:
    // Send register address followed by data byte
    i2c_write_blocking(I2C_PORT, LSM6DSO_ADDR, buffer, 2, false);
}