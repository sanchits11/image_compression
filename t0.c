#include<stdio.h>
#include<stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(void){
    int width,height,channels;
    unsigned char *img=stbi_load("sky.png",&width,&height,&channels,0);
    if(img==NULL){
        printf("Error in loading the image.\n");
        exit(1);
    }  


    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels",width,height,channels);

    

    stbi_write_png("image.png",width,height,channels,img,width*channels);
    stbi_write_jpg("image.jpg",width,height,channels,img,100);

    stbi_image_free(img);
}
