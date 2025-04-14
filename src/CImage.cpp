#include "CImage.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_WINDOWS_UTF8
#include "stb_image_write.h"

CImage::~CImage()
{

}

CBmpImage::CBmpImage()
{

}
CBmpImage::~CBmpImage()
{

}

int CBmpImage::saveImage(string fileName,ImageBitmapInfo bmpInfo)
{
	stbi_write_bmp(fileName.c_str(),bmpInfo.width,bmpInfo.height,3,bmpInfo.data);
	return 0;
	
}

CPngImage::CPngImage()
{

}

CPngImage::~CPngImage()
{

}

int CPngImage::saveImage(string fileName,ImageBitmapInfo bmpInfo)
{
	stbi_write_png(fileName.c_str(),bmpInfo.width,bmpInfo.height,3,bmpInfo.data,0);
	return 0;
}

CJpegImage::CJpegImage()
{

}

CJpegImage::~CJpegImage()
{

}

int CJpegImage::saveImage(string fileName,ImageBitmapInfo bmpInfo)
{
	stbi_write_jpg(fileName.c_str(),bmpInfo.width,bmpInfo.height,3,bmpInfo.data,0);
	return 0;
}