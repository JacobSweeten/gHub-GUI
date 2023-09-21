#include "device.h"

#define LOGITECHVENDORID 0x046d
#define UNIDENTIFIEDPRODUCTID 0xc539

#define NUM_VALID_IDS 1
const short validIDs[] = {
	0xc332
};

libusb_context* global_context = NULL;

static const int bmRequestType = 0x21; // type and recipient: 0b00100001
static const int bRequest = 0x09; // type of request: set_report
static const int wValue = 0x0211; // report type and id: output, 0x11

int initUSB()
{
	int returnCode = libusb_init(NULL);

	#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000106) // >=1.0.22
		libusb_set_option(global_context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_ERROR);
	#else
		libusb_set_debug(global_context, LIBUSB_LOG_LEVEL_ERROR);
	#endif

	return returnCode;
}

void CloseDevice(device_t* device) {
	if (device->handle)
		libusb_close(device->handle);
}

// Detach driver from device
void DetachKernel(device_t* device) {
	if (libusb_kernel_driver_active(device->handle, device->wIndex)) {
		libusb_detach_kernel_driver(device->handle, device->wIndex);
	}
	
	int returnCode = libusb_claim_interface(device->handle, device->wIndex);

	if (returnCode < 0) {
		fprintf(stderr, "Error: Cannot claim interface: %s\n",
		libusb_error_name(returnCode));

		CloseDevice(device);
		return;
	}
}

// Attach driver to device
void AttachKernel(device_t* device) {
	libusb_release_interface(device->handle, device->wIndex);

	if (!libusb_kernel_driver_active(device->handle, device->wIndex)) {
		libusb_attach_kernel_driver(device->handle, device->wIndex);
	}
}

int openDevice(device_t* device) {
	device->handle = libusb_open_device_with_vid_pid(global_context, LOGITECHVENDORID, device->modelID);

	if(device->handle == NULL)
	{
		fprintf(stderr, "Failed to open device\n");
		return 1;
	}

	return 0;
}

bool isvalidProductID(short productID)
{
	for(int i = 0; i < NUM_VALID_IDS; i++)
	{
		if(validIDs[i] == productID)
		{
			return true;
		}
	}

	return false;
}

void populateDeviceInfo(device_t* device)
{
	switch(device->modelID)
	{
		case 0xc332:
			device->type = MOUSE;
			device->mByte3 = 0x02;
			device->model = G502;
			device->wIndex = WIRED_OR_CABLE;
	}
}

deviceList_t* getGDevices() {
	// Destination variables
	libusb_device** list;
	struct libusb_device_descriptor desc;

	// Output list
	deviceList_t* listHead = NULL;

	// Get number of USB devices
	ssize_t count = libusb_get_device_list(global_context, &list);

	for (int i = 0; i < count; i++) {
		libusb_device* device = list[i];

		int res = libusb_get_device_descriptor(device, &desc);

		if(res != 0)
		{
			fprintf(stderr, "Failed to get a device descriptor.\n");
			i++;
			continue;
		}

		if (desc.idProduct == UNIDENTIFIEDPRODUCTID) {
			printf("Found wireless logitech device, but it's UNIDENTIFIED.\n");
			printf("Consider upgrading the kernel to at least version of 5.2.\nOr use wired option of your mouse.\n\n");
			i++;
			continue;
		}

		if (LOGITECHVENDORID == desc.idVendor && isvalidProductID(desc.idProduct)) {
			deviceList_t* newDevice = (deviceList_t*)malloc(sizeof(deviceList_t));

			// Get product name
			libusb_device_handle* tempHandle = libusb_open_device_with_vid_pid(global_context, LOGITECHVENDORID, desc.idProduct);
			
			if(tempHandle == NULL)
			{
				newDevice->dev.name = "";
			}
			else
			{
				newDevice->dev.name = (char*)malloc(51);
				int len = libusb_get_string_descriptor_ascii(tempHandle, desc.iProduct, (unsigned char*)newDevice->dev.name, 50);

				if(len < 0)
				{
					newDevice->dev.name = "";
				}
			}

			libusb_close(tempHandle);

			// Populate details
			newDevice->next = NULL;
			newDevice->dev.handle = 0;
			newDevice->dev.modelID = desc.idProduct;

			populateDeviceInfo(&newDevice->dev);
			
			printf("Device id=0x%x, name=%s - has been found!\n", desc.idProduct, newDevice->dev.name);
			
			// Append to list
			if(listHead == NULL)
			{
				listHead = newDevice;
			}
			else
			{
				deviceList_t* current = listHead;

				while(current->next != NULL)
				{
					current = current->next;
				}

				current->next = listHead;
			}
		}
	}

	// Clean up
	libusb_free_device_list(list, 1);

	return listHead;
}

int setColor(device_t* device, colorType ct, char R, char G, char B)
{
	int devByte[4];

	// we dont choose what we change yet
	// devByte[0] is changed to 0x10 when we change dpi or response rate
	// devByte[3] is changing as well
	devByte[0] = 0x11;
	devByte[2] = device->mByte3;
	devByte[3] = 0x3b;
	// exclusive option for logitech pro wireless
	switch (device->wIndex) {
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

	/*
	if (needed_id == 0xc088) {
		device->wIndex = 2;
		devByte[1] = 0xff;
	}
	*/

	int type = 0x01; // static

	int source;

	switch(ct)
	{
		case PRIMARY:
			source = 1;
			break;
		case SECONDARY:
			source = 0;
			break;
		default:
			source = 0;
	}

	unsigned char data[20] = {devByte[0], devByte[1], devByte[2], devByte[3], source, type, R, G, B, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	/*  detach kernel
		&
		claim the interface on a given device handle */
	DetachKernel(device);

	int returnCode = libusb_control_transfer(device->handle, bmRequestType, bRequest, wValue,
										 device->wIndex, data, sizeof(data), 2000);
	if (returnCode < 0) {
		fprintf(stderr, "Error: Cannot transfer control data: %s\n", libusb_error_name(returnCode));
	}

	/*  release the interface previously claimed
		&
		attach kernel */
	AttachKernel(device);

	return 0;
}

int setPrimaryColor(device_t* device, char R, char G, char B)
{
	return setColor(device, PRIMARY, R, G, B);
}

int setSecondaryColor(device_t* device, char R, char G, char B)
{
	return setColor(device, SECONDARY, R, G, B);
}

void printDevice(device_t device)
{
	printf("Device name: %s\n", device.name);
	printf("\tProduct ID: %i\n\tVendor ID: %i\n\tmByte3 (\?\?\?): %i\n\twIndex: %i\n", device.modelID, LOGITECHVENDORID, device.mByte3, device.wIndex);
}
