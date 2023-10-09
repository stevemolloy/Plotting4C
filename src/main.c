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

size_t get_data(float* x_vec, float* y_vec, size_t N) {
  static float ph = 0;
  ph += 0.002f;
  for (size_t i=0; i<N; i++) {
    x_vec[i] = (float)i;
    y_vec[i] = expf(-(float)i/50.0f) * sinf(2.0f*PI*0.05f * (float)i + ph);
    y_vec[i] += 1.0f;
  }
  return N;
}

int main(void) {
  size_t N = 100;
  float *x_vec = (float*)malloc(sizeof(float) * N);
  float *y_vec = (float*)malloc(sizeof(float) * N);
  
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

    N = get_data(x_vec, y_vec, N);
    data_space = new_data_space(x_vec, y_vec, N);

    BeginDrawing();
      ClearBackground(WHITE);
      draw_axes(data_space, view_area, AXISCOLOR);
      plot_data(data_space, view_area, MARKERSIZE, DATACOLOR);
    EndDrawing();
  }

  free(x_vec);
  free(y_vec);
  return 0;
}

