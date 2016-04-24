#ifndef _RBMP_H_
#define _RBMP_H_
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <vector>
#include <deque>
#include <map>
#include "ibmp.h"
#include "image.h"
#include "ipoint.h"
typedef PIXELS* pPIXELS;
typedef pPIXELS* ppPIXELS;

typedef deque<PIXELS> dPIXELS;
typedef vector<dPIXELS> vdPIXELS;
typedef struct pointxy
{
	int sttx; //start point x
	int endx; //end point x
	int ally; //communal x
}xx_y;
typedef vector<xx_y> vTracktable;
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
		const char** bmppathl;//图像的路径
		map<Position,string> rbmp;
		BMPALLHEAD allhead;
		ppPIXELS allData;//bmp image matrix
		RGBQUAD* pColorTable;//颜色表指针
		BITMAPFILEHEADER head;
		BITMAPINFOHEADER infohead;
		RGBQUAD backGround;
		vdPIXELS boundarys;
		U32 granularity;//图像碎片边缘最少像素
		// pixelTrend: background -> non background : false
		//             non background -> background : true
		bool pixelTrend;
		vTracktable skipTable;//用于抠出轨迹内的部分
		//record the trackdown's result,just for cutout the image
	public:
		~Rbmp();
		Rbmp(const char* bmpname);
		Rbmp(const char** bmpnamel);
		//get image bmpWidth
		int getW();
		//get image bmpHeight
		int getH();
		//get a .bmp image header information
		void get_image_msg();
		//get a point RGB value from the (x,y)
		//if the x,y is out range ,return empty PIXELS
		PIXELS get_pix(int x,int y);
		//get a point RGB value from the pixel
		//if the pixel is out range ,return empty PIXELS
		PIXELS get_pix(PIXELS pixel);
		//get the 8 point info. around a point
		PIXPOT get_pot(PIXELS pixel);
		//set image granularity
		U32 setGranularity(U32 gran);
	public:
		//show the image all pixel all infomation
		//about (x,y), RGB value, isEdge 
		void show_allData();
		//Test a line's Boundary
		bool isBoundaryPoint(PIXELS pot);
		//deal with image with a series of function
		//@ dealType: deal with the image's type way
		bool deal_image(const char* dealType = NULL);
		//Write or Create the new image message into a file
		//Get a new image
		//@ outpath : detail output path
		//@ dealType: deal with the image's type way
		bool write_image(const char* outpath,const char* dealType = NULL);
		//Read serval rows from a line
		//@ beginY : Begin line number
		//@ rows   : Read 'rows' lines
		ppPIXELS readNline(int beginY=0,int rows=0);
		//set BackGround with RGBQUAD
		bool setBackground(RGBQUAD rgb);
		//set BackGround with R,G,B
		bool setBackground(U8 r = 255,U8 g = 255,U8 b = 255);
	private:
		//alike background or not,Mean and the same color as 
		//the background color 
		//NOTE: But not necessarily background 
		//return code:
		//   -1 is empty pixel, 1 alike background,0 not alike background
		inline int alikeBackground(PIXELS pixel);
		//alike background or not,Mean and the same color as 
		//the background color 
		//NOTE: But not necessarily background
		//return code:
		//   -1 is empty pixel, 1 alike background,0 not alike background
		inline int alikeBackground(int x,int y);
		//alike background or not,Mean and the same color as 
		//the background color 
		//NOTE: But not necessarily background
		inline bool alikeBackground(RGBQUAD rgb);
		//Init when open or read a image 
		bool init_image();
		//Init before create or write a image
		bool initSpatialize(const char** imagePath);
		//Test a imageData is new or have some message
		//just for deal with form last step
		bool isNew(ppPIXELS imageData);
		//deal with the pixel's message is out range or not
		bool out_range_error(PIXELS pixel);
		//Set a image allhead message
		bool setHead(BMPALLHEAD& allhead,int W,int H);
		//Write the iamge Data to a file
		bool writeAllData(ppPIXELS& imageData);
		//show bmp image head
		void show_bmp_head(BITMAPFILEHEADER &head);
		//show bmp image info
		void show_info_head(BITMAPINFOHEADER &infohead);
		//show bmp path list
		void show_6path(map<Position,string> pathl);
		//show bmp path list
		void show_line(dPIXELS boundaryline);
		//Get Position's string
		string Pos2str(Position pos);
		//Get colorType's string
		string color2str(colorType color);
		//delete/free the memory image Data after deal with
		bool delImageData(ppPIXELS& imageData,int H);
		//delete/free the memory image same to ~Rbmp()
		bool deleteAll();
		//delete/free the memory image When call readNline
		//@ lineppot : the pointer with the some line data
		//@ rows     : the line number
		void delreadNline(ppPIXELS lineppot,int rows);
		//Add or Create ColorTable if the bmp is 256 or smaller (NOT WORK)
		int addColorTable(PIXELS pixel,BYTE8& linedata);
		//new/malloc some memory to a save the image data will be deal with
		//@ imageData : pointer the image Data
		//@ W         : the row Create two-dimensional array 
		//@ H         : the col Create two-dimensional array
		ppPIXELS newImageData(ppPIXELS& imageData,int W,int H);
		//Copy the imageData to tmpimageData
		//@ imageData    : source image pointer
		//@ tmpimageData : destination iamge pointer
		ppPIXELS imageDatadup2(ppPIXELS imageData,ppPIXELS& tmpimageData);
		//test a point(x,y) if a edge or not
		bool isEdge(int x,int y);
		//just want to get the right point of one point,if get x,y will be reset
		bool getRpoint(Position& direction,int& x,int& y);
		//just want to get the left point of one point,if get x,y will be reset
		bool getLpoint(Position& direction,int& x,int& y);
		//ppPIXELS allData;//bmp image matrix
		bool backGround_ize();
		//is a close/open boundary line or not
		bool isCloseOpen(dPIXELS boundaryline);
		//deburr: delete The Burr on the track
		bool deburrTrack(dPIXELS boundaryline);
	public://The function deal with the bmp image (Macroscopic)
		//Function: generate the image's bar diagram 
		bool     genBardiagram(colorType color = Pricolor);
		//Function: generate the image's Histogram
		bool     genHistogram(colorType color = Pricolor);
		//Function: Move the image 
		//@ mx        : The distance at x direction move 
		//@ my        : The distance at y direction move
		ppPIXELS imageMove(int mx = 0,int my = 0);
		//Function: Mirror the image
		//@ method    : Mirror method (see enum Method)
		ppPIXELS imageMirror(Method method = NONE);
		//Function: get 3 Color(RGB) 
		//@ color     : The color (see enum colorType) 
		ppPIXELS getImage3Color(colorType color = Pricolor);
		//Function: Zoom the image
		//@ scalex    : The zoom rate at x direction move
		//@ scaley    : The zoom rate at y direction move
		ppPIXELS imageZoom(float scalex = 1.0,float scaley = 1.0);
		//Function: Spherize the image
		//@ radius    : Spherize's radius
		//NOTE: if radius = 0,will Adhered with bmpHeight and bmpWidth (oval)
		//else will Adhered with a circle of radius
		ppPIXELS imageSpherize(float radius = 0.0);
		//Function: Transpose the image
		//@ AR        : Antegrade(TRUE) and retrograde(FALSE)
		ppPIXELS imageTranspose(bool AR = true);
		//Function: Revolution the image
		//@ px        : The Revolution point set x
		//@ py        : The Revolution point set y
		//@ angle     : The Revolution angle(+ Antegrade, - retrograde) 
		ppPIXELS imageRevolution(int px = 0,int py = 0,float angle = 90.0);
		//Function: Shear the image
		//@ XorY      : Shear the at x or y direction(TRUE :x FALSE: y)
		//@ angle     : The Shear angle(+ Up/Right, - Down/left)
		ppPIXELS imageShear(bool XorY = true,float angle = 45.0);
		//Function: spatialize the image
		//  [0] : up [1] : down [2] : left [3] : right [4] : front [5] : back  
		ppPIXELS imageSpatialize(string outpath);
	public://The function deal with the bmp image (Microcosmic)
		//Gets the border(boundary) line
		void getBoundaryLine();
		int trackDown(PIXELS& startPoint);
		//Boundary  highlight
		bool boundarysHL();
		bool imageCutOut();

};

#endif
