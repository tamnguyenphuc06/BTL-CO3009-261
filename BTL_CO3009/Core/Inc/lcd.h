#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32f1xx_hal.h"
#include "global.h"

void lcd_init(void);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_send_string(char *str);
void lcd_clear_display(void);
void lcd_goto_XY(int row, int col);

// Hàm cập nhật giao diện 2 dòng định kỳ cho Scheduler
void LCD_Task_Run(void);

#endif /* INC_LCD_H_ */
