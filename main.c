// Windows Header Files:
#include <windows.h>
#include <uxtheme.h>
#include <commctrl.h>

// C RunTime Header Files
#include <math.h>
#include <stdint.h>

#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))

#define CONTINUE_BIT 7
#define SKILL_INCREMENT_BIT 6
char g_skills[][10] = { "strength", "dexterity", "wisdom" };
HWND g_progress_bars[COUNT_OF(g_skills)] = {0};

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        case WM_COMMAND: {

            if (HIWORD(w_param) == LBN_SELCHANGE) {
                char list_item[256];
                LRESULT idx = SendMessage((HWND)l_param, LB_GETCURSEL, 0, 0);
                SendMessage((HWND)l_param, LB_GETTEXT, (WPARAM)idx, (LPARAM)list_item);
                MessageBoxA(hwnd, list_item, "owo", MB_OK);
            }

            if (HIWORD(w_param) == BN_CLICKED) {
                if (LOWORD(w_param) & (1 << CONTINUE_BIT))
                    return 0;

                HRESULT id = LOWORD(w_param);
                int increment = id & (1 << SKILL_INCREMENT_BIT);
                int skill_i = id & ~(1 << SKILL_INCREMENT_BIT);

                HWND pb = g_progress_bars[skill_i];
                LRESULT pos = SendMessage(pb, PBM_GETPOS, 0, 0);
                pos = increment ? pos + 1 : pos - 1;
                SendMessage(pb, PBM_SETPOS, pos, 0); 
            }

        } break;

        case WM_DISPLAYCHANGE: {
            InvalidateRect(hwnd, NULL, FALSE);
        } break;

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 1;
        } break;

        default: {
            return DefWindowProc(hwnd, message, w_param, l_param);
        } break;
    }

    return 0;
}

int WINAPI WinMain(
    HINSTANCE     _hInstance    ,
    HINSTANCE     _hPrevInstance,
    LPSTR         _lpCmdLine    ,
    int           _nCmdShow
) {
    (void) _hInstance;
    (void) _hPrevInstance;
    (void) _lpCmdLine;
    (void) _nCmdShow;

    /* no blurries */
    SetProcessDPIAware();

    HWND hwnd;
    {
        // Register the window class.
        char *class_name = "kingsway32";
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = wnd_proc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = NULL;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);;
        wcex.lpszMenuName  = NULL;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = class_name;

        RegisterClassEx(&wcex);

        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        float dpi_x = 0;
        float dpi_y = 0;
        HDC hdc = GetDC(NULL);
        if (hdc)
        {
            dpi_x = ((float)GetDeviceCaps(hdc, LOGPIXELSX)) / 96.f;
            dpi_y = ((float)GetDeviceCaps(hdc, LOGPIXELSY)) / 96.f;
            ReleaseDC(NULL, hdc);
        }
#define dpi_x(x) ((int)(ceilf(x * dpi_x)))
#define dpi_y(x) ((int)(ceilf(x * dpi_y)))

        float main_win_w = 280;
        float main_win_h = 280;
        hwnd = CreateWindow(
            class_name,
            "kingsway32",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            dpi_x(main_win_w),
            dpi_y(main_win_h),
            NULL,
            NULL,
            NULL,
            NULL
        );

        ShowWindow(hwnd, SW_SHOWNORMAL);
        UpdateWindow(hwnd);

        /* win95 mode */
        if (0) SetWindowTheme(hwnd, L" ", L" ");

        CreateWindow(
            "BUTTON",
            "Continue",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            dpi_x(0.5f*(main_win_h - 100.0f)),
            dpi_y(main_win_h + 15.0f),
            dpi_x(80),
            dpi_y(25),
            hwnd,
            (HMENU)(1 << CONTINUE_BIT),
            NULL,
            NULL
        );

        float y = 80;
        for (uint64_t i = 0; i < 3; i++) {
            HWND pb_hwnd = CreateWindowEx(
                0,
                PROGRESS_CLASS,
                NULL, 
                WS_CHILD | WS_VISIBLE,
                dpi_x( 25), 
                dpi_y( y), 
                dpi_x(120),
                dpi_y( 20), 
                hwnd,
                0,
                NULL, NULL
            );
            g_progress_bars[i] = pb_hwnd;

            SendMessage(pb_hwnd, PBM_SETRANGE, 0, MAKELPARAM(0, 10));
            SendMessage(pb_hwnd, PBM_SETSTEP, (WPARAM) 1, 0); 
            SendMessage(pb_hwnd, PBM_SETPOS, i*2, 0);

            CreateWindowEx(
                0,
                WC_STATIC,
                g_skills[i],
                WS_VISIBLE | WS_CHILD,
                dpi_x(175),
                dpi_y(  2 + y),
                dpi_x( 70),
                dpi_y( 18),
                hwnd,
                0,
                NULL,
                NULL
            );

            CreateWindow(
                "BUTTON",
                "+",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                dpi_x(142),
                dpi_y(  4 + y),
                dpi_x( 14),
                dpi_y( 14),
                hwnd,
                (HMENU)(void *)(i | (1 << SKILL_INCREMENT_BIT)),
                NULL,
                NULL
            );

            CreateWindow(
                "BUTTON",
                "-",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                dpi_x( 15),
                dpi_y(  4 + y),
                dpi_x( 14),
                dpi_y( 14),
                hwnd,
                (HMENU)(void *)(i | (0 << SKILL_INCREMENT_BIT)),
                NULL,
                NULL
            );

            y += 30.0;
        }

        CreateWindow(
            "EDIT",
            "timmy",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
            dpi_x(140),
            dpi_y( 22),
            dpi_x(105),
            dpi_y( 16),
            hwnd,
            0,
            NULL,
            NULL
        );


        {
            // Create the Combobox
            //
            // Uses the CreateWindow function to create a child window of
            // the application window. The WC_COMBOBOX window style specifies
            // that it is a combobox.

            HWND listbox = CreateWindow(
                WC_LISTBOX, "",
                LBS_STANDARD | WS_CHILD | WS_VISIBLE,
                dpi_x( 20),
                dpi_y( 22),
                dpi_x(105),
                dpi_y( 25),
                hwnd, NULL, NULL, NULL
            );

            char classes[][15] = { "Adventurer", "Warrior", "Wizard", "Rogue" };

            for (int k = 0; k < COUNT_OF(classes); k += 1)
                SendMessage(listbox, LB_ADDSTRING, 0, (LPARAM) &classes[k]);

            /* initial item */
            SendMessage(listbox, LB_SETCURSEL, 0, 0);
        }
#undef dpi_x
#undef dpi_y
    }

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
