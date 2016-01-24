#include <Windows.h>
#include <time.h>
#include "CImage.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT OnButtonEvent(HWND,UINT,WPARAM,LPARAM);

static TCHAR szAppName[] = TEXT("ScreenCapture");
static TCHAR szAppVer[] = "version 0.1";
HDC		g_memDc;
HDC		g_selfDc;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND		hwnd;
	MSG			msg;
	WNDCLASS	wndclass;
	HWND		desktopWnd;
	HDC			deskDc;
	HBITMAP		hbitMap;
	int width = ::GetSystemMetrics(SM_CXSCREEN);
	int height = ::GetSystemMetrics(SM_CYSCREEN);

	wndclass.style = CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbWndExtra = 0;
	wndclass.cbClsExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon    = ::LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor  = ::LoadCursor(NULL,IDC_CROSS);
	wndclass.hbrBackground = ::CreateSolidBrush(RGB(30,30,30));
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if(!::RegisterClass(&wndclass))
	{
		MessageBox(NULL,TEXT("This program requires Windows NT!"),
			szAppName,MB_ICONERROR);
		return 0;
	}
	
	desktopWnd = ::GetDesktopWindow();
	deskDc = ::GetDC(desktopWnd);
	hbitMap = ::CreateCompatibleBitmap(deskDc,width,height);
	g_memDc = ::CreateCompatibleDC(deskDc);
	HBITMAP oldBmp = (HBITMAP)::SelectObject(g_memDc,hbitMap);
	::BitBlt(g_memDc,0,0,width,height,deskDc,0,0,SRCCOPY);
	::ReleaseDC(desktopWnd,deskDc);

	hwnd = ::CreateWindowEx(WS_EX_LAYERED, szAppName,TEXT("ÆÁÄ»½ØÍ¼"),WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,0,0,hInstance,0);
	
	::ShowWindow(hwnd,iCmdShow);
	::UpdateWindow(hwnd);

	while(::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	::DeleteObject(hbitMap);
	::DeleteDC(g_memDc);
	::DeleteDC(g_selfDc);
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//HDC				hdc;
	int width = ::GetSystemMetrics(SM_CXSCREEN);
	int height = ::GetSystemMetrics(SM_CYSCREEN);
	int vkey = int(wParam);
	//DWORD	style;
	switch(message)
	{
	case WM_CREATE:
		::SetWindowLong(hwnd,GWL_STYLE,WS_BORDER);
		::SetWindowPos(hwnd,HWND_TOP,0,0,width,height,SWP_SHOWWINDOW);
		::SetLayeredWindowAttributes(hwnd,0,70,LWA_ALPHA);
		return 0;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		return OnButtonEvent(hwnd,message,wParam,lParam);
	case WM_KEYUP:
		switch(vkey)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			return 0;
		}
		break;
	}
	return ::DefWindowProc(hwnd,message,wParam,lParam);
}

int mouseDown = 0;
POINT	startPos;
POINT	endPos;

