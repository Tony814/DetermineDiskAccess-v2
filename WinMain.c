#include "DDAv2.h"

BOOL RegisterMainWindowClass();
HWND CreateMainWindow();
void ShowAboutDialog(HWND owner);
LONG __export FAR PASCAL MainWndProc(HWND hWnd, unsigned msg, UINT wParam, LONG lParam);
extern short GetDiskAccess();

short PageFileResult;
HINSTANCE g_hInstance = NULL;
static const char MainWndClass[] = "DDAv2";

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hWnd;
    MSG msg;

    g_hInstance = hInstance;

    if (!hPrevInstance)
    {
        if (!RegisterMainWindowClass())
        {
            MessageBox(NULL, "Error registering main windows class", "DDAv2 ERROR", MB_ICONHAND | MB_OK);
            return 0;
        }
    }

    if (!(hWnd = CreateMainWindow()))
    {
        MessageBox(NULL, "Error creating main window", "DDAv2 ERROR", MB_ICONHAND | MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    SendMessage(hWnd, WM_GETDISKACCESS, 0, 0);
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}


BOOL RegisterMainWindowClass()
{
    WNDCLASS wc;
    BOOL rc;

    wc.lpfnWndProc = &MainWndProc;
    wc.hInstance = g_hInstance;
    wc.hIcon = LoadIcon(g_hInstance, "IDI_APPICON");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = "IDR_MAINMENU";
    wc.lpszClassName = MainWndClass;
    rc = RegisterClass(&wc);
    return rc;
}

HWND CreateMainWindow()
{
    HWND hWnd;

    hWnd = CreateWindow(
        MainWndClass,
        "DDAv2",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        320,
        200,
        NULL,
        NULL,
        g_hInstance,
        NULL
        );

    return hWnd;
}

LONG __export FAR PASCAL MainWndProc(HWND hWnd, unsigned msg, UINT wParam, LONG lParam)
{
    HDC hDc;
    HICON hStatusIcon;
    int SavedMapMode;
    PAINTSTRUCT ps;

    switch (msg)
    {
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case ID_HELP_ABOUT:
                {
                    ShowAboutDialog(hWnd);
                    break;
                }
                case ID_FILE_EXIT:
                {
                    DestroyWindow(hWnd);
                    break;
                }
                default:
                    return DefWindowProc(hWnd, msg, wParam, lParam);
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_PAINT:
        {
            BeginPaint(hWnd, &ps);
            hDc = GetDC(hWnd);
            SavedMapMode = GetMapMode(hDc);
            SetMapMode(hDc, MM_TEXT);
            SelectObject(hDc, GetStockObject(SYSTEM_FONT));
            TextOut(hDc, 100, 2, "The PageFile VxD reports:", 25);
            switch (PageFileResult)
            {
                case -1:
                    TextOut(hDc, 100, 100, "An error occurred", 17);
                    hStatusIcon = LoadIcon(g_hInstance, "ERRDONOT_ICON");
                    //Icon to 144,84
                    break;
                case 0:
                    TextOut(hDc, 100, 100, "Paging disabled", 15);
                    hStatusIcon = LoadIcon(g_hInstance, "DONOT_ICON");
                    break;
                case 1:
                    TextOut(hDc, 100, 100, "Using MS-DOS", 12);
                    hStatusIcon = LoadIcon(g_hInstance, "MSDOS_ICON");
                    break;
                case 2:
                    TextOut(hDc, 100, 100, "Using BIOS", 10);
                    hStatusIcon = LoadIcon(g_hInstance, "BIOS_ICON");
                    break;
                case 3:
                    TextOut(hDc, 100, 100, "Using FastDisk", 14);
                    hStatusIcon = LoadIcon(g_hInstance, "HW_ICON");
                    break;
                default:
                    TextOut(hDc, 100, 100, "Unknown error", 13);
                    hStatusIcon = LoadIcon(g_hInstance, "ERRDONOT_ICON");
                    break;
            }
            DrawIcon(hDc, 144, 50, hStatusIcon);
            SetMapMode(hDc, SavedMapMode);
            ReleaseDC(hWnd, hDc);
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_CREATE:
            PageFileResult = GetDiskAccess();
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0L;
}

BOOL __export FAR PASCAL AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            WORD id = wParam;
            switch (id)
            {
                case IDOK:
                case IDCANCEL:
                {
                    EndDialog(hwndDlg, TRUE);
                    return TRUE;
                }
            }
            break;
        }
        case WM_INITDIALOG:
            return TRUE;
    }
    return FALSE;
}

void ShowAboutDialog(HWND owner)
{
    FARPROC aboutProc = MakeProcInstance(&AboutDialogProc, g_hInstance);
    DialogBox(g_hInstance, "IDD_ABOUTDIALOG", owner, aboutProc);
    FreeProcInstance(aboutProc);
}

