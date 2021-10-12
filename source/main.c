// Includes
#include "main.h"
#include "ft800.h"
#include "spi.h"

// Peripheral handles
SPI_HandleTypeDef hspi2;

// Function prototypes
void SystemClock_Config(void);
uint8_t FT800_init(void);
void FT800_clear_screen(void);
void FT800_demo_screen(uint8_t button);

// Main function
int main(void)
{
    // Initialize HAL peripheral library
    HAL_Init();

    // Initialize system, core and peripheral clocks
    SystemClock_Config();

    // Initialize SPI2 for the display
    SPI_init();

    while(FT800_init())
    {
        // Keep on trying until FT800 is initialized
    }

    // Use the built-in HAL_Delay function to delay for a specific duration in
    // millisec. Note: this function blocks!
    HAL_Delay(500);

    // Clear display screen
    FT800_clear_screen();

    // Start demo screen
    FT800_demo_screen(0);

    // Main loop
    while(1)
    {
        uint32_t tag = HOST_MEM_RD32(REG_TOUCH_TAG);

        // Check if button has been pressed and draw the button with the
        // corresponding visual state
        if(tag == 1)
        {
            FT800_demo_screen(1);
        }
        else
        {
            FT800_demo_screen(0);
        }
    }
}

// Configure system, core and peripheral clocks
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Enable system clocks
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    // Initialize core, AHB and APB buses:
    //  - use HSI16 as clock source (16 MHz)
    //  - use PLL to generate core clock of 80 MHz
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 1;
    RCC_OscInitStruct.PLL.PLLN            = 10;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    // Configure flash latency (required as the core clock runs at 80Mhz)
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }

    // Configure the main internal regulator output voltage
    if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }
}

// Init function for an 5" LCD display
// This function is directly copied from the repository. The only change I made
// is that the function HAL_Delay is used for delays and the GPIO toggling
// functions are now using HAL.
// The function returns 0 if everything is OK and a non-zero number upon error.
uint8_t FT800_init(void)
{
    // Variable for holding the read device id
    uint8_t dev_id = 0;

    // Set the PDN pin low
    FT_PD_assert();

    // Delay 50 ms for stability
    HAL_Delay(50);

    // Set the PDN pin high
    FT_PD_deassert();

    // Delay 50 ms for stability
    HAL_Delay(50);

    // Wake up
    HOST_CMD_ACTIVE();
    HAL_Delay(500);

    // Configure external clock
    HOST_CMD_WRITE(CMD_CLKEXT);  // Send CLK_EXT Command (0x44)

    // Configure PLL (48M) clock
    HOST_CMD_WRITE(CMD_CLK48M);  // Send CLK_48M Command (0x62)

    // Read dev ID
    dev_id = HOST_MEM_RD8(REG_ID);  // Read device id
    if(dev_id != 0x7C)              // Device ID should always be 0x7C
    {
        return 1;
    }

    // Set REG_GPIO to 0 to turn off the LCD DISP signal
    HOST_MEM_WR8(REG_GPIO, 0x00);

    // Pixel Clock Output disable
    HOST_MEM_WR8(REG_PCLK, 0x00);

    HOST_MEM_WR16(REG_HCYCLE, 548);  // Set H_Cycle to 548
    HOST_MEM_WR16(REG_HOFFSET, 43);  // Set H_Offset to 43
    HOST_MEM_WR16(REG_HSYNC0, 0);    // Set H_SYNC_0 to 0
    HOST_MEM_WR16(REG_HSYNC1, 41);   // Set H_SYNC_1 to 41
    HOST_MEM_WR16(REG_VCYCLE, 292);  // Set V_Cycle to 292
    HOST_MEM_WR16(REG_VOFFSET, 12);  // Set V_OFFSET to 12
    HOST_MEM_WR16(REG_VSYNC0, 0);    // Set V_SYNC_0 to 0
    HOST_MEM_WR16(REG_VSYNC1, 10);   // Set V_SYNC_1 to 10
    HOST_MEM_WR8(REG_SWIZZLE, 0);    // Set SWIZZLE to 0
    HOST_MEM_WR8(REG_PCLK_POL, 1);   // Set PCLK_POL to 1
    HOST_MEM_WR8(REG_CSPREAD, 1);    // Set CSPREAD to 1
    HOST_MEM_WR16(REG_HSIZE, 480);   // Set H_SIZE to 480
    HOST_MEM_WR16(REG_VSIZE, 272);   // Set V_SIZE to 272

    // Configure touch & audio
    HOST_MEM_WR8(REG_TOUCH_MODE, 0x03);        // Set touch on: continous
    HOST_MEM_WR8(REG_TOUCH_ADC_MODE, 0x01);    // Set touch mode: differential
    HOST_MEM_WR8(REG_TOUCH_OVERSAMPLE, 0x0F);  // Set touch oversampling to max
    HOST_MEM_WR16(REG_TOUCH_RZTHRESH, 5000);   // Set touch resistance threshold
    HOST_MEM_WR8(REG_VOL_SOUND, 0xFF);         // Set the volume to maximum

    // Write first display list
    HOST_MEM_WR32(RAM_DL + 0,
                  CLEAR_COLOR_RGB(0, 0, 0));    // Set Initial Color to BLACK
    HOST_MEM_WR32(RAM_DL + 4, CLEAR(1, 1, 1));  // Clear to the Initial Color
    HOST_MEM_WR32(RAM_DL + 8, DISPLAY());       // End Display List

    // Make this display list active on the next frame
    HOST_MEM_WR8(REG_DLSWAP, DLSWAP_FRAME);

    HOST_MEM_WR8(REG_GPIO_DIR, 0x80);   // Set Disp GPIO Direction
    HOST_MEM_WR8(REG_GPIO, 0x80);       // Enable Disp (if used)
    HOST_MEM_WR16(REG_PWM_HZ, 0x00FA);  // Backlight PWM frequency
    HOST_MEM_WR8(REG_PWM_DUTY, 0x80);   // Backlight PWM duty

    HOST_MEM_WR8(REG_PCLK, 0x05);  // After this display is visible on the LCD

    return 0;
}

