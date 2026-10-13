
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Password.h>

#define buzzer 11

// Servo motor
Servo servo;

// LCD: I2C address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Password setup
Password password = Password("1234");

const byte maxPasswordLength = 4;
byte currentPasswordLength = 0;

// LCD cursor position for entered digits
byte a = 5;

// 4x4 keypad configuration
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Keypad row and column pins
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);

void setup() {
  Serial.begin(9600);

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  // Initialize servo
  servo.attach(10);
  servo.write(90);  // Locked position

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(3, 0);
  lcd.print("WELCOME TO");

  lcd.setCursor(0, 1);
  lcd.print("DOOR LOCK SYSTEM");

  delay(2500);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("ENTER PASSWORD");
  lcd.setCursor(5, 1);
}

void loop() {
  char key = keypad.getKey();

  if (key != NO_KEY) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    // Clear password using C
    if (key == 'C') {
      resetPassword();
    }

    // Submit password using D
    else if (key == 'D') {
      if (currentPasswordLength == maxPasswordLength) {
        dooropen();
      } else {
        lcd.setCursor(0, 0);
        lcd.print("ENTER 4 DIGITS  ");
        delay(1000);

        lcd.setCursor(0, 0);
        lcd.print("ENTER PASSWORD  ");
      }
    }

    // Accept numeric keys only
    else if (key >= '0' && key <= '9') {
      processNumberKey(key);
    }
  }
}

void processNumberKey(char key) {
  // Do not accept more than four digits
  if (currentPasswordLength >= maxPasswordLength) {
    return;
  }

  // Display the actual digit entered
  lcd.setCursor(a, 1);
  lcd.print(key);

  // Print detected digit in Serial Monitor
  Serial.print("Entered digit: ");
  Serial.println(key);

  a++;
  currentPasswordLength++;

  // Add digit to password checking library
  password.append(key);
}

void dooropen() {
  if (password.evaluate()) {
    // Correct password
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);

    servo.write(180);  // Unlock position

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CORRECT PASSWORD");

    lcd.setCursor(0, 1);
    lcd.print("DOOR OPENED");

    Serial.println("Correct password. Door opened.");

    delay(5000);

    // Lock the door again
    servo.write(90);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DOOR LOCKED");

    Serial.println("Door locked again.");

    delay(1500);
  } 
  else {
    // Incorrect password
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WRONG PASSWORD!");

    lcd.setCursor(0, 1);
    lcd.print("TRY AGAIN");

    Serial.println("Wrong password.");

    delay(2000);
  }

  resetPassword();
}

void resetPassword() {
  password.reset();
  currentPasswordLength = 0;
  a = 5;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER PASSWORD");
  lcd.setCursor(5, 1);

  Serial.println("Password input cleared.");
}
