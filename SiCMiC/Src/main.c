
/**
******************************************************************************
* @file           : main.c
* @brief          : Main program body
******************************************************************************
** This notice applies to any and all portions of this file
* that are not between comment pairs USER CODE BEGIN and
* USER CODE END. Other portions of this file, whether 
* inserted by the user or by software development tools
* are owned by their respective copyright owners.
*
* COPYRIGHT(c) 2018 STMicroelectronics
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
#include "header.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
char bufftx[20]="X?\r";
//char txD[8]="Hussei\n\r";
//uint8_t rxD[10];
char buffrx[40];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#define EXPERIMENTSIZE sizeof(experiments)
/* Size of Transmission buffer */
#define TXBUFFERSIZE                      20
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);
uint8_t aRxBuffer[10];
uint8_t aTxBuffer[20];
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
* @brief  The application entry point.
*
* @retval None
*/
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */
  
  /* MCU Configuration----------------------------------------------------------*/
  
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  
  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */
  
  /* Configure the system clock */
  SystemClock_Config();
  
  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */
  
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_DAC_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  
  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){
    Flush_Buffer((uint8_t *)aRxBuffer,RXBUFFERSIZE);	  //Flush the buffer so the OBC can send message
    receive_OBC_message();				  //Checks if something 											//Receive instructions from the OBC
    check_OBC_message();				  // Checks which task shall be done												//Check instructions from the OBC
    
    
  }
  
  
}



/* USER CODE END WHILE */

/* USER CODE BEGIN 3 */
/**
@brief receive OBC Message, waits for the transmitted message from OBC. Checks if
the aRxBuffer is empty. If it is empty it lopps in this function until a command 
is received.    
 
@param char string, Checks if the received aRxbuffer are empty or not. 
      
@returns, 
*/
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
/**
@brief Check OBC Message, Checks the received message from OBC to see which command 
should be made.   
 
@param char string, compares the Char string from aRxbuffer with the typed in 
to see if they are the same. 
      
@returns, 
*/
void check_OBC_message(void){
  
  if(strcmp(aRxBuffer, "start_test") == 0)
  {
    start_test(); //starts the test and activates the voltage planes
    
    return;
  }
  if(strcmp(aRxBuffer, "piezo48V") == 0)
  {
    HAL_GPIO_WritePin(GPIOB,Piezo_48V_ON_Pin,GPIO_PIN_RESET); //turns off 48V to piezo
    return;
  }
 
  if(strcmp(aRxBuffer, "piezoON") == 0)
  {

    HAL_GPIO_WritePin(GPIOB,Piezo_ON_Pin,GPIO_PIN_RESET); //turns off 5V to piezo

    return;
  }
  if(strcmp(aRxBuffer, "linear10V") == 0)
  {

    HAL_GPIO_WritePin(GPIOB,Linear_10V_ON_Pin,GPIO_PIN_RESET);//turns off 10V to BJT circuits

    return;
  }
  if(strcmp(aRxBuffer, "BatteryON") == 0)
  {

    HAL_GPIO_WritePin(GPIOB,Battery_SW_ON_Pin,GPIO_PIN_RESET);//turns off CubeSat battery 12.8-15V

    return;
  }
  if(strcmp(aRxBuffer, "RS485") == 0)
  {
    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    This code works for communication with Pizo legs.
    But the ASCII commands that Piezo LEGS needs are not implemented
    here. A list can be made where all the commands are available
    and implemented in the same way as of SiC.  
   //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/ 
    HAL_RS485Ex_Init(&huart1, UART_DE_POLARITY_HIGH, 10, 10);
    HAL_UART_Transmit(&huart1, (uint8_t *)bufftx ,strlen(bufftx),1000);
    HAL_UART_Receive(&huart1, (uint8_t *)buffrx, 40,1000);

    return;
  }
  else return;
}
/**
@brief Flush buffer, flushes the buffer after every received data from OBC.  
 
@param uint8_t , The received messeage from OBC aRxbuffer
      
 
@returns, Empty aRxBuffer array
*/
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    *pBuffer = 0;
    
    pBuffer++;
  }
} 
/* USER CODE END 3 */



/**
* @brief System Clock Configuration
* @retval None
*/


/* USER CODE BEGIN 4 */
/* void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
/* Prevent unused argument(s) compilation warning */
//  UNUSED(huart);



/* NOTE : This function Should not be modified, when the callback is needed,
the HAL_UART_TxCpltCallback could be implemented in the user file
*/ 
// }
/* USER CODE END 4 */

/**
* @brief  This function is executed in case of error occurrence.
* @param  file: The file name as string.
* @param  line: The line in file as a number.
* @retval None
*/
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
