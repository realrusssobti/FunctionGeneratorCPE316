/* USER CODE BEGIN Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "keypad.h"
#include "dac.h"

int sine_table[SIZE];
int triangle_table[SIZE];
int saw_table[SIZE];

Waveform waveform = SQUARE;
uint16_t duty_cycle = 50;
uint32_t ccr1 = SIZE;
uint32_t count = 0;
uint16_t frequency = 1;
int dac_value;

void TIM2_init() {
	  NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));

	  RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);	// turn on TIM2
	  TIM2->CCR1 = ccr1;			            // set channel 1 compare value
	  TIM2->DIER |= (TIM_DIER_CC1IE);	        // enable interrupts on channel 1
	  TIM2->SR &= ~(TIM_SR_CC1IF);		        // clear interrupt flag
	  TIM2->ARR = RELOAD_VAL;			        // set count reload value 2^32
	  TIM2->CR1 |= TIM_CR1_CEN;                 // start timer
}

//generate waveform tables
void table_init() {
    for (int i = 0; i < SIZE; i++) {
    	sine_table[i] = (int) (DC_BIAS * sin(2 * M_PI * i / SIZE) + DC_BIAS);
    	saw_table[i] = (int) (VMAX * i / SIZE);
    }

    double slope = (double) VMAX / HALF_SIZE;

    for (int i = 0; i < HALF_SIZE; i++) {
      triangle_table[i] = (int)(slope * i);
    }

    for (int i = HALF_SIZE; i < SIZE; i++) {
      triangle_table[i] = (int)(VMAX - slope * (i - HALF_SIZE));
    }
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  //clocked at 32 MHz
  SystemClock_Config();

  //generate waveform look-up tables
  table_init();

  initKeypad();

  //init SPI1 peripheral
  DAC_init();

  //init and start timer
  TIM2_init();

  __enable_irq();

  while (1)
  {
	  char enteredChar = readChar();
	  switch(enteredChar)
	  			  {
	  				  case '1':
	  					  frequency = 1;
	  					  break;

	  				  case '2':

	  					  frequency = 2;
	  					  break;

	  				  case '3':
	  					  frequency = 3;
	  					  break;

	  				  case '4':
	  					  frequency = 4;
	  					  break;

	  				  case '5':
	  					  frequency = 5;
	  					  break;

	  				  case '6':
	  					  waveform = SINE;
	  					  break;

	  				  case '7':
	  					  waveform = TRIANGLE;
	  					  break;

	  				  case '8':
	  					  waveform = SAW;
	  					  break;

	  				  case '9':
	  					  waveform = SQUARE;
	  					  break;

	  				  case '0':
	  					  duty_cycle = 50;
	  					  break;

	  				  case '*':
	  					  if (duty_cycle > 10)
	  						  duty_cycle -= 10;
	  					  break;

	  				  case '#':
	  					  if (duty_cycle < 90)
	  						  duty_cycle += 10;
	  					  break;

	  			  }
	  		  }
}

void TIM2_IRQHandler(void)
{

  //clear interrupt flags
  TIM2->SR &= ~((TIM_SR_UIF) | (TIM_SR_CC1IF));

  switch(waveform) {
  	  case SQUARE:
  		    if(count < (duty_cycle * ccr1) / 100)
  			{
  				DAC_write_volt(VMAX);
  			}
  			else{
  				DAC_write_volt(0);
  			}

  		    //sample less points for higher frequency waves
  		    count = (count + frequency);


  			if(count > (uint32_t)(ccr1))
  			{
  				count = 0;
  			}
  		   break;

  	  case SINE:
          dac_value = sine_table[count];
          DAC_write_volt(dac_value);
          count = (count + frequency) % SIZE;
          break;

  	  case TRIANGLE:
          dac_value = triangle_table[count];
          DAC_write_volt(dac_value);
          count = (count + frequency) % SIZE;
          break;

  	  case SAW:
          dac_value = saw_table[count];
          DAC_write_volt(dac_value);
          count = (count + frequency) % SIZE;
          break;
  }

	//reload count register
	TIM2->CCR1 += ccr1;

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
