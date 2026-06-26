#include "_icm20948.h"

#define SPI_DmaBuffSize (uint8_t)24
__attribute__((aligned(4))) uint8_t icm_tx_buffer[SPI_DmaBuffSize];
__attribute__((aligned(4))) uint8_t icm_rx_buffer[SPI_DmaBuffSize] = {0, 0, 0, 0};

__attribute__((aligned(4))) uint8_t debug_string[32];

void Select_Bank(uint8_t bank );
// void delay_ms(uint16_t ms);
// static uint8_t is_dma_trnsmit();
static uint8_t is_icm_dma_transmit();
uint8_t Spi_Dma_Read(uint8_t reg);
void Spi_Dma_Write(uint8_t reg, uint8_t data);

icm20948Config_t icm20948Config = {.dmaTX_callback = NULL};
mpuData_t raw_data;

// state IQR data

#define icm_spi_Enable() GPIOA->BSRR = GPIO_BSRR_BR3;
#define icm_spi_Disable() GPIOA->BSRR = GPIO_BSRR_BS3;

#define time_iqr_GET(byte) (uint8_t)((1 << 0) & byte)
#define dmaTX_iqr_GET(byte) (uint8_t)((1 << 1) & byte)
#define dmaRX_iqr_GET(byte) (uint8_t)((1 << 2) & byte)
// #define is_data_read(byte)  (uint8_t)((1 << 3) & byte)
// #define time_iqr_end(byte)  (uint8_t)((1 << 0) & byte)
#define dmaTX_iqr_SET(byte) SET_BIT(byte, 0x02)
#define dmaRX_iqr_SET(byte) SET_BIT(byte, 0x04)
#define dmaTX_iqr_CLEAR(byte) CLEAR_BIT(byte, 0x02)
#define dmaRX_iqr_CLEAR(byte) CLEAR_BIT(byte, 0x04)

#define TX_DMA_SPI DMA2_Stream3
#define RX_DMA_SPI DMA2_Stream2

//
mpuData_t icm20948_getData(){
    #define D icm20948Config.data
        D.ax = LE_CONVERTER(icm_rx_buffer[1], icm_rx_buffer[2]) *   icm20948Config.inv_accel_sens;
        D.ay = LE_CONVERTER(icm_rx_buffer[3], icm_rx_buffer[4]) *   icm20948Config.inv_accel_sens;
        D.az = LE_CONVERTER(icm_rx_buffer[5], icm_rx_buffer[6]) *   icm20948Config.inv_accel_sens;
        D.gx = LE_CONVERTER(icm_rx_buffer[7], icm_rx_buffer[8]) *   icm20948Config.inv_gyro_sens;
        D.gy = LE_CONVERTER(icm_rx_buffer[9], icm_rx_buffer[10]) *  icm20948Config.inv_gyro_sens;
        D.gz = LE_CONVERTER(icm_rx_buffer[11], icm_rx_buffer[12]) * icm20948Config.inv_gyro_sens;
        D.mx = MAG_CONVERTER(icm_rx_buffer[17], icm_rx_buffer[16]);
        D.my = MAG_CONVERTER(icm_rx_buffer[19], icm_rx_buffer[18]);
        D.mz = MAG_CONVERTER(icm_rx_buffer[21], icm_rx_buffer[20]);
    #undef D
    return icm20948Config.data;
}
void Select_Bank(uint8_t bank){
	Spi_Dma_Write(0x7F, bank<<4);
	delay_ms(5);
}
void Dma_Mpu_Config_Callback()
{
    icm20948Config.is_data_read = 0;
}
void Dma_Transmit_Callback()
{
    icm_spi_Disable();
    // icm20948Config.data = raw_data;
    raw_data.ax = LE_CONVERTER(icm_rx_buffer[1], icm_rx_buffer[2]) *   icm20948Config.inv_accel_sens;
    raw_data.ay = LE_CONVERTER(icm_rx_buffer[3], icm_rx_buffer[4]) *   icm20948Config.inv_accel_sens;
    raw_data.az = LE_CONVERTER(icm_rx_buffer[5], icm_rx_buffer[6]) *   icm20948Config.inv_accel_sens;
    raw_data.gx = LE_CONVERTER(icm_rx_buffer[7], icm_rx_buffer[8]) *   icm20948Config.inv_gyro_sens;
    raw_data.gy = LE_CONVERTER(icm_rx_buffer[9], icm_rx_buffer[10]) *  icm20948Config.inv_gyro_sens;
    raw_data.gz = LE_CONVERTER(icm_rx_buffer[11], icm_rx_buffer[12]) * icm20948Config.inv_gyro_sens;
    raw_data.mx = MAG_CONVERTER(icm_rx_buffer[17], icm_rx_buffer[16]);
    raw_data.my = MAG_CONVERTER(icm_rx_buffer[19], icm_rx_buffer[18]);
    raw_data.mz = MAG_CONVERTER(icm_rx_buffer[21], icm_rx_buffer[20]);
    raw_data.time = icm20948Config.timer;
    icm20948Config.mesurement_flag = 0;
    if ((icm20948Config.prescalerCount == icm20948Config.prescaler))
    {
        icm20948Config.is_data_read = 1;
        icm20948Config.prescalerCount = 0;
    }
}
void DMA2_Stream2_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF2)
    { // Transfer Complete
        DMA2->LIFCR |= DMA_LIFCR_CTCIF2;

        icm_spi_Disable();
        dmaRX_iqr_SET(icm20948Config.stateIQR);

        if (icm20948Config.dmaTX_callback)
            icm20948Config.dmaTX_callback();
    }
}
void DMA2_Stream3_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF3)
    { // Resiwer Complete

        // addElement(icm_rx_buffer, SPI_DmaBuffSize);
        DMA2->LIFCR |= DMA_LIFCR_CTCIF3;
        dmaTX_iqr_SET(icm20948Config.stateIQR);
    }
}
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR = ~TIM_SR_UIF;
        icm_spi_Enable();
        dmaRX_iqr_CLEAR(icm20948Config.stateIQR);
        dmaTX_iqr_CLEAR(icm20948Config.stateIQR);
        Dma_Start(TX_DMA_SPI, icm_tx_buffer, SPI_DmaBuffSize);
        Dma_Start(RX_DMA_SPI, icm_rx_buffer, SPI_DmaBuffSize);
        icm20948Config.mesurement_flag = 1;
        icm20948Config.prescalerCount += 1;
        icm20948Config.timer +=1;
    }
}

