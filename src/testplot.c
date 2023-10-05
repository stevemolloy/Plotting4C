#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#define WIDTH 800
#define HEIGHT 600

size_t get_data(double* data_vec, size_t N) {
  static double ph = 0;
  ph += 0.01;

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
  size_t N = 100;
  double *data_vec = (double*)malloc(sizeof(double) * N);
  
  InitWindow(WIDTH, HEIGHT, "A test");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    N = get_data(data_vec, N);

    double max = return_max(data_vec, N);
    double min = return_min(data_vec, N);
    double span = max - min;

    BeginDrawing();
      ClearBackground(WHITE);
      for (size_t i=0; i<N; i++) {
        int endPosX = (int)(i) * WIDTH/(int)N;
        int endPosY = (int)((data_vec[i] - min)/span * HEIGHT);

        if (i>0) {
          int startPosX = (int)(i-1) * WIDTH/(int)N;
          int startPosY = (int)((data_vec[i-1] - min)/span * HEIGHT);
          DrawLine(startPosX, startPosY, endPosX, endPosY, BLUE);
        }

        DrawCircle(endPosX, endPosY, 5, RED);
      }
    EndDrawing();
  }

  free(data_vec);
  return 0;
}

