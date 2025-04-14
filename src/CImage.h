#ifndef CIMAGE_INCLUDE_H
#define CIMAGE_INCLUDE_H
#include <string>
#include "ImageBitmapInfo.h"

using namespace std;
class CImage
{
public:
	virtual ~CImage();
	virtual int saveImage(string fileName,ImageBitmapInfo bmpInfo)=0;
};

//NullObject pattern,CNullImge does nothing.
class CNullImage:public CImage
{
public:
	CNullImage(){}
	~CNullImage(){}
	virtual int saveImage(string fileName,ImageBitmapInfo bmpInfo){return 0;}
};

class CBmpImage:public CImage
{
public:
	CBmpImage();
	virtual ~CBmpImage();
	virtual int saveImage(string fileName,ImageBitmapInfo bmpInfo);
};

class CPngImage:public CImage
{
public:
	CPngImage();
	virtual ~CPngImage();
	virtual int saveImage(string fileName,ImageBitmapInfo bmpInfo);
};

class CJpegImage:public CImage
{
public:
	CJpegImage();
	virtual	~CJpegImage();
	virtual  int saveImage(string fileName,ImageBitmapInfo bmpInfo);
};
#endif