LRESULT OnButtonEvent(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);
	char	currentPath[MAX_PATH] = {0};
	char	fileName[1024] = {0};
	HWND	deskWnd;
	HDC		deskDc;
	int width = ::GetSystemMetrics(SM_CXSCREEN);
	int height = ::GetSystemMetrics(SM_CYSCREEN);
	::GetCurrentDirectory(MAX_PATH-1,currentPath);
	OPENFILENAME	ofn = {0};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "BMP file(*.bmp)\0*.bmp\0PNG file(*.png)\0*.png\0\0";
	ofn.lpstrDefExt = "BMP";
	ofn.nFilterIndex = ofn.lpstrFilter?1:0;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 1024;
	ofn.lpstrInitialDir = currentPath;
	ofn.lpstrTitle = "±£´æÍ¼Æ¬";
	ofn.Flags = OFN_OVERWRITEPROMPT;

	POINT	screenCoord;
	RECT rect;
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		if(mouseDown == 0)
		{
			HDC hdc = ::GetDC(hwnd);
			HBITMAP selfBmp = ::CreateCompatibleBitmap(hdc,width,height);
			g_selfDc = ::CreateCompatibleDC(hdc);
			::SelectObject(g_selfDc,selfBmp);
			::BitBlt(g_selfDc,0,0,width,height,hdc,0,0,SRCCOPY);
			::DeleteObject(selfBmp);
			::ReleaseDC(hwnd,hdc);
			mouseDown = 1;
			startPos.x = xPos;
			startPos.y = yPos;
		}
		return 0;
	case WM_LBUTTONUP:
		if(mouseDown)
		{
			mouseDown = 0;
			endPos.x = xPos;
			endPos.y = yPos;

			rect.left = startPos.x<endPos.x?startPos.x:endPos.x;
			rect.right = startPos.x>endPos.x?startPos.x:endPos.x;
			rect.top = startPos.y<endPos.y?startPos.y:endPos.y;
			rect.bottom = startPos.y>endPos.y?startPos.y:endPos.y;
			rect.right--;
			rect.bottom--;
			screenCoord.x = rect.left;
			screenCoord.y = rect.top;
			::ClientToScreen(hwnd,&screenCoord);
			HDC curDc = ::GetDC(GetDesktopWindow());
			HDC	selfDc = ::CreateCompatibleDC(curDc);
			HBITMAP hbitMap = ::CreateCompatibleBitmap(curDc,rect.right-rect.left,rect.bottom-rect.top);
			::SelectObject(selfDc,hbitMap);
			::BitBlt(selfDc,0,0,rect.right-rect.left,rect.bottom-rect.top,
						curDc,screenCoord.x,screenCoord.y,SRCCOPY);
			if(::GetSaveFileName(&ofn) && strlen(fileName)>0)
			{
				clock_t start = clock();
				CImage *image = NULL;
				if(ofn.nFilterIndex == 1)
				{
					image = new CBmpImage(selfDc,hbitMap);
				}
				else
				{
					image = new CPngImage(selfDc,hbitMap);
				}
				image->saveImage(fileName);
				int interval = clock()-start;
				char msg[256] = {0};
				sprintf(msg,"%d",interval);
				MessageBox(NULL,msg,"ÏûÏ¢",MB_OK);
			}
			::ReleaseDC(GetDesktopWindow(),curDc);
			::DeleteDC(selfDc);
			::DeleteObject(hbitMap);
			::PostQuitMessage(0);
		}
		return 0;
	case WM_MOUSEMOVE:
		if(mouseDown)
		{
			HDC	curDc = ::GetDC(hwnd);
			rect.left = startPos.x<xPos?startPos.x:xPos;
			rect.right = startPos.x>xPos?startPos.x:xPos;
			rect.top = startPos.y<yPos?startPos.y:yPos;
			rect.bottom = startPos.y>yPos?startPos.y:yPos;
			screenCoord.x = rect.left;
			screenCoord.y = rect.top;
			::ClientToScreen(hwnd,&screenCoord);
			::BitBlt(curDc,0,0,width,height,g_selfDc,0,0,SRCCOPY);
			::BitBlt(curDc,rect.left,rect.top,rect.right-rect.left,
						rect.bottom-rect.top,
						g_memDc,screenCoord.x,screenCoord.y,SRCCOPY);
			HPEN hPen = ::CreatePen(PS_DASH,1,RGB(255,0,0));
			HPEN oldPen = (HPEN)::SelectObject(curDc,hPen);
			::MoveToEx(curDc,rect.left,rect.top,NULL);
			::LineTo(curDc,rect.right,rect.top);
			::LineTo(curDc,rect.right,rect.bottom);
			::LineTo(curDc,rect.left,rect.bottom);
			::LineTo(curDc,rect.left,rect.top);
			::SelectObject(curDc,oldPen);
			::DeleteObject(hPen);
			
			::ReleaseDC(hwnd,curDc);
		}
		return 0;
	}
	return 0;
}