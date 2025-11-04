#include "PmodOLED.h"
#include "PmodNAV.h"
#include "xparameters.h"

#define OLED_SPI_ADDR XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR
#define OLED_GPIO_ADDR XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR
#define NAV_SPI_ADDR XPAR_PMODNAV_0_AXI_LITE_SPI_BASEADDR
#define NAV_GPIO_ADDR XPAR_PMODNAV_0_AXI_LITE_GPIO_BASEADDR

// To change between PmodOLED and OnBoardOLED is to change Orientation
const u8 orientation = 0x0; // Set up for Normal PmodOLED(false) vs normal
                            // Onboard OLED(true)
const u8 invert = 0x0; // true = whitebackground/black letters
                       // false = black background /white letters
//OLED and NAV Instance
PmodOLED OLEDInst;
PmodNAV NAVInst;
//Buffer for displaying data to OLED
static char dataOut[50];


int main() {
	//Initialize OLED
	OLED_Begin(&OLEDInst, OLED_GPIO_ADDR, OLED_SPI_ADDR, orientation, invert);
	//Initialize NAV and all sensors
	NAV_Begin(&NAVInst, NAV_GPIO_ADDR, NAV_SPI_ADDR);
	NAV_Init(&NAVInst);
	//Data polling/output loop
	while(1) {
		NAV_GetData(&NAVInst);
		//Not enough space on the OLED to get the Z coordinate, but this is only a test
		OLED_SetCursor(&OLEDInst, 0, 0);
		snprintf(dataOut, sizeof(dataOut), "A:%.2f %.2f", NAVInst.acclData.X, NAVInst.acclData.Y);
		OLED_PutString(&OLEDInst, dataOut);
		OLED_SetCursor(&OLEDInst, 0, 1);
		snprintf(dataOut, sizeof(dataOut), "G:%.2f %.2f", NAVInst.gyroData.X, NAVInst.gyroData.Y);
		OLED_PutString(&OLEDInst, dataOut);
		OLED_SetCursor(&OLEDInst, 0, 2);
		snprintf(dataOut, sizeof(dataOut), "M:%.2f %.2f", NAVInst.magData.X, NAVInst.magData.Y);
		OLED_PutString(&OLEDInst, dataOut);
		OLED_SetCursor(&OLEDInst, 0, 3);
		snprintf(dataOut, sizeof(dataOut), "P:%.2f T:%.1f", NAVInst.hPa, NAVInst.tempC);
		OLED_PutString(&OLEDInst, dataOut);
		//Push data to the display
		OLED_Update(&OLEDInst);
		//Set for a 50hz data refresh rate
		NAV_usleep(&NAVInst, 20000);
	}
}
