/*
 * dac.h
 *
 *      Author: aoberai
 */
#include <stdint.h>

#ifndef INC_DAC_H_
#define INC_DAC_H_

void DAC_init(void);

void DAC_write(uint16_t value);

uint16_t DAC_volt_convert(int mV);

void DAC_write_volt(int mV);

#define DAC_CMD         3
#define DAC_MAX_STEPS   4095
#define DAC_MAX_INPUT   0xFFF
#define RELOAD_VAL      0xFFFFFFFF
#define VREF            3300


#endif /* INC_DAC_H_ */
