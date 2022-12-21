/*
Прошивка адаптирована с https://github.com/NorthernWidget/DS3231/blob/master/examples/set_echo/set_echo.ino
Руссифицировал комментарии и интерфейс, оптимизировал код
Олег "VelOBor" Величко, https://github.com/VelOBor/
*/

#include <DS3231.h>
#include <Wire.h>
//создаём объект часов реального времени и называем его myRTC для удобства
DS3231 myRTC;

//определяем типы переменных для хранения данных
byte year;
byte month;
byte date;
byte dow;
byte hour;
byte minute;
byte second;


bool century = false;
bool h12Flag;
bool pmFlag;

/*****************************************************************************************************
 * Setup
 *  - Открываем serial коммуникации на скорости 57600 (в serial monitor не забываем выставить ту же скорость)
 *  - Открываем шину I2C
 *  - Прочие операции "разового" характера
 *****************************************************************************************************/
void setup() {
  // Открываем серийный порт
  Serial.begin(57600);

  // Запускаем интерфейс I2C
  Wire.begin();

  // Делаем запрос от юзера на корректировку времени
  delay(4000);
  Serial.println("Формат времени: YYMMDDwhhmmssx");
  Serial.println("Где YY = Год (например. 22 это 2022)");
  Serial.println("      MM = Месяц (например. 12 это Декабрь)");
  Serial.println("      DD = Число месяца (например. 12 это 12ое число)");
  Serial.println("      w  = День недели от 1 до 7, 1 = Понедельник (например 5 это Пятница)");
  Serial.println("      hh = Час в 24-х часовом формате (например 22 это 10 вечера или 08 это 8 часов утра)");
  Serial.println("      mm = Минуты (от 00 до 59)");
  Serial.println("      ss = Секунды (от 00 до 59)");
  Serial.println("пример ввода: 2212213223300x");
  Serial.println("-----------------------------------------------------------------------------");
  Serial.println("Пожалуйста введите текущее время для DS3231 в указаном формате, ВАЖНО в конце латинская 'x':");
}

/*****************************************************************************************************
 * Loop
 *  - Запрашиываем корректировку времени от юзера
 *  - Выставляем время на DS3231
 *  - Выводим значение с DS3231 в течение первых 5 секунд
 *****************************************************************************************************/
void loop() {
  // Выводим запрос через последовательный порт на корректировку времени
  if (Serial.available()) {
    inputDateFromSerial();

    myRTC.setClockMode(false);  // ставим в режим 24ч, чтобы использовать 12ч нужно в скобках указать true

    myRTC.setYear(year);
    myRTC.setMonth(month);
    myRTC.setDate(date);
    myRTC.setDoW(dow);
    myRTC.setHour(hour);
    myRTC.setMinute(minute);
    myRTC.setSecond(second);

    // Выводим время в течение пяти секунд
    for (uint8_t i = 0; i < 5; i++){
        delay(1000);
        Serial.print(myRTC.getYear(), DEC);
        Serial.print("-");
        Serial.print(myRTC.getMonth(century), DEC);
        Serial.print("-");
        Serial.print(myRTC.getDate(), DEC);
        Serial.print(" ");
        Serial.print(myRTC.getHour(h12Flag, pmFlag), DEC); //24-hr
        Serial.print(":");
        Serial.print(myRTC.getMinute(), DEC);
        Serial.print(":");
        Serial.println(myRTC.getSecond(), DEC);
    }

    // Уведомляем о готовности принять значения времени
    Serial.println("Пожалуйста введите значение времени для DS3231 в конце ВАЖНО ЛАТИНСКАЯ 'x':");
  }
  delay(1000);
}

/*****************************************************************************************************
 * inputDateFromSerial
 *  - Получаем и переводим в нужный формат данные с последовательного порта
 *  - Храним значения в глобальных переменных
 *****************************************************************************************************/
void inputDateFromSerial() {
	// Вызываем эту функцию для вноса корректировки времени на DS3231
	// в формате YYMMDDwHHMMSS, и в конце ЛАТИНСКАЯ 'x'.
	boolean isStrComplete = false;
	char inputChar;
	byte temp1, temp2;
	char inputStr[20];

	uint8_t currentPos = 0;
	while (!isStrComplete) {
		if (Serial.available()) {
			inputChar = Serial.read();
			inputStr[currentPos] = inputChar;
			currentPos += 1;

      // Проверка что введённое значение в правильном формате (в конце ЛАТИНСКАЯ "x")
			if (inputChar == 'x') {
				isStrComplete = true;
			}
		}
	}
	Serial.println(inputStr);

  // находим в конце латинскую "x"
  int posX = -1;
  for(uint8_t i = 0; i < 20; i++) {
    if(inputStr[i] == 'x') {
      posX = i;
      break;
    }
  }

  // Нулевой знак в ASCII
  uint8_t zeroAscii = '0';

	// Читаем год
	temp1 = (byte)inputStr[posX - 13] - zeroAscii;
	temp2 = (byte)inputStr[posX - 12] - zeroAscii;
	year = temp1 * 10 + temp2;

	// месяц
	temp1 = (byte)inputStr[posX - 11] - zeroAscii;
	temp2 = (byte)inputStr[posX - 10] - zeroAscii;
	month = temp1 * 10 + temp2;

	// число
	temp1 = (byte)inputStr[posX - 9] - zeroAscii;
	temp2 = (byte)inputStr[posX - 8] - zeroAscii;
	date = temp1 * 10 + temp2;

	// день недели
	dow = (byte)inputStr[posX - 7] - zeroAscii;

	// час
	temp1 = (byte)inputStr[posX - 6] - zeroAscii;
	temp2 = (byte)inputStr[posX - 5] - zeroAscii;
	hour = temp1 * 10 + temp2;

	// минута
	temp1 = (byte)inputStr[posX - 4] - zeroAscii;
	temp2 = (byte)inputStr[posX - 3] - zeroAscii;
	minute = temp1 * 10 + temp2;

	// секунда
	temp1 = (byte)inputStr[posX - 2] - zeroAscii;
	temp2 = (byte)inputStr[posX - 1] - zeroAscii;
	second = temp1 * 10 + temp2;
}