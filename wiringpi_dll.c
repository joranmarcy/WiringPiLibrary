/**********************************************************************
 * Filename    : Blink.c
 * Description : Basic usage of GPIO. Let led blink.
 * auther      : www.freenove.com
 * modification: 2019/12/26
 **********************************************************************/
#include <wiringPi.h>
#include <stdio.h>
#include <wiringPiI2C.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>
#include <wiringShift.h>

#include "wiringpi_dll.h"

#define dataPin 21	// DS Pin of 74HC595(GPIO5)
#define latchPin 22 // ST_CP Pin of 74HC595(GPIO6)
#define clockPin 23 // CH_CP Pin of 74HC595(GPIO13)

int pcf8574_address = 0x27; // PCF8574T:0x27, PCF8574AT:0x3F
#define BASE 64				// BASE any number above 64
// Define the output pins of the PCF8574, which are directly connected to the LCD1602 pin.
#define RS BASE + 0
#define RW BASE + 1
#define EN BASE + 2
#define LED BASE + 3
#define D4 BASE + 4
#define D5 BASE + 5
#define D6 BASE + 6
#define D7 BASE + 7

int lcdhd; // used to handle LCD

int init = 0;

int detectI2C(int addr)
{
	int _fd = wiringPiI2CSetup(addr);
	if (_fd < 0)
	{
		printf("Error address : 0x%x \n", addr);
		return 0;
	}
	else
	{
		if (wiringPiI2CWrite(_fd, 0) < 0)
		{
			printf("Not found device in address 0x%x \n", addr);
			return 0;
		}
		else
		{
			printf("Found device in address 0x%x \n", addr);
			return 1;
		}
	}
}

void shiftOut(int dPin, int cPin, int order, int val)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		digitalWrite(cPin, LOW);
		if (order == LSBFIRST)
		{
			digitalWrite(dPin, ((0x01 & (val >> i)) == 0x01) ? LOW : HIGH);
			delayMicroseconds(10);
		}
		else
		{ // if(order == MSBFIRST){
			digitalWrite(dPin, ((0x80 & (val << i)) == 0x80) ? LOW : HIGH);
			delayMicroseconds(10);
		}
		digitalWrite(cPin, HIGH);
		delayMicroseconds(10);
	}
}

unsigned char percentToChar(int percentValue)
{
	if (percentValue == 0)
	{
		return 0x00;
	}
	else
	{
		unsigned char x = 0x01;
		int index = percentValue / 14;
		for (int i = 0; i < index; i++)
		{
			x = x | (x << 1);
		}
		return x;
	}
}

void Initialize()
{
	if (init == 0)
	{
		wiringPiSetup();

		int i;

		printf("Program is starting ...\n");

		wiringPiSetup();

		// 74HC595
		pinMode(dataPin, OUTPUT);
		pinMode(latchPin, OUTPUT);
		pinMode(clockPin, OUTPUT);

		if (detectI2C(0x27))
		{
			pcf8574_address = 0x27;
		}
		else if (detectI2C(0x3F))
		{
			pcf8574_address = 0x3F;
		}
		else
		{
			printf("No correct I2C address found, \n"
				   "Please use command 'i2cdetect -y 1' to check the I2C address! \n"
				   "Program Exit. \n");
		}
		pcf8574Setup(BASE, pcf8574_address); // initialize PCF8574
		for (i = 0; i < 8; i++)
		{
			pinMode(BASE + i, OUTPUT); // set PCF8574 port to output mode
		}
		digitalWrite(LED, HIGH); // turn on LCD backlight
		digitalWrite(RW, LOW);	 // allow writing to LCD

		lcdhd = lcdInit(2, 16, 4, RS, EN, D4, D5, D6, D7, 0, 0, 0, 0); // initialize LCD and return “handle” used to handle LCD

		if (lcdhd == -1)
		{
			printf("lcdInit failed !");
		}

		init++;
	}
}

void turnLedOn(int ledPin)
{
	Initialize();
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
}

void turnLedOff(int ledPin)
{
	Initialize();
	pinMode(ledPin, LOW);
	digitalWrite(ledPin, LOW);
}

void printLCD(char *arr, int line)
{
	Initialize();
	lcdPosition(lcdhd, 0, line); // set the LCD cursor position to (0,0)
	lcdPrintf(lcdhd, arr);		 // Display  on LCD
}

void ledBarPercent(int percent)
{
	digitalWrite(latchPin, LOW);									// Output low level to latchPin
	shiftOut(dataPin, clockPin, LSBFIRST, percentToChar(percent)); // Send serial data to 74HC595
	digitalWrite(latchPin, HIGH);									// Output high level to latchPin, and 74HC595 will update the data to the parallel output port.			
}
