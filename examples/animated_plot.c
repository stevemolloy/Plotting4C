#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"
#include "data_space.h"
#include "view_area.h"

#define WINDOWTITLE "Data visualisation with a game engine"
#define AXISCOLOR DARKGRAY
#define DATACOLOR DARKBLUE
#define MARKERSIZE 2
#define MARGIN 5

size_t get_data(dyn_array_float *x_data, dyn_array_float *y_data) {
  size_t N = 1024;
  x_data->size = 0;
  y_data->size = 0;

  static float ph = 0.0f;
  ph += 0.002f;

  for (size_t i=0; i<N; i++) {
    add_to_dyn_arr_float(x_data, (float)i);
    add_to_dyn_arr_float(y_data, expf(-(float)i/100.0f) * sinf(2.0f*PI*0.05f * (float)i + ph));
  }

  return N;
}

int main(void) {
  dyn_array_float x_data = new_dyn_arr_float();
  dyn_array_float y_data = new_dyn_arr_float();
  size_t N;
  
  int window_width = 800;
  int window_height = 600;
  InitWindow(window_width, window_height, WINDOWTITLE);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  N = get_data(&x_data, &y_data);
  Data_Space data_space = new_data_space(x_data, y_data, N);
  Rectangle zoom_rect = (Rectangle){
    .x = data_space.x_axis.ticks[0],
    .y = data_space.y_axis.ticks[0],
    .width = data_space.x_axis.ticks[data_space.x_axis.num_ticks - 1] - data_space.x_axis.ticks[0],
    .height = data_space.y_axis.ticks[data_space.y_axis.num_ticks - 1] - data_space.y_axis.ticks[0],
  };
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
    EndDrawing();

    N = get_data(&x_data, &y_data);
    data_space = new_data_space(x_data, y_data, N);
  }

  free(x_data.vals);
  free(y_data.vals);
  return 0;
}

