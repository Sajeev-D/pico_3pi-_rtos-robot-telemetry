#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/*
@description
 * Initialize I2C communication for IMU sensor
 * Must be called before any other IMU functions

@note
* Peripherals - specialized hardware modules that do specific jobs
* Each GPIO pin can be configured to do one function -  refer to datasheet
* The purpose of the I2C module is to offload tasks from the CPU
* Blocking means a program's execution waits for a specific task to complete before it can continue
*/
void imu_i2c_init(void);

/*
@description
 * Test if LSM6DSO responds on I2C bus
 * Returns: true if device acknowledges, false otherwise
*/
bool imu_test_connection(void);

/*
@description
 * Read WHO_AM_I register to verify chip identity
 * Returns: true if correct chip ID detected
 */
bool imu_check_who_am_i(void);

/*
@description
 * Read a single register from LSM6DSO
 * reg_addr: Register address to read
 * Returns: Register value (8-bit)
 */
uint8_t imu_read_register(uint8_t reg_addr);

/*
@description
 * Write a single register to LSM6DSO
 * reg_addr: Register address to write
 * data: Value to write (8-bit)
 */
void imu_write_register(uint8_t reg_addr, uint8_t data);

#endif // IMU_DRIVER.H