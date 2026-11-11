#include "scheduler.h"
#include <stddef.h>

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint8_t current_task_count = 0;

void SCH_Init(void) {
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = NULL;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = 0;
    }
    current_task_count = 0;
}

// O(1)
void SCH_Update(void) {
    if (current_task_count > 0 && SCH_tasks_G[0].pTask != NULL) {
        if (SCH_tasks_G[0].Delay > 0) {
            SCH_tasks_G[0].Delay--;
        }
        if (SCH_tasks_G[0].Delay == 0) {
            SCH_tasks_G[0].RunMe++;
        }
    }
}

// Thêm tác vụ vào hàng đợi
uint8_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    if (current_task_count >= SCH_MAX_TASKS || pFunction == NULL) {
        return SCH_MAX_TASKS;
    }

    uint8_t index = 0;
    uint32_t accumulated_delay = 0;

    // Tìm vị trí chèn
    while (index < current_task_count && DELAY >= SCH_tasks_G[index].Delay) {
        DELAY -= SCH_tasks_G[index].Delay;
        index++;
    }

    // Dịch chuyển các tác vụ phía sau sang phải
    for (int8_t i = current_task_count; i > index; i--) {
        SCH_tasks_G[i] = SCH_tasks_G[i - 1];
    }

    // Gán thông tin tác vụ mới
    SCH_tasks_G[index].pTask = pFunction;
    SCH_tasks_G[index].Delay = DELAY;
    SCH_tasks_G[index].Period = PERIOD;
    SCH_tasks_G[index].RunMe = (DELAY == 0) ? 1 : 0;
    SCH_tasks_G[index].TaskID = index;

    // Trừ độ trễ của tác vụ ngay sau
    if (index + 1 <= current_task_count && SCH_tasks_G[index + 1].pTask != NULL) {
        SCH_tasks_G[index + 1].Delay -= DELAY;
    }

    current_task_count++;
    return index;
}

uint8_t SCH_Delete_Task(uint32_t task_index) {
    if (task_index >= current_task_count) {
        return 0;
    }

    for (uint8_t i = task_index; i < current_task_count - 1; i++) {
        SCH_tasks_G[i] = SCH_tasks_G[i + 1];
    }

    SCH_tasks_G[current_task_count - 1].pTask = NULL;
    SCH_tasks_G[current_task_count - 1].Delay = 0;
    SCH_tasks_G[current_task_count - 1].Period = 0;
    SCH_tasks_G[current_task_count - 1].RunMe = 0;

    current_task_count--;
    return 1;
}

// Thực thi tác vụ tại vòng lặp while(1)
void SCH_Dispatch_Tasks(void) {
    while (current_task_count > 0 && SCH_tasks_G[0].RunMe > 0) {
        // Thực thi hàm tác vụ
        (*SCH_tasks_G[0].pTask)();

        void (*temp_pTask)(void) = SCH_tasks_G[0].pTask;
        uint32_t temp_period = SCH_tasks_G[0].Period;

        // Xóa tác vụ vừa thực thi ở đầu hàng đợi
        SCH_Delete_Task(0);

        // Nếu là tác vụ lặp lại, thêm lại vào hàng đợi
        if (temp_period > 0) {
            SCH_Add_Task(temp_pTask, temp_period, temp_period);
        }
    }
}
