/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#define WIN32_LEAN_AND_MEAN
#include "../../../libkc3/kc3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xkbcommon/xkbcommon.h>
#include "window_egl_windows.h"

static LRESULT CALLBACK WndProc (HWND hWnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam)
{
  switch (msg) {
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    const char *txt = "Hello from Win32 (no wchar)!";
    TextOutA(hdc, 10, 10, txt, (int)lstrlenA(txt));
    EndPaint(hWnd, &ps);
    return 0;
  }
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProcA(hWnd, msg, wParam, lParam);
  }
}

bool window_egl_run (s_window_egl *window)
{
  return window_egl_windows_run(window);
}

bool window_egl_windows_run (s_window_egl *window)
{
  HINSTANCE hInst = GetModuleHandleA(NULL);
  int nCmdShow = SW_SHOWDEFAULT;
  const char *clsName = "KC3WindowEGLWindowsClass";
  WNDCLASSEXA wc = {0};
  wc.cbSize        = sizeof(wc);
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = WndProc;
  wc.hInstance     = hInst;
  wc.hIcon         = LoadIconA(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursorA(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wc.lpszClassName = clsName;
  wc.hIconSm       = wc.hIcon;
  if (!RegisterClassExA(&wc)) {
    MessageBoxA(NULL, "RegisterClassExA failed", "Error",
                MB_ICONERROR | MB_OK);
    return 1;
  }
  DWORD dwStyle = WS_OVERLAPPEDWINDOW;
  DWORD dwEx    = 0;
  HWND hWnd = CreateWindowExA(dwEx, clsName, window->title, dwStyle,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              window->w, window->h, NULL, NULL,
                              hInst, NULL);
  if (!hWnd) {
    MessageBoxA(NULL, "CreateWindowExA failed", "Error", MB_ICONERROR|MB_OK);
    return 1;
  }
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  MSG msg;
  while (GetMessageA(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }
  return true;
}
