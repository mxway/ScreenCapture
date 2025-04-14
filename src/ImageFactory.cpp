#include "ImageFactory.h"

CImage *CImageFactory::GetImage(int type)
{
	switch(type)
	{
	case 1:
		return new CBmpImage;
		break;
	case 2:
		return new CPngImage;
		break;
	case 3:
		return new CJpegImage;
		break;
	}
	return new CNullImage;
}