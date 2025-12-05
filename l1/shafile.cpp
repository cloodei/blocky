#include <iostream>
#include <cmath>
#include <string>
typedef __int128 i128;
using namespace std;

    FILE *file = fopen("filename.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1; // Or handle the error appropriately
    }

        fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

        char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

        size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return 1;
    }
        buffer[file_size] = '\0';
            fclose(file);
                // Use 'buffer' here
    printf("File content:\n%s\n", buffer);

    free(buffer);

int main(int argc, char** argv) {
    if (argc > 2) {
    	cerr << '?';
    	return -1;
    }



    return 0;
}
