#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main() {
    DIR *directory;
    struct dirent *entry;

    // Open the current directory
    directory = opendir(".");

    if (directory == NULL) {
        perror("Unable to open current directory");
        return 1;
    }

    // Read and print all entries in the directory
    while ((entry = readdir(directory)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    

    // Close the directory
    closedir(directory);


    char filename[20];
    scanf("Enter name: %19s\n", filename);
    if (scanf("%19s", filename) == 1) {
        // Input was successfully read
        printf("You entered: %s\n", filename);
    } else {
        // Input was not successful
        printf("Invalid input.\n");
    }

    // Check if the file exists
    if (access(filename, F_OK) == 0) {
        printf("true\n");
    } else {
        printf("false\n");
    }

    

    return 0;
}
