#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

/**
 * Name: Roland Nguyen
 * Class: CMPS-1600
 * Lab Session: 03
 * Date: 4/10/18
 * Project 1: Away with the Photobombers!
 *
 *
 *
 * To use, change the text in the "source.txt" input file in the project folder to the absolute path of your folder
 * containing only the ppm file set that you want to use, then run. This way, the program will work for any directory
 * containing only ppm files and nothing more.
 * It shall output the new ppm file to somewhere within your project folder. (In my case, using CLion IDE, it outputs to
 * the project's "cmake-build-debug" folder.
 *
 *
 *
 * I use Windows operating system, and so I made my program work for one.
 * Due to the differences in Windows' file system and OSX's file system, I only know that this works for Windows.
 * I don't own a Mac nor have the experience with one to make this also work for OSX.
 */



/**
 * Pixl struct that has been typedef'ed
 * Contains integer values r,b,g for red, green, and blue respectively
 */
typedef struct{
    int r;
    int g;
    int b;

}pixl;

/**
 * img struct that has been typedef'ed
 * Contains integer values for x (image width), y (image height), and max (max color value).
 * Also contains **pixels which is array of pixl pointers. Will be made into a pixl matrix based on image's
 * width and height values and each pixl within the matrix will correspond to one of the ppm's pixels.
 */
typedef struct {
    pixl **pixels;
    int x,y;
    int max;
} img;



/**
 * Prepends a "string" (char array) to another "string"
 * Used to take name of each image and prepend the directory name to it to obtain the absolute path.
 * @param char* s
 * @param const char* t
 * NO RETURN - VOID
 */
void prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    size_t i;
    memmove(s + len, s, strlen(s) + 1);

    for (i = 0; i < len; ++i)
    {
        s[i] = t[i];
    }
}



/**
 * Finds to mode of an array of integers and returns that.
 * Used to find the mode for each pixel.
 * @param int a[] - a[] is the integer array
 * @param int k - n being size of array which will always be 3 since r,g,b.
 * @return int
 *
 */
int findmode(int a[],int k){
    int val = 0;
    int count = 0;
    int i, j;
    for (i = 0; i < k; ++i){
        int num = 0;
        for (j = 0; j < k; ++j){
            if (a[j] == a[i])
                ++num;
        }
        if (num > count){
            count = num;
            val = a[i];
        }
    }
    return val;
}



/**
 * Used to read a FILE *fp and create and img* i which will be returned
 * @param FILE* fp
 * @return img* i
 */
img* readImage(FILE *fp) {
    if (fp == NULL) { /*Checks if file is null. Exits program if null*/
        printf("File is null");
        exit(1);
    }
    int width;
    int height;
    fscanf(fp, "P3 %d %d\n", &width, &height); /*Scans header of the FILE into int variables width and height to calloc correct amount of memory for img* i*/
    img *i = (img *) calloc(width * height * 3, sizeof(img)); /*Creates img* i and allocates memory based on dimensions*/
    rewind(fp); /*Rewinds directory to start from beginning and read it into the image i instead of into variables now that it has correct memory allocated*/
    fscanf(fp, "P3 %d %d %d%*[\\n]", &i->x, &i->y, &i->max); /*Scans header of the FILE into the newly created img* i*/

    if (height > width) { /*For vertical images*/
        i->pixels = (pixl **) calloc(i->y * 3, sizeof(pixl *)); /*Allocates memory of i's pixels as a pixl matrix*/
        for (int j = 0; j < i->y; j++) {
            i->pixels[j] = (pixl *) calloc(i->x * 3, sizeof(pixl));
        }
        for (int j = 0; j < i->x; j++) { /*Scans values rbg into the pixl matrix*/
            for (int k = 0; k < i->y; k++) {
                fscanf(fp, "%d %d %d", &i->pixels[j][k].r, &i->pixels[j][k].g, &i->pixels[j][k].b);
            }
        }
    }
    else if (width > height) { /*For horizontal images*/
        i->pixels = (pixl **) calloc(i->x * 3, sizeof(pixl *)); /*Allocates memory of i's pixels as a pixl matrix*/
        for (int j = 0; j < i->x; j++) {
            i->pixels[j] = (pixl *) calloc(i->y * 3, sizeof(pixl));
        }
        for (int j = 0; j < i->y; j++) { /*Scans values rbg into the pixl matrix*/
            for (int k = 0; k < i->x; k++) {
                fscanf(fp, "%d %d %d", &i->pixels[j][k].r, &i->pixels[j][k].g, &i->pixels[j][k].b);
            }
        }
    }
    fclose(fp); /*Closes image*/
    return i; /*Returns img* i*/
}


