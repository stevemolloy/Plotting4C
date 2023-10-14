#ifndef _VIEW_AREA_H
#define _VIEW_AREA_H

#include "raylib.h"

#define AXIS_THICKNESS 3.0

typedef Rectangle View_Area;

Vector2 va_to_window_coords(View_Area, Vector2);
Vector2 window_to_va_coords(View_Area, Vector2);

#endif // !_VIEW_AREA_H

