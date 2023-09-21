// Copyright 2020 Mikhail ysph Subbotin
// Modified by Jacob Sweeten

// C includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

// X11 lib (for GUI)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

// USB lib
#include <libusb-1.0/libusb.h>

// Local includes
#include "mouselist.h"
#include "device.h"

#define LIBUSB_OPTION_LOG_LEVEL	0
#define LIBUSB_LOG_LEVEL_ERROR	1

Item* available_head; // the list contains available devices

//temporary
int temp_id;

int main(void) {
	int returnCode = initUSB();
	if(returnCode < 0)
	{
		fprintf(stderr, "Error: Cannot initialize libusb. %s\n", libusb_error_name(returnCode));
		return 1;
	}

	// add known devices
	Item* head = (Item*)malloc(size_of_Item);
	head->next = NULL;
	pushItem(&head, 0xc092, "G102-G203 LIGHTSYNC", WIRED_OR_CABLE, 0x0e);
	pushItem(&head, 0xc084, "G203 Prodigy", WIRED_OR_CABLE, 0x0e);
	pushItem(&head, 0xc083, "G403 Prodigy", WIRED_OR_CABLE, 0x0e);
	//pushItem(&head, 0xc07f, "G302 Daedalus Prime", WIRED_OR_CABLE,);
	//pushItem(&head, 0xc080, "G303 Daedalus Apex", WIRED_OR_CABLE,);
	//pushItem(&head, 0x4074, "G305 Lightspeed Wireless", WIRELESS_RECEIVER,);
	//pushItem(&head, 0xc07e, "G402 Hyperion Fury", WIRED_OR_CABLE,);
	//pushItem(&head, 0xc08f, "G403 Hero", WIRED_OR_CABLE);
	pushItem(&head, 0xc082, "G403 Wireless", WIRED_OR_CABLE, 0x18);
	pushItem(&head, 0x405d, "G403 Wireless", WIRELESS_RECEIVER, 0x18);
	//pushItem(&head, 0xc07d, "G502 Proteus Core", WIRED_OR_CABLE, 0x02);
	//pushItem(&head, 0xc08b, "G502 Hero", WIRED_OR_CABLE);
	pushItem(&head, 0xc332, "G502 Proteus Spectrum", WIRED_OR_CABLE, 0x02);
	//pushItem(&head, 0xc08d, "G502 Lightspeed Wireless", WIRED_OR_CABLE);
	//pushItem(&head, 0x407f, "G502 Lightspeed Wireless", WIRELESS_RECEIVER);
	//pushItem(&head, 0xc08e, "MX518", WIRED_OR_CABLE);
	//pushItem(&head, 0xc24a, "G600 MMO", WIRED_OR_CABLE);
	//pushItem(&head, 0xc537, "G602 Wireless", WIRELESS_RECEIVER);
	//pushItem(&head, 0x406c, "G603 Lightspeed Wireless", WIRELESS_RECEIVER);
	//pushItem(&head, 0xb024, "G604 Lightspeed Wireless", WIRED_OR_CABLE);
	//pushItem(&head, 0x4085, "G604 Lightspeed Wireless", WIRELESS_RECEIVER);
	pushItem(&head, 0xc087, "G703 Lightspeed Wireless", WIRED_OR_CABLE, 0x18);
	pushItem(&head, 0x4070, "G703 Lightspeed Wireless", WIRELESS_RECEIVER, 0x18);
	//pushItem(&head, 0xc090, "G703 Lightspeed Hero Wireless", WIRED_OR_CABLE);
	//pushItem(&head, 0x4086, "G703 Lightspeed Hero Wireless", WIRELESS_RECEIVER);
	//pushItem(&head, 0xc081, "G900 Chaos Spectrum Wireless", WIRED_OR_CABLE);
	//pushItem(&head, 0x4053, "G900 Chaos Spectrum Wireless", WIRELESS_RECEIVER);
	//pushItem(&head, 0xc086, "G903 Lightspeed Wireless", WIRED_OR_CABLE);
	//pushItem(&head, 0x4067, "G903 Lightspeed Wireless", WIRELESS_RECEIVER);
	//pushItem(&head, 0xc091, "G903 Lightspeed Hero Wireless", WIRED_OR_CABLE);
	//pushItem(&head, 0x4087, "G903 Lightspeed Hero Wireless", WIRELESS_RECEIVER);
	//pushItem(&head, 0xc085, "PRO", WIRED_OR_CABLE);
	//pushItem(&head, 0xc08c, "PRO HERO", WIRED_OR_CABLE);
	pushItem(&head, 0xc088, "PRO Wireless", WIRED_OR_CABLE, 0x07);
	pushItem(&head, 0x4079, "PRO Wireless", WIRELESS_RECEIVER, 0x07);

	// list for available devices
	available_head = (Item*)malloc(size_of_Item);
	available_head->next = NULL;

	device_t device;

	// find device
	returnCode = getDevice(head);
	if (!returnCode) {
		fprintf(stderr, "Error: Cannot find any logitech mouse. %s\n", libusb_error_name(returnCode));
		CloseDevice(&device);

		return returnCode;
	}

	returnCode = openDevice(&device, head);
	if (returnCode == 2) {
		deleteLinkedList(&head);
		deleteLinkedList(&available_head);
		CloseDevice(&device);
		return EXIT_SUCCESS;
	}
	if (returnCode < 0) {
		fprintf(stderr, "Error: Cannot operate logitech mouse. %s\n", libusb_error_name(returnCode));
		CloseDevice(&device);
		return EXIT_FAILURE;
	}


	deleteLinkedList(&head);
	deleteLinkedList(&available_head);

	libusb_exit(NULL);

	return EXIT_SUCCESS;
}
