#include <stdio.h>
#include "PmodOLED.h"
#include "PmodNAV.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

#define OLED_SPI_ADDR XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR
#define OLED_GPIO_ADDR XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR
#define NAV_SPI_ADDR XPAR_PMODNAV_0_AXI_LITE_SPI_BASEADDR
#define NAV_GPIO_ADDR XPAR_PMODNAV_0_AXI_LITE_GPIO_BASEADDR
#define INTC_DEVICE_ID 		XPAR_PS7_SCUGIC_0_DEVICE_ID
#define TMRCTR_DEVICE_ID		XPAR_TMRCTR_0_DEVICE_ID
#define INTC_TMR_INTERRUPT_ID XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR
#define TMR_LOAD			0xFFFF0000

// To change between PmodOLED and OnBoardOLED is to change Orientation
const u8 orientation = 0x0; // Set up for Normal PmodOLED(false) vs normal
                            // Onboard OLED(true)
const u8 invert = 0x0; // true = whitebackground/black letters
                       // false = black background /white letters
//OLED and NAV Instance
PmodOLED OLEDInst;
PmodNAV NAVInst;
XScuGic INTCInst;
XTmrCtr TMRInst;
//Buffer for displaying data to OLED
static char dataOut[50];
static float fahren;
volatile int updateFlag;
//Function Prototypes
static int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int IntcInitFunction(u16 DeviceId, XTmrCtr *TmrInstancePtr);
static void TMR_Intr_Handler(void *CallBackRef, u8 TmrCtrNumber);
static void celsToFah(float tempC);

int main() {
	int status;
	//Initialize OLED
	OLED_Begin(&OLEDInst, OLED_GPIO_ADDR, OLED_SPI_ADDR, orientation, invert);
	//Initialize NAV and all sensors
	NAV_begin(&NAVInst, NAV_GPIO_ADDR, NAV_SPI_ADDR);
	NAV_Init(&NAVInst);
	status = XTmrCtr_Initialize(&TMRInst, TMRCTR_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;
	XTmrCtr_SetHandler(&TMRInst, TMR_Intr_Handler, &TMRInst);
	XTmrCtr_SetResetValue(&TMRInst, 0, TMR_LOAD);
	XTmrCtr_SetOptions(&TMRInst, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	status = IntcInitFunction(INTC_DEVICE_ID, &TMRInst);
	if(status != XST_SUCCESS) return XST_FAILURE;
	XTmrCtr_Start(&TMRInst, 0);
	//Interrupt sensor update loop
	while(1) {
		if(updateFlag){
			updateFlag = 0;
			NAV_GetData(&NAVInst);
			celsToFah(NAVInst.tempC);
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
			snprintf(dataOut, sizeof(dataOut), "P:%.2f T:%.1f", NAVInst.hPa, fahren);
			OLED_PutString(&OLEDInst, dataOut);
			//Push data to the display
			OLED_Update(&OLEDInst);
		}
	}
}

void celsToFah(float tempC) {
	fahren = (tempC * 1.8) + 32.0;
}

//Timer interrupt handler based on lab 2 to allow PWM. Remember Timer 0 controls period and Timer 1 controls high time.
void TMR_Intr_Handler(void *CallBackRef, u8 TmrCtrNumber)
{
	updateFlag = 1;
}

//Initial setup functions based on lab 2
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
	// Enable interrupt
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
								 XScuGicInstancePtr);
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

int IntcInitFunction(u16 DeviceId, XTmrCtr *TmrInstancePtr)
{
	XScuGic_Config *IntcConfig;
	int status;

	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if(status != XST_SUCCESS) return XST_FAILURE;

	status = InterruptSystemSetup(&INTCInst);
	if(status != XST_SUCCESS) return XST_FAILURE;
	// Connect the driver's built-in handler to the interrupt controller.
	// The handler will acknowledge the interrupt and then call TMR_Intr_Handler to handle it.
	status = XScuGic_Connect(&INTCInst,
							 INTC_TMR_INTERRUPT_ID,
							 (Xil_ExceptionHandler)XTmrCtr_InterruptHandler, // Driver's handler
							 (void *)TmrInstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;

	XScuGic_Enable(&INTCInst, INTC_TMR_INTERRUPT_ID);
	return XST_SUCCESS;
}
