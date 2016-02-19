#ifndef _RBMP_H_
#define _RBMP_H_
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "ibmp.h"
#include "image.h"
#include "ipoint.h"
using namespace std;
//class Rbmp: public Image
class Rbmp
{
	enum Method
	{
		UD,
		LR,
		UR,
		NONE
	};
	typedef PIXELS** (*Tfun)(Method);
	typedef PIXELS** (*Mfun)(int,int);
	private:
		FILE* fp;
		FILE* fpo;
		int bmpWidth;//图像的宽
		int bmpHeight;//图像的高
		int bfOffBits;
		int biBitCount;//图像类型，每像素位数
		string bmppath;//图像的路径
		BMPALLHEAD* allhead;
		BYTE8* pBmpBuf;//读入图像数据的指针
		PIXELS** allData;//bmp image matrix
		RGBQUAD* pColorTable;//颜色表指针
		BITMAPFILEHEADER head;
		BITMAPINFOHEADER infohead;
	public:
		~Rbmp();
		Rbmp(const char* bmpname);
		//get a point info.
		void get_image_msg();
		PIXELS get_pix(int x,int y);
		PIXELS get_pix(PIXELS pixel);
		//get the 8 point info. around a point
		PIXPOT get_pot(PIXELS pixel);
	public:
		bool deal_image(PIXELS**& imageData);
		//bool deal_image(Tfun udlf);
		//bool deal_image(Mfun move);
		bool writeAllData(PIXELS** imageData);
		PIXELS** readIline(int beginY=0,int rows=0);
		void delReadIline(PIXELS** lineppot,int rows);
		bool isBoundary(PIXELS* lineppot);
		bool write_image(const char* outpath);
		void show_allData();
	private:
		bool init_image();
		bool initWimage();
		bool out_range_error(PIXELS pixel);
		void show_bmp_head(BITMAPFILEHEADER &head);
		void show_info_head(BITMAPINFOHEADER &infohead);
		int addColorTable(PIXELS pixel,BYTE8& linedata);
		bool isNew(PIXELS** imageData);
		PIXELS** imageDatadup2(PIXELS** imageData,PIXELS**& tmpimageData);
		bool delImageData(PIXELS**& imageData);
		PIXELS** newImageData(PIXELS**& imageData,int W,int H);
	public:
		PIXELS** imageMove(PIXELS**& imageData,int mx = 0,int my = 0);
		PIXELS** imageMirror(PIXELS**& imageData,Method method = NONE);
		PIXELS** getImage3Color(PIXELS** imageData,colorType color = Pricolor);
		//bx by begin x,y; ex ey end x,y;
		//-1 is the max size(x=bmpWidth,y=bmpHeight);
		//scale + zoom up, - zoom down
		PIXELS** imageZoom(PIXELS** imageData,int scalex = 1,int scaley = 1);
		//PIXELS** imageZoom(PIXELS** imageData,int scale = 1,int bx = 0,int by = 0,int ex = -1, int ey = -1);

};

#endif
