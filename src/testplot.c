#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#define AXISCOLOR DARKGRAY
#define DATACOLOR DARKBLUE
#define MARGIN 5
#define AXIS_THICKNESS 3.0
#define FONTSIZE_DIV 60

size_t calculateNiceTicks(float minVal, float maxVal, float* ticks) {
  size_t numTicks;

  float range = maxVal - minVal;
  if (range == 0) {
    numTicks = 2;
    ticks[0] = minVal - 0.5f;
    ticks[1] = minVal + 0.5f;
    return numTicks;
  }
  float exponent = floorf(log10f(range));
  float mantissa = range / powf(10, exponent);

  // Determine the number of ticks based on the mantissa
  float niceTickInterval;
  if (mantissa <= 1.5) {
    numTicks = 11;
    niceTickInterval = 0.15f * powf(10, exponent);
  } else if (mantissa <= 2.0) {
    numTicks = 9;
    niceTickInterval = 0.25f * powf(10, exponent);
  } else if (mantissa <= 4.0) {
    numTicks = 9;
    niceTickInterval = 0.5f * powf(10, exponent);
  } else if (mantissa <= 5.0) {
    numTicks = 11;
    niceTickInterval = 0.5f * powf(10, exponent);
  } else {
    numTicks = 11;
    niceTickInterval = 1.0f * powf(10, exponent);
  }

  // Populate the array with tick values
  float startTick = floorf(minVal / niceTickInterval) * niceTickInterval;
  for (int i = 0; i < numTicks; i++) {
    ticks[i] = startTick + (float)i * niceTickInterval;
  }

  // Clear up any excess at the end
  while (ticks[numTicks - 2] > maxVal) --numTicks;

  return numTicks;
}

typedef struct Axis {
  Vector2 start;
  Vector2 end;
} Axis;

size_t get_data(double* x_vec, double* y_vec, size_t N) {
  static double ph = 0;
  ph += 0.002;
  for (size_t i=0; i<N; i++) {
    x_vec[i] = (double)i;
    y_vec[i] = sin(2*(22/7)*0.05 * (double)i + ph);
  }
  return N;
}

void get_maxmin(double *data_vec, size_t N, double *max, double *min) {
  *max = -INFINITY;
  *min = INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] > *max) *max = data_vec[i];
    if (data_vec[i] < *min) *min = data_vec[i];
  }
}

double return_max(double *data_vec, size_t N) {
  double max = -INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] > max) max = data_vec[i];
  }
  return max;
}

double return_min(double *data_vec, size_t N) {
  double min = INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] < min) min = data_vec[i];
  }
  return min;
}

