#! /bin/sh
gcc -shared -o wiringpi.so -fPIC wiringpi_dll.c -lwiringPi -lwiringPiDev
