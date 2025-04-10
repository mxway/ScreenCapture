#ifndef IMAGE_FACTORY_INCLUDE_H
#define IMAGE_FACTORY_INCLUDE_H
#include "CImage.h"

class CImageFactory
{
public:
	static CImage *GetImage(int type);
};

#endif