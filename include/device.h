#ifndef DEVICE_H
#define DEVICE_H

#include <stdio.h>

// USB lib
#include <libusb-1.0/libusb.h>

// Local includes
#include "mouselist.h"

typedef enum deviceType {KEYBOARD, MOUSE} deviceType;
typedef enum deviceModel {G502} deviceModel;
typedef enum colorType {PRIMARY, SECONDARY} colorType;

typedef struct device_t {
	libusb_device_handle* handle;
	char* name;
	deviceType type;
	deviceModel model;
	int wIndex;
	uint8_t mByte3;
} device_t;

int initUSB();
int setColor(device_t*, colorType, char, char, char);
int setPrimaryColor(device_t*, char, char, char);
int setSecondaryColor(device_t*, char, char, char);

void CloseDevice(device_t*);
void DetachKernel(device_t*);
void AttachKernel(device_t*);
int openDevice(device_t*, Item*);
int getDevice(Item*);

#endif