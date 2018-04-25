#include "CImage.h"
#include "../libpng/png.h"
#include <jpeglib.h>


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
	unsigned totalSize = lineSize * bmi.bmiHeader.biHeight;
	m_buf = (char*)malloc(totalSize);
	if(!::GetDIBits(m_hdc,m_hBitMap,0,m_bitMap.bmHeight,m_buf,&bmi,DIB_RGB_COLORS))
	{
		return -1;
	}
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
	BITMAPFILEHEADER	bmpFileHeader;
	BITMAPINFOHEADER bmpHeader;
	unsigned int lineSize = 0;
	this->getBmpFromDc();
	lineSize = this->m_bitMap.bmWidth * 3;
	lineSize = (lineSize+3)&~(3);
	memset(&bmpFileHeader,0,sizeof(BITMAPFILEHEADER));
	bmpFileHeader.bfType = 0x4d42;
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + lineSize * m_bitMap.bmHeight;

	memset(&bmpHeader,0,sizeof(BITMAPINFOHEADER));
	bmpHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpHeader.biWidth = m_bitMap.bmWidth;
	bmpHeader.biHeight = m_bitMap.bmHeight;
	bmpHeader.biPlanes = 1;
	bmpHeader.biBitCount = 24;
	bmpHeader.biCompression = 0;
	bmpHeader.biSizeImage = bmpFileHeader.bfSize - sizeof(BITMAPFILEHEADER)-sizeof(BITMAPINFOHEADER);
	
	FILE *fp;
	fopen_s(&fp,fileName.c_str(),"wb");
	if(fp==NULL)
	{
		return -1;
	}
	fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&bmpHeader,sizeof(BITMAPINFOHEADER),1,fp);
	fwrite(m_buf,lineSize*m_bitMap.bmHeight,1,fp);
	fclose(fp);
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
	unsigned int lineSize = m_bitMap.bmWidth*3;
	lineSize = (lineSize+3)&~(3);
	png_struct *png = NULL;
	png_info *info = NULL;
	png_color_8 sbit;

	FILE *fp ;
	fopen_s(&fp,fileName.c_str(),"wb");
	if(fp==NULL)
	{
		return -1;
	}
	if(!(png = png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0)))goto END;
	if(!(info=png_create_info_struct(png)))goto END;
	if(setjmp(png_jmpbuf(png)))
	{
		goto END;
	}
	png_init_io(png,fp);
	png_set_IHDR(png,info,m_bitMap.bmWidth,m_bitMap.bmHeight,8,
					PNG_COLOR_TYPE_RGB,PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT,
					PNG_FILTER_TYPE_DEFAULT);
	sbit.red = sbit.green = sbit.blue = 8;
	sbit.alpha = 0;
	png_set_sBIT(png,info,&sbit);
	png_write_info(png,info);
	png_set_bgr(png);
	png_byte **images = (png_bytepp)malloc(sizeof(png_bytep)*m_bitMap.bmHeight);
	for(int i=0; i<m_bitMap.bmHeight; i++)
	{
		images[i] = (png_bytep)(m_buf+lineSize*(m_bitMap.bmHeight-i-1));
	}
	png_write_image(png,images);
	png_write_end(png,info);
	free(images);
END:
	if(fp)fclose(fp);
	if(png)png_destroy_write_struct(&png,&info);
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
	struct jpeg_error_mgr jerr;
	FILE	*fp = NULL;
	fopen_s(&fp,fileName.c_str(),"wb");
	if(fp==NULL)
	{
		return -1;
	}
	jpeg_std_error(&jerr);

	struct jpeg_compress_struct compressStruct;
	compressStruct.err = &jerr;
	jpeg_create_compress(&compressStruct);

	
	jpeg_stdio_dest(&compressStruct,fp);

	compressStruct.image_width = m_bitMap.bmWidth;
	compressStruct.image_height = m_bitMap.bmHeight;
	compressStruct.input_components = 3;
	compressStruct.in_color_space = JCS_RGB;

	jpeg_set_defaults(&compressStruct);
	jpeg_set_quality(&compressStruct,50,1);
	jpeg_start_compress(&compressStruct,1);
	unsigned int tmpLineSize = m_bitMap.bmWidth*3;
	tmpLineSize = (tmpLineSize+3)&(~3);
	JSAMPROW	row_pointer[1];
	for(int i=0;i<m_bitMap.bmHeight;i++)
	{
		row_pointer[0] = (JSAMPROW)(m_buf + tmpLineSize*(m_bitMap.bmHeight-1-i));
		for(int j=0;j<m_bitMap.bmWidth;j++)
		{
			//修改rgb颜色的顺序，将rgb改成gbr
			BYTE	r = row_pointer[0][j*3];
			BYTE	b = row_pointer[0][j*3+2];
			row_pointer[0][j*3] = b;
			row_pointer[0][j*3+2] = r;
		}
		//向jpeg一行一行写入数据
		jpeg_write_scanlines(&compressStruct,row_pointer,1);
	}
	jpeg_finish_compress(&compressStruct);
	jpeg_destroy_compress(&compressStruct);
	fclose(fp);
	return 0;
}