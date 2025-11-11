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
uint8_t LSpi_Transfer(void* virtualPtr, uint8_t sendByte) {
    uint32_t controlReg;
    if (virtualPtr == NULL) {
        fprintf(stderr, "Linux_Spi_Transfer: Error, virtual_ptr is NULL.\n");
        return -EFAULT; // Return Linux error code
    }
    // Wait for status to be ready and FIFO to be not full before transmission
    while((LSpiReadReg(virtualPtr, LSP_SR_OFFSET)) & LSP_SR_TX_FULL_MASK);
    // Load data into transmit register
    LSpiWriteReg(virtualPtr, LSP_DTR_OFFSET, (uint32_t)sendByte);
    // Start master transmitter to transmit data (inverse of inhibit mask)
    controlReg = LSpiReadReg(virtualPtr, LSP_CR_OFFSET);
    controlReg &= ~LSP_CR_TRANS_INHIBIT_MASK;
    LSpiWriteReg(virtualPtr, LSP_CR_OFFSET, controlReg);
    // Wait receive register to get data (transfer complete)
    while((LSpiReadReg(virtualPtr, LSP_SR_OFFSET)) & LSP_SR_RX_EMPTY_MASK);
    // Inhibit master transmitter to end transmission
    controlReg = LSpiReadReg(virtualPtr, LSP_CR_OFFSET);
    controlReg |= LSP_CR_TRANS_INHIBIT_MASK;
    LSpiWriteReg(virtualPtr, LSP_CR_OFFSET, controlReg);
    // Return data received from the transmission
    return (uint8_t)LSpiReadReg(virtualPtr, LSP_DRR_OFFSET);
}

/* Selects or deselect the slave with which the master communicates. Each slave
* that can be selected is represented in the slave select register by a bit.
* The argument passed to this function is the bit mask with a 1 in the bit
* position of the slave being selected. Only one slave can be selected.*/
int LSpi_SetSlaveSelect(void* virtualPtr, uint8_t SlaveMask){
    uint32_t controlReg;
    int numAsserted;
    int i;
    if (virtualPtr == NULL) {
        fprintf(stderr, "Linux_Spi_SetSlaveSelect: Error, virtual_ptr is NULL.\n");
        return -EFAULT; // Return Linux error code
    }
    // Check if the transmitter is busy (transmitter not empty), cancel slave selection if busy
    if ((LSpiReadReg(virtualPtr, LSP_SR_OFFSET) & LSP_SR_TX_EMPTY_MASK) == 0)
    {
        fprintf(stderr, "Transfer busy, try again later.\n");
        return -EFAULT;
    }
    // Write inverse of slave mask to the slave selection register
    LSpiWriteReg(virtualPtr, LSP_SSR_OFFSET, ~(uint32_t)SlaveMask);
    return 0;
}

