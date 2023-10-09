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
  while (x_data->capacity < N) expand_dyn_arr_float(x_data);
  while (y_data->capacity < N) expand_dyn_arr_float(y_data);

  static float ph = 0.0f;
  ph += 0.002f;
  for (size_t i=0; i<N; i++) {
    x_data->vals[i] = (float)i;
    y_data->vals[i] = expf(-(float)i/100.0f) * sinf(2.0f*PI*0.05f * (float)i + ph);
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

  Data_Space data_space;
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

    N = get_data(&x_data, &y_data);
    data_space = new_data_space(x_data, y_data, N);

    BeginDrawing();
      ClearBackground(WHITE);
      draw_axes(data_space, view_area, AXISCOLOR);
      plot_data(data_space, view_area, MARKERSIZE, DATACOLOR);
    EndDrawing();
  }

  free(x_data.vals);
  free(y_data.vals);
  return 0;
}

