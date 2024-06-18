// COEN 146L : Lab 1 - template to use for file transfer (steps 3, 4, and 5)
// You need to adapt this template according to the requirements of each of the steps

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()
#include <sys/mman.h> // mmap()

#define BUF_SIZE 2048 //buffer size

// copies a files from src_filename to dst_filename using functions fopen(), fread(), fwrite(), fclose()
int func_copy(char* src_filename, char* dst_filename) {
  printf("func has been called\n");
	FILE *src, *dst;
  char* buf;

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

  size_t bytes_read;
	// reads content of file is loop iterations until end of file
	// writes bytes_read to dst_filename 
  while ((bytes_read = fread(buf, 1, BUF_SIZE, src)) > 0) {
    // Write the read data to the destination file
    fwrite(buf, 1, bytes_read, dst);
  }
	
	// closes src file pointer
  fclose(src);
  fclose(dst);
	// closes dst file pointer
	// frees memory used for buf
  free(buf);
	return 0;
}

// copies a files form src_filename to dst_filename using syscalls open(), read(), write(), close()
int syscall_copy(char* src_filename, char* dst_filename){
  printf("syscall has been called\n");
  int src_fd, dst_fd;
  char *buf;

    // Open source file for reading
    src_fd = open(src_filename, O_RDONLY);
    if (src_fd < 0) {
        fprintf(stderr, "Unable to open %s for reading: %i\n", src_filename, errno);
        exit(EXIT_FAILURE);
    }

    // Open destination file for writing (create if doesn't exist, truncate existing file)
    dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        fprintf(stderr, "Unable to open/create %s for writing: %i\n", dst_filename, errno);
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    // Allocate a buffer to store read data
    buf = malloc(BUF_SIZE);

    size_t bytes_read, bytes_written;

    //read until end of file
    while ((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0) {
        bytes_written = write(dst_fd, buf, bytes_read);
    }

    //close files
    close(src_fd);
    close(dst_fd);

    //frees buffer
    free(buf);

    return 0;
}

// checks the runtime to copy a file using functions vs syscalls
void check_copy_times(char* src_filename, char* dst_filename) {
	clock_t func_start, func_end, syscall_start, syscall_end;
	double func_time, syscall_time;
	// capture runtime of func_copy() using start clock, call the copy, end the clock
  func_start = clock();
  func_copy(src_filename, dst_filename);
  func_end = clock();

	// capture runtime of syscall_copy() using start clock, call the copy, end the clock
  syscall_start = clock();
  syscall_copy(src_filename, dst_filename);
  syscall_end = clock();

	func_time = ((double) (func_end - func_start)) / CLOCKS_PER_SEC;
  syscall_time = ((double) (syscall_end - syscall_start)) / CLOCKS_PER_SEC;

	printf("time to copy using functions: %.7f\n", func_time);
	printf("time to copy using syscalls: %.7f\n", syscall_time);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {  // check correct usage
		fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
		exit(1);
	}
	//get the source and destination files from the command line arguments
  char* source = argv[1];
  char* dest = argv[2];

  //call the check copy times function
  check_copy_times(source, dest);
	
	return 0;
}