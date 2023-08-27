/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "sys.h"//???????
#include "delay.h"
#include "lcd.h"//??

#include "arm_math.h"
#include "arm_const_structs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);
	USART1->DR = (uint8_t) ch;      
	return ch;
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define width	800
//#define high	480	//4.3寸
#define high	400	//4.3寸
#define TEST_LENGTH_SAMPLES 4096
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t fpga_get[2048]={0};

float32_t testInput[TEST_LENGTH_SAMPLES];
static float32_t testOutput[TEST_LENGTH_SAMPLES/2];

uint32_t fftSize = 2048;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t flag = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		flag = 1;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_DMA_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	lcd_init();//??
	lcd_clear(WHITE);
	HAL_Delay(500);
	lcd_display_dir(1);
	HAL_UART_Receive_DMA(&huart2,fpga_get,2048);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	char* temp1[20] = {"1M"};
	lcd_show_string (width*0/12,440,60,32,32,*temp1,RED);
	*temp1 = "10M";
	lcd_show_string (width*1/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "20M";
	lcd_show_string (width*2/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "30M";
	lcd_show_string (width*3/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "40M";
	lcd_show_string (width*4/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "50M";
	lcd_show_string (width*5/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "60M";
	lcd_show_string (width*6/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "70M";
	lcd_show_string (width*7/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "80M";
	lcd_show_string (width*8/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "90M";
	lcd_show_string (width*9/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "100M";
	lcd_show_string (width*10/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "110M";
	lcd_show_string (width*11/12-15,440,60,32,32,*temp1,RED);
	*temp1 = "120M";
	lcd_show_string (width*12/12-30,440,60,32,32,*temp1,RED);
	
  while (1)
  {
		if(flag)
		{
//			for(int i=0; i<2048;i++)
//			{
//				printf("%d\r\n",fpga_get[i]);
//			}
//			HAL_GPIO_WritePin(FPGA_CTL_GPIO_Port,FPGA_CTL_Pin,GPIO_PIN_RESET);
			for(int i=0; i<1024;i++)
			{
				lcd_draw_line (i/1.28,high,i/1.28,high-testOutput[i]/2*480,WHITE);
			}
			
			for(int i = 0; i < 2048; i++)
			{
				//将偶数定义为实部，将奇数定义为虚部
					testInput[i * 2] = fpga_get[i] * 2 / 256;//实部赋值，* 3.3 / 4096是为了将ADC采集到的值转换成实际电压
					testInput[i * 2 + 1] = 0;//虚部赋值，固定为0
			}
			
			/* Process the data through the CFFT/CIFFT module */
			arm_cfft_f32(&arm_cfft_sR_f32_len2048, testInput , ifftFlag, doBitReverse);

			/* Process the data through the Complex Magnitude Module for
			calculating the magnitude at each bin */
			arm_cmplx_mag_f32(testInput, testOutput, fftSize);

			testOutput[0] /= 2048;

			for (int i = 1; i < 1024; i++)//输出各次谐波幅值
			{
					testOutput[i] /= 1024;
			}
			
			for(int i=0; i<1024;i++)
			{
//				printf("%f\r\n",testOutput[i]);
				if(i==0)
					lcd_draw_line (3,high,3,high-testOutput[0]/2*480,RED);
				lcd_draw_line (i/1.28,high,i/1.28,high-testOutput[i]/2*480,RED);
			}

			HAL_UART_Receive_DMA(&huart2,fpga_get,2048);
			flag = 0;
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
