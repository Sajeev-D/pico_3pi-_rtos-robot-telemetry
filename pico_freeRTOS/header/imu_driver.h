#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

void imu_i2c_init(void);
bool imu_test_connection(void);
bool imu_check_who_am_i(void);
uint8_t imu_read_register(uint8_t reg_addr);
void imu_write_register(uint8_t reg_addr, uint8_t data);

#endif // IMU_DRIVER.H