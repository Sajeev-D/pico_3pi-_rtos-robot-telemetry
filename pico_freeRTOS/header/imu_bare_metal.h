#ifndef IMU_BARE_METAL_H
#define IMU_BARE_METAL_H

#include <stdint.h>

// ============================================================================
// I2C0 Hardware Register Definitions (from RP2040 Datasheet Section 4.7)
// ============================================================================

// Base address for I2C0 peripheral
// Note: I2C0_BASE is already defined by the Pico SDK in hardware/regs/addressmap.h
// Using SDK definition instead of redefining it

#define IC_CON_OFFSET           0x00  // Control register [1]
#define IC_TAR_OFFSET           0x04  // Target address register [1]
#define IC_DATA_CMD_OFFSET      0x10  // Data buffer and command register [1]
#define IC_FS_SCL_HCNT_OFFSET   0x1c  // Fast mode SCL high count [2]
#define IC_FS_SCL_LCNT_OFFSET   0x20  // Fast mode SCL low count [2]
#define IC_FS_SPKLEN_OFFSET     0xa0  // Fast mode spike suppression limit [3]
#define IC_ENABLE_OFFSET        0x6c  // Enable register [4]
#define IC_STATUS_OFFSET        0x70  // Status register [5]
#define IC_RAW_INTR_STAT_OFFSET 0x34  // Raw interrupt status [2]
#define IC_TX_ABRT_SOURCE_OFFSET 0x80 // TX abort source register [5]
#define IC_CLR_TX_ABRT_OFFSET   0x54  // Clear TX_ABRT interrupt

// Helper macro to access registers
#define I2C0_REG(offset) (*(volatile uint32_t*)(I2C0_BASE + (offset)))

// Specific register access macros
#define IC_CON           I2C0_REG(IC_CON_OFFSET)
#define IC_TAR           I2C0_REG(IC_TAR_OFFSET)
#define IC_DATA_CMD      I2C0_REG(IC_DATA_CMD_OFFSET)
#define IC_FS_SCL_HCNT   I2C0_REG(IC_FS_SCL_HCNT_OFFSET)
#define IC_FS_SCL_LCNT   I2C0_REG(IC_FS_SCL_LCNT_OFFSET)
#define IC_FS_SPKLEN     I2C0_REG(IC_FS_SPKLEN_OFFSET)
#define IC_ENABLE        I2C0_REG(IC_ENABLE_OFFSET)
#define IC_STATUS        I2C0_REG(IC_STATUS_OFFSET)
#define IC_RAW_INTR_STAT I2C0_REG(IC_RAW_INTR_STAT_OFFSET)
#define IC_TX_ABRT_SOURCE I2C0_REG(IC_TX_ABRT_SOURCE_OFFSET)
#define IC_CLR_TX_ABRT    I2C0_REG(IC_CLR_TX_ABRT_OFFSET)

// ============================================================================
// IC_CON Register Bit Definitions
// ============================================================================
#define IC_CON_MASTER_MODE      (1 << 0)  // Bit 0: Master mode enabled [1]
#define IC_CON_SPEED_FAST       (2 << 1)  // Bits 2:1: Speed mode (2 = Fast Mode/400kHz) [2]
#define IC_CON_IC_SLAVE_DISABLE (1 << 6)  // Bit 6: Slave mode disabled [3]
#define IC_CON_IC_RESTART_EN    (1 << 5)  // Bit 5: Master restart enabled [4]

// ============================================================================
// IC_DATA_CMD Register Bit Definitions
// ============================================================================
#define IC_DATA_CMD_CMD         (1 << 8)   // 1 = read, 0 = write [1]
#define IC_DATA_CMD_STOP        (1 << 9)   // Issue STOP after this byte [2]
#define IC_DATA_CMD_RESTART     (1 << 10)  // Issue RESTART before this byte [3]

// ============================================================================
// IC_RAW_INTR_STAT Register Bit Definitions
// ============================================================================
#define IC_RAW_INTR_STAT_RX_FULL (1 << 2)  // RX FIFO is full
#define IC_RAW_INTR_STAT_TX_ABRT (1 << 6)  // Transaction aborted

// ============================================================================
// IC_STATUS Register Bit Definitions
// ============================================================================
#define IC_STATUS_ACTIVITY      (1 << 0)  // I2C activity status
#define IC_STATUS_RFNE          (1 << 3)  // RX FIFO not empty
#define IC_STATUS_TFE           (1 << 2)  // TX FIFO completely empty

// ============================================================================
// IC_ENABLE Register
// ============================================================================
#define IC_ENABLE_ENABLE        (1 << 0)  // Enable I2C controller

// ============================================================================
// Clock Configuration (you already calculated these!)
// ============================================================================
#define I2C_FAST_MODE_HCNT  100
#define I2C_FAST_MODE_LCNT  213
#define I2C_FAST_MODE_SPKLEN 1

// ============================================================================
// IMU Device Definitions
// ============================================================================
#define LSM6DSO_I2C_ADDR    0x6B
#define LSM6DSO_WHO_AM_I    0x0F
#define LSM6DSO_EXPECTED_ID 0x6C

// ============================================================================
// Function Declarations
// ============================================================================

/**
 * Initialize I2C0 peripheral for bare metal operation
 * Returns: 0 on success, error code on failure
 */
uint8_t imu_bare_metal_init(void);

/**
 * Read a single register from I2C device
 * @param device_addr: 7-bit I2C device address
 * @param reg_addr: Register address to read from
 * Returns: Register value, or 0xFF on error
 */
uint8_t imu_bare_metal_read_register(uint8_t device_addr, uint8_t reg_addr);

#endif // IMU_BARE_METAL_H