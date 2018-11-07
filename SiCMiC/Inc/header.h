#include "stm32l0xx_hal.h"
#include "adc.h"
#include "dac.h"
#include "i2c.h"
#include "iwdg.h"
#include "gpio.h"
#include "usart.h"
#include "SystemClock_Config.h"
#include "SystemPower_Config.h"
#include "start_test.h"
#include "string.h"
#include <stdbool.h>


void send_message(uint8_t *);
void check_OBC_message(void);
void receive_OBC_message(void);
void SystemClock_Config(void);