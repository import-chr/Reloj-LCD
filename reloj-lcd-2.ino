#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

RTC_DS1307 RTC;
LiquidCrystal_I2C dLCD(0x27, 16, 2);
#define Buzzer 8				//Buzzer 								-	pin 8
#define Alarma 6				//Switch 								-	pin 6
#define Mode 5					//Button (modo) 				-	pin 5
#define Set 4						//Button (establecer) 	-	pin 4
#define Up 3						//Button (subir) 				-	pin 3
#define Down 2					//Button (bajar) 				-	pin 2
const int inputPines[5] = {Alarma, Mode, Set, Up, Down};
int arrAlarm[2][1] = {{00}, {00}};
int arrTimer[3][1] = {{00}, {00}, {00}};
int contB1, modo = 0, contB2, setear = 0, contB3, contB4, x;
//float tempC;

void setup() {
	Serial.begin(9600);

	for(int i = 0; i < 5; i++) {
		pinMode(inputPines[i], INPUT);
	}

	pinMode(Buzzer, OUTPUT);

	Wire.begin();
	RTC.begin();
	//RTC.adjust(DateTime(__DATE__, __TIME__));
	dLCD.init();
	dLCD.backlight();
}

void loop() {
	//tempC = analogRead(A0);
	//tempC = (tempC * 5 * 100) / 1024;
	//Serial.println(tempC);

	x = digitalRead(Alarma);
	__mode__(modo);
}

void __mode__(int &pushMode) {
	if(digitalRead(Mode) == HIGH && contB1 == 0) {
		++pushMode;
		Serial.println(pushMode);

		dLCD.clear();

		if(pushMode > 2 && digitalRead(Mode) == HIGH) pushMode = 0;

		contB1 = 1;
	}

	if(digitalRead(Mode) == LOW) contB1 = 0;

	switch(pushMode) {
	    case 1: //alarma
	    	dLCD.setCursor(5, 0);
	    	dLCD.print("ALARMA");
    		dLCD.setCursor(6, 1);
    		print2D(arrAlarm[0][0]);
    		dLCD.print(":");
    		print2D(arrAlarm[1][0]);

    		setAlarm();
	      break;
	    case 2: //temporizador
	    	dLCD.setCursor(2, 0);
	    	dLCD.print("TEMPORIZADOR");
        dLCD.setCursor(4, 1);
        print2D(arrTimer[0][0]);
        dLCD.print(":");
        print2D(arrTimer[1][0]);
        dLCD.print(":");
        print2D(arrTimer[2][0]);
       
    		setTimer();
	      break;
	    /*case 3: //temperatura
	    	dLCD.setCursor(3, 0);
	    	dLCD.print("TERMOMETRO");
	      break;*/
	    default: //reloj
	    	reloj();
	    	if(alarm() && x == 0) digitalWrite(Buzzer, HIGH);
        else digitalWrite(Buzzer, LOW);
	    	break;
	}
}

bool __set__() {
	if(digitalRead(Set) == HIGH && contB2 == 0) {
		contB2 = 1;
		return true;
	}

	if(digitalRead(Set) == LOW) contB2 = 0;

	return false;
}

void setAlarm() {
	if(__set__()) setear++;

	//Serial.println(setear);

	switch(setear) {
		case 1:
			if(__up__(arrAlarm[0][0])) {
				if(arrAlarm[0][0] > 23) arrAlarm[0][0] = 0;
			}
			else if(__down__(arrAlarm[0][0])) {
				if(arrAlarm[0][0] < 0) arrAlarm[0][0] = 23;
			}
			break;
		case 2:
			if(__up__(arrAlarm[1][0])) {
				if(arrAlarm[1][0] > 59) arrAlarm[1][0] = 0;
			}
			else if(__down__(arrAlarm[1][0])) {
				if(arrAlarm[1][0] < 0) arrAlarm[1][0] = 59;
			}
			break;
	}

	if(setear > 2) setear = 0;
}

