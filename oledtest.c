#include "PmodOLED.h"
#include "xparameters.h"

#define OLED_SPI_ADDR XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR
#define OLED_GPIO_ADDR XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR

// To change between PmodOLED and OnBoardOLED is to change Orientation
const u8 orientation = 0x0; // Set up for Normal PmodOLED(false) vs normal
                            // Onboard OLED(true)
const u8 invert = 0x0; // true = whitebackground/black letters
                       // false = black background /white letters
//OLED Instance
PmodOLED OLEDInst;

int main() {
	OLED_Begin(&OLEDInst, OLED_GPIO_ADDR, OLED_SPI_ADDR, orientation, invert);
	//Disables automatic updating
	OLED_SetCharUpdate(&myDevice, 0);
	OLED_SetCursor(&OLEDInst, 0, 0);
	OLED_PutString(&OLEDInst, "PmodOLED");
	OLED_SetCursor(&OLEDInst, 0, 1);
	OLED_PutString(&OLEDInst, "test");
	OLED_SetCursor(&OLEDInst, 0, 2);
	OLED_PutString(&OLEDInst, "successful!");
	OLED_End(&OLEDInst);
	return 0;
}