// Clear screen content
void FT800_clear_screen(void)
{
    cmd(CMD_DLSTART);
    cmd(CLEAR_COLOR_RGB(0, 0, 0));
    cmd(CLEAR(1, 1, 1));
    cmd(DISPLAY());
    cmd(CMD_SWAP);
}

// Display demo screen content
// This function displays a demo content with a button that has two visual
// states, depending on the function argument.
void FT800_demo_screen(uint8_t button)
{
    cmd(CMD_DLSTART);
    cmd(CLEAR_COLOR_RGB(0, 0, 0));
    cmd(CLEAR(1, 1, 1));
    cmd_gradient(0, 0, 0xA1E1FF, 0, 250, 0x000080);
    cmd_text(10, 245, 27, 0, "Designed by: Akos Pasztor");
    cmd_text(470, 250, 26, OPT_RIGHTX, "https://akospasztor.com");
    cmd(COLOR_RGB(0xDE, 0x00, 0x08));
    cmd_text(240, 40, 31, OPT_CENTERX, "FT800 Demo");
    cmd(COLOR_RGB(255, 255, 255));
    cmd(TAG(1));

    if(!button)
    {
        cmd_fgcolor(0x228B22);
        cmd_button(130, 150, 220, 48, 28, 0, "Tap to Continue");
    }
    else
    {
        cmd_fgcolor(0x0A520A);
        cmd_button(130, 150, 220, 48, 28, OPT_FLAT, "Tap to Continue");
    }

    cmd(DISPLAY());
    cmd(CMD_SWAP);
}

// Built-in error handler function of the HAL library
void Error_Handler(void)
{
    while(1)
    {
        // Do nothing
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    while(1)
    {
    }
}
#endif /* USE_FULL_ASSERT */
