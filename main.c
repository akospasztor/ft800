#include "stm32f4xx.h"
#include "spi.h"
#include "ft800.h"

/* Delaying function */
void sysDms(uint32_t millisec)
{
    /* ... */
}

/* Init function for an 5" LCD display */
uint8_t initFT800(void)
{   
	uint8_t dev_id = 0;                  // Variable for holding the read device id    
	GPIO_ResetBits(GPIOE, GPIO_Pin_8);   // Set the PDN pin low 

	sysDms(50);                          // Delay 50 ms for stability
	GPIO_SetBits(GPIOE, GPIO_Pin_8); 	 // Set the PDN pin high
	sysDms(50);                          // Delay 50 ms for stability

	//WAKE
	HOST_CMD_ACTIVE();
	sysDms(500);

	//Ext Clock
	HOST_CMD_WRITE(CMD_CLKEXT);         // Send CLK_EXT Command (0x44)

	//PLL (48M) Clock
	HOST_CMD_WRITE(CMD_CLK48M);         // Send CLK_48M Command (0x62)

	//Read Dev ID
	dev_id = HOST_MEM_RD8(REG_ID);      // Read device id
	if(dev_id != 0x7C)                  // Device ID should always be 0x7C
	{   
		return 1;
	}

	HOST_MEM_WR8(REG_GPIO, 0x00);			// Set REG_GPIO to 0 to turn off the LCD DISP signal
	HOST_MEM_WR8(REG_PCLK, 0x00);      		// Pixel Clock Output disable

	HOST_MEM_WR16(REG_HCYCLE, 548);         // Set H_Cycle to 548
	HOST_MEM_WR16(REG_HOFFSET, 43);         // Set H_Offset to 43
	HOST_MEM_WR16(REG_HSYNC0, 0);           // Set H_SYNC_0 to 0
	HOST_MEM_WR16(REG_HSYNC1, 41);          // Set H_SYNC_1 to 41
	HOST_MEM_WR16(REG_VCYCLE, 292);         // Set V_Cycle to 292
	HOST_MEM_WR16(REG_VOFFSET, 12);         // Set V_OFFSET to 12
	HOST_MEM_WR16(REG_VSYNC0, 0);           // Set V_SYNC_0 to 0
	HOST_MEM_WR16(REG_VSYNC1, 10);          // Set V_SYNC_1 to 10
	HOST_MEM_WR8(REG_SWIZZLE, 0);           // Set SWIZZLE to 0
	HOST_MEM_WR8(REG_PCLK_POL, 1);          // Set PCLK_POL to 1
	HOST_MEM_WR8(REG_CSPREAD, 1);           // Set CSPREAD to 1
	HOST_MEM_WR16(REG_HSIZE, 480);          // Set H_SIZE to 480
	HOST_MEM_WR16(REG_VSIZE, 272);          // Set V_SIZE to 272

	/* configure touch & audio */
	HOST_MEM_WR8(REG_TOUCH_MODE, 0x03);     	//set touch on: continous
	HOST_MEM_WR8(REG_TOUCH_ADC_MODE, 0x01); 	//set touch mode: differential
	HOST_MEM_WR8(REG_TOUCH_OVERSAMPLE, 0x0F); 	//set touch oversampling to max
	HOST_MEM_WR16(REG_TOUCH_RZTHRESH, 5000);	//set touch resistance threshold
	HOST_MEM_WR8(REG_VOL_SOUND, 0xFF);      	//set the volume to maximum

	/* write first display list */
	HOST_MEM_WR32(RAM_DL+0, CLEAR_COLOR_RGB(0,0,0));  // Set Initial Color to BLACK
	HOST_MEM_WR32(RAM_DL+4, CLEAR(1,1,1));            // Clear to the Initial Color
	HOST_MEM_WR32(RAM_DL+8, DISPLAY());               // End Display List

	HOST_MEM_WR8(REG_DLSWAP, DLSWAP_FRAME);           // Make this display list active on the next frame

	HOST_MEM_WR8(REG_GPIO_DIR, 0x80);                 // Set Disp GPIO Direction 
	HOST_MEM_WR8(REG_GPIO, 0x80);                     // Enable Disp (if used)
	HOST_MEM_WR16(REG_PWM_HZ, 0x00FA);                // Backlight PWM frequency
	HOST_MEM_WR8(REG_PWM_DUTY, 0x80);                 // Backlight PWM duty    

	HOST_MEM_WR8(REG_PCLK, 0x05);                     // After this display is visible on the LCD

	return 0;
}

/* Clear Screen */
void clrscr(void)
{
	cmd(CMD_DLSTART);
	cmd(CLEAR_COLOR_RGB(0,0,0));
	cmd(CLEAR(1,1,1));
	cmd(DISPLAY());
	cmd(CMD_SWAP);
}

/* Demo Screen */
void lcd_start_screen(uint8_t button)
{
	cmd(CMD_DLSTART);
	cmd(CLEAR_COLOR_RGB(0,0,0));
	cmd(CLEAR(1,1,1));
	cmd_gradient(0,0,0xA1E1FF, 0,250,0x000080);
	cmd_text(10,245, 27,0, "Designed by: Akos Pasztor");
	cmd_text(470,250, 26,OPT_RIGHTX, "http://akospasztor.com");
	cmd(COLOR_RGB(0xDE,0x00,0x08));
	cmd_text(240,40, 31,OPT_CENTERX, "FT800 Demo");
	cmd(COLOR_RGB(255,255,255));
	cmd(TAG(1));

	if(!button)
	{
		cmd_fgcolor(0x228B22);
		cmd_button(130,150, 220,48, 28,0, "Tap to Continue");
	}
	else
	{
		cmd_fgcolor(0x0A520A);
		cmd_button(130,150, 220,48, 28,OPT_FLAT, "Tap to Continue");
	}

	cmd(DISPLAY());
	cmd(CMD_SWAP);	
}

/*** Main **************************************************************************/
int main(void)
{
	SPI_init();
	while(initFT800());
	sysDms(500);
	SPI_speedup();

	clrscr();

	lcd_start_screen(0);

	while(1)
	{

		uint32_t tag = HOST_MEM_RD32(REG_TOUCH_TAG);
		
		//button pressed
		if(tag == 1)
		{
			lcd_start_screen(1);
		}
		else
		{
			lcd_start_screen(0);
		}
		
	}
}