void setTimer() {
	if(__set__()) setear++;

	//Serial.println(setear);

	switch(setear) {
		case 1:
			if(__up__(arrTimer[0][0])) {
				if(arrTimer[0][0] > 23) arrTimer[0][0] = 0;
			}
			else if(__down__(arrTimer[0][0])) {
				if(arrTimer[0][0] < 0) arrTimer[0][0] = 23;
			}
			break;
		case 2:
			if(__up__(arrTimer[1][0])) {
				if(arrTimer[1][0] > 59) arrTimer[1][0] = 0;
			}
			else if(__down__(arrTimer[1][0])) {
				if(arrTimer[1][0] < 0) arrTimer[1][0] = 59;
			}
			break;
		case 3:
			if(__up__(arrTimer[2][0])) {
				if(arrTimer[2][0] > 59) arrTimer[2][0] = 0;
			}
			else if(__down__(arrTimer[2][0])) {
				if(arrTimer[2][0] < 0) arrTimer[2][0] = 59;
			}
			break;
		case 4:
			timer();
			dLCD.setCursor(4, 1);
      print2D(arrTimer[0][0]);
      dLCD.print(":");
      print2D(arrTimer[1][0]);
      dLCD.print(":");
      print2D(arrTimer[2][0]);

			if(timer() && x == 0) digitalWrite(Buzzer, HIGH);
      else digitalWrite(Buzzer, LOW);
			break;
	}

	if(setear > 4) setear = 0;
}

bool __up__(int &num) {
	if(digitalRead(Up) == HIGH && contB3 == 0) {
		num++;
		contB3 = 1;
		return true;
	}

	if(digitalRead(Up) == LOW) contB3 = 0;

	return false;
}

bool __down__(int &num) {
	if(digitalRead(Down) == HIGH && contB4 == 0) {
		num--;
		contB4 = 1;
		return true;
	}

	if(digitalRead(Down) == LOW) contB4 = 0;

	return false;
}

void reloj() {
  DateTime dWatch = RTC.now();
	printDigits(dWatch.hour());
	Serial.print(":");
	printDigits(dWatch.minute());
	Serial.print(":");
	printDigits(dWatch.second());
	Serial.print(" ");
	printDigits(dWatch.day());
	Serial.print("/");
	printDigits(dWatch.month());
	Serial.print("/");
	printDigits(dWatch.year());
	Serial.println();

	dLCD.setCursor(4, 0);
	print2D(dWatch.hour());
	dLCD.print(":");
	print2D(dWatch.minute());
	dLCD.print(":");
	print2D(dWatch.second());
	dLCD.setCursor(3, 1);
	print2D(dWatch.day());
	dLCD.print("/");
	print2D(dWatch.month());
	dLCD.print("/");
	print2D(dWatch.year());
	delay(1000);
}

bool alarm() {
	DateTime dWatch_2 = RTC.now();
  if(arrAlarm[0][0] == dWatch_2.hour() && arrAlarm[1][0] == dWatch_2.minute() && x == 0) return true;
  
 return false;
}

bool timer() {
	bool salida = false;

	while(arrTimer[0][0] >= 0) {
		while(arrTimer[1][0] >= 0) {
			while(arrTimer[2][0] >= 0) {
				dLCD.setCursor(4, 1);
        print2D(arrTimer[0][0]);
        dLCD.print(":");
        print2D(arrTimer[1][0]);
        dLCD.print(":");
        print2D(arrTimer[2][0]);
        delay(1000);
        arrTimer[2][0]--;
			}
			arrTimer[2][0] = 59;
			arrTimer[1][0]--;
		}
		arrTimer[1][0] = 59;
		arrTimer[0][0]--;
		salida = true;
	}

	if(salida) {
		arrTimer[0][0] = 0;
		arrTimer[1][0] = 0;
		arrTimer[2][0] = 0;
	}

	return true;
}

//imprimir a 2 digitos - Serial
void printDigits(int num) {
	if(num >= 0 && num < 10) {
		Serial.write('0');
	}

	Serial.print(num);
}

void print2D(int num) {
	if(num >= 0 && num < 10) {
		dLCD.print('0');
	}

	dLCD.print(num);
}
