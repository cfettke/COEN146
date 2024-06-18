// COEN 146L : Lab 2 - template to use for creating multiple thread to make file transfers (step 1)
#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()
#include <string.h>


#define BUF_SIZE 2048 // buffer size

// data structure to hold copy
typedef struct copy_struct {
  int thread_id;
  char *src_filename;
  char *dst_filename;
} COPY;

// copies from src_filename to dst_filename using functions fopen(),
// fread(), fwrite(), fclose()
static int func_copy(char *src_file, char *dst_file) {
  FILE *src = fopen(src_file, "r"); // opens a file for reading
  if (src == NULL) {                // fopen() error checking
    fprintf(stderr, "unable to open %s for reading: %i\n", src_file, errno);
    exit(0);
  }

  FILE *dst = fopen(dst_file, "w");
  // opens a file for writing; erases old file/creates a new file
  if (dst == NULL) { // fopen() error checking
    fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_file,
            errno);
    exit(0);
  }

  void *buf = malloc((size_t)BUF_SIZE); // allocate a buffer to store read data
  int bytes_read = 0;
  while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0) {
    // reads content of file is loop iterations until end of file
    fwrite(buf, 1, bytes_read, dst); // writes bytes_read to dst_filename
  }

  fclose(src); // closes src file pointer
  fclose(dst); // closes dst file pointer
  free(buf);   // frees memory used for buf
  return 0;
}

