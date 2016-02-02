#ifndef _RBMP_H_
#define _RBMP_H_
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "ibmp.h"
#include "image.h"
using namespace std;
//class Rbmp: public Image
class Rbmp
{
	private:
		int bmpWidth;//图像的宽
		int bmpHeight;//图像的高
		int biBitCount;//图像类型，每像素位数
		FILE* fp;
		string bmppath;//图像的路径
		unsigned char *pBmpBuf;//读入图像数据的指针
		RGBQUAD *pColorTable;//颜色表指针
		int bfOffBits;
	public:
		Rbmp(const char* bmpname);
		~Rbmp();
		bool get_image_msg();
		bool read_image();
		PIXPOT get_pot(PIXPOS pixel);
		bool write_image(){return true;};
		bool save_image(char *bmppath,unsigned char *imgBuf,int width,int height, int biBitCount, RGBQUAD *pColorTable);
	private:
		void show_bmp_head(BITMAPFILEHEADER &head);
		void show_info_head(BITMAPINFOHEADER &infohead);

};

#endif
