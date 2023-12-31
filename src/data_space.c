#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "data_space.h"
#include "view_area.h"
#include "raylib.h"

#define FONTSIZE 25
#define MAXTICKS 20
#define INIT_DATA_SIZE 1024

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

dyn_array_float new_dyn_arr_float(void) {
  return (dyn_array_float) {
    .vals = malloc(INIT_DATA_SIZE * sizeof(float)),
    .size = 0,
    .capacity = INIT_DATA_SIZE,
  };
}

int expand_dyn_arr_float(dyn_array_float *in) {
  in->capacity *= 2;
  in->vals = (float*)realloc(in->vals, in->capacity * sizeof(float));
  if (in->vals == NULL) {
    return -1;
  }
  return (int)in->capacity;
}

int add_to_dyn_arr_float(dyn_array_float *in, float data) {
  if (in->size == in->capacity) {
    expand_dyn_arr_float(in);
  }
  in->vals[in->size++] = data;
  return 0;
}

void plot_data(Data_Space ds, Rectangle zoom_rect, View_Area va, float marker_size, Color color) {
  Vector2 pts[ds.N];
  for (size_t i=0; i<ds.N; i++) {
    Vector2 pt = (Vector2){ds.x_data[i], ds.y_data[i]};
    Vector2 pt_in_va = data_to_va_coords(zoom_rect, va, pt);
    pts[i] = va_to_window_coords(va, pt_in_va);

    DrawCircleV(pts[i], marker_size, color);
  }
  DrawLineStrip(pts, (int)ds.N, color);
}

void draw_axes(Data_Space ds, Rectangle zoom_rect, View_Area va, Color color) {
  static bool font_loaded = false;
  static Font font;
  if (!font_loaded) {
    font = LoadFontEx("fonts/NotoSans-Regular.ttf", FONTSIZE, NULL, 0);
    font_loaded = true;
  }

  size_t num_x_ticks = ds.x_axis.num_ticks;
  Vector2 x_startPos = (Vector2) { ds.x_axis.ticks[0], 0.0f };
  Vector2 x_endPos = (Vector2) { ds.x_axis.ticks[num_x_ticks - 1], 0.0f };

  size_t num_y_ticks = ds.y_axis.num_ticks;
  Vector2 y_startPos = (Vector2) { 0.0f, ds.y_axis.ticks[0] };
  Vector2 y_endPos = (Vector2) { 0.0f, ds.y_axis.ticks[num_y_ticks - 1] };

  Vector2 x_start_va = data_to_va_coords(zoom_rect, va, x_startPos);
  Vector2 y_start_va = data_to_va_coords(zoom_rect, va, y_startPos);
  Vector2 x_end_va   = data_to_va_coords(zoom_rect, va, x_endPos);
  Vector2 y_end_va   = data_to_va_coords(zoom_rect, va, y_endPos);

  Vector2 x_start_win = va_to_window_coords(va, x_start_va);
  Vector2 y_start_win = va_to_window_coords(va, y_start_va);
  Vector2 x_end_win   = va_to_window_coords(va, x_end_va);
  Vector2 y_end_win   = va_to_window_coords(va, y_end_va);

  DrawLineEx(x_start_win, x_end_win, AXIS_THICKNESS, color);
  DrawLineEx(y_start_win, y_end_win, AXIS_THICKNESS, color);

  float tick_gap = ds.x_axis.ticks[1] - ds.x_axis.ticks[0];
  for (size_t i=0; i<ds.x_axis.num_ticks; i++) { // x axis ticks
    if (ds.x_axis.ticks[i] < tick_gap * 0.25f & ds.x_axis.ticks[i] > -tick_gap * 0.25f) {
      continue;
    }
    Vector2 tick_in_ds = (Vector2){ds.x_axis.ticks[i], 0.0f};
    Vector2 tick_in_va = data_to_va_coords(zoom_rect, va, tick_in_ds);
    Vector2 tick_start = va_to_window_coords(va, tick_in_va);
    Vector2 tick_end = {
      .x = tick_start.x,
      .y = tick_start.y + va.height/100
    };
    DrawLineEx(tick_start, tick_end, 3.0, color);
    char buf[20];
    sprintf(buf, "%.1f", ds.x_axis.ticks[i]);
    Vector2 text_size = MeasureTextEx(font, buf, FONTSIZE, 0.0f);
    Vector2 position = {tick_end.x - text_size.x/2, tick_end.y};
    DrawTextEx(font, buf, position, FONTSIZE, 0.0f, color);
  }

  tick_gap = ds.y_axis.ticks[1] - ds.y_axis.ticks[0];
  for (size_t i=0; i<ds.y_axis.num_ticks; i++) { // y axis ticks
    if (ds.y_axis.ticks[i] < tick_gap * 0.25f & ds.y_axis.ticks[i] > -tick_gap * 0.25f) {
      continue;
    }
    Vector2 tick_in_ds = (Vector2){0.0f, ds.y_axis.ticks[i]};
    Vector2 tick_in_va = data_to_va_coords(zoom_rect, va, tick_in_ds);
    Vector2 tick_start = va_to_window_coords(va, tick_in_va);
    Vector2 tick_end = {
      .x = tick_start.x + va.height/100,
      .y = tick_start.y
    };
    DrawLineEx(tick_start, tick_end, 3.0, color);
    char buf[20];
    sprintf(buf, "%.2f", ds.y_axis.ticks[i]);
    Vector2 text_size = MeasureTextEx(font, buf, FONTSIZE, 0.0f);    // Measure string size for Font
    Vector2 position = {tick_end.x + text_size.x/8, tick_end.y - text_size.y/2};
    DrawTextEx(font, buf, position, FONTSIZE, 0.0f, color); // Draw text using font and additional parameters
  }
}

