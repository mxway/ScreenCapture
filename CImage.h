#ifndef CIMAGE_INCLUDE_H
#define CIMAGE_INCLUDE_H
#include <windows.h>
#include <string>
using namespace std;
class CImage
{
public:
	CImage(HDC hdc,HBITMAP hbitMap);
	virtual ~CImage();
	virtual int saveImage(string fileName)=0;
protected:
	int  getBmpFromDc();
protected:
	HDC	m_hdc;
	char *m_buf;
	HBITMAP	m_hBitMap;
	BITMAP	m_bitMap;
};

class CBmpImage:public CImage
{
public:
	CBmpImage(HDC hdc,HBITMAP hbitMap);
	virtual ~CBmpImage();
	virtual int saveImage(string fileName);
};

class CPngImage:public CImage
{
public:
	CPngImage(HDC hdc,HBITMAP hbitMap);
	virtual ~CPngImage();
	virtual int saveImage(string fileName);
};
#endif