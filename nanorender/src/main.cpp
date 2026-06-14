#include "MiniFB.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern "C"
{
#include "microui.h"
}
#include "ui_bridge.h"
#include "ui_renderer.h"

#define WIDTH 1600
#define HEIGHT 1200

static uint32_t g_buffer[WIDTH * HEIGHT];
void put_pixel(int x, int y, uint32_t color)
{
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
  {
    return;
  }

  g_buffer[y * WIDTH + x] = color;
}
void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;

  int err = dx - dy;

  while (true)
  {
    put_pixel(x0, y0, color);

    if (x0 == x1 && y0 == y1)
    {
      break;
    }

    int e2 = 2 * err;

    if (e2 > -dy)
    {
      err -= dy;
      x0 += sx;
    }

    if (e2 < dx)
    {
      err += dx;
      y0 += sy;
    }
  }
}
struct Line
{
  int x0;
  int y0;
  int x1;
  int y1;
  uint32_t color;
};
int main()
{
  struct mfb_window *window =
      mfb_open_ex("MiniGUI Platform", WIDTH, HEIGHT, MFB_WF_RESIZABLE);
  if (!window)
    return 1;

  mu_Context *ctx = (mu_Context *)malloc(sizeof(mu_Context));
  mu_init(ctx);

  // Set font callbacks for microui
  ctx->text_width = [](mu_Font font, const char *str, int len)
  {
    return (len < 0 ? (int)strlen(str) : len) * 8;
  };
  ctx->text_height = [](mu_Font font)
  { return 8; };

  UIRenderer renderer(WIDTH, HEIGHT);

  static int background_mode = 0;
  static float color_shift = 0.0f;
  static Line lines[1000];
  static int line_count = 0;
  static bool is_drawing = false;
  static int start_x = 0;
  static int start_y = 0;
  static int current_x = 0;
  static int current_y = 0;
  static bool was_mouse_down = false;
  static float line_r = 255.0f;
static float line_g = 255.0f;
static float line_b = 255.0f;

  // Set up char input callback for textbox input
  mfb_set_char_input_callback(
      [](struct mfb_window *w, unsigned int c)
      {
        if (c == 'c' || c == 'C')
        {
          background_mode = (background_mode + 1) % 3;
          return;
        }
        extern void ui_bridge_char_input(struct mfb_window *, unsigned int);
        ui_bridge_char_input(w, c);
      },
      window);

  while (mfb_update_events(window) != MFB_STATE_EXIT)
  {
    // 1. Input
    ui_bridge_input(ctx, window);
  int mouse_x = ctx->mouse_pos.x;
int mouse_y = ctx->mouse_pos.y;

bool mouse_down = ctx->mouse_down & MU_MOUSE_LEFT;
bool mouse_pressed = mouse_down && !was_mouse_down;
bool mouse_released = !mouse_down && was_mouse_down;
if (mouse_pressed)
{
    is_drawing = true;

    start_x = mouse_x;
    start_y = mouse_y;

    current_x = mouse_x;
    current_y = mouse_y;
}
if (is_drawing && mouse_down)
{
  current_x = mouse_x;
  current_y = mouse_y;
}
if (is_drawing && mouse_released)
{
  if (line_count < 1000)
  {
lines[line_count] = {start_x, start_y, mouse_x, mouse_y, MFB_RGB((int)line_r, (int)line_g, (int)line_b)};
    line_count++;
  }

  is_drawing = false;
}
    // 2. Scene Rendering (Background)
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
      // Simple gradient background
      int x = i % WIDTH;
      int y = i / WIDTH;
      uint8_t r, g, b;

      if (background_mode == 0)
      {
  r = ((x ^ y) + (int)color_shift) % 255;
g = ((x | y) + (int)(color_shift * 0.5f)) % 255;
b = ((x & y) + (int)(color_shift * 0.25f)) % 255;
      }
      else if (background_mode == 1)
      {
        r = (x + y) % 255;
        g = (x * 2) % 255;
        b = (y * 2) % 255;
      }
      else
      {
        r = (y * 255) / HEIGHT;
        g = 100;
        b = (x * 255) / WIDTH;
      }
      g_buffer[i] = MFB_RGB(r, g, b);
    }
for (int i = 0; i < line_count; i++)
{
  draw_line(lines[i].x0, lines[i].y0, lines[i].x1, lines[i].y1, lines[i].color);
}
if (is_drawing)
{
  draw_line(start_x, start_y, current_x, current_y, MFB_RGB((int)line_r, (int)line_g, (int)line_b));
}
    // 3. UI Logic
    static float slider_val = 50.0f;
    static float number_val = 3.14f;
    static int checkbox_a = 0;
    static int checkbox_b = 1;
    static char textbox_buf[128] = "edit me";
    static bool quit_requested = false;
    static int show_message = 0;
    

    mu_begin(ctx);

    // --- Widgets window ---
    if (mu_begin_window(ctx, "Widgets", mu_rect(20, 20, 360, 540)))
    {
      int w1[] = {-1};

      // label / text
      mu_layout_row(ctx, 1, w1, 0);
      mu_label(ctx, "mu_label: plain static text");
      mu_text(ctx, "mu_text: word-wrapped longer text that will reflow inside "
                   "the window width automatically.");

      // button
      mu_layout_row(ctx, 1, w1, 0);
      if (mu_button(ctx, "mu_button: click me"))
      {
        quit_requested = false; // just a reaction
      }

      // checkbox
      mu_layout_row(ctx, 1, w1, 0);
      mu_checkbox(ctx, "mu_checkbox A (off)", &checkbox_a);
      mu_checkbox(ctx, "mu_checkbox B (on)", &checkbox_b);

      // textbox
      mu_layout_row(ctx, 1, w1, 0);
      mu_label(ctx, "mu_textbox:");
      mu_textbox(ctx, textbox_buf, sizeof(textbox_buf));

      // slider
      mu_layout_row(ctx, 1, w1, 0);
      mu_label(ctx, "mu_slider (0-100):");
      mu_slider(ctx, &slider_val, 0, 100);
      mu_layout_row(ctx, 1, w1, 0);
      mu_label(ctx, "Soft color shift:");
      mu_slider(ctx, &color_shift, 0, 100);
int color_buttons[] = {60, 60, 60, 60, 60};
mu_layout_row(ctx, 5, color_buttons, 0);
if (mu_button(ctx, "White"))
{
    line_r = 255;
    line_g = 255;
    line_b = 255;
}

if (mu_button(ctx, "Red"))
{
    line_r = 255;
    line_g = 0;
    line_b = 0;
}

if (mu_button(ctx, "Green"))
{
    line_r = 0;
    line_g = 255;
    line_b = 0;
}

if (mu_button(ctx, "Blue"))
{
    line_r = 0;
    line_g = 0;
    line_b = 255;
}

if (mu_button(ctx, "Yellow"))
{
    line_r = 255;
    line_g = 255;
    line_b = 0;
}


      // number
      mu_layout_row(ctx, 1, w1, 0);
      mu_label(ctx, "mu_number (step 0.1):");
      mu_number(ctx, &number_val, 0.1f);

      // header (collapsible section)
      if (mu_header(ctx, "mu_header: collapsible section"))
      {
        mu_layout_row(ctx, 1, w1, 0);
        mu_label(ctx, "Content inside the header.");
      }

      // treenode
      if (mu_begin_treenode(ctx, "mu_treenode: root"))
      {
        mu_layout_row(ctx, 1, w1, 0);
        mu_label(ctx, "child item A");
        if (mu_begin_treenode(ctx, "nested node"))
        {
          mu_layout_row(ctx, 1, w1, 0);
          mu_label(ctx, "deeply nested item");
          mu_end_treenode(ctx);
        }
        mu_end_treenode(ctx);
      }

      // quit button
      mu_layout_row(ctx, 1, w1, 0);
      if (mu_button(ctx, "Quit"))
      {
        quit_requested = true;
      }
      mu_layout_row(ctx, 1, w1, 0);

      if (mu_button(ctx, "Show message"))
      {
        show_message = !show_message;
        printf("Show message button clicked!\n");
      }

if (show_message)
{
  mu_label(ctx, "Hello from my widget!");
}

mu_layout_row(ctx, 1, w1, 0);
if (mu_button(ctx, "Clear Lines"))
{
  line_count = 0;
}

mu_end_window(ctx);
}
    // --- Panel window ---
    if (mu_begin_window(ctx, "Panel Demo", mu_rect(395, 20, 380, 200)))
    {
      int w2[] = {-1};
      mu_layout_row(ctx, 1, w2, 120);
      mu_begin_panel(ctx, "scrollable panel");
      int wp[] = {-1};
      for (int i = 1; i <= 12; i++)
      {
        mu_layout_row(ctx, 1, wp, 0);
        char line[32];
        snprintf(line, sizeof(line), "Panel row %d", i);
        mu_label(ctx, line);
      }
      mu_end_panel(ctx);
      mu_end_window(ctx);
    }

    // --- Popup demo window ---
    if (mu_begin_window(ctx, "Popup Demo", mu_rect(395, 235, 380, 80)))
    {
      int w3[] = {-1};
      mu_layout_row(ctx, 1, w3, 0);
      if (mu_button(ctx, "Open popup"))
      {
        mu_Container *popup = mu_get_container(ctx, "my popup");
        popup->rect = mu_rect(ctx->mouse_pos.x, ctx->mouse_pos.y, 260, 84);
        popup->open = 1;
        ctx->hover_root = ctx->next_hover_root = popup;
        mu_bring_to_front(ctx, popup);
      }
      int popup_opt = MU_OPT_POPUP | MU_OPT_NORESIZE | MU_OPT_NOSCROLL |
                      MU_OPT_NOTITLE | MU_OPT_CLOSED;
      if (mu_begin_window_ex(ctx, "my popup", mu_rect(0, 0, 260, 84),
                             popup_opt))
      {
        int wp[] = {-1};
        mu_layout_row(ctx, 1, wp, 0);
        mu_label(ctx, "mu_popup: click outside to close");
        if (mu_button(ctx, "Close"))
        {
          mu_get_current_container(ctx)->open = 0;
        }
        mu_end_window(ctx);
      }
      mu_end_window(ctx);
    }

    mu_end(ctx);

    if (quit_requested)
    {
      mfb_close(window);
      break;
    }

    // 4. UI Rendering
    renderer.render(ctx, g_buffer);

    // 5. Display
    mfb_update_state state = mfb_update_ex(window, g_buffer, WIDTH, HEIGHT);
    if (state < 0)
      break;
was_mouse_down = mouse_down;
    // Cap FPS (optional, minifb has built-in sync)
    mfb_wait_sync(window);
  }

  mfb_close(window);
  free(ctx);
  return 0;
}
