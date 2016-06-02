#include <Windows.h>
#include <tchar.h>
#include "CImage.h"

#pragma comment(lib,"Msimg32.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT OnButtonEvent(HWND,UINT,WPARAM,LPARAM);
void    DrawRect(HWND hwnd);

static TCHAR szAppName[] = TEXT("ScreenCapture");
static TCHAR szAppVer[] = "version 0.1";
HDC		g_memDc;
HDC		g_darkDc;
//HDC		g_selfDc;

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
	BLENDFUNCTION	blendFuction = {AC_SRC_OVER,0,200,0};

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
	g_darkDc = ::CreateCompatibleDC(deskDc);
	HBITMAP hDarkBmp = ::CreateCompatibleBitmap(deskDc,width,height);
	HBITMAP oldBmp = (HBITMAP)::SelectObject(g_memDc,hbitMap);
	HBITMAP oldDarkBmp = (HBITMAP)::SelectObject(g_darkDc,hDarkBmp);
	::BitBlt(g_memDc,0,0,width,height,deskDc,0,0,SRCCOPY);
	::BitBlt(g_darkDc,0,0,width,height,deskDc,0,0,BLACKNESS);
	::AlphaBlend(g_darkDc,0,0,width,height,g_memDc,0,0,width,height,blendFuction);
	::ReleaseDC(desktopWnd,deskDc);

	hwnd = ::CreateWindowEx(WS_EX_LTRREADING, szAppName,TEXT("ÆÁÄ»½ØÍ¼"),WS_POPUP,
						CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,0,0,hInstance,0);
	::ShowWindow(hwnd,iCmdShow);
	::UpdateWindow(hwnd);

	while(::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	::DeleteObject(hDarkBmp);
	::DeleteObject(hbitMap);
	::DeleteDC(g_darkDc);
	::DeleteDC(g_memDc);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC				hdc;
	PAINTSTRUCT		ps;
	int width = ::GetSystemMetrics(SM_CXSCREEN);
	int height = ::GetSystemMetrics(SM_CYSCREEN);
	int vkey = int(wParam);
	
	switch(message)
	{
	case WM_PAINT:
		hdc = ::BeginPaint(hwnd,&ps);
		::BitBlt(hdc,0,0,width,height,g_darkDc,0,0,SRCCOPY);
		::EndPaint(hwnd,&ps);
		return 0;
	case WM_CREATE:
		::SetWindowPos(hwnd,HWND_TOPMOST,0,0,width,height,SWP_SHOWWINDOW);
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
	HDC		hdc;
	
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

	RECT rect;
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		if(mouseDown == 0)
		{
			mouseDown = 1;
			startPos.x = xPos;
			startPos.y = yPos;
		}
		return 0;
	case WM_LBUTTONUP:
		if(mouseDown==1)
		{
			mouseDown = 2;

			rect.left = startPos.x<endPos.x?startPos.x:endPos.x;
			rect.right = startPos.x>endPos.x?startPos.x:endPos.x;
			rect.top = startPos.y<endPos.y?startPos.y:endPos.y;
			rect.bottom = startPos.y>endPos.y?startPos.y:endPos.y;

			hdc = ::GetDC(hwnd);
			HDC	selfDc = ::CreateCompatibleDC(hdc);
			HBITMAP hbitMap = ::CreateCompatibleBitmap(hdc,rect.right-rect.left+1,rect.bottom-rect.top+1);
			::SelectObject(selfDc,hbitMap);
			::BitBlt(selfDc,0,0,rect.right-rect.left+1,rect.bottom-rect.top+1,
				g_memDc,rect.left,rect.top,SRCCOPY);
			
			::ReleaseDC(hwnd,hdc);
			if(::GetSaveFileName(&ofn) && strlen(fileName)>0)
			{
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
			}
			::DeleteDC(selfDc);
			::DeleteObject(hbitMap);
			::PostQuitMessage(0);
		}
		return 0;
	case WM_MOUSEMOVE:
		if(mouseDown == 1)
		{
			hdc = ::GetDC(hwnd);
			endPos.x = xPos;
			endPos.y = yPos;
			rect.left = startPos.x<endPos.x?startPos.x:endPos.x;
			rect.right = startPos.x>endPos.x?startPos.x:endPos.x;
			rect.top = startPos.y<endPos.y?startPos.y:endPos.y;
			rect.bottom = startPos.y>endPos.y?startPos.y:endPos.y;

			HRGN hRgn = ::CreateRectRgn(rect.left,rect.top,rect.right+1,rect.bottom+1);
			::ExtSelectClipRgn(hdc,hRgn,RGN_DIFF);
			::BitBlt(hdc,0,0,width,height,g_darkDc,0,0,SRCCOPY);
			::SelectClipRgn(hdc,NULL);
			::DeleteObject(hRgn);

			::BitBlt(hdc,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
						g_memDc,rect.left,rect.top,SRCCOPY);
			::ReleaseDC(hwnd,hdc);
			DrawRect(hwnd);
		}
		return 0;
	}
	return 0;
}

void DrawRect(HWND hwnd)
{
	HDC  hdc = ::GetDC(hwnd);
	RECT rect;
	TCHAR	szBuf[MAX_PATH] = {0};
	int width = ::GetSystemMetrics(SM_CXSCREEN);

	rect.left = startPos.x<endPos.x?startPos.x:endPos.x;
	rect.right = startPos.x>endPos.x?startPos.x:endPos.x;
	rect.top = startPos.y<endPos.y?startPos.y:endPos.y;
	rect.bottom = startPos.y>endPos.y?startPos.y:endPos.y;
	
	int xPos = rect.left;
	int yPos = rect.top-17;
	if(yPos<17)
	{
		yPos = rect.top+1;
		xPos += 5;
	}
	if(xPos>width-50)
	{
		xPos = width-50;
	}

	_sntprintf_s(szBuf,MAX_PATH,_T("%d x %d"),rect.right-rect.left+1,rect.bottom-rect.top+1);
	
	HPEN hPen = ::CreatePen(PS_DASH,1,RGB(255,0,0));
	HPEN oldPen = (HPEN)::SelectObject(hdc,hPen);
	::MoveToEx(hdc,rect.left,rect.top,NULL);
	::LineTo(hdc,rect.right,rect.top);
	::LineTo(hdc,rect.right,rect.bottom);
	::LineTo(hdc,rect.left,rect.bottom);
	::LineTo(hdc,rect.left,rect.top);
	::SetTextColor(hdc,RGB(255,0,0));
	::TextOut(hdc,xPos,yPos,szBuf,lstrlen(szBuf));
	::SelectObject(hdc,oldPen);
	::DeleteObject(hPen);


	::ReleaseDC(hwnd,hdc);
}