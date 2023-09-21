#ifndef DEVICE_H
#define DEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// USB lib
#include <libusb-1.0/libusb.h>

typedef enum deviceType {KEYBOARD, MOUSE} deviceType;
typedef enum deviceModel {G502} deviceModel;
typedef enum colorType {PRIMARY, SECONDARY} colorType;

#define WIRED_OR_CABLE 			1
#define WIRELESS_RECEIVER 		2
#define CABLE_BUT_SECOND		2

typedef struct device_t {
	libusb_device_handle* handle;
	char* name;
	deviceType type;
	deviceModel model;
	int modelID;
	int wIndex;
	uint8_t mByte3;
} device_t;

typedef struct deviceList_t deviceList_t;

typedef struct deviceList_t {
	device_t dev;
	deviceList_t* next;
} deviceList_t;

int initUSB();
int setPrimaryColor(device_t*, char, char, char);
int setSecondaryColor(device_t*, char, char, char);

void CloseDevice(device_t*);
int openDevice(device_t*);
deviceList_t* getGDevices(void);

void printDevice(device_t);

#endif