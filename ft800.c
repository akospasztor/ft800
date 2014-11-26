/**
  ******************************************************************************
  * C Library for FT800 EVE module
  ******************************************************************************
  * @author  Akos Pasztor    (http://akospasztor.com)
  * @file    ft800.c
  * @brief   FT800 Display Controller Library
  *          This file contains the initialization and functions for
  *          the FT800 EVE module.
  * @info    http://www.ftdichip.com/Products/ICs/FT800.html
  ******************************************************************************
  * Copyright (c) 2014 Akos Pasztor. All rights reserved.
  ******************************************************************************
**/


#include "stm32f4xx.h"
#include "spi.h"
#include "ft800.h"

#include <stdlib.h>
#include <string.h>

/*
    Function: HOST_MEM_READ_STR
    ARGS:     addr: 24 Bit Command Address 
              pnt:  output buffer for read data
              len:  length of bytes to be read

    Description: Reads len(n) bytes of data, starting at addr into pnt(buffer)
*/
void HOST_MEM_READ_STR(uint32_t addr, uint8_t *pnt, uint8_t len)
{
  FT_spi_select();
  SPI_send(((addr>>16)&0x3F) );			// Send out bits 23:16 of addr, bits 7:6 of this byte must be 00 
  SPI_send(((addr>>8)&0xFF));       	// Send out bits 15:8 of addr
  SPI_send((addr&0xFF));            	// Send out bits 7:0 of addr

  SPI_send(0);                      	// Send out DUMMY (0) byte

  while(len--)                      	// While Len > 0 Read out n bytes
    *pnt++ = SPI_send(0);
  
  FT_spi_deselect();
}

/*
    Function: HOST_MEM_WR_STR
    ARGS:     addr: 24 Bit Command Address 
              pnt:  input buffer of data to send
              len:  length of bytes to be send

    Description: Writes len(n) bytes of data from pnt (buffer) to addr
*/
void HOST_MEM_WR_STR(uint32_t addr, uint8_t *pnt, uint8_t len)
{
  FT_spi_select();
  SPI_send(((addr>>16)&0x3F)|0x80);     // Send out 23:16 of addr, bits 7:6 of this byte must be 10
  SPI_send(((addr>>8)&0xFF));           // Send out bits 15:8 of addr
  SPI_send((addr&0xFF));                // Send out bits 7:0 of addr

  while(len--)                          // While Len > 0 Write *pnt (then increment pnt)
    SPI_send(*pnt++);
  
  FT_spi_deselect();
}

/*
    Function: HOST_CMD_WRITE
    ARGS:     CMD:  5 bit Command
             
    Description: Writes Command to FT800
*/
void HOST_CMD_WRITE(uint8_t CMD)
{
  FT_spi_select();
  SPI_send((uint8_t)(CMD|0x40));        // Send out Command, bits 7:6 must be 01
  SPI_send(0x00);
  SPI_send(0x00);
  FT_spi_deselect();
}

void HOST_CMD_ACTIVE(void)
{
  FT_spi_select();
  SPI_send(0x00);      
  SPI_send(0x00);
  SPI_send(0x00);
  FT_spi_deselect();
}

/*
    Function: HOST_MEM_WR8
    ARGS:     addr: 24 Bit Command Address 
              data: 8bit Data Byte

    Description: Writes 1 byte of data to addr
*/
void HOST_MEM_WR8(uint32_t addr, uint8_t data)
{
  FT_spi_select();
  SPI_send((addr>>16)|0x80);
  SPI_send(((addr>>8)&0xFF));
  SPI_send((addr&0xFF));

  SPI_send(data);
  
  FT_spi_deselect();  
}

/*
    Function: HOST_MEM_WR16
    ARGS:     addr: 24 Bit Command Address 
              data: 16bit (2 bytes)

    Description: Writes 2 bytes of data to addr
*/
void HOST_MEM_WR16(uint32_t addr, uint32_t data)
{
  FT_spi_select();
  SPI_send((addr>>16)|0x80);
  SPI_send(((addr>>8)&0xFF));
  SPI_send((addr&0xFF));

  /* Little-Endian: Least Significant Byte to: smallest address */
  SPI_send( (uint8_t)((data&0xFF)) );    //byte 0
  SPI_send( (uint8_t)((data>>8)) );      //byte 1
  
  FT_spi_deselect();  
}

