#include "image.h"
#include "utils.h"
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Image_load(Image *img, const char *fname)
{
    if((img->data=stbi_load(fname, &img->width, &img->height, &img->channels, 0))!=NULL) {
        img->size=img->width*img->height*img->channels;
        img->allocation_ = STB_ALLOCATED;
    }
}

void Image_create(Image *img,int width,int height,int channels, bool zeroed)
{
    size_t size = height * width * channels;
    if(zeroed){
        img->data = calloc(size,1);
    }
    else{
        img->data = malloc(size);
    }

    if(img!=NULL){
        img->width=width;
        img->height=height;
        img->size=size;
        img->channels=channels;
        img->allocation_=SELF_ALLOCATED;
    }

}

void Image_save(const Image *img,const char *fname){
    //Check if file name ends with .jpg/.JPEG/.jpeg or .png/.PNG 
    if(str_ends_in(fname,".jpg")||str_ends_in(fname,".JPG")||str_ends_in(fname,".jpeg ")||str_ends_in(fname,".JPEG")){
        stbi_write_jpg(fname,img->width,img->height, img->channels,img->data,100);
    }
    else if(str_ends_in(fname,".png")||str_ends_in(fname,".PNG")){
        stbi_write_png(fname,img->width, img->height, img->channels, img->data, img->width*img->channels);
    }
    else{
        ON_ERROR_EXIT(false,"");
    }
}

void Image_free(Image *img){
    if(img->allocation_!=NO_ALLOCATION && img->data!=NULL){
        if(img->allocation_== STB_ALLOCATED){
            stbi_image_free(img->data);
        }
        else{
            free(img->data);
        }
        img->data=NULL;
        img->width = 0;
        img->height = 0;
        img->size = 0;
        img->allocation_=NO_ALLOCATION;
    }
}

void Image_to_gray(const Image *original, Image *gray){
    int channels = original->channels ==  4 ? 2 : 1;
    Image_create(gray,original->width,original->height,channels,false);
    ON_ERROR_EXIT(gray->data == NULL, "Error in creating the image!");

    for(unsigned char *p = original->data, *pg = gray->data; p != original->data + original->size; p+=original->channels, pg+=gray->channels){
        *pg = (uint8_t)((*p + *(p+1) + *(p+2))/3.0);
        if(original->channels==4){
            *(pg+1)=*(p+3);
        }
    }
}

void Image_to_sepia(const Image *original, Image *sepia){
    Image_create(sepia,original->width,original->height,original->channels,false);
    ON_ERROR_EXIT(sepia->data == NULL, "Error in creating the image!");

    for(unsigned char *p = original->data, *pg = sepia->data; p!= original->data + original->size; p+=original->channels, pg+=sepia->channels) {
        *pg       = (uint8_t)fmin(0.393 * *p + 0.769 * *(p + 1) + 0.189 * *(p + 2), 255.0);         // red
        *(pg + 1) = (uint8_t)fmin(0.349 * *p + 0.686 * *(p + 1) + 0.168 * *(p + 2), 255.0);         // green
        *(pg + 2) = (uint8_t)fmin(0.272 * *p + 0.534 * *(p + 1) + 0.131 * *(p + 2), 255.0);         // blue        
        if(original->channels == 4) {
            *(pg + 3) = *(p + 3);
        }
    }
}
