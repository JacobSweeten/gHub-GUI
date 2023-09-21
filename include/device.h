#pragma once

#include <stdio.h>

// USB lib
#include <libusb-1.0/libusb.h>

#include "mouselist.h"

enum deviceType {KEYBOARD, MOUSE};
enum deviceModel {G502};

typedef struct dev_s {
    libusb_device_handle* handle;
    char* name;
    deviceType type;
    deviceModel model;
} dev_t;

int initUSB();
int setPrimaryColor(dev_t*, char, char, char);
int setSecondaryColor(dev_t*, char, char, char);