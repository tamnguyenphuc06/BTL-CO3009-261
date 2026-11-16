#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include "main.h"
#include "global.h"

// Giá trị x10: [0] độ ẩm (%), [1] nhiệt độ (°C); nhiệt độ âm hiện được chặn về 0.
extern uint16_t value_x10[2];

void dht20_init(void);
// Lên lịch chuỗi reset; các bước I2C được thực hiện trong DHT20_Task_Run().
void dht20_reset(uint8_t regist);
// Hàm đọc đồng bộ giữ để tương thích mã cũ; không gọi trong scheduler.
void dht20_read(uint16_t* value);

// Task đọc cảm biến mỗi 100ms: không dùng HAL_Delay; mỗi giao dịch I2C chờ tối đa 5ms.
void DHT20_Task_Run(void);

#endif /* INC_DHT20_H_ */
