#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <data_space.h>

#define MAXLINELENGTH 200
#define MAXTOKLENGTH 100

int main(void) {
  // Open the file with the data
  const char* filename = "examples/sine_data.csv";
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Could not read %s: %s\n", filename, strerror(errno));
    return 1;
  }

  // Figure out the size of the file
  struct stat filestats;
  int res = fstat(fd, &filestats);
  if (res != 0) {
    fprintf(stderr, "Could not stat %s: %s\n", filename, strerror(errno));
    close(fd);
    return 1;
  }

  size_t file_size = (size_t)filestats.st_size;

  // Map the file into writeable memory
  char *file_contents = mmap(NULL, file_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (file_contents == MAP_FAILED) {
    fprintf(stderr, "Could not mmap %s: %s\n", filename, strerror(errno));
    return 1;
  }

  // size_t line_count = 0;
  // size_t chr_ind = 0;
  // while (file_contents[chr_ind] != '\0') {
  //   if (file_contents[chr_ind++] == '\n') {
  //     line_count++;
  //   }
  // }
  
  // Parse the file
  dyn_array_float xs = new_dyn_arr_float();
  dyn_array_float ys = new_dyn_arr_float();
  char *line = malloc(MAXLINELENGTH * sizeof(char));
  line = strtok(file_contents, "\n");
  while (line) {
    float x, y;
    int processed = sscanf(line, "%f, %f", &x, &y);
    if (processed != 2) {
      printf("Huh?\n");
    }
    add_to_dyn_arr_float(&xs, x);
    add_to_dyn_arr_float(&ys, y);
    line = strtok(NULL, "\n");
  }

  close(fd);
  free(line);
  munmap(file_contents, file_size);

  return 0;
}
