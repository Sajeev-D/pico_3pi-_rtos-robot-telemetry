#include "imu_bare_metal.h"
#include "hardware/gpio.h"

uint8_t imu_bare_metal_init(void) {
    // Step 1: Configure GPIO pins for I2C function
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    
    // Step 2: Disable I2C before configuration
    // We must write 0 to the ENABLE bit to modify most configuration registers [13, 14]
    IC_ENABLE = 0;
    
    // Step 3: Configure IC_CON register
    // Enable Master Mode, Fast Mode (400kHz), Disable Slave Mode, Enable Restarts [4, 15-21]
    IC_CON = IC_CON_MASTER_MODE | 
             IC_CON_SPEED_FAST | 
             IC_CON_IC_SLAVE_DISABLE | 
             IC_CON_IC_RESTART_EN;
    
    // Step 4: Set target address
    // Set the 7-bit address of the LSM6DSO (0x6B) [8, 22]
    IC_TAR = LSM6DSO_I2C_ADDR;
    
    // Step 5: Configure clock timing
    // Write the calculated high/low counts and spike suppression limit [7, 23-26]
    IC_FS_SCL_HCNT = I2C_FAST_MODE_HCNT;
    IC_FS_SCL_LCNT = I2C_FAST_MODE_LCNT;
    IC_FS_SPKLEN = I2C_FAST_MODE_SPKLEN;
    
    // Step 6: Enable I2C
    // Write 1 to the ENABLE bit to activate the controller [7, 27]
    IC_ENABLE = IC_ENABLE_ENABLE;
    
    return 0; // Success
}

uint8_t imu_bare_metal_read_register(uint8_t device_addr, uint8_t reg_addr) {
    // Note: We assume IC_TAR is already set to device_addr from init
    // If you need to change devices, disable/re-enable I2C first
    
    // Step 1: Write register address to read from
    // CMD=0 (Write), RESTART=0, STOP=0
    IC_DATA_CMD = reg_addr;
    
    // Step 2: Request one byte read
    // CMD=1 (Read), RESTART=1 (change direction), STOP=1 (end transaction)
    IC_DATA_CMD = IC_DATA_CMD_CMD | IC_DATA_CMD_RESTART | IC_DATA_CMD_STOP;
    
    // Step 3: Wait for data and check for errors
    while (1) {
        // Check for transaction abort
        if (IC_RAW_INTR_STAT & IC_RAW_INTR_STAT_TX_ABRT) {
            // Read abort source for debugging (optional but useful)
            uint32_t abort_source = IC_TX_ABRT_SOURCE;
            // Clear the abort interrupt
            (void)IC_CLR_TX_ABRT;
            return 0xFF; // Error
        }

        // Check if data is available
        if (IC_STATUS & IC_STATUS_RFNE) {
            break; // Data ready
        }
    }
    
    // Step 4: Read and return data
    return (uint8_t)(IC_DATA_CMD & 0xFF);
}