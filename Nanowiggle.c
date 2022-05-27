#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "resource.h"

#define WIGGLE_INTERVAL_MS 2000
#define WIGGLE_OFFSET         5

#define MAX_LOADSTRING 100

/* Globals */
HINSTANCE hInst;                      /* current instance */
TCHAR szTitle[MAX_LOADSTRING];        /* title bar text */
TCHAR szWindowClass[MAX_LOADSTRING];  /* title bar text */

/* Foward declarations */
BOOL              InitInstance(HINSTANCE, int);
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK  About(HWND, UINT, WPARAM, LPARAM);
void              WiggleTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  MSG msg;
  HACCEL hAccelTable;
  WNDCLASSEX wcex;

  /* Initialize global strings */
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_NANOWIGGLE, szWindowClass, MAX_LOADSTRING);

  /* Register Window class */

  wcex.cbSize = sizeof(WNDCLASSEX); 

  wcex.style          = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc    = (WNDPROC)WndProc;
  wcex.cbClsExtra     = 0;
  wcex.cbWndExtra     = 0;
  wcex.hInstance      = hInstance;
  wcex.hIcon          = LoadIcon(hInstance, (LPCTSTR)IDI_NANOWIGGLE);
  wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName   = (LPCSTR)IDC_NANOWIGGLE;
  wcex.lpszClassName  = szWindowClass;
  wcex.hIconSm        = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

  RegisterClassEx(&wcex);

  /* Initialize application */
  if (!InitInstance (hInstance, nCmdShow)) {
    return FALSE;
  }

  hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_NANOWIGGLE);

  /* Main Loop */
  while (GetMessage(&msg, NULL, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   RECT windowRect;

   hInst = hInstance;

   hWnd = CreateWindow(
     szWindowClass,
     szTitle,
     WS_OVERLAPPEDWINDOW,
     CW_USEDEFAULT,
     0,
     320, /* width */
     240, /* height */
     NULL,
     NULL,
     hInstance,
     NULL
  );

  if (!hWnd) {
    return FALSE;
  }

  /* Create wiggle timer */
  SetTimer(hWnd, 1337, WIGGLE_INTERVAL_MS, (TIMERPROC)WiggleTimerProc);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  /* Place mouse in window.
     On Windows 2000, it sometimes takes a couple of seconds before
     this actually happens and the program moves on. Why? */
   GetWindowRect(hWnd, &windowRect);
   SetCursorPos(windowRect.left + 35, windowRect.top + 70);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;
  TCHAR szText[MAX_LOADSTRING];
  LoadString(hInst, IDS_RUNNING, szText, MAX_LOADSTRING);

  switch (message) {
    case WM_COMMAND:
      wmId    = LOWORD(wParam); 
      wmEvent = HIWORD(wParam); 

      switch (wmId) {
        case IDM_ABOUT:
          DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
          break;
        case IDM_EXIT:
           DestroyWindow(hWnd);
           break;
        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
      }
      break;
    case WM_PAINT: {
      RECT rt;

      hdc = BeginPaint(hWnd, &ps);
      GetClientRect(hWnd, &rt);
      DrawText(hdc, szText, strlen(szText), &rt, DT_CENTER);
      EndPaint(hWnd, &ps);
      break;
    }
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

static int sign = -1;
void WiggleTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  POINT pt;

  sign = sign * (-1);

  if (GetCursorPos(&pt)) {
    if (!SetCursorPos(
        pt.x + (WIGGLE_OFFSET * sign),
        pt.y + (WIGGLE_OFFSET * sign))) {
      MessageBox(hWnd, "Could not set cursor position.", "Oh, no!", MB_OK | MB_ICONEXCLAMATION);	
    };
  }
}

/* About box */
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
    case WM_INITDIALOG:
      return TRUE;

    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
        EndDialog(hDlg, LOWORD(wParam));
        return TRUE;
      }
      break;
  }
    return FALSE;
}
