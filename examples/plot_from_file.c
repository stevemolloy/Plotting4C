#include <raylib.h>
#include <raymath.h>

#include "data_space.h"
#include "plotting.h"

#define MAXTOKLENGTH 100
#define WINDOWTITLE "Data visualisation with a game engine"
#define MARGIN 5
#define AXISCOLOR DARKGRAY
#define DATACOLOR DARKBLUE
#define MARKERSIZE 2

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
  Rectangle zoom_rect = (Rectangle){
    .x = data_space.x_axis.ticks[0],
    .y = data_space.y_axis.ticks[0],
    .width = data_space.x_axis.ticks[data_space.x_axis.num_ticks - 1] - data_space.x_axis.ticks[0],
    .height = data_space.y_axis.ticks[data_space.y_axis.num_ticks - 1] - data_space.y_axis.ticks[0],
  };

  // The GUI
  int window_width = 800;
  int window_height = 600;
  InitWindow(window_width, window_height, WINDOWTITLE);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

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

      draw_axes(data_space, zoom_rect, view_area, AXISCOLOR);
      plot_data(data_space, zoom_rect, view_area, MARKERSIZE, DATACOLOR);

      activate_plot_dragging(&zoom_rect, view_area);
      activate_box_zoom(&zoom_rect, view_area);
      activate_scroll_zoom(&zoom_rect, view_area);

      if (IsKeyPressed(KEY_R)) {
        zoom_rect = (Rectangle){
          .x = data_space.x_axis.ticks[0],
          .y = data_space.y_axis.ticks[0],
          .width = data_space.x_axis.ticks[data_space.x_axis.num_ticks - 1] - data_space.x_axis.ticks[0],
          .height = data_space.y_axis.ticks[data_space.y_axis.num_ticks - 1] - data_space.y_axis.ticks[0],
        };
        TraceLog(LOG_INFO, "Recreating data_space from the input data");
      }
    EndDrawing();
  }


  return 0;
}
