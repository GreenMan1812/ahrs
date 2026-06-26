/*
 * icm20948.h
 *
 *  Created on: May 6, 2026
 *      Author: frees
 */

#ifndef APPLICATION_ICM20948_H_
#define APPLICATION_ICM20948_H_

#include "main.h"

/* ICM-20948 Registers */
#define ICM20948_ID						0xEA
#define REG_BANK_SEL					0x7F

// USER BANK 0
#define B0_WHO_AM_I						0x00
#define B0_USER_CTRL					0x03
#define B0_LP_CONFIG					0x05
#define B0_PWR_MGMT_1					0x06
#define B0_PWR_MGMT_2					0x07
#define B0_INT_PIN_CFG					0x0F
#define B0_INT_ENABLE					0x10
#define B0_INT_ENABLE_1					0x11
#define B0_INT_ENABLE_2					0x12
#define B0_INT_ENABLE_3					0x13
#define B0_I2C_MST_STATUS				0x17
#define B0_INT_STATUS					0x19
#define B0_INT_STATUS_1					0x1A
#define B0_INT_STATUS_2					0x1B
#define B0_INT_STATUS_3					0x1C
#define B0_DELAY_TIMEH					0x28
#define B0_DELAY_TIMEL					0x29
#define B0_ACCEL_XOUT_H					0x2D
#define B0_ACCEL_XOUT_L					0x2E
#define B0_ACCEL_YOUT_H					0x2F
#define B0_ACCEL_YOUT_L					0x30
#define B0_ACCEL_ZOUT_H					0x31
#define B0_ACCEL_ZOUT_L					0x32
#define B0_GYRO_XOUT_H					0x33
#define B0_GYRO_XOUT_L					0x34
#define B0_GYRO_YOUT_H					0x35
#define B0_GYRO_YOUT_L					0x36
#define B0_GYRO_ZOUT_H					0x37
#define B0_GYRO_ZOUT_L					0x38
#define B0_TEMP_OUT_H					0x39
#define B0_TEMP_OUT_L					0x3A
#define B0_EXT_SLV_SENS_DATA_00			0x3B
#define B0_EXT_SLV_SENS_DATA_01			0x3C
#define B0_EXT_SLV_SENS_DATA_02			0x3D
#define B0_EXT_SLV_SENS_DATA_03			0x3E
#define B0_EXT_SLV_SENS_DATA_04			0x3F
#define B0_EXT_SLV_SENS_DATA_05			0x40
#define B0_EXT_SLV_SENS_DATA_06			0x41
#define B0_EXT_SLV_SENS_DATA_07			0x42
#define B0_EXT_SLV_SENS_DATA_08			0x43
#define B0_EXT_SLV_SENS_DATA_09			0x44
#define B0_EXT_SLV_SENS_DATA_10			0x45
#define B0_EXT_SLV_SENS_DATA_11			0x46
#define B0_EXT_SLV_SENS_DATA_12			0x47
#define B0_EXT_SLV_SENS_DATA_13			0x48
#define B0_EXT_SLV_SENS_DATA_14			0x49
#define B0_EXT_SLV_SENS_DATA_15			0x4A
#define B0_EXT_SLV_SENS_DATA_16			0x4B
#define B0_EXT_SLV_SENS_DATA_17			0x4C
#define B0_EXT_SLV_SENS_DATA_18			0x4D
#define B0_EXT_SLV_SENS_DATA_19			0x4E
#define B0_EXT_SLV_SENS_DATA_20			0x4F
#define B0_EXT_SLV_SENS_DATA_21			0x50
#define B0_EXT_SLV_SENS_DATA_22			0x51
#define B0_EXT_SLV_SENS_DATA_23			0x52
#define B0_FIFO_EN_1					0x66
#define B0_FIFO_EN_2					0x67
#define B0_FIFO_RST						0x68
#define B0_FIFO_MODE					0x69
#define B0_FIFO_COUNTH					0X70
#define B0_FIFO_COUNTL					0X71
#define B0_FIFO_R_W						0x72
#define B0_DATA_RDY_STATUS				0x74
#define B0_FIFO_CFG						0x76

