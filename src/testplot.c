#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

size_t get_data(double* data_vec, size_t N) {
  static double ph = 0;
  // ph += 0.01;
  ph += 0.0;

  for (size_t i=0; i<N; i++) {
    data_vec[i] = sin(2*(22/7)*0.05 * (double)i + ph);
  }

  return N;
}

double return_max(double *data_vec, size_t N) {
  double max = -INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] > max) {
      max = data_vec[i];
    }
  }

  return max;
}

double return_min(double *data_vec, size_t N) {
  double min = INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] < min) {
      min = data_vec[i];
    }
  }

  return min;
}

int main(void) {
  int window_width = 800;
  int window_height = 600;
  size_t N = 100;
  double *data_vec = (double*)malloc(sizeof(double) * N);
  
  InitWindow(window_width, window_height, "A test");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    N = get_data(data_vec, N);
    window_width = GetScreenWidth();
    window_height = GetScreenHeight();

    double max = return_max(data_vec, N);
    double min = return_min(data_vec, N);
    double span = max - min;

    BeginDrawing();
      ClearBackground(WHITE);
      // Vector2 startPos, endPos;
      Vector2 startXaxis = {
        .x = 0,
        .y = (float)(-min/span*window_height)
      };
      Vector2 endXaxis = {
        .x = (float)window_width,
        .y = startXaxis.y
      };
      // Draw the x axis
      DrawLineEx(startXaxis, endXaxis, 3.0, BLACK);
      for (size_t i=0; i<N; i+=10) {
        Vector2 tick_start = {
          .x = (float)(i * (size_t)window_width/N),
          .y = startXaxis.y
        };
        Vector2 tick_end = {
          .x = tick_start.x,
          .y = tick_start.y + (float)(window_height/100)
        };
        DrawLineEx(tick_start, tick_end, 3.0, BLACK);
      }

      // Draw the data
      for (size_t i=0; i<N; i++) {
        int endPosX = (int)(i) * window_width/(int)N;
        int endPosY = (int)((data_vec[i] - min)/span * window_height);

        if (i>0) {
          int startPosX = (int)(i-1) * window_width/(int)N;
          int startPosY = (int)((data_vec[i-1] - min)/span * window_height);
          DrawLine(startPosX, startPosY, endPosX, endPosY, BLUE);
        }

        DrawCircle(endPosX, endPosY, 5, BLUE);
      }
    EndDrawing();
  }

  free(data_vec);
  return 0;
}

