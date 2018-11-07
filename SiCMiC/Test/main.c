/**
******************************************************************************
* File Name          : main.c

Main program for SiC in Space experiment. 


Note that in the current configuration, the software communicates via
USART, and not via I2C (As the final software will have to do). 
Some I2C functionality is written, but at the moment this will most 
likely not work without further configuration. 

To use the software as is, download the processing software from github 
(https://github.com/Happsson/SiC-Analyzing), 
connect the USART to the computer using for example this cable:
(http://www.lawicel-shop.se/prod/TTL-232R-USB-5Vcable-33V-IO_794885/Sparkfun_64668/SWE/SEK) 
and run the Processing code. 
On the current most recent board, the TX and RX pinouts are located next to the SWD port like so:

[TX]
[RX]
[NC]
[GND]

where TX is closest to the SWD port, and GND is closest to the reset button. 

The I2C connections are located on the pin header with three pins like this:
[SCL]
[SDA]
[GND]
where GND is closest to the 3.3 power supply.

The pin header for the SWD is configured in the following way:

[NC] 
[SWCLK]
[GND]
[SWDIO]
[NRST]
[NC]
Where NRST and NC are closest to the USART port. 

Refer to the Diptrace files for schematics and PCB design for 
further pin connections. 

******************************************************************************
*
* COPYRIGHT(c) 2016 STMicroelectronics
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "adc.h"
#include "dac.h"
#include "i2c.h"
#include "iwdg.h"
#include "gpio.h"
#include "usart.h"
#include "SystemClock_Config.h"
#include "SystemPower_Config.h"
#include "mantest.h"
#include "string.h"
#include <stdbool.h>


/* Private function prototypes -----------------------------------------------*/
//void SystemClock_Config(void);
#define EXPERIMENTSIZE sizeof(experiments)
/* Size of Transmission buffer */
#define TXBUFFERSIZE                      20
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);
uint8_t aRxBuffer[10];
uint8_t aTxBuffer[20];

unsigned char buffer [7];

uint8_t* create_i2c_package(uint8_t[]);
void send_message(uint8_t *);
void check_OBC_message(void);
void receive_OBC_message(void);

//void setDAC(uint32_t);
//void readRollingADC(int);
//void shiftAverages(void);
//void normalRun(void);
//void graphTestSweep(int);
//void testProgram(void);
//void SystemPower_Config(void);
uint8_t* create_graph_package(uint8_t[]);


uint32_t tickCounterStart;
uint32_t tickCounterStop;
uint32_t value;
uint32_t timeTaken;



 int main(void)
{ 
  
  tickCounterStart = 0;
  tickCounterStop  = 0;
  value=0;
  timeTaken=0;
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_IWDG_Init();
  MX_USART1_UART_Init();
  MX_ADC_Init();
  MX_DAC_Init();
 
  

  

    HAL_GPIO_WritePin(GPIOB,On__Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,SHDN_Linear_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,SHDN_Piezo_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,LD_G_Pin,GPIO_PIN_SET);
  
  while (1){ 
  
mantest();

		Flush_Buffer((uint8_t *)aRxBuffer,RXBUFFERSIZE);	  //Flush the buffer so the OBC can send message
		receive_OBC_message();															//Receive instructions from the OBC
		check_OBC_message();																//Check instructions from the OBC
			
	
	}
   /* HAL_UART_Receive_IT(&huart1, buffrx,5);
    HAL_UART_Transmit_IT(&huart1, (uint8_t *)bufftx, 8);
     HAL_Delay(2000);  
  
  }*/
  //Run experiment manually
 
}

void receive_OBC_message(){
	
	  /*##-2- Put I2C peripheral in reception process ###########################*/  
  while(HAL_I2C_Slave_Receive(&hi2c1, (uint8_t *)aRxBuffer, RXBUFFERSIZE,100) != HAL_OK)
  {
		
    /*If some instructions has been send then return to main*/
   if(aRxBuffer[0] != '\0')
		return;     
  }
	
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
  {
  }
		return;
}	

void check_OBC_message(void){
	
			if(strcmp(aRxBuffer, "mantest") == 0)
	{
          mantest();
     	
		return;
	}
	else return;
}
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    *pBuffer = 0;

    pBuffer++;
  }
}


void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
    
    printf("somthing");
  }
  
  /* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

/**
* @brief Reports the name of the source file and the source line number
* where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
  
}

#endif

/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
