#include "device.h"

libusb_context* global_context;

int initUSB()
{
	returnCode = libusb_init(NULL);

	#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000106) // >=1.0.22
		libusb_set_option(global_context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_ERROR);
	#else
		libusb_set_debug(global_context, LIBUSB_LOG_LEVEL_ERROR);
	#endif

    return returnCode;
}

void CloseDevice(dev_t* device) {
	if (device->handle)
		libusb_close(device->handle);
}

// Detach driver from device
void DetachKernel(dev_t* device) {
	if (libusb_kernel_driver_active(device->handle, wIndex)) {
		libusb_detach_kernel_driver(device->handle, wIndex);
	}

	returnCode = libusb_claim_interface(device->handle, wIndex);

	if (returnCode < 0) {
		fprintf(stderr, "Error: Cannot claim interface: %s\n",
		libusb_error_name(returnCode));

		CloseDevice();
		return;
	}
}

// Attach driver to device
void AttachKernel(dev_t* device) {
	libusb_release_interface(device->handle, wIndex);

	if (!libusb_kernel_driver_active(device->handle, wIndex)) {
		libusb_attach_kernel_driver(device->handle, wIndex);
	}
}

int openDevice(dev_t* device) {
	// Get number of devices available
	const int available = getSize(available_head);

	// Ask the user which one
	int choice;

	printf("\nChoose what device you would like to operate on. Available devices:\n");
	printAllItems(available_head);
	printf("Enter [0] to exit.\n");

	// Loop until proper input is given
	while(1)
	{
		choice = getchar() - '0';
		if ((choice < 0) || (choice > available)) {
			printf("Choose correct number or exit!\n");
			fflush(stdin);
		} else if (choice == 0) {
			printf("Exiting...\n");
			fflush(stdin);
			return 2;
		}
		else
		{
			break;
		}
	}

	// After selection, set the color

	// Get the ID and name
	const int needed_id = getNthId(available_head, choice);
	const char* temp_name = getName(available_head, needed_id);

	//open device
	devh = libusb_open_device_with_vid_pid(NULL, ID_VENDOR, needed_id);
	if (!devh) {
		fprintf(stderr, "Error: Cannot open %s\n", temp_name);
		return -1;
	}
	printf("\nDevice %s is operating...\n", temp_name);

	//process
	srand((unsigned)time(NULL));
	wIndex = getInterface(available_head, needed_id);
	int devByte[4];

	// we dont choose what we change yet
	// devByte[0] is changed to 0x10 when we change dpi or response rate
	// devByte[3] is changing as well
	devByte[0] = 0x11;
	devByte[2] = getByte3(available_head, needed_id);
	devByte[3] = 0x3b;
	// exclusive option for logitech pro wireless
	switch (wIndex) {
		case 1:
			devByte[1] = 0xff;
			break;
		case 2:
			devByte[1] = 0x01;
			break;
		default:
			printf("Error: Wrong interface!\n");
			return -1;
	}

	if (needed_id == 0xc088) {
		wIndex = 2;
		devByte[1] = 0xff;
	}

	uint32_t random = (uint32_t)rand();

	type = 0x01; // static
	source = random & 0x01; // 0 - primary, 1 - logo
	R = 0xFF;
	G = 0x66;
	B = 0x00;

	unsigned char data[20] = {devByte[0], devByte[1], devByte[2], devByte[3], source, type, R, G, B, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	/*  detach kernel
		&
		claim the interface on a given device handle */
	DetachKernel();
	returnCode = libusb_control_transfer(devh, bmRequestType, bRequest, wValue,
										 wIndex, data, sizeof(data), 2000);
	if (returnCode < 0) {
		fprintf(stderr, "Error: Cannot transfer control data: %s\n", libusb_error_name(returnCode));
	}

	/*  release the interface previously claimed
		&
		attach kernel */
	AttachKernel();
	
	if (devh)
		libusb_close(devh);

	return EXIT_SUCCESS;
}

int getDevice(Item* head) {
	libusb_device **list;
	struct libusb_device_descriptor desc;

	int i;
	ssize_t count = libusb_get_device_list(global_context, &list);

	for (i = 0; i < count; ++i) {
		libusb_device *device = list[i];

		if (!libusb_get_device_descriptor(device, &desc)) {
			if (desc.idProduct == ID_PRODUCT_UNIDENTIFIED) {
				printf("Found wireless logitech device, but it's UNIDENTIFIED.\n");
				printf("Consider upgrading the kernel to at least version of 5.2.\nOr use wired option of your mouse.\n\n");
				continue;
			}

			if (ID_VENDOR == desc.idVendor && searchItem(head, desc.idProduct)) {
				const char* temp_name = getName(head, desc.idProduct);
				const int temp_interface = getInterface(head, desc.idProduct);
				const int temp_byte3 = getByte3(head, desc.idProduct);

				pushItem(&available_head, desc.idProduct, temp_name, temp_interface, temp_byte3);
				printf("\nDevice id=0x%x, name=%s, interface=%x - has been found!\n", desc.idProduct, temp_name, temp_interface);
				found++;
			}
		}
	}
	if (!found) return found;
	libusb_free_device_list(list, 1);

	return 1;
}

int setPrimaryColor(dev_t* deviceHandle, char R, char G, char B)
{

}

int setSecondaryColor(dev_t* deviceHandle, char R, char G, char B)
{

}