/**
 * Used to write to a FILE *fp (specifically write desired contents of new image) from img *i. Checks if vertical or horizontal image
 * to know which to do.
 * @param FILE* fp
 * @param img* i
 * @return img* i
 */
img* writeImage(FILE *fp, img *i){
    fprintf(fp, "P3 %d %d %d ", i->x, i->y, i->max); /*Writes to the newModeImage the header of img_new*/
    if(i->y > i->x) { /*For Vertical images: Loops through new img and writes to newModeImage the contents of img_new*/
        for (int k = 0; k < i->x; k++) { /*For loop through pixl matrix*/
            for (int l = 0; l < i->y; l++) {
                fprintf(fp, "%d %d %d\n", i->pixels[k][l].r, i->pixels[k][l].g, i->pixels[k][l].b);
            }
        }
    }
    if(i->x > i->y) { /*Same as above, but for Horizontal images: Loops through new img and writes to newModeImage the contents of img_new*/
        for (int k = 0; k < i->y; k++) { /*For loop through pixl matrix*/
            for (int l = 0; l < i->x; l++) {
                fprintf(fp, "%d %d %d\n", i->pixels[k][l].r, i->pixels[k][l].g, i->pixels[k][l].b);
            }
        }
    }
    return i;
}

/**
 * Allocates memory of an image given to it based on dimensions of another image passed to it
 * If branches to determine whether or not a vertical or horizontal image for efficiency.
 * @param img* img_new
 * @param img* old
 * @return img* img_new
 */
img* memAllocImg(img* img_new, img* old){ /*OLD Image used to obtain dimensions to correctly allocate new image*/
    img_new = (img*) calloc(1, sizeof(img)); /*Creates img* img_new which will be the newly created image*/

    if (old->x > old->y) { /*If a horizontal image: Allocates memory for the img_new's pixl matrix based on dimensions of any given image*/
        img_new->pixels = (pixl **) calloc(old->x * 3, sizeof(pixl *));
        for (int j = 0; j < old->x; j++) {
            img_new->pixels[j] = (pixl *) calloc(old->y * 3, sizeof(pixl));
        }
    }
    if (old->y > old->x) { /*Same as above, but if a vertical image: Allocates memory for the img_new's pixl matrix based on dimensions of any given image*/
        img_new->pixels = (pixl **) calloc(old->y * 3, sizeof(pixl *));
        for (int j = 0; j < old->y; j++) {
            img_new->pixels[j] = (pixl *) calloc(old->x * 3, sizeof(pixl));
        }
    }
    return img_new;
}
/**
 * Writes to an image the data of the mode of an array of image pointers.
 * @param img* img_new
 * @param img** old
 * @param int img_count
 * @return img* img_new
 */
img* newImageData(img* img_new, img** old, int img_count){
    /*Sets header of new image equal to one of the img_arr's given images*/
    img_new->x = old[0]->x;
    img_new->y = old[0]->y;
    img_new->max = old[0]->max;
    for (int k = 0; k < img_new->y; k++) { /*For loop through pixl matrix*/
        for (int l = 0; l < img_new->x; l++) {
            int red[img_count]; /*In each loop, creates an int array of size img_count for each color r,b,g.*/
            int green[img_count];
            int blue[img_count];
            /*Now to take mode of each pixl(rgb) and assign to new image*/
            for (int j = 0; j < img_count; j++) { /*For loops through each image in img_arr setting the image's r,g,b of index j equal to the red, green, blue of index j*/
                red[j] = old[j]->pixels[k][l].r;
                green[j] = old[j]->pixels[k][l].g;
                blue[j] = old[j]->pixels[k][l].b;
            }
            /*Now that each array red, green, and blue are filled with the values from each image, it finds the mode and returns that as the new image's r,g,b values*/
            img_new->pixels[k][l].r = findmode(red, 3);
            img_new->pixels[k][l].g = findmode(green, 3);
            img_new->pixels[k][l].b = findmode(blue, 3);
        }
    }
    return img_new;
}

