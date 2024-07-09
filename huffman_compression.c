#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


//function to generate code length of encoded huffman code
int codelen(char *code){
    int len = 0;
    while(*(code+len)!='\0'){
        len++;
    }
    return len;
}

//to concatenate 0's and 1's as we backtrack from leaf to root
void strconcat(char *str, char *parentcode, char add)
{
	int i=0;
	while(*(parentcode+i)!='\0')
	{
		*(str+i)=*(parentcode+i);
		i++;
	}
	if(add!='2')
	{
		str[i]=add;
		str[i+1]='\0';
	}
	else{
		str[i]='\0';
    }
}

//recursive function for fibonacci series
int fibonacci(int n)
{
    if (n <= 1)
        return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main(){
    //initializing variables
    int i,j,rows,columns;
    char sourcefile[256];
    printf("Enter the image filename: ");
    scanf("%255s", sourcefile); // Read up to 255 characters to avoid buffer overflow
    int data=0;
    int offset,hbytes,height,width;
    long bmpsize=0,bmpdataoff=0,bpp=0;
    int **image;
    int temp=0;

    //reading the BMP source file

    FILE *image_file;
    image_file=fopen(sourcefile,"rb");
    if(image_file==NULL){
        printf("Error opening file!\n");
        exit(1);
    }

    else{
        //start of BMP information processing
        printf("Processing source file Header...\n");
        //to show signature of BMP file
        offset=0;
        fseek(image_file,offset,SEEK_SET);
        printf("File ID is:\n");
        for(i=0;i<2;i++){
            fread(&data,1,1,image_file);
            printf("%c",data);
        }
        fread(&bmpsize,4,1,image_file);
        printf("Size of source image is: %ld bytes\n",bmpsize);

        //getting offset where pixel array starts for image
        offset=10;

        fseek(image_file,offset,SEEK_SET);
        fread(&bmpdataoff,4,1,image_file);
        printf("Bitmap Data Offset:%ld\n",bmpdataoff);

        fread(&hbytes,4,1,image_file);
		printf("Number of bytes in file: %d\n\n",hbytes);

        fread(&height,4,1,image_file);
        fread(&width,4,1,image_file);
        printf("Height of image:%d\n",height);
        printf("width of image:%d\n",width);

        fseek(image_file,2,SEEK_CUR);
		fread(&bpp,2,1,image_file);
		printf("Number of bits per pixel: %ld\n\n",bpp);

        //setting offset to start of pixel data
        fseek(image_file,bmpdataoff,SEEK_SET);

        //image array formation(memory allocation)
		printf("Creating Image array...\n");
        image=(int**)malloc(height*sizeof(int *));
        for(i=0; i<height ; i++){
            image[i] = (int*)malloc(width*sizeof(int));   
        }
        int num_bytes =  (bmpsize-bmpdataoff)/3;
        printf("Number of bytes: %d\n",num_bytes);

        //reading into the image *BMP* file into the image array
        for(rows=0; rows<height; rows++){
            for(columns=0; columns<width; columns++){
                fread(&temp,3,1,image_file);
                temp = temp&0x0000FF;
                image[rows][columns]=temp;
            }
        }
        printf("Image array processed\n");
    }
    fclose(image_file);
    //about image
    for(rows=0;rows<height;rows++){
        for(columns=0;columns<width;columns++){
            printf("%d ",image[rows][columns]);
        }
        printf("\n");
    }
    //creating histogram to store pixel intensities 
    int hist[256];
    for(i=0;i<256;i++){
        hist[i]=0;
    }
    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            hist[image[i][j]]+=1;
        }
    }

    //finding number of non-zero occurences in probabilities
    int nozero=0;
    for(i=0;i<256;i++){
        if(hist[i]!=0){
            nozero+=1;
        }
    }
    printf("Number of non-zero occurences:%d\n",nozero);

    //calculating max length of code word
    float p=1.0,ftemp;
    for(i=0;i<256;i++){
        ftemp = (hist[i]/(float)(height*width));
        if(ftemp>0 && ftemp<=p){
            p = ftemp;
        }
    }
    i=0;
    
    while((1/p)>fibonacci(i)){
        i++;
    }
    int maxlen_code = i - 3;
    printf("Maximum code length: %d\n",maxlen_code);

    //defining stucture for pixel frequencies
    struct pixelfreq{
        int pixel,left_arrloc,right_arrloc;
        float freq;
        struct pixelfreq *left,*right;
        char code[maxlen_code];
    };

    //defining structure for huffman codes for each pixel
    struct huffcodes{
        int pixel,arrloc;
        float freq;
    };

    //defining a tree structure for huffman tree in order to transmit
    struct huff_tree{
        int pixel,left_arrloc,right_arrloc;
        float freq;
        char code[maxlen_code];
    };

    //declaring all structures 
    struct pixelfreq *pixfreq;
    struct huffcodes *huffcode;
    struct huff_tree *hufftree;

    int totalnozero = 2*nozero - 1;
    int total_pixels = width*height;
    pixfreq=(struct pixelfreq*)malloc(sizeof(struct pixelfreq)*totalnozero);
    huffcode=(struct huffcodes*)malloc(sizeof(struct huffcodes)*nozero);
    hufftree=(struct huff_tree*)malloc(sizeof(struct huff_tree)*totalnozero);
    j=0;
    float temp_freq;
    for(i=0;i<256;i++){
        if(hist[i]!=0){
            huffcode[j].pixel=i;
            pixfreq[j].pixel=i;
            hufftree[j].pixel=i;
            hufftree[j].left_arrloc=-1;
            hufftree[j].right_arrloc=-1;
            huffcode[j].arrloc=j;
            temp_freq=(float)hist[i]/(float)total_pixels;
            pixfreq[j].freq=temp_freq;
            huffcode[j].freq=temp_freq;
            pixfreq[j].left=NULL;
            pixfreq[j].right=NULL;
            pixfreq[j].code[0]='\0';
            hufftree[j].code[0]='\0';
            j++;
        }
    }

    //sorting the histogram 
    struct huffcodes temp_huff;
    //sorting w.r.t occurrence probability
    for(i=0;i<nozero;i++){
        for(j=i+1; j<nozero;j++){
            if(huffcode[i].freq < huffcode[j].freq){
                temp_huff=huffcode[i];
                huffcode[i]=huffcode[j];
                huffcode[j]=temp_huff;
            }
        }
    }

    //displaying frequencies for each pixel
    for(i=0;i<nozero;i++){
        printf("%d --> %f\n",huffcode[i].pixel,huffcode[i].freq);
    }

    //building a huffman tree
    float probablesum;
    int pixelsum;
    int n=0,k=0,l=0;
    printf("No zero occurrences: %d\n",nozero);
    int nextnode = nozero;
    while(n<nozero-1){
        //Adding the two lowest probabilities
        probablesum=huffcode[nozero-n-1].freq+huffcode[nozero-n-2].freq;
        pixelsum=huffcode[nozero-n-1].pixel+huffcode[nozero-n-2].pixel;
        i=0;

        //new position of combined node
        while(probablesum<=huffcode[i].freq){
            i++;
        }

        //Appending the minHeap
        pixfreq[nextnode].pixel=pixelsum;
        hufftree[nextnode].pixel=pixelsum;
        pixfreq[nextnode].freq=probablesum;
        hufftree[nextnode].freq=probablesum;
        pixfreq[nextnode].left=&pixfreq[huffcode[nozero-n-2].arrloc];
        pixfreq[nextnode].right=&pixfreq[huffcode[nozero-n-1].arrloc];
        hufftree[nextnode].left_arrloc=huffcode[nozero-n-2].arrloc;
        hufftree[nextnode].right_arrloc=huffcode[nozero-n-1].arrloc;
        pixfreq[nextnode].code[0]='\0';
        hufftree[nextnode].code[0]='\0';

        //sorting array
        for(k =  nozero; k>=0; k--){
            if(k==i){
                huffcode[k].pixel=pixelsum;
                huffcode[k].freq=probablesum;
                huffcode[k].arrloc=nextnode;
            }
            else if(k>i){
                huffcode[k]=huffcode[k-1];
            }
        }
        n+=1;
        nextnode+=1;
    }

    //Alloting huffcode thorugh backtracking
    printf("Assigning code through backtracking...\n");
    char left = '0';
    char right = '1';
    int index;
    for(i=totalnozero-1;i>=nozero;i--){
        if(pixfreq[i].left!=NULL){
            strconcat(pixfreq[i].left->code,pixfreq[i].code,left);
            strconcat(hufftree[hufftree[i].left_arrloc].code,hufftree[i].code,left);
        }
        if(pixfreq[i].right!=NULL){
            strconcat(pixfreq[i].right->code,pixfreq[i].code,right);
            strconcat(hufftree[hufftree[i].right_arrloc].code,hufftree[i].code,right);
        }
    }

    //naming last node left as *root*
    strconcat(pixfreq[totalnozero-1].code,"root",'2');

    //printing huffman codes
    printf("Huffman Codes\n");
    printf("Pixels ---> Huffman Code\n");
    for(i=0;i<nozero;i++){
        if(snprintf(NULL,0,"%d",pixfreq[i].pixel)==2){
            printf("     %d     -> %s\n",pixfreq[i].pixel,pixfreq[i].code);
        }
        else{
            printf("     %d     -> %s\n",pixfreq[i].pixel,pixfreq[i].code);
        }
    }

    printf("Writing huffman codes into a text file.\n");
    FILE *huffman = fopen("huffmancodes.txt","wb");
    fprintf(huffman,"Huffman Codes\n");
    fprintf(huffman,"pixel ->  codes\n");
    for(i=0; i<nozero; i++){
        if(snprintf(NULL,0,"%d",pixfreq[i].pixel)==2){
            fprintf(huffman,"     %d     -> %s\n",pixfreq[i].pixel,pixfreq[i].code);
        }
        else{
            fprintf(huffman,"     %d     -> %s\n",pixfreq[i].pixel,pixfreq[i].code);
        }
    }
    fclose(huffman);

    //writing huffman tree into a DAT file
    printf("Writing huffman tree into a DAT file.\n");
    FILE *tree=fopen("hufftree.dat","wb");
    int tree_size = sizeof(struct huff_tree);
    for(i=0; i<totalnozero; i++){
        fwrite(&hufftree[i],tree_size,1,tree);
    }
    fclose(tree);

    //encode the image into a text file
    int pixelvalue;
    printf("Writing huffman encoded image for simulating Transmission.\n");
    FILE *encoded=fopen("encoded_image.txt","wb");
    for(rows=0; rows<height; rows++){
        for(columns=0; columns<width; columns++){
            pixelvalue=image[rows][columns];
            for(i=0; i<nozero; i++){
                if(pixelvalue == pixfreq[i].pixel){
                    fprintf(encoded,"%s",pixfreq[i].code);
                }
            }
        }
    }
    fclose(encoded);

    //writing information of the image
    printf("Writing image information into a file...");
    FILE *imageinfo =  fopen("image_info.dat","wb");
    fwrite(&nozero,4,1,imageinfo);
    fwrite(&height,4,1,imageinfo);
    fwrite(&width,4,1,imageinfo);
    fclose(imageinfo);

    //Decoding process starts here
    //reading encoded_image.txt generated in encoding process
    FILE *encoded_image = fopen("encoded_image.txt","rb");
    int **dimage;
    char bit;
    dimage = (int**)malloc(height*sizeof(int*));
    for(i=0; i<height; i++){
        dimage[i] = (int*)malloc(width*sizeof(int));
    }
    for(i=0; i<totalnozero; i++){
        printf("%d -> %s\n",pixfreq[i].pixel,pixfreq[i].code);
    }
    struct pixelfreq curr;
    curr = pixfreq[totalnozero-1];
    printf("Decoding...\n");
    for(rows=0; rows<height; rows++){
        for(columns=0; columns<width; columns++){
            curr = pixfreq[totalnozero-1];
            while(curr.left!=NULL && curr.right!=NULL){
                fscanf(encoded_image,"%c ",&bit);
                if(bit==left){
                    curr = *curr.left;
                }
                else{
                    curr = *curr.right;
                }
            }
            if(curr.left==NULL && curr.right==NULL){
                dimage[rows][columns] = curr.pixel;
            }
        }
    }
    printf("Decoding complete :)\n");
    fclose(encoded_image);
    FILE *decoded_image = fopen("decoded_image.txt","wb");
    for(rows=0; rows<height; rows++){
        for(columns=0; columns<width; columns++){
            fprintf(decoded_image,"%d ",dimage[rows][columns]);
        }
        fprintf(decoded_image,"\n");
    }
    fclose(decoded_image);

   
    return 0;

}