#define _XOPEN_SOURCE   600 /* for posix_memalign */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define IN_FILENAME     "in.ppm"
#define OUT_FILENAME    "out.ppm"
#define PPM_MAGIC       "P6"

extern void rgb2y(unsigned char *src, unsigned char *dst, int num);

typedef struct _image_t {
    int width;
    int height;
    unsigned char *src;
    unsigned char *dst;
} image_t;

/*
 * read ppm data from input file
 */
void read_ppm(image_t *img, char *fname)
{
    char *token, *pc, *buf, *del = " \t\n";
    int i, w, h, luma, pixs, filesize;
    struct stat st;
    unsigned char *dot;
    FILE *fp;

    /* read raw data */
    stat(fname, &st);
    filesize = (int) st.st_size;
    buf = (char *) malloc(filesize * sizeof(char));

    if ((fp = fopen(fname, "r")) == NULL) {
        fprintf(stderr, "error: failed to open file %s\n", fname);
        exit(1);
    }

    fseek(fp, 0, SEEK_SET);
    fread(buf, filesize * sizeof(char), 1, fp);
    fclose(fp);

    /* validate file format */
    token = (char *) (unsigned long) strtok(buf, del);
    if (strncmp(token, PPM_MAGIC, 2) != 0) {
        fprintf(stderr, "error: invalid file format\n");
        exit(1);
    }

    /* skip comments */
    token = (char *) (unsigned long) strtok(NULL, del);
    if (token[0] == '#') {
        token = (char *) (unsigned long) strtok(NULL, "\n");
        token = (char *) (unsigned long) strtok(NULL, del);
    }

    /* read picture size (and luma) */
    w = strtoul(token, &pc, 10);
    token = (char *) (unsigned long) strtok(NULL, del);
    h = strtoul(token, &pc, 10);
    token = (char *) (unsigned long) strtok(NULL, del);
    luma = strtoul(token, &pc, 10);

    img->width = w;
    img->height = h;

    /* allocate an aligned memory */
    pixs = w * h;
    posix_memalign((void **) &img->src, 16, pixs*4);
    posix_memalign((void **) &img->dst, 16, pixs*4);

    /* read rgb data with 'r,g,b,0' formatted */
    dot = img->src;
    pc++;
    for (i = 0; i < pixs*4; i++) {
        if (i % 4 == 3) {
            *dot++ = 0;
        } else {
            *dot++ = *pc++;
        }
    }

    return;
}

void write_ppm(image_t *img, char *fname)
{
    int i;
    int w = img->width;
    int h = img->height;
    unsigned char *dot = img->dst;
    FILE *fp;

    if ((fp = fopen(fname, "wb+")) == NULL) {
        fprintf(stderr, "failed to open file %s\n", fname);
        exit(1);
    }

    fprintf(fp, "%s\n%d %d\n255\n", PPM_MAGIC, w, h);

    for (i = 0; i < (w * h * 4); i++) {
        if (i % 4 == 3) {
            dot++;
        } else {
            putc((int) *dot++, fp);
        }
    }

    fclose(fp);
    
    return;
}

void delete_image(image_t *img)
{
    free(img->src);
    free(img->dst);

    return;
}

int main(int argc, char **argv)
{
    image_t image;

    read_ppm(&image, IN_FILENAME);

    rgb2y(image.src, image.dst, image.width * image.height);

    write_ppm(&image, OUT_FILENAME);
    delete_image(&image);

    return 0;
}
