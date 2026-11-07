# BÀI TẬP LỚN VI XỬ LÝ – VI ĐIỀU KHIỂN (CO3009)
## Đề tài: Hệ thống đèn giao thông 2 tuyến đường trên mạch XNUCLEO-F103RB
**Học kỳ:** HK1 / 2026–2027  
**Giảng viên hướng dẫn:** TS. Nguyễn Hòa Hưng  
**Lớp:** L02  
**Nhóm:** 

---

## 1. Thành viên nhóm & Phân công nhiệm vụ
| STT | Họ và tên | MSSV | Vai trò / Nhiệm vụ phụ trách | Tỷ lệ đóng góp |
| :---: | :--- | :---: | :--- | :---: |
| 1 | **Nguyễn Phúc Tâm** | 2413065 | **Kiến trúc phần mềm & Logic:**<br>- Thiết kế và hiện thực Scheduler $O(1)$ (Relative Delay Queue).<br>- Hiện thực 3 máy trạng thái: `fsm_automatic`, `fsm_manual`, `fsm_tuning`.<br>- Tích hợp toàn hệ thống, xử lý tình huống biên.<br>- Cấu hình CubeMX (Clock 64MHz, GPIO, TIM2 ngắt 10ms, I2C1).<br>- Soạn thảo báo cáo: Mục 2 (Thiết kế hệ thống), Mục 3.4 & 3.5 (FSM). 
| 2 | **Nguyễn Đức Phước** | 2412805 | **Phần cứng & Driver ngoại vi:**- Hiện thực các driver: `button` (chống dội, nhấn/giữ), `traffic_light`, `lcd`, `dht20`, `software_timer`.<br>- Kiểm thử phần cứng độc lập.<br>- Soạn thảo báo cáo: Mục 1 (Tổng quan), Mục 3.1 & 3.3 (Hardware & Drivers). 

---

## 2. Tổng quan dự án
Hệ thống điều khiển đèn giao thông cho ngã tư 2 tuyến đường sử dụng vi điều khiển **STM32F103RB** trên bo mạch **XNUCLEO-F103RB**.

### Các tính năng chính:
- **Bộ điều phối tác vụ (Scheduler $O(1)$):** Điều phối toàn bộ hoạt động hệ thống bằng hàng đợi thời gian tương đối (delta-delay), cập nhật ngắt trong $O(1)$, hỗ trợ cả task chu kỳ và one-shot task.
- **Chế độ tự động (Automatic Mode):** Điều khiển 2 tuyến đèn Đỏ – Xanh – Vàng hoạt động luân phiên theo thời gian định sẵn.
- **Chế độ bằng tay (Manual Mode):** Cho phép người điều khiển cưỡng bức chuyển trạng thái đèn bằng nút nhấn khi có sự cố hoặc điều tiết giao thông.
- **Chế độ chỉnh thời gian (Tuning Mode):** Cài đặt lại thời gian đèn trực tiếp qua nút bấm và hiển thị thông số trên LCD.
- **Giám sát môi trường:** Đọc cảm biến DHT20 và hiển thị nhiệt độ, độ ẩm môi trường lên màn hình LCD qua giao tiếp I2C1.
- **Tách lớp kiến trúc:** Tách biệt hoàn toàn giữa tầng điều khiển phần cứng (HAL/Drivers) và tầng logic máy trạng thái (FSM).

---

## 3. Cấu trúc thư mục mã nguồn
```text
.
├── BTL_CO3009/                         # Thư mục dự án STM32CubeIDE
│   ├── Core/
│   │   ├── Inc/
│   │   │   ├── global.h                # Trạng thái FSM, biến thời gian toàn cục
│   │   │   ├── scheduler.h             # Cấu trúc dữ liệu và API Scheduler O(1)
│   │   │   ├── software_timer.h        # Module software timer cho FSM
│   │   │   ├── button.h                # Driver xử lý chống dội và sự kiện nút nhấn
│   │   │   ├── traffic_light.h         # Driver điều khiển trạng thái các cụm đèn
│   │   │   ├── dht20.h                 # Driver cảm biến nhiệt độ - độ ẩm DHT20
│   │   │   ├── lcd.h                   # Driver hiển thị màn hình LCD I2C
│   │   │   ├── fsm_automatic.h         # Máy trạng thái chế độ tự động
│   │   │   ├── fsm_manual.h            # Máy trạng thái chế độ thủ công
│   │   │   └── fsm_tuning.h            # Máy trạng thái chế độ cài đặt thời gian
│   │   └── Src/
│   │       ├── main.c                  # Khởi tạo ngoại vi, clock và vòng lặp chính
│   │       ├── global.c
│   │       ├── scheduler.c
│   │       ├── software_timer.c
│   │       ├── button.c
│   │       ├── traffic_light.c
│   │       ├── dht20.c
│   │       ├── lcd.c
│   │       ├── fsm_automatic.c
│   │       ├── fsm_manual.c
│   │       └── fsm_tuning.c
│   ├── Drivers/                        # Thư viện CMSIS và STM32F1xx HAL Driver
│   └── BTL_CO3009.ioc                  # File cấu hình phần cứng STM32CubeMX
├── .gitignore                          # Cấu hình bỏ qua các tệp build tạm thời
└── README.md                           # Tài liệu hướng dẫn dự án