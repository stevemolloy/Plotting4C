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
#define WINDOWTITLE "Data visualisation with a game engine"
#define MARGIN 5
#define AXISCOLOR DARKGRAY
#define DATACOLOR DARKBLUE
#define MARKERSIZE 2

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

  size_t N = 0;
  size_t chr_ind = 0;
  while (file_contents[chr_ind] != '\0') {
    if (file_contents[chr_ind++] == '\n') {
      N++;
    }
  }
  
  // Parse the file
  dyn_array_float x_data = new_dyn_arr_float();
  dyn_array_float y_data = new_dyn_arr_float();
  char *line = malloc(MAXLINELENGTH * sizeof(char));
  line = strtok(file_contents, "\n");
  while (line) {
    float x, y;
    int processed = sscanf(line, "%f, %f", &x, &y);
    if (processed != 2) {
      printf("Huh?\n");
    }
    add_to_dyn_arr_float(&x_data, x);
    add_to_dyn_arr_float(&y_data, y);
    line = strtok(NULL, "\n");
  }

  Data_Space data_space = new_data_space(x_data, y_data, N);

  int window_width = 800;
  int window_height = 600;
  InitWindow(window_width, window_height, WINDOWTITLE);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(10);

  View_Area view_area;
  while (!WindowShouldClose()) {
    window_width = GetScreenWidth();
    window_height = GetScreenHeight();

    view_area = (View_Area){
      .x = 0.0f + MARGIN,
      .y = 0.0f + MARGIN,
      .width = (float)window_width - 2.0f * MARGIN,
      .height = (float)window_height - 2.0f * MARGIN,
    };

    BeginDrawing();
      ClearBackground(WHITE);
      draw_axes(data_space, view_area, AXISCOLOR);
      plot_data(data_space, view_area, MARKERSIZE, DATACOLOR);
    EndDrawing();
  }

  close(fd);
  free(line);
  munmap(file_contents, file_size);

  return 0;
}
