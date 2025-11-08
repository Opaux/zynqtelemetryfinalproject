#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

// Define the offsets for AXI GPIO registers
#define GPIO_DATA_OFFSET 0x00
#define GPIO_TRI_OFFSET  0x04 // Tri-state (direction) register

// The size of the memory to map.
#define MAP_SIZE 0x10000

int main() {
    int uio_fd;
    void *gpio_ptr_virtual;

    // Use a volatile pointer to ensure the compiler doesn't optimize
    // away your hardware-accessing code.
    volatile uint32_t *gpio_data_reg;
    volatile uint32_t *gpio_tri_reg;

    // Open UIO device file
    uio_fd = open("/dev/uio2", O_RDWR);
    if (uio_fd < 0) {
        perror("Failed to open /dev/uio2");
        return -1;
    }

    // Memory-map the hardware's physical address into virtual memory
    gpio_ptr_virtual = mmap(NULL,           // Any virtual address
                            MAP_SIZE,       // Size of memory to map
                            PROT_READ | PROT_WRITE, // We want to read and write
                            MAP_SHARED,     // Share changes
                            uio_fd,         // File descriptor for the UIO device
                            0);             // Offset (0 for UIO's map0)

    if (gpio_ptr_virtual == MAP_FAILED) {
        perror("mmap failed");
        close(uio_fd);
        return -1;
    }

    // Create register-access pointers
    gpio_data_reg = (uint32_t *)(gpio_ptr_virtual + GPIO_DATA_OFFSET);
    gpio_tri_reg  = (uint32_t *)(gpio_ptr_virtual + GPIO_TRI_OFFSET);

    printf("Setting AXI GPIO direction to 'output'...\n");
    // Set all 32 bits to output (write 0)
    *gpio_tri_reg = 0x00000000;

    printf("Blinking LEDs (writing to data register)...\n");
    while (1) {
        *gpio_data_reg = 0xFFFFFFFF; // Turn all on
        sleep(1);
        *gpio_data_reg = 0x00000000; // Turn all off
        sleep(1);
    }


    // Clean up
    printf("Unmapping memory and closing device.\n");
    munmap(gpio_ptr_virtual, MAP_SIZE);
    close(uio_fd);

    return 0;
}
