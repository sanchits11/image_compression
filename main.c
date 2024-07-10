#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>

// Define image dimensions
#define WIDTH 64
#define HEIGHT 64

// Function to save an image in PNG format
void saveImage(const char* filename, unsigned char image[HEIGHT][WIDTH]) {
    if (stbi_write_png(filename, WIDTH, HEIGHT, 1, image, WIDTH * sizeof(unsigned char)) == 0) {
        fprintf(stderr, "Error saving image\n");
    } else {
        printf("Image saved successfully\n");
    }
}

int main() {
    // Open the text file containing pixel intensity data for reading
    FILE *file = fopen("decoded_image.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    printf("File opened successfully\n");

    // Define a 2D array to store the pixel intensity data
    unsigned char pixel_intensities[HEIGHT][WIDTH];

    // Read pixel intensity data from the file into the 2D array
    for (int y = 0; y < WIDTH; y++) {
        for (int x = 0; x < HEIGHT; x++) {
            int result = fscanf(file, "%hhu", &pixel_intensities[y][x]);
            if (result != 1) {
                fprintf(stderr, "Error reading pixel intensity data at [%d, %d], fscanf returned %d\n", y, x, result);
                return 1;
            }
            printf("Read value at [%d, %d]: %hhu\n", y, x, pixel_intensities[y][x]);
        }
    }

    printf("File read successfully\n");

    // Close the file
    fclose(file);

    printf("File closed\n");

    // Save the reconstructed image as a PNG file
    saveImage("reconstructed_image.png", pixel_intensities);

    return 0;
}