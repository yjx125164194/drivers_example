#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

void encrypt(FILE* input, FILE* output, const char* password);
void decrypt(FILE* input, FILE* output, const char* password);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s [encrypt/decrypt] [file path] [password]\n", argv[0]);
        return 1;
    }

    const char* mode = argv[1];
    const char* file_path = argv[2];
    const char* password = argv[3];

    FILE* input = fopen(file_path, "rb");
    if (!input) {
        printf("Error opening input file.\n");
        return 1;
    }

    char output_file_name[strlen(file_path) + 5];
    strcpy(output_file_name, file_path);
    
    if (strcmp(mode, "encrypt") == 0) {
    	strcat(output_file_name, ".enc");
    } else if (strcmp(mode, "decrypt") == 0) {
    	strcat(output_file_name, ".dec");
    } else {
        printf("Unknown operation: %s\n", mode);
        fclose(input);
        return 1;
    }

    FILE* output = fopen(output_file_name, "wb");
    if (!output) {
        printf("Error opening output file.\n");
        fclose(input);
        return 1;
    }

    if (strcmp(mode, "encrypt") == 0) {
        encrypt(input, output, password);
    } else if (strcmp(mode, "decrypt") == 0) {
        decrypt(input, output, password);
    } else {
        printf("Unknown operation: %s\n", mode);
        fclose(input);
        fclose(output);
        return 1;
    }

    fclose(input);
    fclose(output);

    return 0;
}

void encrypt(FILE* input, FILE* output, const char* password) {
    char buf[BUF_SIZE];
    int len = strlen(password);
    int i = 0;

    while (1) {
        size_t bytes_read = fread(buf, 1, BUF_SIZE, input);

        if (bytes_read == 0) {
            break;
        }

        for (size_t j = 0; j < bytes_read; j++) {
            buf[j] ^= password[i++ % len];
        }

        fwrite(buf, 1, bytes_read, output);
    }
}

void decrypt(FILE* input, FILE* output, const char* password) {
    char buf[BUF_SIZE];
    int len = strlen(password);
    int i = 0;

    while (1) {
        size_t bytes_read = fread(buf, 1, BUF_SIZE, input);

        if (bytes_read == 0) {
            break;
        }

        for (size_t j = 0; j < bytes_read; j++) {
            buf[j] ^= password[i++ % len];
        }

        fwrite(buf, 1, bytes_read, output);
    }
}
