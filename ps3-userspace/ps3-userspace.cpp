

#include <iostream>
#include <stdlib.h>
#include <libusb.h>

#define PS3_VID 0x054c
#define PS3_PID 0x0268

#define SUCCESS 0
#define INITFAILURE 1
#define MISSINGDEVICEFAILURE 2

int main()
{
    libusb_device_handle* dHandle = NULL;
    std::cout << "Hello World!\n";

    libusb_context* ctx = NULL;
    int libusbRet;

    libusbRet = libusb_init(&ctx);
    if (libusbRet < 0) {
        std::cout << "Failed to initialise libusb: " << libusb_error_name(libusbRet) << std::endl;
        return INITFAILURE;
    }

    dHandle = libusb_open_device_with_vid_pid(ctx, PS3_VID, PS3_PID);
    
    if (dHandle == NULL) {
        std::cout << "PS3 Controller not found." << std::endl;
        libusb_exit(ctx);
        return MISSINGDEVICEFAILURE;

    }
	std::cout << "PS3 Controller found!" << std::endl;

    libusb_close(dHandle);
    libusb_exit(ctx);
    return 0;   

}