/*
    Function: HOST_MEM_WR32
    ARGS:     addr: 24 Bit Command Address 
              data: 32bit (4 bytes)

    Description: Writes 4 bytes of data to addr
*/
void HOST_MEM_WR32(uint32_t addr, uint32_t data)
{
  FT_spi_select();
  SPI_send((addr>>16)|0x80);
  SPI_send(((addr>>8)&0xFF));
  SPI_send((addr&0xFF));

  SPI_send( (uint8_t)(data&0xFF) );
  SPI_send( (uint8_t)((data>>8)&0xFF) );
  SPI_send( (uint8_t)((data>>16)&0xFF) );
  SPI_send( (uint8_t)((data>>24)&0xFF) );
  
  FT_spi_deselect();  
}

/*
    Function: HOST_MEM_RD8
    ARGS:     addr: 24 Bit Command Address 

    Description: Returns 1 byte of data from addr
*/
uint8_t HOST_MEM_RD8(uint32_t addr)
{
  uint8_t data_in;

  FT_spi_select();
  SPI_send((uint8_t)((addr>>16)&0x3F));
  SPI_send((uint8_t)((addr>>8)&0xFF));
  SPI_send((uint8_t)(addr));
  SPI_send(0);

  data_in = SPI_send(0);
  
  FT_spi_deselect();
  return data_in;
}

/*
    Function: HOST_MEM_RD16
    ARGS:     addr: 24 Bit Command Address 

    Description: Returns 2 byte of data from addr in a 32bit variable
*/
uint32_t HOST_MEM_RD16(uint32_t addr)
{
  uint8_t data_in = 0;
  uint32_t data = 0;
  uint8_t i;

  FT_spi_select();
  SPI_send(((addr>>16)&0x3F));
  SPI_send(((addr>>8)&0xFF));
  SPI_send((addr&0xFF));
  SPI_send(0);

  for(i=0;i<2;i++)
  {
    data_in = SPI_send(0);
    data |= ( ((uint32_t)data_in) << (8*i) );
  }
  
  FT_spi_deselect();
  return data;
}

/*
    Function: HOST_MEM_RD32
    ARGS:     addr: 24 Bit Command Address 

    Description: Returns 4 byte of data from addr in a 32bit variable
*/
uint32_t HOST_MEM_RD32(uint32_t addr)
{
  uint8_t data_in = 0;
  uint32_t data = 0;
  uint8_t i;

  FT_spi_select();
  SPI_send(((addr>>16)&0x3F));
  SPI_send(((addr>>8)&0xFF));
  SPI_send((addr&0xFF));
  SPI_send(0);

  for(i=0;i<4;i++)
  {
    data_in = SPI_send(0);
    data |= ( ((uint32_t)data_in) << (8*i) );
  }
  
  FT_spi_deselect();
  return data;
}

/*** CMD Functions *****************************************************************/
uint8_t cmd_execute(uint32_t data)
{
	uint32_t cmdBufferRd = 0;
    uint32_t cmdBufferWr = 0;
    
    cmdBufferRd = HOST_MEM_RD32(REG_CMD_READ);
    cmdBufferWr = HOST_MEM_RD32(REG_CMD_WRITE);
    
    uint32_t cmdBufferDiff = cmdBufferWr-cmdBufferRd;
    
    if( (4096-cmdBufferDiff) > 4)
    {
        HOST_MEM_WR32(RAM_CMD + cmdBufferWr, data);
        HOST_MEM_WR32(REG_CMD_WRITE, cmdBufferWr + 4);
        return 1;
    }
    return 0;
}

uint8_t cmd(uint32_t data)
{
	uint8_t tryCount = 255;
	for(tryCount = 255; tryCount > 0; --tryCount)
	{
		if(cmd_execute(data)) { return 1; }
	}
	return 0;
}

uint8_t cmd_ready(void)
{
    uint32_t cmdBufferRd = HOST_MEM_RD32(REG_CMD_READ);
    uint32_t cmdBufferWr = HOST_MEM_RD32(REG_CMD_WRITE);
    
    return (cmdBufferRd == cmdBufferWr) ? 1 : 0;
}

/*** Track *************************************************************************/
void cmd_track(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag)
{
    cmd(CMD_TRACK);
    cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)h<<16)|(w & 0xffff) );
    cmd( (uint32_t)tag );
}

/*** Draw Spinner ******************************************************************/
void cmd_spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale)
{    
    cmd(CMD_SPINNER);
    cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)scale<<16)|style );
    
}

/*** Draw Slider *******************************************************************/
void cmd_slider(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range)
{
	cmd(CMD_SLIDER);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
	cmd( ((uint32_t)h<<16)|(w & 0xffff) );
	cmd( ((uint32_t)val<<16)|(options & 0xffff) );
	cmd( (uint32_t)range );
}

