#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#define AXISCOLOR DARKGRAY
#define DATACOLOR DARKBLUE
#define MARGIN 5
#define AXIS_THICKNESS 3.0
#define FONTSIZE_DIV 60

typedef Rectangle View_Area;

void get_maxmin(float *data_vec, size_t N, float *max, float *min) {
  *max = -INFINITY;
  *min = INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] > *max) *max = data_vec[i];
    if (data_vec[i] < *min) *min = data_vec[i];
  }
}

typedef struct Axis {
  float* ticks;
  size_t num_ticks;
} Axis;

typedef struct Data_Space {
  float x_min;
  float y_min;
  float x_range;
  float y_range;
  Axis x_axis;
  Axis y_axis;
} Data_Space;

Vector2 data_to_va_coords(Data_Space ds, View_Area va, Vector2 data_pt) {
  float W_x = va.width;
  float W_y = va.height;
  float x_range = ds.x_range;
  float y_range = ds.y_range;
  float x_min = ds.x_min;
  float y_min = ds.y_min;

  float A_x = W_x / x_range;
  float A_y = W_y / y_range;

  float B_x = -W_x * x_min / x_range;
  float B_y = -W_y * y_min / y_range;

  return (Vector2) { A_x*data_pt.x + B_x, va.height - (A_y*data_pt.y + B_y) };
}

Vector2 va_to_window_coords(Rectangle va, Vector2 va_vector) {
  return (Vector2) { va_vector.x + va.x, va_vector.y + va.y };
}

void draw_axes(Data_Space ds, View_Area va, Color color) {
  size_t num_x_ticks = ds.x_axis.num_ticks;
  Vector2 x_startPos = (Vector2) { ds.x_axis.ticks[0], 0.0f };
  Vector2 x_endPos = (Vector2) { ds.x_axis.ticks[num_x_ticks - 1], 0.0f };

  size_t num_y_ticks = ds.y_axis.num_ticks;
  Vector2 y_startPos = (Vector2) { 0.0f, ds.y_axis.ticks[0] };
  Vector2 y_endPos = (Vector2) { 0.0f, ds.y_axis.ticks[num_y_ticks - 1] };

  Vector2 x_start_va = data_to_va_coords(ds, va, x_startPos);
  Vector2 y_start_va = data_to_va_coords(ds, va, y_startPos);
  Vector2 x_end_va   = data_to_va_coords(ds, va, x_endPos);
  Vector2 y_end_va   = data_to_va_coords(ds, va, y_endPos);

  Vector2 x_start_win = va_to_window_coords(va, x_start_va);
  Vector2 y_start_win = va_to_window_coords(va, y_start_va);
  Vector2 x_end_win   = va_to_window_coords(va, x_end_va);
  Vector2 y_end_win   = va_to_window_coords(va, y_end_va);

  DrawLineEx(x_start_win, x_end_win, AXIS_THICKNESS, color);
  DrawLineEx(y_start_win, y_end_win, AXIS_THICKNESS, color);

  float tick_gap = ds.x_axis.ticks[1] - ds.x_axis.ticks[0];
  int font_size = (int)(va.width/FONTSIZE_DIV);
  for (size_t i=0; i<ds.x_axis.num_ticks; i++) { // x axis ticks
    if (ds.x_axis.ticks[i] < tick_gap * 0.25f & ds.x_axis.ticks[i] > -tick_gap * 0.25f) {
      continue;
    }
    Vector2 tick_in_ds = (Vector2){ds.x_axis.ticks[i], 0.0f};
    Vector2 tick_in_va = data_to_va_coords(ds, va, tick_in_ds);
    Vector2 tick_start = va_to_window_coords(va, tick_in_va);
    Vector2 tick_end = {
      .x = tick_start.x,
      .y = tick_start.y + va.height/100
    };
    DrawLineEx(tick_start, tick_end, 3.0, color);
    char buf[20];
    sprintf(buf, "%.1f", ds.x_axis.ticks[i]);
    int label_width = MeasureText(buf, 20);
    DrawText(buf, (int)tick_start.x - label_width/2, (int)tick_end.y, font_size, color);
  }

  tick_gap = ds.y_axis.ticks[1] - ds.y_axis.ticks[0];
  for (size_t i=0; i<ds.y_axis.num_ticks; i++) { // y axis ticks
    if (ds.y_axis.ticks[i] < tick_gap * 0.25f & ds.y_axis.ticks[i] > -tick_gap * 0.25f) {
      continue;
    }
    Vector2 tick_in_ds = (Vector2){0.0f, ds.y_axis.ticks[i]};
    Vector2 tick_in_va = data_to_va_coords(ds, va, tick_in_ds);
    Vector2 tick_start = va_to_window_coords(va, tick_in_va);
    Vector2 tick_end = {
      .x = tick_start.x + va.height/100,
      .y = tick_start.y
    };
    DrawLineEx(tick_start, tick_end, 3.0, color);
    char buf[20];
    sprintf(buf, "%.2f", ds.y_axis.ticks[i]);
    DrawText(buf, (int)tick_start.x + font_size/2, (int)tick_end.y - font_size/2, font_size, color);
  }
}

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

