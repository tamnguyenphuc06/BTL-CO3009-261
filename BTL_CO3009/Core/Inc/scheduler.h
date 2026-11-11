#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

#define SCH_MAX_TASKS   10

typedef struct {
    void (*pTask)(void);    // Con trỏ hàm thực thi tác vụ
    uint32_t Delay;         // Độ trễ tương đối
    uint32_t Period;        // Chu kỳ lặp lại
    uint8_t  RunMe;         // Cờ báo tác vụ sẵn sàng chạy
    uint32_t TaskID;        // Định danh
} sTask;

void     SCH_Init(void);
void     SCH_Update(void);          // Gọi trong ngắt Timer (O(1))
uint8_t  SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
uint8_t  SCH_Delete_Task(uint32_t task_index);
void     SCH_Dispatch_Tasks(void);  // Gọi trong vòng lặp while(1)

#endif /* INC_SCHEDULER_H_ */
