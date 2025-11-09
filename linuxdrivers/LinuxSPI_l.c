#include "LinuxSPI_l.h"

// Reads in a value from a specific register and returns the 32 bit value read in.
uint32_t LSpiReadReg(void* virtualPtr, uint32_t RegOffset) {
    uint32_t readVal = *(volatile uint32_t*)(virtualPtr + RegOffset);
    return readVal;
}

//Writes a value to a specific register
void LSpiWriteReg (void* virtualPtr, uint32_t RegOffset, uint32_t RegValue) {
    *(volatile uint32_t*)(virtualPtr + RegOffset) = regValue;
}
