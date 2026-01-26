/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#if (defined(WIN32) || defined(WIN64))
# include <winsock2.h>
# include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>
#include <cairo.h>
#include <cairo-win32.h>
#include "../../../libkc3/kc3.h"
#include "vk_to_xkbcommon.h"
#include "window_cairo_win32.h"

bool window_cairo_run (s_window_cairo *window)
{
  return window_cairo_win32_run(window);
}

LRESULT CALLBACK window_cairo_win32_proc (HWND hwnd, UINT message,
					  WPARAM wParam, LPARAM lParam)
{
  HBITMAP buffer_hbitmap = NULL;
  HDC buffer_hdc = NULL;
  cairo_t *cr = NULL;
  HDC hdc = NULL;
  u32 keysym;
  PAINTSTRUCT ps = {0};
  cairo_surface_t *surface = NULL;
  u32 vk;
  s_window_cairo *window;
  HDC window_hdc = NULL;
  window = (s_window_cairo *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
  switch (message) {
  case WM_DESTROY:
    printf("WM_DESTROY\n");
    PostQuitMessage(0);
    break;
  case WM_ERASEBKGND:
    return 1;
  case WM_KEYDOWN:
    vk = (u32) wParam;
    keysym = vk_to_xkbcommon(vk);
    if (! window->key(window, keysym)) {
      printf("window->key -> false\n");
      PostQuitMessage(1);
    }
    break;
  case WM_PAINT:
    if (! surface) {
      window_hdc = GetDC(hwnd);
      buffer_hdc = CreateCompatibleDC(window_hdc);
      buffer_hbitmap = CreateCompatibleBitmap(window_hdc, window->w,
                                              window->h);
      SelectObject(buffer_hdc, buffer_hbitmap);
      surface = cairo_win32_surface_create(buffer_hdc);
      cr = cairo_create(surface);
      window->cr = cr;
    }
    if (! window->render(window)) {
      printf("render -> false\n");
      PostQuitMessage(1);
    }
    cairo_surface_flush(surface);
    hdc = BeginPaint(hwnd, &ps);
    BitBlt(hdc, 0, 0, window->w, window->h, buffer_hdc, 0, 0, SRCCOPY);
    EndPaint(hwnd, &ps);
    break;
  case WM_SIZE:
    if (surface) {
      DeleteObject(buffer_hbitmap);
      DeleteDC(buffer_hdc);
      DeleteDC(window_hdc);
      DeleteDC(hdc);
    }
    window_hdc = GetDC(hwnd);
    buffer_hdc = CreateCompatibleDC(window_hdc);
    buffer_hbitmap = CreateCompatibleBitmap(window_hdc, window->w,
                                            window->h);
    SelectObject(buffer_hdc, buffer_hbitmap);
    surface = cairo_win32_surface_create(buffer_hdc);
    cr = cairo_create(surface);
    window->cr = cr;
    if (! window->resize(window, LOWORD(lParam), HIWORD(lParam))) {
      err_puts("window_cairo_win32_proc: window->resize");
      PostQuitMessage(1);
    }
    window->w = LOWORD(lParam);
    window->h = HIWORD(lParam);
    InvalidateRect(hwnd, NULL, TRUE);
    break;
  default:
    return DefWindowProc(hwnd, message, wParam, lParam);
  }
  return 0;
}

bool window_cairo_win32_run (s_window_cairo *window)
{
  HWND hwnd;
  MSG msg;
  s_timespec sleep;
  WNDCLASSEX wc;
  wc.cbSize        = sizeof(WNDCLASSEX);
  wc.style         = 0;
  wc.lpfnWndProc   = window_cairo_win32_proc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = GetModuleHandle(NULL);
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "Libkc3WindowCairoWin32";
  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
  if (!RegisterClassEx(&wc)) {
    MessageBox(NULL, "Window Registration Failed!", "Error!",
               MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }
  hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                        "Libkc3WindowCairoWin32",
                        window->title,
                        WS_OVERLAPPEDWINDOW,
                        window->x, window->y, window->w, window->h,
                        NULL, NULL, GetModuleHandle(NULL), NULL);
  if (hwnd == NULL) {
    MessageBox(NULL, "Window Creation Failed!", "Error!",
               MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }
  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) window);
  if (! window->load(window))
    PostQuitMessage(1);
  ShowWindow(hwnd, SW_SHOWDEFAULT);
  UpdateWindow(hwnd);
  while (true) {
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
        goto quit;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    sleep.tv_sec = 0;
    sleep.tv_nsec = 1000000000 / 120;
    nanosleep(&sleep, NULL);
    InvalidateRect(hwnd, NULL, TRUE);
  }
 quit:
  return msg.wParam ? false : true;
}
