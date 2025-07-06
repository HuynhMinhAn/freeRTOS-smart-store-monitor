#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Buzzer.h>
#include <DHT.h>
#include <FreeRTOSConfig.h>

// --- Định nghĩa phần cứng ---
#define SCREEN_WIDTH 128 // Chiều rộng màn hình OLED
#define SCREEN_HEIGHT 64 // Chiều cao màn hình OLED
#define OLED_RESET -1 // Không sử dụng chân reset
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define trigPinIn 12 // Chân trigger của cảm biến siêu âm bên trong
#define echoPinIn 13 // Chân echo của cảm biến siêu âm bên trong
#define trigPinOut 27 // Chân trigger của cảm biến siêu âm bên ngoài
#define echoPinOut 14 // Chân echo của cảm biến siêu âm bên ngoài
#define ledPin 26 // Chân điều khiển LED chính
#define led2 23 // Chân điều khiển LED phụ
#define buzzerPin 33 // Chân kết nối còi buzzer
#define DHTPIN 4 // Chân kết nối cảm biến DHT22
#define DHTTYPE DHT22 // Loại cảm biến DHT
#define PTR 18 // Chân cảm biến ánh sáng hoặc nút nhấn

// --- Biến toàn cục ---
int i = 0; // Biến đếm cho các vòng lặp
int count = 0; // Số người hiện tại trong cửa hàng
int totalCount = 0; // Tổng số người đã vào cửa hàng
bool isInside = false; // Trạng thái phát hiện người bên trong
bool isOutside = false; // Trạng thái phát hiện người bên ngoài

TimerHandle_t xTimer; // Timer để cập nhật màn hình OLED
Buzzer alertBuzzer(buzzerPin); // Đối tượng buzzer
DHT dht(DHTPIN, DHTTYPE); // Đối tượng cảm biến DHT22
SemaphoreHandle_t alarmSemaphore; // Semaphore để báo động

int SemCount = 0; // Đếm số lượng semaphore được gửi
int SemLimit = 0; // Giới hạn số lần semaphore
float currentTemp = 0; // Nhiệt độ hiện tại
float currentHumid = 0; // Độ ẩm hiện tại
bool tempExceeded = false; // Trạng thái nhiệt độ vượt ngưỡng

// --- Hàm phát nhạc khi có khách vào ---
void playBuzzer() {
    alertBuzzer.sound(NOTE_E7, 250); // Phát âm thanh tần số E7 trong 250 ms
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay 100 ms
}

// --- Hàm callback cho Timer để cập nhật màn hình OLED ---
void vTimerLCDCallback(TimerHandle_t xTimer) {
    display.clearDisplay(); // Xóa màn hình
    display.setCursor(0, 0);
    display.setTextSize(1); // Kích thước chữ nhỏ
    display.setTextColor(SSD1306_WHITE); // Màu chữ trắng

    if (tempExceeded) {
        display.print("Nguy hiem: ");
        display.setCursor(0, 16);
        display.print("Nhiet do vuot nguong!");
        display.setCursor(0, 32);
        display.print("Hay kiem tra");
    } else {
        display.print("KH hien tai: ");
        display.print(count); // Hiển thị số khách hiện tại
        display.setCursor(0, 16);
        display.print("TONG KHACH VAO: ");
        display.print(totalCount); // Hiển thị tổng số khách
        display.setCursor(0, 32);
        display.print("Nhiet do: ");
        display.print(currentTemp); // Hiển thị nhiệt độ
        display.print(" C");
        display.setCursor(0, 48);
        display.print("Do am: ");
        display.print(currentHumid); // Hiển thị độ ẩm
        display.print(" %");
    }

    display.display(); // Cập nhật màn hình
}

