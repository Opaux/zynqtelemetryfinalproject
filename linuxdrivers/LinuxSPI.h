#ifndef LINUX_SPI_H
#define LINUX_SPI_H

#include "LinuxSPI_l.h"
#include <stdint.h>

/************************** Constant Definitions *****************************/

/** @name Configuration options
 *
 * The following options may be specified or retrieved for the device and
 * enable/disable additional features of the SPI.  Each of the options
 * are bit fields, so more than one may be specified.
 */
#define XSP_MASTER_OPTION		0x1
#define XSP_CLK_ACTIVE_LOW_OPTION	0x2
#define XSP_CLK_PHASE_1_OPTION		0x4
#define XSP_LOOPBACK_OPTION		0x8
#define XSP_MANUAL_SSELECT_OPTION	0x10
/*@}*/

/************************** Function Definitions *****************************/
int LSpi_Start(void* virtualPtr);
int LSpi_Stop(void* virtualPtr);
void LSpi_Reset(void* virtualPtr);
int LSpi_SetSlaveSelect(void* virtualPtr, uint32_t SlaveMask);
int LSpi_Transfer(void* virtualPtr, uint8_t *SendBufPtr, uint8_t *RecvBufPtr, unsigned int ByteCount);
