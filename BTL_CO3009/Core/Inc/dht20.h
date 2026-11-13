#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include <stdio.h>
#include "main.h"
#include "global.h"

extern uint16_t value_x10[2];

void dht20_init(void);
void dht20_reset(uint8_t regist);
void dht20_read(uint16_t* value);

// Task đọc cảm biến không gây trễ (Non-blocking) cho Scheduler
void DHT20_Task_Run(void);

#endif /* INC_DHT20_H_ */
