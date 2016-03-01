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
	private:
		FILE* fp;     //input file path
		FILE* fpo;   //output file path
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
		//get a .bmp image header information
		void get_image_msg();
		//get a point RGB value from the (x,y)
		PIXELS get_pix(int x,int y);
		//get a point RGB value from the pixel
		PIXELS get_pix(PIXELS pixel);
		//get the 8 point info. around a point
		PIXPOT get_pot(PIXELS pixel);
	public:
		//show the image all pixel all infomation
		//about (x,y), RGB value, isEdge 
		void show_allData();
		//Test a line's Boundary
		bool isBoundary(PIXELS* lineppot);
		//deal with image with a series of function
		bool deal_image(PIXELS**& imageData);
		//Write or Create the new image message into a file
		//Get a new image
		//@ outpath : detail output path
		bool write_image(const char* outpath);
		//Read serval rows from a line
		//@ beginY : Begin line number
		//@ rows   : Read 'rows' lines
		PIXELS** readIline(int beginY=0,int rows=0);
	private:
		//Init when open or read a image 
		bool init_image();
		//Init before create or write a image
		bool initWimage();
		//Test a imageData is new or have some message
		//just for deal with form last step
		bool isNew(PIXELS** imageData);
		//deal with the pixel's message is out range or not
		bool out_range_error(PIXELS pixel);
		//Write the iamge Data to a file
		bool writeAllData(PIXELS** imageData);
		//show bmp image head
		void show_bmp_head(BITMAPFILEHEADER &head);
		//show bmp image info
		void show_info_head(BITMAPINFOHEADER &infohead);
		//delete/free the memory image Data after deal with
		bool delImageData(PIXELS**& imageData,int H);
		//delete/free the memory image When call readIline
		//@ lineppot : the pointer with the some line data
		//@ rows     : the line number
		void delReadIline(PIXELS** lineppot,int rows);
		//Add or Create ColorTable if the bmp is 256 or smaller (NOT WORK)
		int addColorTable(PIXELS pixel,BYTE8& linedata);
		//new/malloc some memory to a save the image data will be deal with
		//@ imageData : pointer the image Data
		//@ W         : the row Create two-dimensional array 
		//@ H         : the col Create two-dimensional array
		PIXELS** newImageData(PIXELS**& imageData,int W,int H);
		//Copy the imageData to tmpimageData
		//@ imageData    : source image pointer
		//@ tmpimageData : destination iamge pointer
		PIXELS** imageDatadup2(PIXELS** imageData,PIXELS**& tmpimageData);
	public://The function deal with the bmp image
		//Function: Move the image 
		//@ imageData : image data pointer
		//@ mx        : The distance at x direction move 
		//@ my        : The distance at y direction move
		PIXELS** imageMove(PIXELS**& imageData,int mx = 0,int my = 0);
		//Function: Mirror the image
		//@ imageData : image data pointer
		//@ method    : Mirror method (see enum Method)
		PIXELS** imageMirror(PIXELS**& imageData,Method method = NONE);
		//Function: get 3 Color(RGB) 
		//@ imageData : image data pointer
		//@ color     : The color (see enum colorType) 
		PIXELS** getImage3Color(PIXELS** imageData,colorType color = Pricolor);
		//Function: Zoom the image
		//@ imageData : image data pointer
		//@ scalex    : The zoom rate at x direction move
		//@ scaley    : The zoom rate at y direction move
		PIXELS** imageZoom(PIXELS** imageData,float scalex = 1.0,float scaley = 1.0);
		//Function: Spherize the image
		//@ imageData : image data pointer
		//@ radius    : Spherize's radius
		//NOTE: if radius = 0,will Adhered with bmpHeight and bmpWidth (oval)
		//else will Adhered with a circle of radius
		PIXELS** imageSpherize(PIXELS** imageData,float radius = 0.0);
		//Function: Transpose the image
		//@ imageData : image data pointer
		//@ AR        : Antegrade(TRUE) and retrograde(FALSE)
		PIXELS** imageTranspose(PIXELS** imageData,bool AR = true);
		//Function: Revolution the image
		//@ imageData : image data pointer
		//@ px        : The Revolution point set x
		//@ py        : The Revolution point set y
		//@ angle     : The Revolution angle(+ Antegrade, - retrograde) 
		PIXELS** imageRevolution(PIXELS** imageData,int px = 0,int py = 0,float angle = 90.0);
		//Function: Shear the image
		//@ imageData : image data pointer
		//@ XorY      : Shear the at x or y direction(TRUE :x FALSE: y)
		//@ angle     : The Shear angle(+ Up/Right, - Down/left)
		PIXELS** imageShear(PIXELS** imageData,bool XorY = true,float angle = 45.0);

};

#endif
