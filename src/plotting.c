#include <raylib.h>
#include <raymath.h>

#include "data_space.h"
#include "view_area.h"
#include "plotting.h"

void drag_plot(Rectangle *zr, View_Area va) {
  static bool dragging_plot = false;
  static Vector2 dragging_start_pos;

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 mouse_pos = GetMousePosition();
    if (CheckCollisionPointRec(mouse_pos, va)) {
      if (!dragging_plot) {
        dragging_start_pos = va_to_data_coords(*zr, va, window_to_va_coords(va, mouse_pos));
        TraceLog(LOG_INFO, "Starting to drag the plot");
        dragging_plot = true;
      }
      Vector2 dragging_end_pos = va_to_data_coords(*zr, va, window_to_va_coords(va, mouse_pos));
      Vector2 diff = Vector2Subtract(dragging_end_pos, dragging_start_pos);
      zr->x -= diff.x;
      zr->y -= diff.y;
    }
  }
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) & dragging_plot) {
    dragging_plot = false;
    TraceLog(LOG_INFO, "Dragging operation finished");
  }
}

void zoom_plot(Rectangle *zr, View_Area va) {
  static bool drawing_zoom = false;
  static Rectangle zoom_rect_view = {0};
  static Vector2 zoom_start_pos = {0};

  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    Vector2 mouse_pos = GetMousePosition();
    if (CheckCollisionPointRec(mouse_pos, va)) {
      if (!drawing_zoom) {
        zoom_start_pos = mouse_pos;
        TraceLog(LOG_INFO, "Drawing zoom");
        drawing_zoom = true;
        zoom_rect_view = (Rectangle) {
          .x = zoom_start_pos.x,
          .y = zoom_start_pos.y,
          .width = 0.0f,
          .height = 0.0f,
        };
      }
      if (mouse_pos.x < zoom_start_pos.x) {
        zoom_rect_view.x = mouse_pos.x;
        zoom_rect_view.width = zoom_start_pos.x - mouse_pos.x;
      } else {
        zoom_rect_view.x = zoom_start_pos.x;
        zoom_rect_view.width = mouse_pos.x - zoom_start_pos.x;
      }
      if (mouse_pos.y < zoom_start_pos.y) {
        zoom_rect_view.y = mouse_pos.y;
        zoom_rect_view.height = zoom_start_pos.y - mouse_pos.y;
      } else {
        zoom_rect_view.y = zoom_start_pos.y;
        zoom_rect_view.height = mouse_pos.y - zoom_start_pos.y;
      }
    }
    DrawRectangleLinesEx(zoom_rect_view, 2.0f, BLACK);
  }
  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) & drawing_zoom) {
    drawing_zoom = false;
    Vector2 zoom_rect_topleft_window = (Vector2) {
      zoom_rect_view.x,
      zoom_rect_view.y,
    };
    Vector2 zoom_rect_bottomright_window = (Vector2) {
      zoom_rect_view.x + zoom_rect_view.width,
      zoom_rect_view.y + zoom_rect_view.height,
    };
    
    Vector2 zoom_rect_topleft_va = window_to_va_coords(va, zoom_rect_topleft_window);
    Vector2 zoom_rect_bottomright_va = window_to_va_coords(va, zoom_rect_bottomright_window);

    Vector2 zoom_rect_topleft_data = va_to_data_coords(*zr, va, zoom_rect_topleft_va);
    Vector2 zoom_rect_bottomright_data = va_to_data_coords(*zr, va, zoom_rect_bottomright_va);

    *zr = (Rectangle) {
      .x = zoom_rect_topleft_data.x,
      .y = zoom_rect_bottomright_data.y,
      .width = zoom_rect_bottomright_data.x - zoom_rect_topleft_data.x,
      .height = zoom_rect_topleft_data.y - zoom_rect_bottomright_data.y,
    };
    TraceLog(LOG_INFO, "Applying zoom");
  }
 }