// USER BANK 1
#define B1_SELF_TEST_X_GYRO				0x02
#define B1_SELF_TEST_Y_GYRO				0x03
#define B1_SELF_TEST_Z_GYRO				0x04
#define B1_SELF_TEST_X_ACCEL			0x0E
#define B1_SELF_TEST_Y_ACCEL			0x0F
#define B1_SELF_TEST_Z_ACCEL			0x10
#define B1_XA_OFFS_H					0x14
#define B1_XA_OFFS_L					0x15
#define B1_YA_OFFS_H					0x17
#define B1_YA_OFFS_L					0x18
#define B1_ZA_OFFS_H					0x1A
#define B1_ZA_OFFS_L					0x1B
#define B1_TIMEBASE_CORRECTION_PLL		0x28

// USER BANK 2
#define B2_GYRO_SMPLRT_DIV				0x00
#define B2_GYRO_CONFIG_1				0x01
#define B2_GYRO_CONFIG_2				0x02
#define B2_XG_OFFS_USRH					0x03
#define B2_XG_OFFS_USRL 				0x04
#define B2_YG_OFFS_USRH					0x05
#define B2_YG_OFFS_USRL					0x06
#define B2_ZG_OFFS_USRH					0x07
#define B2_ZG_OFFS_USRL					0x08
#define B2_ODR_ALIGN_EN					0x09
#define B2_ACCEL_SMPLRT_DIV_1			0x10
#define B2_ACCEL_SMPLRT_DIV_2			0x11
#define B2_ACCEL_INTEL_CTRL				0x12
#define B2_ACCEL_WOM_THR				0x13
#define B2_ACCEL_CONFIG					0x14
#define B2_ACCEL_CONFIG_2				0x15
#define B2_FSYNC_CONFIG					0x52
#define B2_TEMP_CONFIG					0x53
#define B2_MOD_CTRL_USR					0X54

// USER BANK 3
#define B3_I2C_MST_ODR_CONFIG			0x00
#define B3_I2C_MST_CTRL					0x01
#define B3_I2C_MST_DELAY_CTRL			0x02
#define B3_I2C_SLV0_ADDR				0x03
#define B3_I2C_SLV0_REG					0x04
#define B3_I2C_SLV0_CTRL				0x05
#define B3_I2C_SLV0_DO					0x06
#define B3_I2C_SLV1_ADDR				0x07
#define B3_I2C_SLV1_REG					0x08
#define B3_I2C_SLV1_CTRL				0x09
#define B3_I2C_SLV1_DO					0x0A
#define B3_I2C_SLV2_ADDR				0x0B
#define B3_I2C_SLV2_REG					0x0C
#define B3_I2C_SLV2_CTRL				0x0D
#define B3_I2C_SLV2_DO					0x0E
#define B3_I2C_SLV3_ADDR				0x0F
#define B3_I2C_SLV3_REG					0x10
#define B3_I2C_SLV3_CTRL				0x11
#define B3_I2C_SLV3_DO					0x12
#define B3_I2C_SLV4_ADDR				0x13
#define B3_I2C_SLV4_REG					0x14
#define B3_I2C_SLV4_CTRL				0x15
#define B3_I2C_SLV4_DO					0x16
#define B3_I2C_SLV4_DI					0x17


/* AK09916 Registers */
#define AK09916_ID						0x09
#define MAG_SLAVE_ADDR                  0x0C

#define MAG_WIA2						0x01
#define MAG_ST1							0x10
#define MAG_HXL							0x11
#define MAG_HXH							0x12
#define MAG_HYL							0x13
#define MAG_HYH							0x14
#define MAG_HZL							0x15
#define MAG_HZH							0x16
#define MAG_ST2							0x18
#define MAG_CNTL2						0x31
#define MAG_CNTL3						0x32
#define MAG_TS1							0x33
#define MAG_TS2							0x34



// Set initial input parameters
#define  AFS_2G  0
#define  AFS_4G  1
#define  AFS_8G  2
#define  AFS_16G 3