// thread function to copy one file
static void *copy_thread(void *arg) {
  COPY params = *(COPY *)arg; // cast/dereference void* to copy_struct
  printf("thread[%i] - copying %s to %s\n", params.thread_id,
         params.src_filename, params.dst_filename);
  // call file copy function
  func_copy(params.src_filename, params.dst_filename);

  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  if (argc < 3) { // check correct usage
    fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
    exit(1);
  }

  int i;
  char *src_filename;
  char *dst_filename;
  int num_threads = (argc - 1) / 2; // number of threads to create
  // char *src_filenames[num_threads]; // array of source files
  // char *dst_filenames[num_threads]; // array of desintation files

  pthread_t threads[num_threads];  // initialize threads
  COPY thread_params[num_threads]; // structure for each thread

  // initialize thread parameters and create each copy thread
  for (i = 0; i < num_threads; i++) {
    src_filename = argv[i * 2 + 1];
    dst_filename = argv[i * 2 + 2];
    // src_filenames[i] = src_filename;
    // dst_filenames[i] = dst_filename;

    thread_params[i].thread_id = i;
    thread_params[i].src_filename = src_filename;
    thread_params[i].dst_filename = dst_filename;

    pthread_create(&threads[i], NULL, copy_thread, (void *)&thread_params[i]);
  }

  // wait for all threads to finish
  for (i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_exit(NULL);
}











/*
// Function to copy a file
void copyFile(const char *sourceFile, const char *destinationFile) {
    FILE *source, *destination;
    char ch;

    source = fopen(sourceFile, "rb");
    if (source == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    destination = fopen(destinationFile, "wb");
    if (destination == NULL) {
        perror("Error opening destination file");
        fclose(source);
        exit(EXIT_FAILURE);
    }

    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, destination);
    }

    fclose(source);
    fclose(destination);
}

// Function executed by each thread
void *copy_thread(void *arg) {
    struct copy_struct *copyInfo = (struct copy_struct *)arg;
    printf("Thread %d copying from %s to %s\n", copyInfo->thread_id, copyInfo->src_filename, copyInfo->dst_filename);
    copyFile(copyInfo->src_filename, copyInfo->dst_filename);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
  if (argc < 3) { // check correct usage
    fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
    exit(1);
  }

  int i;
  char *src_filename;
  char *dst_filename;
  int num_threads = (argc - 1) / 2; // number of threads to create
  // char *src_filenames[num_threads]; // array of source files
  // char *dst_filenames[num_threads]; // array of desintation files

  pthread_t threads[num_threads];  // initialize threads
  COPY thread_params[num_threads]; // structure for each thread

  // initialize thread parameters and create each copy thread
  for (i = 0; i < num_threads; i++) {
    src_filename = argv[i * 2 + 1];
    dst_filename = argv[i * 2 + 2];
    // src_filenames[i] = src_filename;
    // dst_filenames[i] = dst_filename;

    thread_params[i].thread_id = i;
    thread_params[i].src_filename = src_filename;
    thread_params[i].dst_filename = dst_filename;

    pthread_create(&threads[i], NULL, copy_thread, (void *)&thread_params[i]);
  }

  // wait for all threads to finish
  for (i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_exit(NULL);
}
*/

/*

int main() {
    pthread_t threads[NUM_THREADS];
    struct copy_struct copyInfo[NUM_THREADS];

    // Initialize parameters for each thread
	int i;
	char* intVal;
    for(i = 0; i < NUM_THREADS; ++i) {
        char* newString = (char*)malloc(10);   // Adjust the array size as needed based on the expected length of your final string

        // Combine the value of i with the string "hello"
        copyInfo[i].dst_filename = "000000000";
        copyInfo[i].dst_filename = "000000000";
        copyInfo[i].thread_id = i;
        sprintf(newString, "%ds.dat", i + 1);
        printf("%s, ", newString);
        printf("break 1\n");
        strcpy(copyInfo[i].src_filename, newString);
        printf("break 2\n");
        //copyInfo[i].src_filename = newString; // Replace with actual source file names
        
        printf("%s, ", newString);
        printf("break 3\n");
        //sprintf(newString, "%dd.dat", i + 1);
        copyInfo[i].dst_filename = "2d.dat"; // Replace with actual destination file names
		
        //copyInfo[i].src_filename = "1s.dat"; // Replace with actual source file names
        //copyInfo[i].dst_filename = "1d.dat"; // Replace with actual destination file names
		
		//strcat(intVal, copyInfo[i].src_filename);
		//strcat(intVal, copyInfo[i].dst_filename);
		//printf("%s ", copyInfo[i].src_filename);
		

        if(pthread_create(&threads[i], NULL, copyThread, (void *)&copyInfo[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for(i = 0; i < NUM_THREADS; ++i) {
        if(pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    printf("All files copied successfully.\n");

    return 0;
}

*/













/*




#define BUF_SIZE 2048 //buffer size
#define NUM_THREADS 3

// data structure to hold copy
struct copy_struct {
	int thread_id; 
	char* src_filename;
	char* dst_filename; 
};



void copyFile(const char *sourceFile, const char *destinationFile) {
    FILE *source, *destination;
    char ch;

    source = fopen(sourceFile, "rb");
    if (source == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    destination = fopen(destinationFile, "wb");
    if (destination == NULL) {
        perror("Error opening destination file");
        fclose(source);
        exit(EXIT_FAILURE);
    }

    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, destination);
    }

    fclose(source);
    fclose(destination);
}




// copies a files from src_filename to dst_filename using functions fopen(), fread(), fwrite(), fclose()
int func_copy(char* src_filename, char* dst_filename) {
	
	src = fopen(src_filename, "r");	// opens a file for reading
	if (src == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		exit(0);
	}
	dst = fopen(dst_filename, "w");	// opens a file for writing; erases old file/creates a new file
	if (dst == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		exit(0);
	}

	buf = malloc((size_t)BUF_SIZE);  // allocate a buffer to store read data
		buf = malloc((size_t)BUF_SIZE);  // allocate a buffer to store read data

    size_t bytes_read;
	// reads content of file is loop iterations until end of file
	// writes bytes_read to dst_filename 
    while ((bytes_read = fread(buf, 1, BUF_SIZE, src)) > 0) {
    	// Write the read data to the destination file
    	fwrite(buf, 1, bytes_read, dst);
    }
	
	// closes files
    fclose(src);
    fclose(dst);
	
	// frees memory used for buf
    free(buf);
	return 0;
}

// thread function to copy one file
void* copy_thread(void* arg) {
	struct copy_struct params = *(struct copy_struct*)arg;  // cast/dereference void* to copy_struct
	printf("thread[%i] - copying %s to %s\n", params.thread_id, params.src_filename, params.dst_filename);
	//call file copy function
	func_copy(copyInfo->src_filename, copyInfo->dst_filename);
	
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	/*
	 // check correct usage of arguments in command line
	if (argc != ....) {  
		fprintf(stderr, "usage: %s <n connections> ........\n", argv[0]);
		exit(1);
	}
	char* src_filename;
	char* dst_filename;
	char* src_filenames[]; // array of source files
	char* dst_filenames[]; // array of desintation files
	int num_threads; // number of threads to create
	
	
	pthread_t threads[num_threads]; //initialize threads
	struct copy_struct thread_params[num_threads]; // structure for each thread
	int i;
	for (i = 0; i < num_threads; i++) {
		copyInfo[i].thread_id = i;
		// initialize thread parameters
		// create each copy thread
		// use pthread_create(.....);
	}

	// wait for all threads to finish
	for (i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	pthread_exit(NULL);
	
	pthread_t threads[NUM_THREADS];
    struct copy_struct copyInfo[NUM_THREADS];

	// Initialize parameters for each thread
    for (int i = 0; i < NUM_THREADS; ++i) {
        copyInfo[i].thread_id = i;
        copyInfo[i].src_filename = "source_file.txt"; // Replace with actual source file names
        copyInfo[i].dst_filename = "destination_file.txt"; // Replace with actual destination file names

        if (pthread_create(&threads[i], NULL, copyThread, (void *)&copyInfo[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    printf("All files copied successfully.\n");

    return 0;
}

*/