/*** Draw Text *********************************************************************/
void cmd_text(int16_t x, int16_t y, int16_t font, uint16_t options, const char* str)
{
	/* 	
		i: data pointer
		q: str  pointer
		j: loop counter
	*/
	
	uint16_t i,j,q;
	const uint16_t length = strlen(str);
	if(!length) return ;	
	
	uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
	
	q = 0;
	for(i=0; i<(length/4); ++i, q=q+4)
	{
		data[i] = (uint32_t)str[q+3]<<24 | (uint32_t)str[q+2]<<16 | (uint32_t)str[q+1]<<8 | (uint32_t)str[q];
	}
	for(j=0; j<(length%4); ++j, ++q)
	{
		data[i] |= (uint32_t)str[q] << (j*8);
	}
	
	cmd(CMD_TEXT);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
    cmd( ((uint32_t)options<<16)|(font & 0xffff) );
	for(j=0; j<(length/4)+1; ++j)
	{
		cmd(data[j]);
	}
	free(data);
}

/*** Draw Button *******************************************************************/
void cmd_button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str)
{	
	/* 	
		i: data pointer
		q: str  pointer
		j: loop counter
	*/
	
	uint16_t i,j,q;
	const uint16_t length = strlen(str);
	if(!length) return ;
	
	uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
	
	q = 0;
	for(i=0; i<(length/4); ++i, q=q+4)
	{
		data[i] = (uint32_t)str[q+3]<<24 | (uint32_t)str[q+2]<<16 | (uint32_t)str[q+1]<<8 | (uint32_t)str[q];
	}
	for(j=0; j<(length%4); ++j, ++q)
	{
		data[i] |= (uint32_t)str[q] << (j*8);
	}
	
	cmd(CMD_BUTTON);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
	cmd( ((uint32_t)h<<16)|(w & 0xffff) );
    cmd( ((uint32_t)options<<16)|(font & 0xffff) );
	for(j=0; j<(length/4)+1; ++j)
	{
		cmd(data[j]);
	}
	free(data);
}

/*** Draw Keyboard *****************************************************************/
void cmd_keys(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str)
{
	/* 	
		i: data pointer
		q: str  pointer
		j: loop counter
	*/
	
	uint16_t i,j,q;
	const uint16_t length = strlen(str);
	if(!length) return ;
	
	uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));
	
	q = 0;
	for(i=0; i<(length/4); ++i, q=q+4)
	{
		data[i] = (uint32_t)str[q+3]<<24 | (uint32_t)str[q+2]<<16 | (uint32_t)str[q+1]<<8 | (uint32_t)str[q];
	}
	for(j=0; j<(length%4); ++j, ++q)
	{
		data[i] |= (uint32_t)str[q] << (j*8);
	}
	
	cmd(CMD_KEYS);
	cmd( ((uint32_t)y<<16)|(x & 0xffff) );
	cmd( ((uint32_t)h<<16)|(w & 0xffff) );
    cmd( ((uint32_t)options<<16)|(font & 0xffff) );
	for(j=0; j<(length/4)+1; ++j)
	{
		cmd(data[j]);
	}
	free(data);
}

/*** Write zero to a block of memory ***********************************************/
void cmd_memzero(uint32_t ptr, uint32_t num)
{
	cmd(CMD_MEMZERO);
	cmd(ptr);
	cmd(num);
}

/*** Set FG color ******************************************************************/
void cmd_fgcolor(uint32_t c)
{
	cmd(CMD_FGCOLOR);
	cmd(c);
}

/*** Set BG color ******************************************************************/
void cmd_bgcolor(uint32_t c)
{
	cmd(CMD_BGCOLOR);
	cmd(c);
}

/*** Set Gradient color ************************************************************/
void cmd_gradcolor(uint32_t c)
{
	cmd(CMD_GRADCOLOR);
	cmd(c);
}

/*** Draw Gradient *****************************************************************/
void cmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1)
{
	cmd(CMD_GRADIENT);
	cmd( ((uint32_t)y0<<16)|(x0 & 0xffff) );
	cmd(rgb0);
	cmd( ((uint32_t)y1<<16)|(x1 & 0xffff) );
	cmd(rgb1);
}

/*** Matrix Functions **************************************************************/
void cmd_loadidentity(void)
{
	cmd(CMD_LOADIDENTITY);
}

void cmd_setmatrix(void)
{
	cmd(CMD_SETMATRIX);
}

void cmd_rotate(int32_t angle)
{
	cmd(CMD_ROTATE);
	cmd(angle);
}

void cmd_translate(int32_t tx, int32_t ty)
{
	cmd(CMD_TRANSLATE);
	cmd(tx);
	cmd(ty);
}
