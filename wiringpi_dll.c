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
#include "wiringpi_dll.h"

int pcf8574_address = 0x27; // PCF8574T:0x27, PCF8574AT:0x3F
#define BASE 64				// BASE any number above 64
//Define the output pins of the PCF8574, which are directly connected to the LCD1602 pin.
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

void Initialize()
{
	if (init == 0)
	{
		wiringPiSetup();

		int i;

		printf("Program is starting ...\n");

		wiringPiSetup();

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
		pcf8574Setup(BASE, pcf8574_address); //initialize PCF8574
		for (i = 0; i < 8; i++)
		{
			pinMode(BASE + i, OUTPUT); //set PCF8574 port to output mode
		}
		digitalWrite(LED, HIGH);									   //turn on LCD backlight
		digitalWrite(RW, LOW);										   //allow writing to LCD

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
	lcdPosition(lcdhd, 0, line);	 // set the LCD cursor position to (0,0)
	lcdPrintf(lcdhd, arr); // Display  on LCD
}

