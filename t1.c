#include<stdio.h>
#include<stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(void){
    int width,height,channels;
    unsigned char *img=stbi_load("sky.jpg",&width,&height,&channels,0);
    if(img==NULL){
        printf("Error in loading the image.\n");
        exit(1);
    }  


    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels",width,height,channels);

    //Convert image to gray
    size_t img_size = width * height * channels;
    int gray_channels = channels == 4 ? 2 : 1;
    size_t gray_img_size = width * height * gray_channels;
    unsigned char *gray_img = malloc(gray_img_size);

    if(gray_img == NULL){
        printf("Unable to allocate memory for the gray image.\n");
        exit(1);
    }

    for(unsigned char *p = img, *pg = gray_img; p!= img + img_size; p+=channels, pg+=gray_channels) {
        *pg = (uint8_t)((*p + *(p+1) + *(p+2))/3.0);
        if(channels==4){
            *(pg+1)=*(p+3);
        }
    }

    stbi_write_jpg("sky_gray.jpg",width,height,gray_channels,gray_img,100);

    //Convert the image into sepia
    unsigned char *sepia_img = malloc(img_size);
    if(sepia_img==NULL){
        printf("Unable to allocate memory for sepia");
    }

    for(unsigned char *p = img, *pg = sepia_img; p!= img + img_size; p+=channels, pg+=channels) {
        *pg       = (uint8_t)fmin(0.393 * *p + 0.769 * *(p + 1) + 0.189 * *(p + 2), 255.0);         // red
        *(pg + 1) = (uint8_t)fmin(0.349 * *p + 0.686 * *(p + 1) + 0.168 * *(p + 2), 255.0);         // green
        *(pg + 2) = (uint8_t)fmin(0.272 * *p + 0.534 * *(p + 1) + 0.131 * *(p + 2), 255.0);         // blue        
        if(channels == 4) {
            *(pg + 3) = *(p + 3);
        }
    }

    stbi_write_jpg("sky_sepia.jpg",width,height,channels,sepia_img,100);

    //stbi_write_png("sky.png",width,height,channels,img,width*channels);
    //stbi_write_jpg("sky2.png",width,height,channels,img,100);

    stbi_image_free(img);
    free(gray_img);
}
