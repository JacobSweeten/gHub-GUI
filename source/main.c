// Copyright 2020 Mikhail ysph Subbotin
// Modified by Jacob Sweeten

// C includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

// USB lib
#include <libusb-1.0/libusb.h>

// Local includes
#include "device.h"
#include "window.h"

#define LIBUSB_OPTION_LOG_LEVEL	0
#define LIBUSB_LOG_LEVEL_ERROR	1

//temporary
int temp_id;

int main(void) {
	int returnCode = initUSB();
	if(returnCode < 0)
	{
		fprintf(stderr, "Error: Cannot initialize libusb. %s\n", libusb_error_name(returnCode));
		return 1;
	}

	deviceList_t* deviceList = getGDevices();

	deviceList_t* current = deviceList;

	while(current != NULL)
	{
		printDevice(current->dev);
		openDevice(&current->dev);
		setPrimaryColor(&current->dev, 0xFF, 0x66, 0x00);
		setSecondaryColor(&current->dev, 0xFF, 0x66, 0x00);
		closeDevice(&current->dev);
		current = current->next;
	}

	libusb_exit(NULL);

	initDisplay();

	return EXIT_SUCCESS;
}
