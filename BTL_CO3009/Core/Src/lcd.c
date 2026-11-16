#include "lcd.h"
#include "dht20.h"
#include "fsm_manual.h"
#include "fsm_tuning.h"
#include <stdio.h>
#include "fsm_automatic.h"
extern I2C_HandleTypeDef hi2c1;

#define SLAVE_ADDRESS_LCD (0x21 << 1)
#define LCD_I2C_TIMEOUT_MS 5U

static uint8_t lcd_i2c_failed = 0;

/* Điền khoảng trắng để xóa nội dung cũ. */
static void lcd_pad_line(char line[17]) {
    uint8_t i = 0;
    while (i < 16 && line[i] != '\0') i++;
    while (i < 16) line[i++] = ' ';
    line[16] = '\0';
}

void lcd_send_cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C; // en=1, rs=0
    data_t[1] = data_u | 0x08; // en=0, rs=0
    data_t[2] = data_l | 0x0C; // en=1, rs=0
    data_t[3] = data_l | 0x08; // en=0, rs=0
    if (HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, data_t, 4, LCD_I2C_TIMEOUT_MS) != HAL_OK) {
        lcd_i2c_failed = 1;
    }
}

void lcd_send_data(char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    data_t[0] = data_u | 0x0D; // en=1, rs=1
    data_t[1] = data_u | 0x09; // en=0, rs=1
    data_t[2] = data_l | 0x0D; // en=1, rs=1
    data_t[3] = data_l | 0x09; // en=0, rs=1
    if (HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, data_t, 4, LCD_I2C_TIMEOUT_MS) != HAL_OK) {
        lcd_i2c_failed = 1;
    }
}

void lcd_init(void) {
    lcd_send_cmd(0x33); /* set 4-bits interface */
    lcd_send_cmd(0x32);
    HAL_Delay(50);
    lcd_send_cmd(0x28); /* start to set LCD function */
    HAL_Delay(50);
    lcd_send_cmd(0x01); /* clear display */
    HAL_Delay(50);
    lcd_send_cmd(0x06); /* set entry mode */
    HAL_Delay(50);
    lcd_send_cmd(0x0c); /* set display to on */
    HAL_Delay(50);
    lcd_send_cmd(0x02); /* move cursor to home and set data address to 0 */
    HAL_Delay(50);
    lcd_send_cmd(0x80);
}

void lcd_send_string(char *str) {
    while (*str && !lcd_i2c_failed) lcd_send_data(*str++);
}

void lcd_clear_display(void) {
    lcd_send_cmd(0x01);
}

void lcd_goto_XY(int row, int col) {
    uint8_t pos_Addr;
    if (row == 1) {
        pos_Addr = 0x80 + row - 1 + col;
    } else {
        pos_Addr = 0x80 | (0x40 + col);
    }
    lcd_send_cmd(pos_Addr);
}

// Cập nhật nội dung hiển thị trên LCD 16x2
void LCD_Task_Run(void) {
    char line1[17];
    char line2[17];

    //  bỏ lượt cập nhật này, tránh chặn task nút và đèn.
    if (HAL_I2C_IsDeviceReady(&hi2c1, SLAVE_ADDRESS_LCD, 1,
                              LCD_I2C_TIMEOUT_MS) != HAL_OK) {
        return;
    }
    lcd_i2c_failed = 0;

    // Dòng 1: chế độ, màu đèn từng đường và thời gian còn lại.
    switch (current_mode) {
        case MODE_AUTOMATIC:
            switch (auto_state) {
                case AUTO_RED_GREEN:
                    snprintf(line1, sizeof(line1), "A W1:R%02u W2:G%02u",
                             countdown_way1, countdown_way2);
                    break;

                case AUTO_RED_YELLOW:
                    snprintf(line1, sizeof(line1), "A W1:R%02u W2:Y%02u",
                             countdown_way1, countdown_way2);
                    break;

                case AUTO_GREEN_RED:
                    snprintf(line1, sizeof(line1), "A W1:G%02u W2:R%02u",
                             countdown_way1, countdown_way2);
                    break;

                case AUTO_YELLOW_RED:
                    snprintf(line1, sizeof(line1), "A W1:Y%02u W2:R%02u",
                             countdown_way1, countdown_way2);
                    break;

                default:
                    snprintf(line1, sizeof(line1), "AUTO STATE ERROR");
                    break;
            }
            break;

        case MODE_MANUAL:
            if (manual_state == MANUAL_WAY1_PRIORITY) {
                snprintf(line1, sizeof(line1), "MAN W1:G W2:R");
            } else {
                snprintf(line1, sizeof(line1), "MAN W1:R W2:G");
            }
            break;

        case MODE_TUNING:
            // Hai đường cùng nhấp nháy màu đang được chỉnh.
            if (tuning_state == TUNING_RED) {
                snprintf(line1, sizeof(line1), "SET W1/W2:R %02u",
                         temp_tuning_value);
            } else if (tuning_state == TUNING_YELLOW) {
                snprintf(line1, sizeof(line1), "SET W1/W2:Y %02u",
                         temp_tuning_value);
            } else {
                snprintf(line1, sizeof(line1), "SET W1/W2:G %02u",
                         temp_tuning_value);
            }
            break;

        default:
            snprintf(line1, sizeof(line1), "MODE ERROR");
            break;
    }

    // Dòng 2: nhiệt độ và độ ẩm từ DHT20.
    if (value_x10[0] >= 1000U) {
        snprintf(line2, sizeof(line2), "T:%02u.%u%cC H:100%%",
                 (unsigned)(value_x10[1] / 10U),
                 (unsigned)(value_x10[1] % 10U),
                 0xDF);
    } else {
        snprintf(line2, sizeof(line2), "T:%02u.%u%cC H:%02u.%u%%",
                 (unsigned)(value_x10[1] / 10U),
                 (unsigned)(value_x10[1] % 10U),
                 0xDF,
                 (unsigned)(value_x10[0] / 10U),
                 (unsigned)(value_x10[0] % 10U));
    }

    // Ghi đủ 16 cột để xóa ký tự còn sót từ lần trước.
    lcd_pad_line(line1);
    lcd_pad_line(line2);

    lcd_goto_XY(1, 0);
    lcd_send_string(line1);
    lcd_goto_XY(2, 0);
    lcd_send_string(line2);
}