/**
 * Obtains amount of images within a directory. That number is used several times throughout the program.
 * @param DIR* myDirectory
 * @param struct dirent* inFile
 * @return int img_count
 */
int getImageCount(DIR *myDirectory, struct dirent *inFile){
    int img_count = 0;
    while ((inFile = readdir(myDirectory)) != NULL) { /*Scans through entire directory setting inFile->d_name to each file*/
        if (!strcmp(inFile->d_name, ".")) /*The two if branches ignore current and parent folders*/
            continue;
        if (!strcmp(inFile->d_name, ".."))
            continue;
        else { /*Each files goes to this else branch*/
            img_count++; /*Increments the img_count for each ppm file to know how large to dynamically allocate the array of FILES*/
        }
    }
    return img_count;
}

/**
 * Main function
 * @return
 */
int main() {
    printf("THE PROGRAM WILL ONLY WORK ON WINDOWS!!!\n\n\n");
    FILE *source = fopen("source.txt", "rb"); /*Source Text file that is to be located in project folder. Contains only absolute path of wanted folder containing ppms*/
    if(source == NULL){ /*Checks if it is empty or not*/
        printf("Source file containing full path of folder containing ppm images is unable to be read");
        exit (1);
    }
    char mypath[255]; /*Path of Directory.*/
    fgets(mypath, 255, source);  /*Program reads source folder containing text of wanted path that contains ppm images at runtime.*/
    printf("%s PATH\n", mypath);
    DIR *myDirectory; /*Directory*/
    myDirectory = opendir(mypath); /*Opens the myDirectory to mypath*/
    if (myDirectory == NULL){ /*Checks if myDirectory is null*/
        printf("Unable to open since directory is empty");
        return 1;
    }
    struct dirent *inFile; /*Will be used to scan through entire directory*/
    int img_count = getImageCount(myDirectory, inFile); /*Img_count is obtained to know how many ppm files are to be used*/
    FILE **files = (FILE**) calloc(img_count * 3, sizeof(FILE*)); /*Creates an array of FILE pointers with size based on a amount of files in directory*/
    rewinddir(myDirectory); /*Rewinds directory to loop through the directory again*/


    char strs[img_count][300]; /*Array of "strings" (array of array of chars) that will contain full paths of the ppm's*/
    int itr = 0; /*integer itr initialized to 0 which will make sure the next while loop stays within amount of images*/
    while (((inFile = readdir(myDirectory)) != NULL) && itr < img_count) { /*Scans through entire directory setting inFile->d_name to each file*/
        if (!strcmp(inFile->d_name, ".")) /*The two if branches ignore current and parent folders*/
            continue;
        if (!strcmp(inFile->d_name, ".."))
            continue;
        else { /*Each files goes to this else branch*/
            strcpy(strs[itr], inFile->d_name); /*Copies name of each image to the array of strings*/
            prepend(strs[itr], "\\"); /*Prepends "\\" then the mypath to each image name in strs to obtain the absolute path of each image*/
            prepend(strs[itr], mypath);
            itr++; /*Increments itr*/
        }
    }
    img **img_arr = (img**) calloc(img_count * 3, sizeof(img*)); /*Making Array of pointers to images and allocating memory to it. Each of these in the array will be one of the ppms in the directory*/
    for (int j = 0; j < img_count; j++){
        img_arr[j] = (img*)calloc(img_count * 3, sizeof(img));  /*Malloc each image pointer in array of images based on amount of ppms*/
    }
    for (int j = 0; j < img_count; j++){ /*Loops through FILE array "files" and opens the absolute path then reads it and gives each returned value to the img_arr's img*/
        printf("%s\n", strs[j]);
        files[j] = fopen(strs[j], "rb");
        img_arr[j] = readImage(files[j]);
    }
    img* img_new = memAllocImg(img_new, img_arr[0]); /*Creates a new image (the final outputted one) and memory allocates it*/
    img_new = newImageData(img_new, img_arr, img_count); /* Gets the data of the new image*/
    FILE* newModeImage = fopen("output.ppm", "wb"); /*Creates the new file "NewModeImage.ppm" and opens it*/
    newModeImage = writeImage(newModeImage, img_new); /*Writes to it using data of the img_new*/
    printf("FILE SUCCESSFULLY CREATED. LOOK FOR IN PROJECT'S FOLDER OR SUBFOLDER DEPENDING ON IDE\n\n");
    return 0;
}