#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", filename);
        return 1;
    }

    unsigned char byte;
    while (fread(&byte, sizeof(byte), 1, fp) == 1) {
        printf("%02x ", byte);
    }

    fclose(fp);
    return 0;
}