// #define is_gyro_accel_config_change (uint8_t)(0x03 & icm20948Config.changeStatus)
// #define is_accel_config_change (uint8_t)(0x01 & icm20948Config.changeStatus)
// #define is_gyro_config_change (uint8_t)(0x02 & icm20948Config.changeStatus)
// #define is_presc_config_change (uint8_t)(0x04 & icm20948Config.changeStatus)

// #define accel_config_change_clear CLEAR_BIT(icm20948Config.changeStatus, 0x01)
// #define gyro_config_change_clear CLEAR_BIT(icm20948Config.changeStatus, 0x02)
// #define presc_config_change_clear CLEAR_BIT(icm20948Config.changeStatus, 0x04)

// #define accel_config_change_set SET_BIT(icm20948Config.changeStatus, 0x01)
// #define gyro_config_change_set SET_BIT(icm20948Config.changeStatus, 0x02)
// #define presc_config_change_set SET_BIT(icm20948Config.changeStatus, 0x04)

void change_in_flow(uint32_t prescaler)
{

    icm20948_stop_mesurement();
    while (!is_icm_dma_transmit())
    {
        __NOP();
    }

    if (is_presc_config_change)
    {
        icm20948Config.prescaler = prescaler;
        icm20948Config.prescalerCount = 0;
    }
    if (is_gyro_accel_config_change)
    {

        icm20948Config.dmaTX_callback = Dma_Mpu_Config_Callback;
        Spi1_Disable();
        MODIFY_REG(SPI1->CR1, SPI_CR1_BR_Msk, SPI_BAUDRATEPRESCALER_DIV128);
        Spi1_Enable();

        SET_BIT(SPI1->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);

        LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        delay_ms(100);

        LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        LL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        Select_Bank(2);
        // delay_ms(1);

        if (is_accel_config_change)
        {

            uint8_t temp = Spi_Dma_Read(B2_ACCEL_CONFIG);
            MODIFY_REG(temp, (0x03 << 1), icm20948Config.accel_conf);
            Spi_Dma_Write(B2_ACCEL_CONFIG, temp);
            icm20948Config.inv_accel_sens = ((float)(1 << icm20948Config.accel_conf)) * 2.0f / 32767.0f;
        }
        if (is_gyro_config_change)
        {

            uint8_t temp = Spi_Dma_Read(B2_GYRO_CONFIG_1);
            MODIFY_REG(temp, (0x03 << 1), icm20948Config.gyro_conf);
            Spi_Dma_Write(B2_GYRO_CONFIG_1, temp);
            icm20948Config.inv_gyro_sens = ((float)(1 << icm20948Config.gyro_conf)) * 250.0f / 32767.0f;
        }

        Select_Bank(0);
        Spi1_Disable();
        MODIFY_REG(SPI1->CR1, SPI_CR1_BR_Msk, SPI_BAUDRATEPRESCALER_DIV16);
        Spi1_Enable();
        SET_BIT(SPI1->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
    }

    icm20948Config.dmaTX_callback = Dma_Transmit_Callback;
    icm20948_start_mesurement();
}

void icm20948_init()
{

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);

    uint32_t Gpio_Moder_Msk = GPIO_MODER_MODER3_Msk | GPIO_MODER_MODER5_Msk | GPIO_MODER_MODER6_Msk;
    uint32_t Gpio_Moder = GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER3_0;
    MODIFY_REG(GPIOA->MODER, Gpio_Moder_Msk, Gpio_Moder);
    MODIFY_REG(GPIOD->MODER, GPIO_MODER_MODER7_Msk, GPIO_MODER_MODER7_1);

    uint32_t Gpio_Arf_0 = (AF05 << GPIO_AFRL_AFRL5_Pos) | (AF05 << GPIO_AFRL_AFRL6_Pos);
    uint32_t Gpio_Arf_Msk = GPIO_AFRL_AFRL5_Msk | GPIO_AFRL_AFRL6_Msk;
    MODIFY_REG(GPIOA->AFR[0], Gpio_Arf_Msk, Gpio_Arf_0);
    MODIFY_REG(GPIOD->AFR[0], GPIO_AFRL_AFRL7_Msk, (AF05 << GPIO_AFRL_AFRL7_Pos));

    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
    uint32_t Spi_Cr1 = 0;
    uint32_t Spi_Cr2 = 0;
    Spi_Cr1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_BAUDRATEPRESCALER_DIV128;
    //				|SPI_CR1_CPOL | SPI_CR1_CPHA;
    Spi_Cr2 = SPI_DATAWIDTH_8BIT | SPI_CR2_FRXTH;
    WRITE_REG(SPI1->CR1, Spi_Cr1);
    WRITE_REG(SPI1->CR2, Spi_Cr2);

    /*---------------------------------------------------*/
    /*---------------------------------------------------*/
    /*---------------------------------------------------*/
    /*---------------------------------------------------*/
    // TX_DMA_INIT Stream DMA2_Stream3

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);
    Dma_Disable(DMA2_Stream3);
    Dma_Disable(DMA2_Stream2);

    uint32_t Dma2_Lifcr = DMA_LIFCR_CTCIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTEIF3 |
                          DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3 |
                          DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 |
                          DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2;
    SET_BIT(DMA2->LIFCR, Dma2_Lifcr);

    uint32_t Dma2_Stream3_Cr = (DMA_CHSEL(3)) | DMA_SxCR_PL_0 |
                               DMA_SxCR_MINC | DMA_SxCR_TCIE |
                               DMA_SxCR_DIR_0;
    CLEAR_REG(DMA2_Stream3->CR);
    SET_BIT(DMA2_Stream3->CR, Dma2_Stream3_Cr);

    uint32_t Dma2_Stream2_Cr = (DMA_CHSEL(3)) | DMA_SxCR_PL_0 |
                               DMA_SxCR_MINC | DMA_SxCR_TCIE;
    CLEAR_REG(DMA2_Stream2->CR);
    SET_BIT(DMA2_Stream2->CR, Dma2_Stream2_Cr);
    // RX_DMA_INIT Stream DMA2_Stream2

    DMA2_Stream3->PAR = (uint32_t)&(SPI1->DR);
    DMA2_Stream2->PAR = (uint32_t)&(SPI1->DR); // rx

    NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    NVIC_SetPriority(DMA2_Stream2_IRQn, 5);
    NVIC_SetPriority(DMA2_Stream3_IRQn, 5);
    for (uint8_t i = 0; i < SPI_DmaBuffSize; i++)
    {
        icm_tx_buffer[i] = 0;
        icm_rx_buffer[i] = 0;
    }
    icm20948Config.data.byte[0] = '#';
    icm20948Config.data.byte[1] = '#';
    icm20948Config.data.byte[2] = '#';
    icm20948Config.data.byte[3] = '#';

    icm_spi_Disable();
    // TIM2 ------------------------------------------- //
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
    CLEAR_REG(TIM2->CR1);
    CLEAR_REG(TIM2->CR2);
    TIM2->PSC = icm20948Config.timPre;
    TIM2->ARR = icm20948Config.timArr;
    CLEAR_BIT(TIM2->SR, TIM_SR_UIF); // 1e3 тиков: 1e-6 * 1e3 = 1e-3 c
    SET_BIT(TIM2->DIER, TIM_DIER_UIE);
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 0);

    icm20948Config.dmaTX_callback = Dma_Mpu_Config_Callback;
    // TIM7 ------------------------------------------- //
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    CLEAR_REG(TIM7->CR1);
    CLEAR_REG(TIM7->CR2);
    TIM7->PSC = 10799;          // Делитель 96: 96 MHz / 96 => 1 тик 1e-4 s. =>
    TIM7->ARR = 9;              // 10 тиков: 1e-4 * 1e1 = 1e-3 c
    TIM7->SR = ~TIM_SR_UIF;     // Сброс флага обновления
    TIM7->DIER |= TIM_DIER_UIE; // Разрешить прерывание по переполнению
    // TIM7->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM7_IRQn);
    NVIC_SetPriority(TIM7_IRQn, 0);
}
volatile uint16_t timerCount = 0;
void delay_ms(uint16_t ms)
{

    // LL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    timerCount = ms;
    // TIM7->CNT = 0;
    NVIC_EnableIRQ(TIM7_IRQn);
    TIM7->CR1 |= TIM_CR1_CEN;
    while (timerCount)
    {
        __NOP();
    }
    NVIC_DisableIRQ(TIM7_IRQn);
    // CLEAR_BIT(TIM7->CR1, TIM_CR1_CEN);
    timerCount = 0;
}
void TIM7_IRQHandler(void)
{
    if (TIM7->SR & TIM_SR_UIF)
    {
        TIM7->SR = ~TIM_SR_UIF;
        timerCount--;
    }
}

