#ifndef _DATA_SPACE_H
#define _DATA_SPACE_H

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "axis.h"
#include "view_area.h"

#define FONTSIZE_DIV 60
#define MAXLINELENGTH 200

typedef struct dyn_array_float {
  float* vals;
  size_t size;
  size_t capacity;
} dyn_array_float;

typedef struct Data_Space {
  float *x_data;
  float *y_data;
  size_t N;
  Axis x_axis;
  Axis y_axis;
} Data_Space;

size_t load_two_column_csv(char*, dyn_array_float*, dyn_array_float*);
dyn_array_float new_dyn_arr_float(void);
int expand_dyn_arr_float(dyn_array_float*);
int add_to_dyn_arr_float(dyn_array_float*, float);
Data_Space new_data_space(dyn_array_float, dyn_array_float, size_t);
Vector2 data_to_va_coords(Rectangle, View_Area, Vector2);
Vector2 va_to_data_coords(Rectangle, View_Area, Vector2);
void draw_axes(Data_Space, Rectangle, View_Area, Color);
void plot_data(Data_Space, Rectangle, View_Area, float, Color);

#endif // !_DATA_SPACE_H

