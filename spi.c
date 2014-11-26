/**
  ******************************************************************************
  * C Library for FT800 EVE module 
  ******************************************************************************
  * @author  Akos Pasztor    (http://akospasztor.com)
  * @file    spi.c
  * @brief   SPI functions
  *          This file contains the GPIO initializations and functions
  *          for SPI peripherials.
  ******************************************************************************
  * Copyright (c) 2014 Akos Pasztor. All rights reserved.
  ******************************************************************************
**/

#include "stm32f4xx.h"
#include "main.h"
#include "spi.h"

/*** SPI INIT **********************************************************************/
void SPI_init(void)
{	
    /* FT800 pin configuration
	 * SCK  = PA5
	 * MISO = PA6
	 * MOSI = PB5
	 * CS   = PA4
	 * PDN  = PE8
     */
    
    SPI_InitTypeDef SPI_InitTypeDefStruct;
    GPIO_InitTypeDef GPIO_InitTypeDefStruct;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
     
    SPI_InitTypeDefStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitTypeDefStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitTypeDefStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitTypeDefStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitTypeDefStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitTypeDefStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitTypeDefStruct.SPI_FirstBit = SPI_FirstBit_MSB;
     
    SPI_Init(SPI1, &SPI_InitTypeDefStruct);
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE);
    
    //SCK, MISO
    GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);
	
	//MOSI
    GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitTypeDefStruct);
    
    //CS
    GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);
    
    //PDN
    GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOE, &GPIO_InitTypeDefStruct);
     
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
    
	//CS, PDN to pull up
    GPIO_SetBits(GPIOE, GPIO_Pin_8);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
    
    SPI_Cmd(SPI1, ENABLE);
}

void SPI_speedup(void)
{
    SPI_InitTypeDef SPI_InitTypeDefStruct;
     
    SPI_InitTypeDefStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitTypeDefStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitTypeDefStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitTypeDefStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitTypeDefStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitTypeDefStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    
    SPI_InitTypeDefStruct.SPI_FirstBit = SPI_FirstBit_MSB;
     
    SPI_Init(SPI1, &SPI_InitTypeDefStruct);
}

/*** SEND **************************************************************************/
char SPI_send(char data)
{    
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
    SPI_I2S_SendData(SPI1, data);
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    
    return SPI_I2S_ReceiveData(SPI1);
}

/*** REC ***************************************************************************/
char SPI_rec(char address)
{      
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
    SPI_I2S_SendData(SPI1, address);
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    SPI_I2S_ReceiveData(SPI1);
     
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); 
    SPI_I2S_SendData(SPI1, 0x00);
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
     
    return  SPI_I2S_ReceiveData(SPI1);
}

/*** FT800 SPI select **************************************************************/
void FT_spi_select(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

/*** FT800 SPI deselect ************************************************************/
void FT_spi_deselect(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
