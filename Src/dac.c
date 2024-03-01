/*
 * dac.c
 *
 *      Author: aoberai
 */

#include "dac.h"
#include "stm32l4xx_hal.h"

//init SPI1 on PA4-PA7
void DAC_init(void) {
	  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
	  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5 |		// mask AF selection
			  	  	  	  GPIO_AFRL_AFSEL7);
	  GPIOA->AFR[0] |= ((5 << GPIO_AFRL_AFSEL4_Pos) |				// select SPI_1 (AF5)
	  		  	  	    (5 << GPIO_AFRL_AFSEL5_Pos) |
					    (5 << GPIO_AFRL_AFSEL7_Pos));
	  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |		// push-pull output
			  	  	  	 GPIO_OTYPER_OT7);

	  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |		// no pull ups or pull downs
			  	  	  	GPIO_PUPDR_PUPD7);
	  GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED4 | 					// low speed
			  	  	  	  GPIO_OSPEEDR_OSPEED5 |
						  GPIO_OSPEEDR_OSPEED7);
	  GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |		// mask function
			  	  	    GPIO_MODER_MODE7);
	  GPIOA->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 |	// enable alternate function
			  	  	   GPIO_MODER_MODE7_1);

	  // configure SPI 1
	  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);		// enable SPI1 clock
	  SPI1->CR1 = (SPI_CR1_MSTR);				// enable master mode, fck/2, hardware CS, MSB first, full duplex
	  SPI1->CR2 = (SPI_CR2_SSOE |				// enable CS output
			  	   SPI_CR2_NSSP |				// create CS pulse
				   (0xF << SPI_CR2_DS_Pos));	// 16-bit data frames
	  SPI1->CR1 |= (SPI_CR1_SPE);				// enable SPI
}

//Function that writes 12-bit DAC SPI data with a command in the upper nibble
void DAC_write(uint16_t value) {
	  while(!(SPI1->SR & SPI_SR_TXE));		// ensure room in TXFIFO before writing
	  SPI1->DR = value | (DAC_CMD << 12);
	  while ((SPI1->SR & SPI_SR_BSY));  	// wait until transmit buffer is empty
}

//takes in voltage value in mV and converts to 12-bit value for DAC
uint16_t DAC_volt_convert(int voltage) {

	uint16_t volt_conv = (uint16_t)(voltage * DAC_MAX_STEPS / VREF);

	if(volt_conv > DAC_MAX_INPUT) {
		return DAC_MAX_INPUT;
	}

	return volt_conv;
}

void DAC_write_volt(int mV) {
	DAC_write(DAC_volt_convert(mV));
}

