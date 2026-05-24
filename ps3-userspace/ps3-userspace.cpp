

#include <iostream>
#include <stdlib.h>
#include <libusb.h>
#include "ps3-controller.h" 

#define PS3_VID 0x054c
#define PS3_PID 0x0268


#define SUCCESS 0
#define INITFAILURE 1
#define MISSINGDEVICEFAILURE 2
#define INTERERROR 3
#define PSINITERROR 4

int main()
{
    unsigned char buf[64];
    int actual_length;
    unsigned char INITPACKET[4] = { 0x42, 0x0C, 0x00, 0x00 };
    libusb_device_handle* dHandle = NULL;
    PS3State state;
	initPS3State(&state);
    PS3State previousState = state;
    libusb_context* ctx = NULL;
    int libusbRet;
	int LEDFlag = 0;



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

	libusbRet = libusb_claim_interface(dHandle, 0);

	if (libusbRet < 0) {
		std::cout << "Failed to claim interface: " << libusb_error_name(libusbRet) << std::endl;
		libusb_close(dHandle);
		libusb_exit(ctx);
		return INTERERROR;
	}

    libusbRet = libusb_control_transfer(
        dHandle,
        0x21,
        0x09,
        0x03F4,
        0,
        INITPACKET,
        sizeof(INITPACKET),
        1000
    );

	if (libusbRet < 0) {
		std::cout << "Failed to send control transfer: " << libusb_error_name(libusbRet) << std::endl;
		libusb_release_interface(dHandle, 0);
		libusb_close(dHandle);
		libusb_exit(ctx);
		return PSINITERROR;
	}

	std::cout << "Initialization packet sent successfully!" << std::endl;

    

	std::cout << "Listening for input..." << std::endl << "Press Ctrl+C to exit." << std::endl;

    while (1) {
		libusbRet = libusb_interrupt_transfer(
            dHandle, 
            0x81,
            buf, 
            sizeof(buf), 
            &actual_length, 
            1000);
        if (libusbRet < 0) {
            std::cout << "Failed to receive interrupt transfer: " << libusb_error_name(libusbRet) << std::endl;
        }
	
        if (!LEDFlag) {
            libusbRet = setLED(dHandle, DS3_LED_1 | DS3_LED_3);
            printf("LED result: %d\n", libusbRet);
            LEDFlag = 1;
        }
        parseReport(buf, &state);
        
        if (!compareStates(&previousState, &state)) {
            printf("Report: ");
            printState(&state);
            std::cout << std::dec << std::endl;
        }
        previousState = state;
        sendRumble(dHandle, 0xFF, 0x00); 
    }

 
    sendRumble(dHandle, 0x00, 0x00);
    libusb_release_interface(dHandle, 0);
    libusb_close(dHandle);
    libusb_exit(ctx);
    return 0;   

}


