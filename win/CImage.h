#ifndef CIMAGE_INCLUDE_H
#define CIMAGE_INCLUDE_H
#include <windows.h>
#include <string>
using namespace std;
class CImage
{
public:
	CImage();
	virtual ~CImage();
	virtual int saveImage(HDC hdc,HBITMAP hbitMap,string fileName)=0;
protected:
	int  getBmpFromDc();
protected:
	HDC	m_hdc;
	unsigned char *m_buf;//store the rgb values of selected region,each pixel use 3bytes
	HBITMAP	m_hBitMap;
	BITMAP	m_bitMap;
};

//NullObject pattern,CNullImge does nothing.
class CNullImage:public CImage
{
public:
	CNullImage(){}
	~CNullImage(){}
	virtual int saveImage(HDC hdc,HBITMAP hbitMap,string fileName){return 0;}
};

class CBmpImage:public CImage
{
public:
	CBmpImage();
	virtual ~CBmpImage();
	virtual int saveImage(HDC hdc,HBITMAP hbitMap,string fileName);
};

class CPngImage:public CImage
{
public:
	CPngImage();
	virtual ~CPngImage();
	virtual int saveImage(HDC hdc,HBITMAP hbitMap,string fileName);
};

class CJpegImage:public CImage
{
public:
	CJpegImage();
	virtual	~CJpegImage();
	virtual  int saveImage(HDC hdc,HBITMAP hbitMap,string fileName);
};
#endif