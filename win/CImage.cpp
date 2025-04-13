#include "CImage.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_WINDOWS_UTF8
#include "../stb_image_write.h"


CImage::CImage():m_buf(NULL)
{
	
}

CImage::~CImage()
{
	if(m_buf)
	{
		free(m_buf);
	}
}

int CImage::getBmpFromDc()
{
	BITMAPINFO	bmi;
	unsigned int lineSize = 0;
	if(!::GetObject(m_hBitMap,sizeof(BITMAP),&m_bitMap))
	{
		return -1;
	}
	m_bitMap.bmBitsPixel = 24;
	memset(&bmi,0,sizeof(BITMAPINFO));
	bmi.bmiHeader.biBitCount = m_bitMap.bmBitsPixel;
	bmi.bmiHeader.biCompression = 0;
	bmi.bmiHeader.biWidth = m_bitMap.bmWidth;
	bmi.bmiHeader.biHeight = m_bitMap.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	
	lineSize = bmi.bmiHeader.biWidth*3;
	lineSize = (lineSize+3)&~(3);
	unsigned totalSize = lineSize * 3 * bmi.bmiHeader.biHeight;
	m_buf = (unsigned char*)malloc(m_bitMap.bmWidth * m_bitMap.bmHeight * 3);
	// GetDIBits,pixels for each row of picture should be the width align with 4.but when we use stbi_write_png/stbi_write_jpg/stbi_write_bmp,
	//we just needs (width) ,the padding pixel(s) should be ignore.
	unsigned char *tmpBuffer = (unsigned char*)malloc(totalSize);
	if(!::GetDIBits(m_hdc,m_hBitMap,0,m_bitMap.bmHeight,tmpBuffer,&bmi,DIB_RGB_COLORS))
	{
		return -1;
	}

	//tmpBuffer's data is stored in reverse order of row.
	//so first row of data stored in tmpBuffer,should be the last row of data of the actual picture

	//should flip the picture data by each row , each pixel use 3bytes(red,green,blue)
	//bytes of each line should the number of pixels times 3
	for (int i=0;i<m_bitMap.bmHeight;i++) {
		unsigned char *originBytes = tmpBuffer + (m_bitMap.bmHeight-i-1)*lineSize;
		unsigned char *dst = m_buf + i*m_bitMap.bmWidth*3;
		for (int j=0;j<m_bitMap.bmWidth;j++) {
			dst[j*3] = originBytes[j*3+2];
			dst[j*3+1] = originBytes[j*3+1];
			dst[j*3+2] = originBytes[j*3];
		}
	}
	free(tmpBuffer);
	return 0;
}

CBmpImage::CBmpImage()
{

}
CBmpImage::~CBmpImage()
{

}

int CBmpImage::saveImage(HDC hdc,HBITMAP hbitMap,string fileName)
{
	m_hdc = hdc;
	m_hBitMap = hbitMap;
	this->getBmpFromDc();
	stbi_write_bmp(fileName.c_str(),m_bitMap.bmWidth,m_bitMap.bmHeight,3,m_buf);
	return 0;
	
}

CPngImage::CPngImage()
{

}

CPngImage::~CPngImage()
{

}

int CPngImage::saveImage(HDC hdc,HBITMAP hbitMap,string fileName)
{
	m_hdc = hdc;
	m_hBitMap = hbitMap;
	this->getBmpFromDc();
	stbi_write_png(fileName.c_str(),m_bitMap.bmWidth,m_bitMap.bmHeight,3,m_buf,0);
	return 0;
}

CJpegImage::CJpegImage()
{

}

CJpegImage::~CJpegImage()
{

}

int CJpegImage::saveImage(HDC hdc,HBITMAP hbitMap,string fileName)
{
	m_hdc = hdc;
	m_hBitMap = hbitMap;
	this->getBmpFromDc();
	stbi_write_jpg(fileName.c_str(),m_bitMap.bmWidth,m_bitMap.bmHeight,3,m_buf,0);
	return 0;
}