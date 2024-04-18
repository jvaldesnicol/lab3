#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include "bmp_utility.h"

#define HW_REGS_BASE (0xff200000)
#define HW_REGS_SPAN (0x00200000)
#define HW_REGS_MASK (HW_REGS_SPAN - 1)
#define LED_BASE 0x1000
#define PUSH_BASE 0x3010
#define VIDEO_BASE 0x0000

#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240


#define FPGA_ONCHIP_BASE     (0xC8000000)
#define IMAGE_SPAN (IMAGE_WIDTH * IMAGE_HEIGHT * 4)
#define IMAGE_MASK (IMAGE_SPAN - 1)




int main(void) {
    volatile unsigned int *video_in_dma = NULL;
    volatile unsigned int *key_ptr = NULL;
    volatile unsigned short *video_mem = NULL;
    void *virtual_base;
    int fd;

    // Open /dev/mem
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return 1;
    }

    // Map physical memory into virtual address space
    virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
    if (virtual_base == MAP_FAILED) {
        printf("ERROR: mmap() failed...\n");
        close(fd);
        return 1;
    }

    // Calculate the virtual address where our device is mapped
    video_in_dma = (unsigned int *)(virtual_base + ((VIDEO_BASE) & (HW_REGS_MASK)));
    key_ptr = (unsigned int *)(virtual_base + ((PUSH_BASE) & (HW_REGS_MASK)));

    unsigned short pixels[IMAGE_HEIGHT][IMAGE_WIDTH];
    unsigned short pixels_bw[IMAGE_HEIGHT][IMAGE_WIDTH];

    // Read the value from the video_in_dma register
    int value = *(video_in_dma+3); // read the updated value from the video_in_dma register

    printf("Video In DMA register updated at:0%x\n",(video_in_dma));

    // Modify the PIO register
    *(video_in_dma+3) = 0x4; // enable video

    // Read the updated value from the video_in_dma register
    value = *(video_in_dma+3);

    printf("enabled video:0x%x\n",value);

    // Button press

    while (1) {
        if (*key_ptr != 0x07) { // when botton is pressed
            *(video_in_dma+3) = 0x00; // disable video 
            while (*key_ptr != 0x07); // wait until button is released
            break;  // exit the loop
        }
    }

    value = *(video_in_dma+3); // read the updated value from the video_in_dma register
    printf("disabled video:0x%x\n",value);

    // Set image virtual_base (mmap used to map into virtual memory)
    virtual_base = mmap(NULL, IMAGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, FPGA_ONCHIP_BASE);
    if (virtual_base == MAP_FAILED) {
        printf("ERROR: mmap() failed...\n");
        close(fd);
        return 1;
    }

    video_mem = (unsigned short *)(virtual_base + ((FPGA_ONCHIP_BASE) & (IMAGE_MASK)));

    // Get black/white threshold
    int x; 
    int y;
    int sum = 0; // Sum of all gray scaled pixel values
    for (y = 0; y < IMAGE_HEIGHT; y++) {
        for (x = 0; x < IMAGE_WIDTH; x++) {
            unsigned short pixel = *(video_mem + (y << 9) + x); // Read pixel data
            int red = (pixel >> 11) & 0x1F; // Extract the color components
            int green = (pixel >> 5) & 0x3F;
            int blue = pixel & 0x1F;
            int gray = (red + green + blue) / 3;
            sum += gray;
        }
    }
    int t = sum / (IMAGE_HEIGHT * IMAGE_WIDTH); // Threshold value

    for (y = 0; y < IMAGE_HEIGHT; y++) {
        for (x = 0; x < IMAGE_WIDTH; x++) {
            unsigned short pixel = *(video_mem + (y << 9) + x); // Read pixel data
            pixels[y][x] = pixel; 
            // Get black/white pixel
            int red = (pixel >> 11) & 0x1F; // Extract the color components
            int green = (pixel >> 5) & 0x3F;
            int blue = pixel & 0x1F;
            int gray = (red + green + blue) / 3;
            if (gray > t) {
                pixels_bw[y][x] = 0xFFFF; // White
            } else {
                pixels_bw[y][x] = 0x0000; // Black
            }
        }
    }

    // Save image as color
    const char* filename = "final_image_color.bmp";
    saveImageShort(filename,&pixels[0][0],320,240);

    // Save image as black and white
    const char* filename1 = "final_image_bw.bmp";
    saveImageShort(filename1,&pixels_bw[0][0],320,240);

    // Clean up
    if (munmap(virtual_base, IMAGE_SPAN) != 0) {
        printf("ERROR: munmap() failed...\n");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
