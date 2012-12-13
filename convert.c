#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct{
    signed int width,height;
    int* image;
} PKG;

typedef struct {
    char signature[2];      /* used to identify the BMP & DIB file is 0x42 0x4D */
    unsigned int file_size; /* size of the BMP file in bytes */
    unsigned short int reserved1, reserved2;
    unsigned int offset;    /* starting address, of the byte where the bitmap image data (pixel array) can be found. */
} BMP_HEADER;

typedef struct {
    unsigned int dib_size;          /* header size in bytes */
    signed int width,height;        /* width and height of image */
    unsigned short int planes;      /* color planes being used must be set to 1 */
    unsigned short int bits;        /* bits per pixel */
    unsigned int compression;       /* compression method */
    unsigned int image_size;        /* size of the raw bitmap data */
    int xresolution,yresolution;    /* pixels per meter */
    unsigned int ncolors;           /* number of colors */
    unsigned int important_colors;  /* important colors */
} DIB_HEADER;

PKG main (int argv , char *args[]){

    if(argv < 2){
        fprintf(stderr,"usage: convert [file].png\n");
        exit(1);
    }
    if(!strstr(args[1],".png")){
        fprintf(stderr,"usage: convert [file].png\n");
        exit(1);
    }


    FILE *file;
    unsigned char *buffer;
    unsigned long fileLen;
    BMP_HEADER header;
    DIB_HEADER dib_header;

    char* buf = malloc(sizeof(char*));
    //-type truecolor
    sprintf(buf,"convert %s -resize 30x30 -monochrome out.bmp\n",args[1]);
    system(buf);


    file = fopen("out.bmp", "rb");
    if(!file){
        fprintf(stderr, "unable to open file: %s\n",args[1]);
        exit(1);
    }


    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer=(char *)malloc(fileLen);
    if(!buffer){
        fprintf(stderr, "Memory error!\n");
        fclose(file);
        exit(1);
    }
    int read = 0;

        /* BMP Header */
        read+=fread (header.signature, 1, 1, file);
        read+=fread (header.signature+1, 1, 1 , file);
        read+=fread (&(header.file_size), 1, 4, file);
        read+=fread (&(header.reserved1), 1, 2, file);
        read+=fread (&(header.reserved2), 1, 2, file);
        read+=fread (&(header.offset), 1, 4, file);
        
        printf("BMP Header\n");
        printf("Filetype: %c%c\n", header.signature[0], header.signature[1]);
        printf("BMP Size in bytes: %u\n", header.file_size);
        printf("Reserved 1 and 2: %d/%d\n",header.reserved1,header.reserved2);
        printf("Offset starting address of the byte where the bitmap image data (pixel array) can be found: %d\n", header.offset);

        printf("------------------------------\n");

        /* DIB Header */
        read+=fread (&(dib_header.dib_size), 1, 4, file);
        read+=fread(&(dib_header.width), 1, 4, file);
        read+=fread(&(dib_header.height), 1, 4, file);
        read+=fread(&(dib_header.planes), 1, 2, file);
        read+=fread(&(dib_header.bits), 1, 2, file);
        read+=fread(&(dib_header.compression),1,4,file);
        read+=fread(&(dib_header.image_size),1,4,file);
        read+=fread(&(dib_header.xresolution),1,4,file);
        read+=fread(&(dib_header.yresolution),1,4,file);
        read+=fread(&(dib_header.ncolors),1,4,file);
        read+=fread(&(dib_header.important_colors),1,4,file);

        printf("DIB Header\n");
        printf("DIB size: %d\n",dib_header.dib_size);
        printf("Image width/height: %dx%d\n",dib_header.width,dib_header.height);
        printf("Number of planes: %d\n",dib_header.planes);
        printf("Bits per pixel: %d\n",dib_header.bits);
        printf("Compression method: %d\n",dib_header.compression);
        printf("Image size of the raw bitmap data: %d\n",dib_header.image_size);
        printf("Horizontal/Vertical resolution of the image pixel/meter: %dx%d\n",dib_header.xresolution,dib_header.yresolution);
        printf("Number of colors in the palette: %d\n",dib_header.ncolors);
        printf("Number of importan colors 0 means all: %d\n",dib_header.important_colors);
    
        char* junk = malloc(header.offset-read);
        if(read != header.offset){
            fread(junk,header.offset-read,1,file);
        }

    //fread(buffer,fileLen-header.offset,sizeof(unsigned char),file);
    fread(buffer,dib_header.image_size,1,file);
    fclose(file);

    int i=0;
    unsigned char zero = 0x00;
    unsigned char one = 0xFF;
    unsigned char black = 0x7F;
    int j = 1;
    int image[dib_header.height][dib_header.width];
    int l=1,m=0;
    int bytes = (dib_header.width * 3);
    if(bytes%4) bytes+=(bytes%4);
    PKG imageStream;
    imageStream.width = dib_header.width;
    imageStream.height = dib_header.height;
    imageStream.image = malloc(sizeof(int*));

    while (i < dib_header.image_size){
       // Uncomment this to print out raw byte data.
        //printf("%02X ",((unsigned char)buffer[i]));
        
        if(!(j%3) && (unsigned char)buffer[i] == zero){
            image[dib_header.height-l][m] = 0;
            m++;
        }else if(!(j%3) && (unsigned char)buffer[i] == one){
            image[dib_header.height-l][m] = 1;
            m++;
        }

        if(!( j % bytes)){  j =0; l++;m=0;}
        i++;
        j++;
    }

    int p,q,x = 0;

    for(p = 0; p < dib_header.height-1; p++){
        for(q = 0; q < dib_header.width; q++){
            imageStream.image[x] = image[p][q];
            printf("%d",imageStream.image[x]);
            x++;
        }
        printf("\n");
    }


    return imageStream;
}
