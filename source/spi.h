#ifndef __SPI_H
#define __SPI_H

#include "stm32l4xx.h"

// FT800 low-level functions
void SPI_init(void);             // SPI init for FT800
uint8_t SPI_send(uint8_t data);  // Send and receive a character over SPI

void FT_spi_select(void);    // Select FT800
void FT_spi_deselect(void);  // Deselect FT800

void FT_PD_assert(void);    // Assert the PD pin of FT800
void FT_PD_deassert(void);  // De-assert the PD pin of FT800

#endif