Data_Space new_data_space(float *x_data, float*y_data, size_t N) {
  float largest_x, largest_y;
  float smallest_x, smallest_y;
  get_maxmin(x_data, N, &largest_x, &smallest_x);
  get_maxmin(y_data, N, &largest_y, &smallest_y);

  float* xticks = (float*)malloc(15 * sizeof(float)); // Maximum of 15 ticks
  float* yticks = (float*)malloc(15 * sizeof(float)); // Maximum of 15 ticks
  size_t num_x_ticks = calculateNiceTicks(smallest_x, largest_x, xticks);
  size_t num_y_ticks = calculateNiceTicks(smallest_y, largest_y, yticks);

  return (Data_Space) {
    .x_min = xticks[0],
    .y_min = yticks[0],
    .x_range = xticks[num_x_ticks - 1] - xticks[0],
    .y_range = yticks[num_y_ticks - 1] - yticks[0],
    .x_axis = (Axis){ xticks, num_x_ticks },
    .y_axis = (Axis){ yticks, num_y_ticks },
  };
}

size_t get_data(float* x_vec, float* y_vec, size_t N) {
  static float ph = 0;
  ph += 0.002f;
  for (size_t i=0; i<N; i++) {
    x_vec[i] = (float)i;
    y_vec[i] = sinf(2.0f*(22.0f/7.0f)*0.05f * (float)i + ph);
  }
  return N;
}

float return_max(float *data_vec, size_t N) {
  float max = -INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] > max) max = data_vec[i];
  }
  return max;
}

float return_min(float *data_vec, size_t N) {
  float min = INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] < min) min = data_vec[i];
  }
  return min;
}

int main(void) {
  size_t N = 100;
  float *x_vec = (float*)malloc(sizeof(float) * N);
  float *y_vec = (float*)malloc(sizeof(float) * N);
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

    View_Area view_area = {
      .x = 0.0f + MARGIN,
      .y = 0.0f + MARGIN,
      .width = (float)window_width - 2.0f * MARGIN,
      .height = (float)window_height - 2.0f * MARGIN,
    };

    float y_max, y_min, x_max, x_min;
    get_maxmin(x_vec, N, &x_max, &x_min);
    get_maxmin(y_vec, N, &y_max, &y_min);
    float x_span = x_max - x_min;
    float y_span = y_max - y_min;

    Data_Space data_space = new_data_space(x_vec, y_vec, N);

    BeginDrawing();
      ClearBackground(WHITE);
      
      // Draw the axes
      draw_axes(data_space, view_area, AXISCOLOR);

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
    EndDrawing();
  }

  free(x_vec);
  free(y_vec);
  free(ticks);
  return 0;
}