// --- Task kiểm tra người vào cửa hàng ---
void entryTask(void *pvParameters) {
    while (1) {
        long durationIn, distanceIn;

        // Kích hoạt cảm biến siêu âm bên trong
        digitalWrite(trigPinIn, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPinIn, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPinIn, LOW);
        durationIn = pulseIn(echoPinIn, HIGH, 30000); // Đo thời gian phản hồi
        distanceIn = (durationIn / 2) / 29.1; // Tính khoảng cách

        if (durationIn == 0) {
            distanceIn = 400; // Đặt giá trị lớn nếu không nhận được tín hiệu
        }

        Serial.print("KC Vao: ");
        Serial.print(distanceIn);
        Serial.println(" cm");

        // Kiểm tra nếu có người vào
        if (distanceIn < 20) {
            if (!isInside) {
                isInside = true;
                count++; // Tăng số người hiện tại
                totalCount++; // Tăng tổng số khách
                digitalWrite(ledPin, HIGH); // Bật đèn LED
                playBuzzer(); // Phát âm thanh
                vTaskDelay(pdMS_TO_TICKS(50)); // Delay 50 ms
                digitalWrite(ledPin, LOW); // Tắt đèn LED
            }
        } else {
            isInside = false; // Không phát hiện người
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Delay để giảm tải CPU
    }
}

// --- Task kiểm tra người ra khỏi cửa hàng ---
void exitTask(void *pvParameters) {
    while (1) {
        long durationOut, distanceOut;

        // Kích hoạt cảm biến siêu âm bên ngoài
        digitalWrite(trigPinOut, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPinOut, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPinOut, LOW);
        durationOut = pulseIn(echoPinOut, HIGH, 30000); // Đo thời gian phản hồi
        distanceOut = (durationOut / 2) / 29.1; // Tính khoảng cách

        if (durationOut == 0) {
            distanceOut = 400; // Đặt giá trị lớn nếu không nhận được tín hiệu
        }

        Serial.print("KC Ra: ");
        Serial.print(distanceOut);
        Serial.println(" cm");

        // Kiểm tra nếu có người ra
        if (distanceOut < 20) {
            if (!isOutside) {
                isOutside = true;
                count--; // Giảm số người hiện tại
                if (count < 0) count = 0; // Đảm bảo không âm
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        } else {
            isOutside = false; // Không phát hiện người
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Delay để giảm tải CPU
    }
}

// --- Task gửi semaphore khi nhiệt độ vượt ngưỡng ---
void taskSendSemaphore(void *pvParameters) {
    while (1) {
        currentHumid = dht.readHumidity(); // Đọc độ ẩm
        currentTemp = dht.readTemperature(); // Đọc nhiệt độ

        Serial.print("Nhiet do: ");
        Serial.println(currentTemp);
        Serial.print("Do am: ");
        Serial.println(currentHumid);

        if (currentTemp > 30 && SemLimit < 5) {
            Serial.println("Bắt đầu gửi semaphore");
            tempExceeded = true; // Đánh dấu nhiệt độ vượt ngưỡng
            xSemaphoreGive(alarmSemaphore); // Gửi semaphore
            SemCount++;
            SemLimit++;
            Serial.println("Semaphore cấp!");
        } else {
            tempExceeded = false; // Đánh dấu nhiệt độ bình thường
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Đọc dữ liệu mỗi 2 giây
    }
}

// --- Task nhận semaphore và báo động ---
void taskReceiveSemaphore(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(alarmSemaphore, portMAX_DELAY) == pdPASS) {
            SemCount--;
            if (SemCount == 0) {
                SemLimit = 0;
            }
            Serial.println("Semaphore nhận!");

            // Kích hoạt báo động 5 lần
            for (int i = 0; i < 5; i++) {
                alertBuzzer.sound(NOTE_C7, 150); // Âm thanh tần số C7
                Serial.println("Buzzer kêu cảnh báo!");
                vTaskDelay(pdMS_TO_TICKS(100));
                alertBuzzer.sound(NOTE_G7, 150); // Âm thanh tần số G7
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    }
}

// --- Task điều khiển đèn LED phụ ---
void taskLightControl(void *pvParameters) {
    while (1) {
        int lightState = digitalRead(PTR); // Đọc trạng thái cảm biến ánh sáng hoặc nút nhấn
        if (lightState == LOW) {
            digitalWrite(led2, LOW); // Tắt đèn LED phụ
        } else {
            digitalWrite(led2, HIGH); // Bật đèn LED phụ
        }
    }
}

// --- Hàm khởi tạo ---
void setup() {
    Serial.begin(9600);

    // Khởi tạo màn hình OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("Không thể khởi tạo màn hình OLED");
        while (1);
    }
    display.display();
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Khởi tạo các chân I/O
    pinMode(trigPinIn, OUTPUT);
    pinMode(echoPinIn, INPUT);
    pinMode(trigPinOut, OUTPUT);
    pinMode(echoPinOut, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(DHTPIN, INPUT_PULLUP);
    pinMode(PTR, INPUT);
    dht.begin(); // Khởi tạo cảm biến DHT22
    digitalWrite(led2, LOW);

    // Khởi tạo semaphore
    alarmSemaphore = xSemaphoreCreateCounting(5, 0);

    // Khởi tạo Timer
    xTimer = xTimerCreate("TimerLCD", pdMS_TO_TICKS(2000), pdTRUE, (void *)0, vTimerLCDCallback);
    if (xTimerStart(xTimer, 0) != pdPASS) {
        Serial.println("Không thể khởi tạo LCDTimer");
        while (1);
    }

    // Tạo các task
    xTaskCreate(entryTask, "EntryTask", 2048, NULL, 2, NULL);
    xTaskCreate(exitTask, "ExitTask", 2048, NULL, 2, NULL);
    xTaskCreate(taskSendSemaphore, "SendSemaphoreTask", 2048, NULL, 1, NULL);
    xTaskCreate(taskReceiveSemaphore, "ReceiveSemaphoreTask", 2048, NULL, 3, NULL);
    xTaskCreate(taskLightControl, "LightControlTask", 2048, NULL, 3, NULL);
}

// --- Hàm loop ---
void loop() {
    // Không cần làm gì trong loop vì tất cả đã được thực hiện trong các task.
}