#define  GFS_250DPS  0
#define  GFS_500DPS  1
#define  GFS_1000DPS 2
#define  GFS_2000DPS 3

#define  MFS_14BITS  0 // 0.6 mG per LSB
#define  MFS_16BITS  1    // 0.15 mG per LSB

#define M_8Hz   0x02
#define M_100Hz 0x06

#define GYRO_PREC GFS_250DPS
#define ACC_PREC  AFS_2G


#define LE_CONVERTER(VAL1, VAL2) ((int16_t)((VAL1 << 8) | VAL2))

#define MAG_CONVERTER(VAL1, VAL2) ((int16_t)((VAL1 << 8) | VAL2) * 0.15f)
extern uint8_t data_get;
//extern uint8_t mpu9250_tx_buffer[100];
//extern uint8_t mpu9250_rx_buffer[100];

#define AF05 0x05
#define AF06 0x06
#define Gpio_Af_5 0x05
#define Spi1_Enable()	SET_BIT(SPI1->CR1,  SPI_CR1_SPE)
#define Spi1_Disable()	CLEAR_BIT(SPI1->CR1,  SPI_CR1_SPE)
#define Spi3_Enable()	SET_BIT(SPI3->CR1,  SPI_CR1_SPE)
#define Spi3_Disable()	CLEAR_BIT(SPI3->CR1,  SPI_CR1_SPE)


#define SPI_BAUDRATEPRESCALER_DIV2      0x00000000U                                    /*!< BaudRate control equal to fPCLK/2   */
#define SPI_BAUDRATEPRESCALER_DIV4      (SPI_CR1_BR_0)                                 /*!< BaudRate control equal to fPCLK/4   */
#define SPI_BAUDRATEPRESCALER_DIV8      (SPI_CR1_BR_1)                                 /*!< BaudRate control equal to fPCLK/8   */
#define SPI_BAUDRATEPRESCALER_DIV16     (SPI_CR1_BR_1 | SPI_CR1_BR_0)                  /*!< BaudRate control equal to fPCLK/16  */
#define SPI_BAUDRATEPRESCALER_DIV32     (SPI_CR1_BR_2)                                 /*!< BaudRate control equal to fPCLK/32  */
#define SPI_BAUDRATEPRESCALER_DIV64     (SPI_CR1_BR_2 | SPI_CR1_BR_0)                  /*!< BaudRate control equal to fPCLK/64  */
#define SPI_BAUDRATEPRESCALER_DIV128    (SPI_CR1_BR_2 | SPI_CR1_BR_1)                  /*!< BaudRate control equal to fPCLK/128 */
#define SPI_BAUDRATEPRESCALER_DIV256    (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)   /*!< BaudRate control equal to fPCLK/256 */

#define SPI_DATAWIDTH_4BIT              (SPI_CR2_DS_0 | SPI_CR2_DS_1)                               /*!< Data length for SPI transfer:  4 bits */
#define SPI_DATAWIDTH_5BIT              (SPI_CR2_DS_2)                                              /*!< Data length for SPI transfer:  5 bits */
#define SPI_DATAWIDTH_6BIT              (SPI_CR2_DS_2 | SPI_CR2_DS_0)                               /*!< Data length for SPI transfer:  6 bits */
#define SPI_DATAWIDTH_7BIT              (SPI_CR2_DS_2 | SPI_CR2_DS_1)                               /*!< Data length for SPI transfer:  7 bits */
#define SPI_DATAWIDTH_8BIT              (SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0)                /*!< Data length for SPI transfer:  8 bits */
#define SPI_DATAWIDTH_9BIT              (SPI_CR2_DS_3)                                              /*!< Data length for SPI transfer:  9 bits */
#define SPI_DATAWIDTH_10BIT             (SPI_CR2_DS_3 | SPI_CR2_DS_0)                               /*!< Data length for SPI transfer: 10 bits */
#define SPI_DATAWIDTH_11BIT             (SPI_CR2_DS_3 | SPI_CR2_DS_1)                               /*!< Data length for SPI transfer: 11 bits */
#define SPI_DATAWIDTH_12BIT             (SPI_CR2_DS_3 | SPI_CR2_DS_1 | SPI_CR2_DS_0)                /*!< Data length for SPI transfer: 12 bits */
#define SPI_DATAWIDTH_13BIT             (SPI_CR2_DS_3 | SPI_CR2_DS_2)                               /*!< Data length for SPI transfer: 13 bits */
#define SPI_DATAWIDTH_14BIT             (SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_0)                /*!< Data length for SPI transfer: 14 bits */
#define SPI_DATAWIDTH_15BIT             (SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1)                /*!< Data length for SPI transfer: 15 bits */
#define SPI_DATAWIDTH_16BIT             (SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0) /*!< Data length for SPI transfer: 16 bits */



