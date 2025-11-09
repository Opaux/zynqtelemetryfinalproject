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
    controlReg = LSpiReadReg(virtualPtr, LSP_CR_OFFSET); //reads in control register value
    controlReg |= LSP_CR_TXFIFO_RESET_MASK | LSP_CR_RXFIFO_RESET_MASK | LSP_CR_ENABLE_MASK; //reset FIFOs
    LSpiWriteReg(virtualPtr, LSP_CR_OFFSET, controlReg);
    return 0;
}

//Stops SPI communication by disabling the device
int LSpi_Stop(void* virtualPtr) {
    uint32_t controlReg;
    if (virtualPtr == NULL) {
        fprintf(stderr, "Linux_Spi_Stop: Error, virtual_ptr is NULL.\n");
        return -EFAULT; // Return Linux error code
    }
    controlReg = LSpiReadReg(virtualPtr, LSP_CR_OFFSET);
    LSpiWriteReg(virtualPtr, LSP_CR_OFFSET, controlReg & ~LSP_CR_ENABLE_MASK);
    return 0;
}

//Resets the SPI device by writing to the Software Reset register.
int LSpi_Reset(void* virtualPtr) {
    uint32_t controlReg;
    if (virtualPtr == NULL) {
        fprintf(stderr, "Linux_Spi_Reset: Error, virtual_ptr is NULL.\n");
        return -EFAULT; // Return Linux error code
    }
    //xspi_abort logic
    LSpiWriteReg(virtualPtr, LSP_SSR_OFFSET, 0xFFFFFFFF); //disable all slave devices
    controlReg = LSpiReadReg(virtualPtr, LSP_CR_OFFSET);
    controlReg |= LSP_CR_TRANS_INHIBIT_MASK | LSP_CR_TXFIFO_RESET_MASK | LSP_CR_RXFIFO_RESET_MASK; //reset FIFOs and inhibit master transactions
    LSpiWriteReg(virtualPtr, LSP_CR_OFFSET, controlReg); //Write control value
    LSpiWriteReg(virtualPtr, LSP_SRR_OFFSET, LSP_SRR_RESET_MASK); //Reset device
    return 0;
}
/*
 * Transfers the specified data on the SPI bus. If the SPI device is configured
 * to be a master, this function initiates bus communication and sends/receives
 * the data to/from the selected SPI slave. If the SPI device is configured to
 * be a slave, this function prepares the data to be sent/received when selected
 * by a master. For every byte sent, a byte is received. */
int LSpi_Transfer(void* virtualPtr, uint8_t *SendBufPtr, uint8_t *RecvBufPtr, unsigned int ByteCount) {
    uint32_t controlReg;
    uint32_t GlobalIntrReg;
    uint32_t StatusReg;
    uint32_t Data = 0;
    uint8_t  DataWidth;
    if (virtualPtr == NULL) {
        fprintf(stderr, "Linux_Spi_Transfer: Error, virtual_ptr is NULL.\n");
        return -EFAULT; // Return Linux error code
    }
    while((LSpiReadReg(virtualPtr, LSP_SR_OFFSET))
}