void icm20948_start_mesurement()
{
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);
}
void icm20948_stop_mesurement()
{
    CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);
}

void icm20948_start()
{

    LL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    Spi1_Enable();
    SET_BIT(SPI1->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
    Select_Bank(0);
    delay_ms(5);
    uint8_t temp = 0x00;
    // Шаг 1.2: Сброс устройства
    temp = Spi_Dma_Read(B0_PWR_MGMT_1);
    temp |= 0x80;
    Spi_Dma_Write(B0_PWR_MGMT_1, 0x80);
    delay_ms(500);

    // Шаг 2: Пробуждение устройства
    temp = 0x01;
    Spi_Dma_Write(B0_PWR_MGMT_1, temp);
    delay_ms(300);

    LL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    // Шаг 3: Выбор часов (бит 1)
    temp = 0x01;
    Spi_Dma_Write(B0_PWR_MGMT_1, temp);
    delay_ms(100);

    // Шаг 4: SPI интерфейс
    temp = 0x10;
    Spi_Dma_Write(B0_USER_CTRL, temp);
    delay_ms(10);

    LL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    Select_Bank(2);

    uint8_t accel_config = 0x01 | (icm20948Config.accel_conf << 1);
    Spi_Dma_Write(B2_ACCEL_CONFIG, accel_config);
    delay_ms(40);
    uint8_t gyro_config = 0x01 | (icm20948Config.gyro_conf << 1);
    Spi_Dma_Write(B2_GYRO_CONFIG_1, gyro_config);
    delay_ms(40);

    icm20948Config.inv_accel_sens = ((float)(1 << icm20948Config.accel_conf)) * 2.0f / 32767.0f;
    icm20948Config.inv_gyro_sens = ((float)(1 << icm20948Config.gyro_conf)) * 250.0f / 32767.0f;
    // ========== НАСТРОЙКА МАГНИТОМЕТРА ==========
    Spi_Dma_Write(B2_ODR_ALIGN_EN, 1); // выравнивание ODR
    delay_ms(5);

    Select_Bank(0);
    delay_ms(5);

    temp = Spi_Dma_Read(B0_USER_CTRL); // сброс I2C Master
    temp |= (1 << 1);
    Spi_Dma_Write(B0_USER_CTRL, temp);
    delay_ms(100); // было HAL_Delay(100)

    LL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    temp = Spi_Dma_Read(B0_USER_CTRL); // включить I2C Master
    temp |= (1 << 5);
    Spi_Dma_Write(B0_USER_CTRL, temp);
    delay_ms(10); // было HAL_Delay(10)

    Select_Bank(3);
    delay_ms(5);
    Spi_Dma_Write(B3_I2C_MST_CTRL, 0x07); // частота I2C
    delay_ms(10);

    Select_Bank(0);
    delay_ms(5);
    Spi_Dma_Write(B0_LP_CONFIG, (1 << 6)); // duty cycle mode
    delay_ms(100);

    LL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    Select_Bank(3);
    delay_ms(5);
    Spi_Dma_Write(B3_I2C_MST_ODR_CONFIG, 0x03); // ODR 137 Гц
    delay_ms(10);

    // Сброс магнита (CNTL3 = 0x01)
    Spi_Dma_Write(B3_I2C_SLV0_ADDR, MAG_SLAVE_ADDR);
    Spi_Dma_Write(B3_I2C_SLV0_REG, MAG_CNTL3);
    Spi_Dma_Write(B3_I2C_SLV0_DO, 0x01);
    Spi_Dma_Write(B3_I2C_SLV0_CTRL, 0x80 | 0x01);
    delay_ms(50);

    // Непрерывный режим 4 (CNTL2 = 0x08)
    Spi_Dma_Write(B3_I2C_SLV0_ADDR, MAG_SLAVE_ADDR);
    Spi_Dma_Write(B3_I2C_SLV0_REG, MAG_CNTL2);
    Spi_Dma_Write(B3_I2C_SLV0_DO, (1 << 3));
    Spi_Dma_Write(B3_I2C_SLV0_CTRL, 0x80 | 0x01);
    delay_ms(50);

    LL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    // Автоматическое чтение 9 байт начиная с регистра 0x10
    Spi_Dma_Write(B3_I2C_SLV0_ADDR, MAG_SLAVE_ADDR | 0x80);
    Spi_Dma_Write(B3_I2C_SLV0_REG, 0x10);
    Spi_Dma_Write(B3_I2C_SLV0_CTRL, 0x80 | 9);
    delay_ms(100);

    Select_Bank(0);
    Spi_Dma_Write(B0_USER_CTRL, 0x20); // установить I2C_MST_EN = 1
    delay_ms(100);                     // дать время на первый цикл
                   // Возврат в Bank 0 и чтение WHO_AM_I
    // LL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    Select_Bank(0);
    delay_ms(5);
    uint8_t status = Spi_Dma_Read(B0_WHO_AM_I);
    delay_ms(100);

    //	icm20948_update_gyro_config();
    //	icm20948_update_accel_config();

    Spi1_Disable();
    SET_BIT(SPI1->CR1, SPI_BAUDRATEPRESCALER_DIV16);
    Spi1_Enable();
    SET_BIT(SPI1->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
    icm_tx_buffer[0] = 0x80 | B0_ACCEL_XOUT_H;
}

void Spi_Dma_Write(uint8_t reg, uint8_t data)
{
    dmaTX_iqr_CLEAR(icm20948Config.stateIQR);
    dmaRX_iqr_CLEAR(icm20948Config.stateIQR);
    debug_string[0] = reg;
    debug_string[1] = data;
    icm_spi_Enable();
    Dma_Start(TX_DMA_SPI, debug_string, 2);
    Dma_Start(RX_DMA_SPI, debug_string, 2);
    while (!is_icm_dma_transmit())
    {
        __NOP();
    }
    icm_spi_Disable();
}
uint8_t Spi_Dma_Read(uint8_t reg)
{

    dmaTX_iqr_CLEAR(icm20948Config.stateIQR);
    dmaRX_iqr_CLEAR(icm20948Config.stateIQR);

    debug_string[0] = reg | 0x80;
    debug_string[1] = 0;

    icm_spi_Enable();
    Dma_Start(TX_DMA_SPI, debug_string, 2);
    Dma_Start(RX_DMA_SPI, debug_string, 2);
    while (!is_icm_dma_transmit())
    {
        __NOP();
    }
    icm_spi_Disable();
    return debug_string[1];
}
static uint8_t is_icm_dma_transmit()
{
    if (dmaRX_iqr_GET(icm20948Config.stateIQR) && dmaTX_iqr_GET(icm20948Config.stateIQR))
    {

        dmaRX_iqr_CLEAR(icm20948Config.stateIQR);
        dmaTX_iqr_CLEAR(icm20948Config.stateIQR);
        return 1;
    }
    else
    {
        return 0;
    }
}