void mpu9250_init();
void mpu9250_Start();
#define FLOAT 0
#define Q15_T 1
#define valType FLOAT
extern uint32_t mpuPresc;
#if (valType == FLOAT)

void icm20948_byte2mpu();
typedef union{
	uint8_t byte[11*4];
	uint16_t halfWord[22];
	float arr[11];
	struct
	{
		float flag;
		float acc[3];
		float gyr[3];
		float mag[3];
		uint32_t smt;
		// /* data */
	};
	
	struct{
		float data_Flag;
		float ax;
		float ay;
		float az;
		float gx;
		float gy;
		float gz;
		float mx;
		float my;
		float mz;
		uint32_t time;
	};
}mpuData_t;
#elif valType == Q15_T
typedef union{
	uint8_t byte_data[11*2];
	q15_t arr[11];
	struct{
		q15_t data_Flag;
		q15_t ax;
		q15_t ay;
		q15_t az;
		q15_t gx;
		q15_t gy;
		q15_t gz;
		q15_t mx;
		q15_t my;
		q15_t mz;
		q15_t t;
	};
}mpuData_t;
#endif

typedef void (*Dma_Callback)(void);
typedef struct
{
    uint32_t prescaler;
    uint32_t prescalerCount;
    mpuData_t data;
    uint8_t accel_conf; // 1 .. 3
    uint8_t gyro_conf;  //  1 .. 3
    uint8_t stateIQR;
    uint32_t timer;
    uint32_t timArr;
    uint32_t timPre;
    Dma_Callback volatile dmaTX_callback;
    uint8_t is_data_read;
    float inv_accel_sens;
    float inv_gyro_sens;
    uint8_t changeStatus;
	uint8_t mesurement_flag;
} icm20948Config_t;
mpuData_t icm20948_getData();
void change_in_flow(uint32_t prescaler);
void icm20948_start_mesurement();
void icm20948_stop_mesurement();
void icm20948_init();
void icm20948_start();
void delay_ms(uint16_t ms);
extern mpuData_t raw_data;
extern icm20948Config_t icm20948Config;

//extern mpuConfig_t mpuConfig;
// extern uint32_t is_icm_data_read;
#define is_gyro_accel_config_change (uint8_t)(0x03 & icm20948Config.changeStatus)
#define is_accel_config_change (uint8_t)(0x01 & icm20948Config.changeStatus)
#define is_gyro_config_change (uint8_t)(0x02 & icm20948Config.changeStatus)
#define is_presc_config_change (uint8_t)(0x04 & icm20948Config.changeStatus)

#define accel_config_change_clear CLEAR_BIT(icm20948Config.changeStatus, 0x01)
#define gyro_config_change_clear CLEAR_BIT(icm20948Config.changeStatus, 0x02)
#define presc_config_change_clear CLEAR_BIT(icm20948Config.changeStatus, 0x04)

#define accel_config_change_set SET_BIT(icm20948Config.changeStatus, 0x01)
#define gyro_config_change_set SET_BIT(icm20948Config.changeStatus, 0x02)
#define presc_config_change_set SET_BIT(icm20948Config.changeStatus, 0x04)
#endif /* APPLICATION_ICM20948_H_ */
