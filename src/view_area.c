#include "view_area.h"
#include "raylib.h"

Vector2 va_to_window_coords(View_Area va, Vector2 va_vector) {
  return (Vector2) { va_vector.x + va.x, va_vector.y + va.y };
}

Vector2 window_to_va_coords(View_Area va, Vector2 window_vector) {
  return (Vector2) { window_vector.x - va.x, window_vector.y - va.y };
}

