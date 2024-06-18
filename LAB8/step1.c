#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    FILE *file;
    int MAX_LINES = 30;
    int MAX_CHARS = 30;
    char *lines[MAX_LINES];  
    char buffer[MAX_CHARS];
    int count = 0;

    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);
    int random_number = rand();
    int arr[26];

    //generate array and scramble numbers
    int i, temp, a, b;
    for(i = 0; i < 26; i++) {
        arr[i] = i;
    }
    for(i = 0; i < 100; i++) {
        a = rand() % 26;
        b = rand() % 26;
        temp = arr[a];
        arr[a] = arr[b];
        arr[b] = temp;
    }
    for(i = 0; i < 26; i++) {
        printf("%d, ", arr[i]);
    }

    file = fopen("text.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), file)) {
        // Allocate memory for the line
        lines[count] = malloc(strlen(buffer) + 1);
        if (lines[count] == NULL) {
            perror("Error allocating memory");
            fclose(file);
            return 1;
        }

        // Copy the line to the allocated memory
        strcpy(lines[count], buffer);
        count++;

        // Check if we reached the maximum number of lines
        if (count >= MAX_LINES) {
            printf("Maximum number of lines reached\n");
            break;
        }
    }

    printf("%c", lines[0][0]);

    

    

    return 0;
}