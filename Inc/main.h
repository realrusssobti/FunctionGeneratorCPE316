

/* Define to prevent recursive inclusion ------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include <math.h>

/* Private function prototypes ----------------------------*/
void SystemClock_Config(void);

void TIM2_init(void);

void table_init(void);

void Error_Handler(void);

#define VMAX            3000
#define DC_BIAS         (VMAX / 2)
#define SIZE            566
#define HALF_SIZE       (SIZE / 2)

//global variables accessed by ISR

typedef enum {
	SQUARE,
	SINE,
	TRIANGLE,
	SAW
} Waveform;


#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB





#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
