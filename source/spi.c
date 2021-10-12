#include "spi.h"
#include "main.h"
#include "stm32l4xx_hal.h"

// Peripheral handles
extern SPI_HandleTypeDef hspi2;

// SPI2 configuration function
void SPI_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable clocks of GPIO ports
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // GPIO configuration:
    // PC2:   SPI2_MISO
    // PC3:   SPI2_MOSI
    // PB10:  SPI2_SCK
    // PC4:   SPI CS (chip-select of display)
    // PC5:   PD pin of display

    // Note: I just randomly assigned the CS and PD pins to Pin 4 and Pin 5 of
    // GPIO C. You need to change these if you have the CS and PD lines
    // connected to another pins on your board!

    // Configure GPIO pin: SPI MISO, MOSI
    GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure GPIO pin: SPI SCK
    GPIO_InitStruct.Pin       = GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure GPIO pin: CS pin
    // Pushpull configuration, pulled up by default after init
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
    GPIO_InitStruct.Pin   = GPIO_PIN_4;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure GPIO pin: PD pin
    // Pushpull configuration, pulled up by default after init
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    GPIO_InitStruct.Pin   = GPIO_PIN_5;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Enable peripheral clock of SPI2
    __HAL_RCC_SPI2_CLK_ENABLE();

    // Configure SPI2 peripheral
    // - Ca. 20 MHz speed (you can play with the prescaler, but if you use long
    //   cables, then I'd advise to reduce the speed to avoid errors)
    // - 8-bit data mode, full-duplex mode
    // - SPI mode: 0 (CPOL = 0, CPHA = 0)
    //   see: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
    hspi2.Instance               = SPI2;
    hspi2.Init.Mode              = SPI_MODE_MASTER;
    hspi2.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi2.Init.NSS               = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi2.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial     = 7;
    hspi2.Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
    hspi2.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
    if(HAL_SPI_Init(&hspi2) != HAL_OK)
    {
        Error_Handler();
    }
}

// Send and receive one character over SPI2 using HAL
uint8_t SPI_send(uint8_t data)
{
    // Variable for the character that is received after sending data
    uint8_t received = 0;

    // Transmit and receive 1 character using HAL with 100 ms timeout
    // Note: errors are not handled!
    HAL_SPI_TransmitReceive(&hspi2, &data, &received, 1, 100);

    // Return received character
    return received;
}

// Assert the CS (chip-select) line of the display
// Note: I just randomly assigned the CS pin to Pin 4 of GPIO C.
// You need to change this if you have the CS line connected to another pin.
void FT_spi_select(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
}

// De-assert the CS (chip-select) line of the display
// Note: I just randomly assigned the CS pin to Pin 4 of GPIO C.
// You need to change this if you have the CS line connected to another pin.
void FT_spi_deselect(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
}

// Assert the PD line of the display
// Note: I just randomly assigned the PD pin to Pin 5 of GPIO C.
// You need to change this if you have the PD line connected to another pin.
void FT_PD_assert(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
}

// De-assert the PD line of the display
// Note: I just randomly assigned the PD pin to Pin 5 of GPIO C.
// You need to change this if you have the PD line connected to another pin.
void FT_PD_deassert(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
}
