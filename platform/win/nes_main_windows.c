#include<Windows.h>

#include"../../src/nes_hardware.h"

static TCHAR szAppName[] = TEXT("NesEmulator");

static void DrawBitmap(HWND hwnd, HDC hdc, int width, int height)
{
	HDC memDC = CreateCompatibleDC(0);
	HBITMAP bmpBack = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(memDC, bmpBack);
	HPEN penBack = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
	SelectObject(memDC, penBack);
	HBRUSH brushBack = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(memDC, brushBack);
	RECT rcClient;//区域结构  
	GetClientRect(hwnd, &rcClient);//获得客户区域  
	HBRUSH brushTemp = (HBRUSH)GetStockObject(WHITE_BRUSH);//获得库存物体，白色画刷。  
	FillRect(memDC, &rcClient, brushTemp);//填充客户区域。  
	//      
	HBRUSH brushObj = CreateSolidBrush(RGB(0, 255, 0));//创建物体画刷  
	//绘制维网格，矩形画法。  
	int dw = 30;
	int rows = height / dw;
	int cols = width / dw;
	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			if (r == c)
			{
				SelectObject(memDC, brushObj);
			}
			else
			{
				SelectObject(memDC, brushBack);
			}
			Rectangle(memDC, c * dw, r * dw, (c + 1) * dw, (r + 1) * dw);
		}
	}

	DeleteObject(brushObj);
	//  
	BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);//复制到系统设备上显示  
	DeleteObject(penBack);  //释放画笔资源  
	DeleteObject(brushBack);//释放画刷资源  
	DeleteObject(bmpBack);  //释放位图资源  
	DeleteDC(memDC);        //释放辅助绘图设备
	Sleep(1);
}

static nes_hardware_info hardware;

static nes_palette_data data[32 * 30 * 8 * 8];

static void DrawBitmap1(HDC hdc, int width, int height)
{
	HDC ccd = CreateCompatibleDC(NULL);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, 32 * 8, 30 * 8);
	SelectObject(ccd, bmp);

	for (int y = 0; y < 30 * 8; y++)
	{
		for (int x = 0; x < 32 * 8; x++)
		{
			SetPixelV(ccd, x, y, RGB(data[32 * 8 * y + x].r, data[32 * 8 * y + x].g, data[32 * 8 * y + x].b));
		}
	}

	StretchBlt(hdc, 0, 0, width, height, ccd, 0, 0, 32 * 8, 30 * 8, SRCCOPY);
	//BitBlt(hdc, 0, 0, width, height, ccd, 0, 0, SRCCOPY);
	DeleteObject(bmp);
	DeleteDC(ccd);
}

static void RunCpu(HWND hwnd)
{
	run_cpu(&hardware);

	render(hardware.p_ppu_info, data);

	RECT rect;
	GetClientRect(hwnd, &rect);

	HDC hdc = GetDC(hwnd);
	DrawBitmap1(hdc, rect.right, rect.bottom);
	ReleaseDC(hwnd, hdc);

	//InvalidateRect(hwnd, &rect, TRUE);
}

static void init()
{
	memset(data, 0x0, sizeof(nes_palette_data) * 32 * 30 * 8 * 8);

	init_hardware(&hardware);

	reset_hardware(&hardware, "F:\\m_project\\NesEmulator\\readme\\nestest.nes");
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		/*case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			DrawBitmap1(hdc, ps.rcPaint.right, ps.rcPaint.bottom);
			EndPaint(hwnd, &ps);
			return 0;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wnd_class;

	init();

	wnd_class.style = CS_HREDRAW | CS_VREDRAW;
	wnd_class.lpfnWndProc = WndProc;
	wnd_class.cbClsExtra = 0;
	wnd_class.cbWndExtra = 0;
	wnd_class.hInstance = hInstance;

	wnd_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd_class.lpszMenuName = NULL;
	wnd_class.lpszClassName = szAppName;

	if (!RegisterClass(&wnd_class))
	{
		MessageBox(NULL, TEXT("此程序必须运行在NT下!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindowEx(0, szAppName, NULL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, SW_NORMAL);
	UpdateWindow(hwnd);
	ShowCursor(FALSE);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (PeekMessageW(&msg, hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			RunCpu(hwnd);
		}
	}

	ShowCursor(TRUE);

	return msg.wParam;
}