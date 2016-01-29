#include<stdio.h>
#include<iostream>
#include<string.h>
#include<stdlib.h>
#include "ibmp.h"
unsigned char *pBmpBuf;//读入图像数据的指针
int bmpWidth;//图像的宽
int bmpHeight;//图像的高
RGBQUAD *pColorTable;//颜色表指针
int biBitCount;//图像类型，每像素位数
using namespace std;
/*****************************************************/
/*函数名称readBmp()*/

bool readBmp(char *bmpName)
{
	//二进制读方式打开指定的图像文件
	FILE *fp=fopen(bmpName,"rb");
	if(fp==0) return 0;

	//跳过位图文件头结构BITMAPFILEHEADER
	fseek(fp,sizeof(BITMAPFILEHEADER),0);
	printf("BITMAPFILEHEADER:%ld\n",sizeof(BITMAPFILEHEADER));

	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1,fp);
	printf("BITMAPINFOHEADER:%ld\n",sizeof(BITMAPINFOHEADER));

	//获取图像宽、高、每像素所占位数等信息
	bmpWidth = head.biWidth;
	bmpHeight = head.biHeight;
	biBitCount = head.biBitCount;

	//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	//int lineByte=(bmpWidth * biBitCount/8+3)/4*4;
	int lineByte = (bmpWidth*biBitCount+31)/32*4;

	//灰度图像有颜色表，且颜色表表项为256
	if(biBitCount == 8)
	{
		//申请颜色表所需要的空间，读颜色表进内存
		pColorTable=new RGBQUAD[256];
		fread(pColorTable,sizeof(RGBQUAD),256,fp);
	}

	//申请位图数据所需要的空间，读位图数据进内存
	pBmpBuf = new unsigned char[lineByte * bmpHeight];
	fread(pBmpBuf,1,lineByte * bmpHeight,fp);

	int BytesPerLine;
	BYTE data[999999];
	BYTE Color[1300][3];
	BytesPerLine = (bmpWidth*biBitCount+31)/32*4;
	fseek(fp,54,0);
	int k, m,n = 0;
	for(int i=bmpHeight-1;i>=0;i--)
	{  
		fread(data,1,BytesPerLine,fp);  
		cout<<"第"<< i <<"行[R, G, B]:"<<endl;
		for(k=0;k <bmpWidth*3;k++)
		{
			if(k%3==2)
			{
				m = k/3;	
				Color[k/3][0]=data[k-0];//r
				Color[k/3][1]=data[k-1];//g
				Color[k/3][2]=data[k-2];//b
				printf("[%03d,%03d,%03d] ", Color[m][0], Color[m][1], Color[m][2]);
				n++;
				if(n%8 == 0)
					cout << endl;
			}
		}
		n = 0;
		/*
		int m=1,r,g,b;
		cout<<"第"<< i <<"行[R, G, B]:"<<endl;
		for(k=0;k<bmpWidth;k++)
		{
			r=int(Color[k][0]);
			g=int(Color[k][1]);
			b=int(Color[k][2]);
				printf("[%03d,%03d,%03d] ",r,g,b);
			if(k == 8*m-1)
			{
				cout<<endl;
				m++;
			}
		}
		*/
		cout << endl;
	}
	//关闭文件
	fclose(fp);
	return 1;
}

/*函数名称：saveBmp()函数参数：char *bmpName文件名字及路径;
  unsigned char *imgBuf待存盘的位图数据;
  int width以像素为单位待存盘位图的宽;
  int height以像素为单位待存盘位图高;
  int biBitCount每像素所占位数;
  RGBQUAD *pColorTable颜色表指针
  返回值：0为失败，1为成功.
  说明：给定一个图像位图数据、宽、高、颜色表指针及每像素所占的位数等信息，
  将其写到指定文件中*/

bool saveBmp(char *bmpName,unsigned char *imgBuf,int width,int height,
		int biBitCount, RGBQUAD *pColorTable)
{
	//如果位图数据指针为0，则没有数据传入，函数返回
	if(!imgBuf) return 0;  

	//颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0
	int colorTablesize=0;
	if(biBitCount==8)
		colorTablesize=1024;

	//待存储图像数据每行字节数为4的倍数
	//int lineByte=(width * biBitCount/8+3)/4*4;
	int lineByte=(bmpWidth*biBitCount+31)/32*4;

	//以二进制写的方式打开文件
	FILE *fp=fopen(bmpName,"wb");
	if(fp==0) return 0;

	//申请位图文件头结构变量，填写文件头信息
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4D42;

	//bmp类型  
	//bfSize是图像文件4个组成部分之和
	fileHead.bfSize=sizeof(BITMAPFILEHEADER)+
		sizeof(BITMAPINFOHEADER)+colorTablesize+
		lineByte*height;
	fileHead.bfReserved1=0;
	fileHead.bfReserved2=0;

	//bfOffBits是图像文件前3个部分所需空间之和
	fileHead.bfOffBits=54+colorTablesize;

	//写文件头进文件
	fwrite(&fileHead,sizeof(BITMAPFILEHEADER),1,fp);

	//申请位图信息头结构变量，填写信息头信息
	BITMAPINFOHEADER head;
	head.biBitCount=biBitCount;
	head.biClrImportant=0;
	head.biClrUsed=0;head.biCompression=0;
	head.biHeight=height;
	head.biPlanes=1;
	head.biSize=40;
	head.biSizeImage=lineByte*height;
	head.biWidth=width;
	head.biXPelsPerMeter=0;
	head.biYPelsPerMeter=0;

	//写位图信息头进内存
	fwrite(&head,sizeof(BITMAPINFOHEADER),1,fp);

	//如果灰度图像，有颜色表，写入文件
	if(biBitCount==8)
		fwrite(pColorTable,sizeof(RGBQUAD),256,fp);

	//写位图数据进文件
	fwrite(imgBuf,height*lineByte,1,fp);

	//关闭文件fclose(fp);
	return 1;
}

int main(int argc,char* argv[])
{
	if(argc < 2)
	{
		printf("%s usage: pic.bmp pic2.bmp\n",argv[0]);
		return 0;
	}
	//读入指定BMP文件进内存
	readBmp(argv[1]);
	//输出图像的信息
	printf("width=%d,height=%d,biBitCount=%d\n",bmpWidth,bmpHeight,biBitCount);
	cout<<endl;
	//将图像数据存盘
	char writePath[255] = {0};
	if(argc == 2)
	{
		strcpy(writePath,"./source/tmp.bmp");
		printf("%s\n",writePath);
	}
	else
		strcpy(writePath,argv[2]);
	saveBmp(writePath,pBmpBuf,bmpWidth,bmpHeight,biBitCount,pColorTable);
	//清除缓冲区，pBmpBuf和pColorTable是全局变量，在文件读入时申请的空间
	delete []pBmpBuf;
	if(biBitCount==8)
		delete []pColorTable;
	return 0;
}
