#include "LinuxSPI.h"
#include <stdio.h>  // For fprintf, stderr
#include <errno.h>  // For EFAULT

//Initializes SPI communication
int LSpi_Start(void* virtualPtr) {
    uint32_t controlReg;
    if (virtualPtr == NULL) {
        fprintf(stderr, "Linux_Spi_Start: Error, virtual_ptr is NULL.\n");
        return -EFAULT; // Return Linux error code
    }
    controlReg = LSpiReadReg(virtualPtr, LSP_CR_OFFSET);
    controlReg |= LSP_CR_TXFIFO_RESET_MASK | LSP_CR_RXFIFO_RESET_MASK | LSP_CR_ENABLE_MASK;
    LSpiWriteReg(virtualPtr, LSP_CR_OFFSET, controlReg);
    return 0;
}
