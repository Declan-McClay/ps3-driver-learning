#include "ps3-controller.h"
#include <cmath>
#include <iostream>

#define DEADZONE 10

void initPS3State(PS3State* state) {
    state->cross = 0;
    state->circle = 0;
    state->square = 0;
    state->triangle = 0;
    state->l1 = 0;
    state->r1 = 0;
    state->l2 = 0;
    state->r2 = 0;
    state->l3 = 0;
    state->r3 = 0;
    state->up = 0;
    state->down = 0;
    state->left = 0;
    state->right = 0;
    state->start = 0;
    state->select = 0;
    state->ps = 0;
    state->left_x = 0.0f;
    state->left_y = 0.0f;
    state->right_x = 0.0f;
    state->right_y = 0.0f;
}

float normaliseAxis(unsigned char raw)
{
	int centredRaw = (int)raw - 128;
	if (abs(centredRaw) < DEADZONE) {
		return 0.0f;
	}
	return centredRaw / 128.0f;
}	

void parseReport(unsigned char* buf, PS3State* state) {
    state->select = (buf[2] & 0x01) != 0;
    state->l3 = (buf[2] & 0x02) != 0;
    state->r3 = (buf[2] & 0x04) != 0;
    state->start = (buf[2] & 0x08) != 0;
    state->up = (buf[2] & 0x10) != 0;
    state->right = (buf[2] & 0x20) != 0;
    state->down = (buf[2] & 0x40) != 0;
    state->left = (buf[2] & 0x80) != 0;

    state->l2 = (buf[3] & 0x01) != 0;
    state->r2 = (buf[3] & 0x02) != 0;
    state->l1 = (buf[3] & 0x04) != 0;
    state->r1 = (buf[3] & 0x08) != 0;
    state->triangle = (buf[3] & 0x10) != 0;
    state->circle = (buf[3] & 0x20) != 0;
    state->cross = (buf[3] & 0x40) != 0;
    state->square = (buf[3] & 0x80) != 0;

	state->ps = (buf[4] & 0x01) != 0;

    state->left_x = normaliseAxis(buf[6]);
    state->left_y = -normaliseAxis(buf[7]);
    state->right_x = normaliseAxis(buf[8]);
    state->right_y = -normaliseAxis(buf[9]);
}

void printState(PS3State* state) {
	std::cout << "Buttons: "
		<< (state->cross ? "Cross " : "")
		<< (state->circle ? "Circle " : "")
		<< (state->square ? "Square " : "")
		<< (state->triangle ? "Triangle " : "")
		<< (state->l1 ? "L1 " : "")
		<< (state->r1 ? "R1 " : "")
		<< (state->l2 ? "L2 " : "")
		<< (state->r2 ? "R2 " : "")
		<< (state->l3 ? "L3 " : "")
		<< (state->r3 ? "R3 " : "")
		<< (state->up ? "Up " : "")
		<< (state->down ? "Down " : "")
		<< (state->left ? "Left " : "")
		<< (state->right ? "Right " : "")
		<< (state->start ? "Start " : "")
		<< (state->select ? "Select " : "")
		<< (state->ps ? "PS" : "")
		<< std::endl;
	std::cout << std::fixed;
	std::cout.precision(2);
	std::cout << "Left Stick: (" << state->left_x << ", " << state->left_y << ")" << std::endl;
	std::cout << "Right Stick: (" << state->right_x << ", " << state->right_y << ")" << std::endl;
}

bool compareStates(PS3State* prev, PS3State* curr) {
	return (prev->cross == curr->cross) &&
		(prev->circle == curr->circle) &&
		(prev->square == curr->square) &&
		(prev->triangle == curr->triangle) &&
		(prev->l1 == curr->l1) &&
		(prev->r1 == curr->r1) &&
		(prev->l2 == curr->l2) &&
		(prev->r2 == curr->r2) &&
		(prev->l3 == curr->l3) &&
		(prev->r3 == curr->r3) &&
		(prev->up == curr->up) &&
		(prev->down == curr->down) &&
		(prev->left == curr->left) &&
		(prev->right == curr->right) &&
		(prev->start == curr->start) &&
		(prev->select == curr->select) &&
		(prev->ps == curr->ps) &&
		(std::abs(prev->left_x - curr->left_x) < 0.01f) &&
		(std::abs(prev->left_y - curr->left_y) < 0.01f) &&
		(std::abs(prev->right_x - curr->right_x) < 0.01f) &&
		(std::abs(prev->right_y - curr->right_y) < 0.01f);
}

int setLED(libusb_device_handle* dHandle, unsigned char ledFlags) {
    unsigned char buf[DS3_OUTPUT_REPORT_SIZE];
    memcpy(buf, DS3_DEFAULT_OUTPUT_REPORT, DS3_OUTPUT_REPORT_SIZE);
    buf[10] = ledFlags;

    int transferred;
    return libusb_interrupt_transfer(
        dHandle,
        0x02,
        buf,
        DS3_OUTPUT_REPORT_SIZE,
        &transferred,
        1000
    );
}

int sendRumble(libusb_device_handle* dHandle, unsigned char largeRumble, unsigned char smallRumble) {
    unsigned char buf[DS3_OUTPUT_REPORT_SIZE];
    memcpy(buf, DS3_DEFAULT_OUTPUT_REPORT, DS3_OUTPUT_REPORT_SIZE);

    buf[2] = smallRumble > 0 ? 0xFF : 0x00;  
    buf[3] = smallRumble > 0 ? 0x01 : 0x00;  
    buf[4] = largeRumble > 0 ? 0xFF : 0x00;  
    buf[5] = largeRumble;

    int transferred;
    return libusb_interrupt_transfer(
        dHandle,
        0x02,
        buf,
        DS3_OUTPUT_REPORT_SIZE,
        &transferred,
        1000
    );
}