int main(void) {
  size_t N = 100;
  double *x_vec = (double*)malloc(sizeof(double) * N);
  double *y_vec = (double*)malloc(sizeof(double) * N);
  float* ticks = (float*)malloc(15 * sizeof(float)); // Maximum of 15 ticks
  
  int window_width = 800;
  int window_height = 600;
  InitWindow(window_width, window_height, "Plotting with a game engine");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    N = get_data(x_vec, y_vec, N);
    window_width = GetScreenWidth();
    window_height = GetScreenHeight();

    Rectangle view_area = {
      .x = 0.0f + MARGIN,
      .y = 0.0f + MARGIN,
      .width = (float)window_width - 2.0f * MARGIN,
      .height = (float)window_height - 2.0f * MARGIN,
    };

    double y_max, y_min, x_max, x_min;
    get_maxmin(x_vec, N, &x_max, &x_min);
    get_maxmin(y_vec, N, &y_max, &y_min);
    double x_span = x_max - x_min;
    double y_span = y_max - y_min;

    float zero_y = (float) (view_area.y - (view_area.height * y_min/y_span));
    float zero_x = (float) (view_area.x + (view_area.width * x_min/x_span));
    Axis x_axis = {
      .start = (Vector2){ view_area.x, zero_y},
      .end = (Vector2){ view_area.x + view_area.width, zero_y},
    };
    Axis y_axis = {
      .start = (Vector2){ zero_x, view_area.y},
      .end = (Vector2){ zero_x, view_area.y + view_area.height},
    };

    BeginDrawing();
      ClearBackground(WHITE);
      
      DrawRectangleLinesEx(view_area, 1, RED);

      // Ensure nothing is drawn outside the data window
      // BeginScissorMode(
      //     (int)view_area.x, (int)view_area.y,
      //     (int)view_area.width, (int)view_area.height
      //   );
        // Draw the axes
        DrawLineEx(x_axis.start, x_axis.end, AXIS_THICKNESS, AXISCOLOR);  // x axis
        DrawLineEx(y_axis.start, y_axis.end, AXIS_THICKNESS, AXISCOLOR);  // y axis

        size_t num_ticks;
        num_ticks = calculateNiceTicks((float)x_min, (float)x_max, ticks);
        for (size_t i=0; i<num_ticks; i++) { // x axis ticks
          float yloc_of_xaxis = (float) (view_area.y - (view_area.height * (0.0f + y_min)/y_span));
          float tick_start_x = (float) (view_area.x + (view_area.width * (ticks[i] + x_min)/x_span));
          Vector2 tick_start = {
            .x = tick_start_x,
            .y = yloc_of_xaxis
          };
          Vector2 tick_end = {
            .x = tick_start.x,
            .y = tick_start.y + view_area.height/100
          };
          DrawLineEx(tick_start, tick_end, 3.0, AXISCOLOR);
          char buf[20];
          sprintf(buf, "%.1f", ticks[i]);
          int label_width = MeasureText(buf, 20);
          DrawText(buf, (int)tick_start.x - label_width/2, (int)tick_end.y, window_width/FONTSIZE_DIV, AXISCOLOR);
        }

        num_ticks = calculateNiceTicks((float)y_min, (float)y_max, ticks);
        for (size_t i=0; i<num_ticks; i++) { // y axis ticks
          if (-0.01f < ticks[i] && ticks[i] < 0.01f) continue;
          float xloc_of_yaxis = (float) (view_area.x + (view_area.width * (0.0f + x_min)/x_span));
          float tick_start_y = (float) (view_area.y - (view_area.height * (ticks[i] + y_min)/y_span));
          Vector2 tick_start = {
            .x = xloc_of_yaxis,
            .y = tick_start_y,
          };
          Vector2 tick_end = {
            .x = tick_start.x + view_area.height/100,
            .y = tick_start.y,
          };
          DrawLineEx(tick_start, tick_end, 3.0, AXISCOLOR);
          char buf[20];
          sprintf(buf, "%.1f", ticks[i]);
          // int label_width = MeasureText(buf, 20);
          DrawText(buf, (int)(tick_start.x+view_area.height/50.0f), (int)tick_end.y-8, window_width/FONTSIZE_DIV, AXISCOLOR);
        }

        // Draw the data
        for (size_t i=0; i<N; i++) {
          int data_loc_X = (int) (view_area.x + (view_area.width * (x_vec[i] + x_min)/x_span));
          int data_loc_Y = (int) (view_area.y - (view_area.height * (y_vec[i] + y_min)/y_span));

          if (i>0) {
            int last_data_X = (int) (view_area.x + (view_area.width * (x_vec[i-1] + x_min)/x_span));
            int last_data_Y = (int) (view_area.y - (view_area.height * (y_vec[i-1] + y_min)/y_span));
            DrawLine(last_data_X, last_data_Y, data_loc_X, data_loc_Y, DATACOLOR);
          }

          DrawCircle(data_loc_X, data_loc_Y, 5, DATACOLOR);
        }
      // EndScissorMode();
    EndDrawing();
  }

  free(x_vec);
  free(y_vec);
  free(ticks);
  return 0;
}

