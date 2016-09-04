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
#include <stack>
#include "ibmp.h"
#include "image.h"
#include "ipoint.h"
#include "base.h"
typedef PIXELS* pPIXELS;
typedef pPIXELS* ppPIXELS;
typedef struct recImage
{
    ppPIXELS imageDatas;
    int width;
    int height;
}RecImage;
typedef enum PointState
{
	NORMAL = 0,
	INPOT,
	OUTPOT,
	ONLYPOT
}PState;
typedef deque<PIXELS> dPIXELS;
typedef vector<dPIXELS> vdPIXELS;
typedef deque<PIXELS>     dSkipLine;
typedef vector<dSkipLine> SkipTable;
typedef vector<SkipTable> vSkipTable;

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
		vdPIXELS boundarys; //record all boundary line
		vSkipTable skipTables;
		vTrackTable trackTables;//用于抠出轨迹内的部分
		//record the trackdown's result,just for cutout the image
		Frames   frames;    //record all boundaryline's frame Point
		U32 granularity;//图像碎片边缘最少像素
		bool granOpeartor;//contrl the granularity opeartor method
		//granOpeartor: boundarys will save only largger than granularity value's boundaryline :true
		//              boundarys will save only smaller than granularity value's boundaryline :false
		float baseSmlrty;//base Similarity,use to judge is boundary point or not
		U8     testRange;//for use to set the test Range,that can sure a Edge Point is not trackdown again
		//stack<int> skipLine;
	public:
		~Rbmp();
		Rbmp(const char* bmpname);
		Rbmp(const char** bmpnamel);
		//get image bmpWidth
		inline int getW();
		//get image bmpHeight
		inline int getH();
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
		U32 setGranularity(U32 gran,bool opeartor=false);
	public:
		//show the image all pixel all infomation
		//about (x,y), RGB value, isEdge 
		void show_allData();
		//Test a line's Boundary
		bool isBoundaryPoint(PIXELS pot);
		//Test a line's Boundary
		bool isBoundaryPoint(int& x,int& y);
		//deal with image with a series of function
		//@ allData : will dealwith data source
		//@ dealType: deal with the image's type way
		bool deal_image(ppPIXELS allData,const char* dealType = NULL);
		//Write or Create the new image message into a file
		//Get a new image
		//@ outpath : detail output path
		//@ imageData : will dealwith data source,default null
		//@ dealType: deal with the image's type way
		bool write_image(const char* outpath, RecImage imageData = RecImage(),const char* dealType = NULL);
		//Read serval rows from a line
		//@ beginY : Begin line number
		//@ rows   : Read 'rows' lines
		ppPIXELS readNline(int beginY=0,int rows=0);
		//Read a rectangle data
		//@ beginX : Rectangle data Start point x 
		//@ beginY : Rectangle data Start point y
		//@ width  : Rectangle data's width
		//@ height : Rectangle data's height 
		RecImage readRect(int beginX = 0,int beginY = 0,int  = 0,int height  = 0);
		//set BackGround with RGBQUAD
		bool setBackground(RGBQUAD rgb);
		//set BackGround with R,G,B
		bool setBackground(U8 r = 255,U8 g = 255,U8 b = 255);
		//set BackGround with PIXELS
		bool setBackground(const PIXELS& pixel);
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
		bool writeAllData(ppPIXELS& imageData,int width = 0,int height = 0);
		//show bmp image head
		void show_bmp_head(BITMAPFILEHEADER &head);
		//show bmp image info
		void show_info_head(BITMAPINFOHEADER &infohead);
		//show bmp path list
		void show_6path(map<Position,string> pathl);
		//show bmp path list
		void show_line(dPIXELS boundaryline);
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
		bool deburrTrack(dPIXELS& boundaryline);
		//test two border upon point similarity
		float getSimilarity(Position direction,int x,int y,int step = 1);
		//test curr point with background point's similarity
		float getSimilarity(PIXELS backPoint, PIXELS currPoint);
		//Test whether around the start point has been visited
		//bool testStartP(PIXELS& pixel,int range = 2);
		bool testStartP(PIXELS& pixel);
		//Gets the border(boundary) line
		bool getBoundaryLine(int& x, int& y);
		//link each Boundary Line
		void linker(const Frames& frame);
		//Gets pixel point form  boundary line with index
		//boundaryline : boundary line want get pixel
		//index        : index of want to get,support the negative index values,but cannot more than size()
		PIXELS* getBLpixel(dPIXELS& boundaryline,int index = 0);
		void   getNext(Position& pos, int &x,int& y,int& nexts,int& step);
		int    Bsearch(dSkipLine& line,int indexX);
		void   genSkipTable(int x,int y,SkipTable& skipTable2);
		void   genSkipTable(const PIXELS& pixel,SkipTable& skipTable2);
		void   genTrackTable(SkipTable& skipTable2,TrackTable& tracktable);
	public://The function deal with the bmp image (Macroscopic)
		//Function: generate the image's bar diagram 
		//@ imageData: will dealwith data source
		//@ color    : chose a color,default Pricolor
		bool     genBardiagram(ppPIXELS& allData,colorType color = Pricolor);
		//Function: generate the image's Histogram
		//@ allData  : will dealwith data source
		//@ color    : chose a color,default Pricolor
		bool     genHistogram(ppPIXELS& allData,colorType color = Pricolor);
		//Function: Move the image 
		//@ allData   : will dealwith data source
		//@ mx        : The distance at x direction move 
		//@ my        : The distance at y direction move
		ppPIXELS imageMove(ppPIXELS& allData,int mx = 0,int my = 0);
		//Function: Mirror the image
		//@ allData   : will dealwith data source
		//@ method    : Mirror method (see enum Method)
		ppPIXELS imageMirror(ppPIXELS& allData,Method method = NONE);
		//Function: get 3 Color(RGB) 
		//@ allData   : will dealwith data source
		//@ color     : The color (see enum colorType) 
		ppPIXELS getImage3Color(ppPIXELS& allData,colorType color = Pricolor);
		//Function: Zoom the image
		//@ allData   : will dealwith data source
		//@ scalex    : The zoom rate at x direction move
		//@ scaley    : The zoom rate at y direction move
		ppPIXELS imageZoom(ppPIXELS& allData,float scalex = 1.0,float scaley = 1.0);
		//Function: Spherize the image
		//@ radius    : Spherize's radius
		//@ allData   : will dealwith data source
		//NOTE: if radius = 0,will Adhered with bmpHeight and bmpWidth (oval)
		//else will Adhered with a circle of radius
		ppPIXELS imageSpherize(ppPIXELS& allData,float radius = 0.0);
		//Function: Transpose the image
		//@ allData   : will dealwith data source
		//@ AR        : Antegrade(TRUE) and retrograde(FALSE)
		ppPIXELS imageTranspose(ppPIXELS& allData,bool AR = true);
		//Function: Revolution the image
		//@ allData   : will dealwith data source
		//@ px        : The Revolution point set x
		//@ py        : The Revolution point set y
		//@ angle     : The Revolution angle(+ Antegrade, - retrograde) 
		ppPIXELS imageRevolution(ppPIXELS& allData,int px = 0,int py = 0,float angle = 90.0);
		//Function: Shear the image
		//@ allData   : will dealwith data source
		//@ XorY      : Shear the at x or y direction(TRUE :x FALSE: y)
		//@ angle     : The Shear angle(+ Up/Right, - Down/left)
		ppPIXELS imageShear(ppPIXELS& allData,bool XorY = true,float angle = 45.0);
		//Function: spatialize the image
		//  [0] : up [1] : down [2] : left [3] : right [4] : front [5] : back  
		ppPIXELS imageSpatialize(string outpath);
		//Function: Change a image each pixel's Idensity
		//@ allData   : will dealwith data source
		//@ scala     : Thick  or thin scale
		ppPIXELS imageDensity(ppPIXELS& allData,float scale);
	public://The function deal with the bmp image (Microcosmic)
		//Gets all of the border(boundary) line
		void getBoundarys();
		int trackDown(PIXELS& startPoint);
		//Boundary  highlight
		bool boundarysHL();
		bool boundarysHL(vTrackTable& Tables);
		bool boundarylineHL(TrackTable& table);

		bool ImageHL();
		//cut out part of Boundary with cut point(skip table)
		bool imageCutOut();
};

#endif
