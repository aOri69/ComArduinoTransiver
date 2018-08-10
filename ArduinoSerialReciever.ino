/*
Name:    Sketch1.ino
Created: 23.01.2018 14:09:27
Author:  sashk
*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
String bufChar;


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	bufChar.reserve(64);
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setTextWrap(true);
	display.setTextSize(2);
	//display.setCursor(0,0);
	display.println("CPU: ");
	display.println("RAM: ");
	display.display();
}

// the loop function runs over and over again until power down or reset
void loop() {
	//delay(125);
}
void serialEvent() {
	int incByte = 0;
	/*if (Serial.available() > 0)
	{*/
		incByte = Serial.read();
		Serial.println(incByte, DEC);
		if (incByte >= 48  && incByte <= 57 )
		{
			bufChar += (char)incByte;
			Serial.println(bufChar);
		}
		else
		{
			if (bufChar.length()>0)
			{
				int num = bufChar.toInt();
				//led4.display(num);
				display.setCursor(60, 0);
				//display.clearDisplay();
				display.print(num);
				display.display();
			}
			Serial.flush();
			bufChar.remove(0);
			Serial.println(bufChar);
		}
}