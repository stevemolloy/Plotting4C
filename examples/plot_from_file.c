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

size_t load_two_column_csv(char *fname, dyn_array_float *xs, dyn_array_float *ys) {
  int fd = open(fname, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Could not read %s: %s\n", fname, strerror(errno));
    return 1;
  }

  // Figure out the size of the file
  struct stat filestats;
  int res = fstat(fd, &filestats);
  if (res != 0) {
    fprintf(stderr, "Could not stat %s: %s\n", fname, strerror(errno));
    close(fd);
    return 1;
  }

  size_t file_size = (size_t)filestats.st_size;

  // Map the file into writeable memory
  char *file_contents = mmap(NULL, file_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (file_contents == MAP_FAILED) {
    fprintf(stderr, "Could not mmap %s: %s\n", fname, strerror(errno));
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
  char *line = malloc(MAXLINELENGTH * sizeof(char));
  line = strtok(file_contents, "\n");
  while (line) {
    float x, y;
    int processed = sscanf(line, "%f, %f", &x, &y);
    if (processed != 2) {
      printf("Huh?\n");
    }
    add_to_dyn_arr_float(xs, x);
    add_to_dyn_arr_float(ys, y);
    line = strtok(NULL, "\n");
  }

  munmap(file_contents, file_size);
  free(line);
  close(fd);

  return N;
}

int main(void) {
  // The file with the data
  char* filename = "examples/sine_data.csv";

  // Somewhere to store the data
  dyn_array_float x_data = new_dyn_arr_float();
  dyn_array_float y_data = new_dyn_arr_float();

  // Fill the data stores
  size_t N = load_two_column_csv(filename, &x_data, &y_data);

  // Prepare the data space for plotting
  Data_Space data_space = new_data_space(x_data, y_data, N);

  // The GUI
  int window_width = 800;
  int window_height = 600;
  InitWindow(window_width, window_height, WINDOWTITLE);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);
  bool drawing_zoom = false;
  Rectangle zoom_rect = {0};
  Vector2 zoom_start_pos = {0};

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

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_pos = GetMousePosition();
        if (CheckCollisionPointRec(mouse_pos, view_area)) {
          if (!drawing_zoom) {
            zoom_start_pos = mouse_pos;
            TraceLog(LOG_INFO, "Drawing zoom");
            drawing_zoom = true;
            zoom_rect = (Rectangle) {
              .x = zoom_start_pos.x,
              .y = zoom_start_pos.y,
              .width = 0.0f,
              .height = 0.0f,
            };
          }
          if (mouse_pos.x < zoom_start_pos.x) {
            zoom_rect.x = mouse_pos.x;
            zoom_rect.width = zoom_start_pos.x - mouse_pos.x;
          } else {
            zoom_rect.x = zoom_start_pos.x;
            zoom_rect.width = mouse_pos.x - zoom_start_pos.x;
          }
          if (mouse_pos.y < zoom_start_pos.y) {
            zoom_rect.y = mouse_pos.y;
            zoom_rect.height = zoom_start_pos.y - mouse_pos.y;
          } else {
            zoom_rect.y = zoom_start_pos.y;
            zoom_rect.height = mouse_pos.y - zoom_start_pos.y;
          }
        }
        DrawRectangleLinesEx(zoom_rect, 2.0f, BLACK);
      }
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) & drawing_zoom) {
        drawing_zoom = false;
        TraceLog(LOG_INFO, "Zoom!");
      }
    EndDrawing();
  }


  return 0;
}
