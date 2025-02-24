#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// Pin Definitions
#define PAYTM_PIN 2        // Paytm machine signal input
#define MOTOR_IN1 3        // L298 Motor Driver IN1
#define MOTOR_IN2 4        // L298 Motor Driver IN2
#define GSM_TX 10          // SIM800L TX
#define GSM_RX 11          // SIM800L RX
#define RS 7
#define EN 8
#define D4 9
#define D5 6
#define D6 5
#define D7 4
#define STOCK_THRESHOLD 5  // Stock threshold for refill alert

// Initialize LCD and GSM Module
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
SoftwareSerial sim800(GSM_TX, GSM_RX);

int stock = 25; // Initial stock of pads
bool vending = false;

void setup() {
    pinMode(PAYTM_PIN, INPUT);
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Vending Machine");
    
    sim800.begin(9600);
    delay(1000);
}

void loop() {
    if (digitalRead(PAYTM_PIN) == HIGH) {
        vendItem();
    }
    updateDisplay();
}

void vendItem() {
    if (stock > 0) {
        vending = true;
        lcd.setCursor(0, 1);
        lcd.print("Dispensing...");
        
        // Rotate Motor for 1 sec
        digitalWrite(MOTOR_IN1, HIGH);
        digitalWrite(MOTOR_IN2, LOW);
        delay(1000);
        digitalWrite(MOTOR_IN1, LOW);
        digitalWrite(MOTOR_IN2, LOW);
        
        stock--;
        sendSMS("Pad dispensed. Remaining stock: " + String(stock));
        delay(500);
        
        // Check if refill is needed
        if (stock <= STOCK_THRESHOLD) {
            sendSMS("Low stock alert! Remaining: " + String(stock));
        }
        vending = false;
    } else {
        sendSMS("Stock empty! Please refill.");
        lcd.setCursor(0, 1);
        lcd.print("Stock Empty!");
    }
}

void updateDisplay() {
    lcd.setCursor(0, 1);
    lcd.print("Stock: " + String(stock) + "    ");
}

void sendSMS(String message) {
    sim800.println("AT+CMGF=1");
    delay(100);
    sim800.println("AT+CMGS=\"+91XXXXXXXXXX\""); // Replace with vendor number
    delay(100);
    sim800.println(message);
    delay(100);
    sim800.write(26);
    delay(1000);
}