Vector2 va_to_data_coords(Rectangle zoom_rect, View_Area va, Vector2 data_pt) {
  float W_x = va.width;
  float W_y = va.height;
  float x_range = zoom_rect.width;
  float y_range = zoom_rect.height;
  float x_min = zoom_rect.x;
  float y_min = zoom_rect.y;

  float A_x = W_x / x_range;
  float A_y = W_y / y_range;

  float B_x = -W_x * x_min / x_range;
  float B_y = -W_y * y_min / y_range;

  return (Vector2) {
    (data_pt.x - B_x) / A_x,
    (va.height - data_pt.y - B_y) / A_y,
  };
}

Vector2 data_to_va_coords(Rectangle zoom_rect, View_Area va, Vector2 data_pt) {
  float W_x = va.width;
  float W_y = va.height;
  float x_range = zoom_rect.width;
  float y_range = zoom_rect.height;
  float x_min = zoom_rect.x;
  float y_min = zoom_rect.y;

  float A_x = W_x / x_range;
  float A_y = W_y / y_range;

  float B_x = -W_x * x_min / x_range;
  float B_y = -W_y * y_min / y_range;

  return (Vector2) { A_x*data_pt.x + B_x, va.height - (A_y*data_pt.y + B_y) };
}

size_t calculateNiceTicks(float minVal, float maxVal, float* ticks) {
  size_t numTicks;

  float range_margin = 1.0f;//2f;
  float range = maxVal - minVal;
  float new_maxVal = minVal + range*range_margin;
  float new_minVal = maxVal - range*range_margin;
  maxVal = new_maxVal;
  minVal = new_minVal;
  range = maxVal - minVal;
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
  while (ticks[numTicks - 1] < maxVal) {
    ++numTicks;
    assert(numTicks < MAXTICKS);
    ticks[numTicks - 1] = ticks[numTicks - 2] + niceTickInterval;
  }

  return numTicks;
}

void get_maxmin(float *data_vec, size_t N, float *max, float *min) {
  *max = -INFINITY;
  *min = INFINITY;
  for (size_t i=0; i<N; i++) {
    if (data_vec[i] > *max) *max = data_vec[i];
    if (data_vec[i] < *min) *min = data_vec[i];
  }
}

Data_Space new_data_space(dyn_array_float x_data, dyn_array_float y_data, size_t N) {
  float largest_x, largest_y;
  float smallest_x, smallest_y;
  get_maxmin(x_data.vals, N, &largest_x, &smallest_x);
  get_maxmin(y_data.vals, N, &largest_y, &smallest_y);

  float* xticks = (float*)malloc(MAXTICKS * sizeof(float)); // Maximum of 15 ticks
  float* yticks = (float*)malloc(MAXTICKS * sizeof(float)); // Maximum of 15 ticks
  size_t num_x_ticks = calculateNiceTicks(smallest_x, largest_x, xticks);
  size_t num_y_ticks = calculateNiceTicks(smallest_y, largest_y, yticks);

  return (Data_Space) {
    .x_data = x_data.vals,
    .y_data = y_data.vals,
    .N = N,
    .x_axis = (Axis){ xticks, num_x_ticks },
    .y_axis = (Axis){ yticks, num_y_ticks },
  };
}

