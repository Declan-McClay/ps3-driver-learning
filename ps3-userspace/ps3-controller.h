#pragma once
#include <libusb.h>

#define DS3_OUTPUT_REPORT_SIZE 49
#define DS3_LED_1   0x02
#define DS3_LED_2   0x04
#define DS3_LED_3   0x08
#define DS3_LED_4   0x10
#define DS3_LED_OFF 0x20


struct PS3State {
    int cross, circle, square, triangle;
    int l1, r1, l2, r2;
    int l3, r3;
    int up, down, left, right;
    int start, select, ps;

    float left_x, left_y;
    float right_x, right_y;
} ;



static const unsigned char DS3_DEFAULT_OUTPUT_REPORT[DS3_OUTPUT_REPORT_SIZE] = {
    0x01,
    0x00, 0xFF, 0x00, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0xFF, 0x27, 0x10, 0x00, 0x32,
    0xFF, 0x27, 0x10, 0x00, 0x32,
    0xFF, 0x27, 0x10, 0x00, 0x32,
    0xFF, 0x27, 0x10, 0x00, 0x32,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};



void initPS3State(PS3State* state);
float normaliseAxis(unsigned char raw);
void parseReport(unsigned char* buf, PS3State* state);
void printState(PS3State* state);
bool compareStates(PS3State* prev, PS3State* curr);
int setLED(libusb_device_handle* dHandle, unsigned char ledFlags);
int sendRumble(libusb_device_handle* dHandle, unsigned char largeRumble, unsigned char smallRumble);

