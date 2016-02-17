#include <unistd.h>
#include "Rbmp.h"

Rbmp::Rbmp(const char* bmpname):bmppath(bmpname),pBmpBuf(NULL),pColorTable(NULL)
{
	//二进制读方式打开指定的图像文件
	fp = fopen(bmppath.c_str(),"rb");
	if(initRimage())
		cout << "init bmp image is OK!" << endl;
	else
	{
		cout << "init bmp image is fair!" << endl;
		return;
	}
	cout << "create a Rbmp ....\n" << endl;
}
bool Rbmp::initRimage()
{
	if(NULL == fp)
		return false;
	U16	BM;
	fread(&BM,sizeof(U16),1,fp);
	//printf("BBBBBBB:%0X\n",BM);
	if(BM != 0x4d42)
	{
		printf("is not bmp image!");
		return false;
	}
	rewind(fp);
	//跳过位图文件头结构BITMAPFILEHEADER
	fread(&head, sizeof(BITMAPFILEHEADER),1,fp);
	//printf("BITMAPFILEHEADER:%ld\n",sizeof(BITMAPFILEHEADER));
	bfOffBits  = head.bfOffBits;

	//位图信息头结构变量,存放在变量head中
	fread(&infohead, sizeof(BITMAPINFOHEADER), 1,fp);
	//printf("BITMAPINFOHEADER:%ld\n",sizeof(BITMAPINFOHEADER));
	//获取图像宽、高、每像素所占位数等信息
	bmpWidth   = infohead.biWidth;
	bmpHeight  = infohead.biHeight;
	biBitCount = infohead.biBitCount;

	//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	//int lineBYTE = (bmpWidth * biBitCount/8+3)/4*4;
	//int lineBYTE = (bmpWidth * biBitCount + 31)/32*4;

	//灰度图像有颜色表，且颜色表表项为256
	if(biBitCount ==  8)
	{
		//申请颜色表所需要的空间，读颜色表进内存
		pColorTable = new RGBQUAD[256];
		fread(pColorTable,sizeof(RGBQUAD),256,fp);
		/*
		   int i = 0;
		   printf("颜色表:\n");
		   while(i < 256)
		   {
		   printf("$%d %d ",i,pColorTable[i]);
		   printf("r:%d ",pColorTable[i].rgbRed);
		   printf("g:%d ",pColorTable[i].rgbGreen);
		   printf("b:%d\n",pColorTable[i].rgbBlue);
		   i++;
		   }
		   */
	}
	rewind(fp);
	return true;
}
bool Rbmp::initWimage()
{
	if(NULL == fp)
		return false;
	allhead = new U8[bfOffBits];
	//printf("bfOffBits:%d\n",bfOffBits);
	fread(allhead, bfOffBits,1,fp);
	rewind(fp);
	return true;
}

Rbmp::~Rbmp()
{
	if(pBmpBuf)
		delete []pBmpBuf;
	if(biBitCount == 8 && pColorTable)
		delete []pColorTable;
	if(fp)
		fclose(fp);
	cout << "delete a Rbmp ...." << endl;
}

PIXELS Rbmp::get_pix(int x,int y)
{
	printf("In get_pix(x,y)\n");
	PIXELS ppot;
	ppot.setXY(x,y);
	get_pix(ppot);
	return ppot;
}
PIXELS Rbmp::get_pix(PIXELS pixel)
{
	PIXELS ppot;
	try
	{
		if(out_range_error(pixel))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n",pixel.getX(),pixel.getY());
		return ppot;
	}
	ppot.setXY(pixel);

	int lineByte = (bmpWidth * biBitCount + 31)/32*4;
	fseek(fp,bfOffBits + lineByte*(bmpHeight-1-pixel.getY()),0); //左上原点 very important
	//	printf("坐标为(%d,%d)的像素\n",pixel.getX(),pixel.getY());
	BYTE8 *linedata = new BYTE8[lineByte];//一行像素的字节数(一个像素4个字节)
	//	printf("lineByte:%d\n",lineByte);
	fread(linedata,lineByte,1,fp);//读取一行的所有数据
	//fread(linedata,1,lineByte,fp);//读取一行的所有数据
	int rgbsite;
	int tablesite;
	switch(biBitCount)
	{
		case 24://OK
			//			printf("24位图\n");
			rgbsite = pixel.getX()*3;
			ppot.setRGB(linedata[rgbsite],linedata[rgbsite+1],linedata[rgbsite+2]);
			//ppot.setRGB(linedata[rgbsite+2],linedata[rgbsite+1],linedata[rgbsite]);
			break;
		case 8:
			//			printf("8位图\n");
			rgbsite= pixel.getX();
			tablesite = linedata[rgbsite];
			//printf("%03d\n",linedata[k]);
			//printf("颜色表位置:%d\n",tablesite);
			ppot.setRGB( pColorTable[tablesite]);
			break;
		case 4:
			printf("4位图\n");
			break;
		case 1:
			printf("1位图\n");
			break;
		case 32:
			printf("32位图\n");
			break;
		default:
			break;
	}
	//ppot.show_PIXELS();
	delete []linedata;
	rewind(fp);
	return ppot;
}
//get the 8 point rgb value
PIXPOT Rbmp::get_pot(PIXELS pixel)
{
	PIXPOT pots8;
	PIXELS* pos8;
	try
	{
		if(out_range_error(pixel))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pot ,You set (x,y):(%d,%d) is out Range!\n",pixel.getX(),pixel.getY());
		return pots8;
	}
	pos8 = new PIXELS[9];
	//get the 8 point right position(x,y),if isEdge will be fixup
	pos8 = pots8.get_pos8(pixel,pos8,bmpWidth,bmpHeight);
	/*
	 * put the point and the 8 point arond into a array;
	 *
	 printf("------------------------\n");
	 int i = 0;
	 while(i<9)
	 {
	 pos8[i].show_PIXELS();
	 printf("\n");
	 i++;
	 }
	 printf("------------------------\n");
	 */
	pos8[0] = get_pix(pos8[0]);
	//get the 8 point rgb value
	int i = 0;
	while(i<4)
	{
		pos8[i+1] = get_pix(pos8[i+1]);
		pos8[i+5] = get_pix(pos8[i+5]);
		i++;
	}
	//set the 8 point message
	pots8.set_pots8(pos8);
	pots8.show_PIXPOT();
	if(pos8)
	{
		delete []pos8;
	}
	return pots8;
}
/*函数名称read_image()*/
bool Rbmp::read_image()
{
	if(NULL == fp) 
		return false;
	int lineByte;
	lineByte = (bmpWidth * biBitCount + 31)/32*4;
	BYTE8 *linedata = new BYTE8[lineByte];
	PIXELS *lineppot = new PIXELS[bmpWidth];
	fseek(fp,bfOffBits,0); //左上原点
	int k = 0, x = 0 ,y = bmpHeight - 1;
	int tablesite;
	for(;y >= 0;y--)//左上为原点
		//	for(int y = 0; y < bmpHeight; y++)//左下为原点
	{
		fread(linedata,1,lineByte,fp);
		for(;x < bmpWidth/*k < lineByte*/; k++,x++)
		{
			//printf("%03d ",linedata[k]);
			switch(biBitCount)
			{
				case 24:
					lineppot[x].setRGB(linedata[k],linedata[k+1],linedata[k+2]);
					//lineppot[x].setRGB(linedata[k+2],linedata[k+1],linedata[k]);
					lineppot[x].setXY(x,y);
					lineppot[x].isEdge(bmpWidth,bmpHeight);
					k+=2;
					break;
				case 8:
					tablesite = linedata[k];
					//printf("%03d\n",linedata[k]);
					//printf("颜色表位置:%d\n",tablesite);
					lineppot[x].setRGB(pColorTable[tablesite]);
					lineppot[x].setXY(x,y);
					lineppot[x].isEdge(bmpWidth,bmpHeight);
					break;
				default:
					break;
			}
			//lineppot[x].show_PIXELS();
			//printf("\n");
			//get_pot(lineppot[x]);
			//printf("\n");
		}
		k = x = 0;
	}
	delete []linedata;
	delete []lineppot;
	rewind(fp);
	return true;
}
//is Boundary 
//	PIXPOT *lineppot = new PIXPOT[bmpWidth];
bool Rbmp::isBoundary(PIXELS* lineppot)
{
	PIXPOT pots8;
	int x = 0;
	for(;x < bmpWidth; x++)
	{
		pots8 = get_pot(lineppot[x]);
		printf("=======================\n");
	}

	return true; 
}
/*函数名称readIline()*/
PIXELS* Rbmp::readIline(int beginY,int rows)
{
	if(fp ==  NULL)
		return NULL;
	try
	{
		if((beginY < 0) | (beginY >= bmpHeight))
			throw 0;
	}
	catch(...)
	{
		printf("In readIline ,You set beginY:%d is out Range!\n",beginY);
		return NULL;
	}
	if(rows == 0)
	{
		read_image();
	}
	else
	{
		int lineByte;
		lineByte = (bmpWidth * biBitCount + 31)/32*4;
		//printf("kkkkk:%d\n",lineByte);
		int rows2 = ABS(rows);
		BYTE8 *linedata = new BYTE8[lineByte];
		PIXELS *lineppot = new PIXELS[bmpWidth*rows2];
		int y = 0;
		if(rows < 0)
		{
			fseek(fp,bfOffBits + lineByte*(bmpHeight-1-beginY),0); //左上原点 very important
			//printf("rows: %d\n",rows);
			if(beginY + rows <= 0)
			{
				rows2 = beginY+1;//dealwith rows2
				printf("You set rows2 change %d\n",rows2);
			}
			y = beginY;
		}
		else
		{
			fseek(fp,bfOffBits + lineByte*(bmpHeight-1-(beginY+rows-1)),0); //左上原点 very important
			//printf("rows: %d\n",rows);
			if(beginY + rows >= bmpHeight)
			{
				rows2 = bmpHeight-beginY;//dealwith rows2
				printf("You set rows2 change %d\n",rows2);
			}
			y = beginY+rows2-1;
		}
		//printf("here2:%ld\n",ftell(fp));
		int k = 0, x = 0 ,i=0;
		int tablesite;
		int rows3 = rows2;
		for(; rows2-- ;y--)//左上为原点
		{
			fread(linedata,1,lineByte,fp);
			//printf("line %d row:%d\n",y,rows2);
			//printf("here%d:%ld\n",y,ftell(fp));
			for(;x < bmpWidth; k++,x++,i++)
			{
				//printf("%03d ",linedata[k]);
				switch(biBitCount)
				{
					case 24:
						lineppot[i].setRGB(linedata[k],linedata[k+1],linedata[k+2]);
						//lineppot[x].setRGB(linedata[k+2],linedata[k+1],linedata[k]);
						lineppot[i].setXY(x,y);
						lineppot[i].isEdge(bmpWidth,bmpHeight);
						k+=2;
						break;
					case 8:
						tablesite = linedata[k];
						//printf("%03d\n",linedata[k]);
						//printf("颜色表位置:%d\n",tablesite);
						lineppot[i].setRGB(pColorTable[tablesite]);
						lineppot[i].setXY(x,y);
						lineppot[i].isEdge(bmpWidth,bmpHeight);
						break;
					default:
						break;
				}
				lineppot[i].show_PIXELS();
				printf("\n");
				//get_pot(lineppot[x]);
				//printf("\n");
			}
			k = x =0;
			cout << endl;
		}
		delete []linedata;
		printf("===========================\n");
		for(int i=0;i< bmpWidth*rows3 ;i++)
		{
			//printf("hang:%d,lie:%d\t",i/bmpWidth,i%bmpWidth);
			lineppot[i].show_PIXELS();
			printf("\n");
		}
		printf("===========================\n");
		delReadIline(lineppot);
	}
	rewind(fp);
	return NULL;
}
void Rbmp::delReadIline(PIXELS* lineppot)
{
	if(lineppot)
		delete []lineppot;
}
/*函数名称：save_image()函数参数：char *bmppath文件名字及路径;
  unsigned char *imgBuf待存盘的位图数据;
  int biBitCount每像素所占位数;
  RGBQUAD *pColorTable颜色表指针
  返回值：0为失败，1为成功.
  说明：给定一个图像位图数据、宽、高、颜色表指针及每像素所占的位数等信息，
  将其写到指定文件中*/

bool Rbmp::save_image(char *bmppath,unsigned char *imgBuf,int width,int height, int biBitCount, RGBQUAD *pColorTable)
{
	//如果位图数据指针为0，则没有数据传入，函数返回
	if(!imgBuf) return 0;  

	//颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0
	int colorTablesize = 0;
	if(biBitCount ==  8)
		colorTablesize = 1024;

	//待存储图像数据每行字节数为4的倍数
	//int lineBYTE8 = (width * biBitCount/8+3)/4*4;
	int lineBYTE8 = (bmpWidth*biBitCount+31)/32*4;

	//以二进制写的方式打开文件
	FILE *fp = fopen(bmppath,"wb");
	if(fp ==  0) return 0;

	//申请位图文件头结构变量，填写文件头信息
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4D42;

	//bmp类型  
	//bfSize是图像文件4个组成部分之和
	fileHead.bfSize = sizeof(BITMAPFILEHEADER)+
		sizeof(BITMAPINFOHEADER)+colorTablesize+
		lineBYTE8*height;
	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;

	//bfOffBits是图像文件前3个部分所需空间之和
	fileHead.bfOffBits = 54+colorTablesize;

	//写文件头进文件
	fwrite(&fileHead,sizeof(BITMAPFILEHEADER),1,fp);

	//申请位图信息头结构变量，填写信息头信息
	BITMAPINFOHEADER head;
	head.biBitCount = biBitCount;
	head.biClrImportant = 0;
	head.biClrUsed = 0;
	head.biCompression = 0;
	head.biHeight = height;
	head.biPlanes = 1;
	head.biSize = 40;
	head.biSizeImage = lineBYTE8*height;
	head.biWidth = width;
	head.biXPelsPerMeter = 0;
	head.biYPelsPerMeter = 0;

	//写位图信息头进内存
	fwrite(&head,sizeof(BITMAPINFOHEADER),1,fp);

	//如果灰度图像，有颜色表，写入文件
	if(biBitCount == 8)
		fwrite(pColorTable,sizeof(RGBQUAD),256,fp);

	//写位图数据进文件
	fwrite(imgBuf,height*lineBYTE8,1,fp);

	//关闭文件fclose(fp);
	return 1;
}
bool Rbmp::write_image(const char* outpath)
{
	if(!initWimage())
	{
		return false;
	}
	else
	{
		//FILE* fpo = fopen(outpath,"wb");
		fpo = fopen(outpath,"wb");
		fwrite(allhead,bfOffBits,1,fpo);
		deal_image();
	}
	fclose(fpo);
	fpo = NULL;
	delete []allhead;
	return true;
}
bool Rbmp::deal_image()
{
	if(NULL == fp) 
		return false;
	int lineByte;
	lineByte = (bmpWidth * biBitCount + 31)/32*4;
	BYTE8 *linedata = new BYTE8[lineByte];
	PIXELS *lineppot = new PIXELS[bmpWidth];
	fseek(fp,bfOffBits,0); //左上原点
	int k = 0, x = 0 ,y = bmpHeight - 1;
	int tablesite;
	for(;y >= 0;y--)//左上为原点
		//	for(int y = 0; y < bmpHeight; y++)//左下为原点
	{
		fread(linedata,1,lineByte,fp);
		for(;x < bmpWidth/*k < lineByte*/; k++,x++)
		{
			//printf("%03d ",linedata[k]);
			switch(biBitCount)
			{
				case 24:
					lineppot[x].setRGB(linedata[k],linedata[k+1],linedata[k+2]);
					//lineppot[x].setRGB(linedata[k+2],linedata[k+1],linedata[k]);
					lineppot[x].setXY(x,y);
					lineppot[x].isEdge(bmpWidth,bmpHeight);
					lineppot[x].get3Color(Red);
					lineppot[x].setData(linedata[k],linedata[k+1],linedata[k+2]);
					k+=2;
					break;
				case 8:
					tablesite = linedata[k];
					//printf("%03d\n",linedata[k]);
					//printf("颜色表位置:%d\n",tablesite);
					lineppot[x].setRGB(pColorTable[tablesite]);
					lineppot[x].setXY(x,y);
					lineppot[x].isEdge(bmpWidth,bmpHeight);
					break;
				default:
					break;
			}
			//lineppot[x].show_PIXELS();
			//printf("\n");
			//get_pot(lineppot[x]);
			//printf("\n");
		}
		fwrite(linedata,lineByte,1,fpo);
		k = x = 0;
		//	isBoundary(lineppot);
		//cout << endl;
	}
	delete []linedata;
	delete []lineppot;
	rewind(fp);
	return true;
}
void Rbmp::get_image_msg()
{
	//显示位图文件头结构BITMAPFILEHEADER
	show_bmp_head(head);
	//显示位图信息头结构变量,存放在变量head中
	show_info_head(infohead);
}
void Rbmp::show_bmp_head(BITMAPFILEHEADER &head)
{    
	cout << "位图文件头:" << endl;    
	cout << "\t文件头类型  : " << head.bfType << endl;    
	cout << "\t文件的大小  : " << head.bfSize << endl;    
	cout << "\t保留字 _1   : " << head.bfReserved1 << endl;    
	cout << "\t保留字 _2   : " << head.bfReserved2 << endl;    
	cout << "\tRGB偏移字节 : " << head.bfOffBits << endl << endl;    
}    

void Rbmp::show_info_head(BITMAPINFOHEADER &infohead)
{    
	cout << "位图的信息头:" << endl;    
	cout << "\t结构体的长度: " << infohead.biSize << endl;    
	cout << "\t位图宽(像素): " << infohead.biWidth << endl;    
	cout << "\t位图高(像素): " << infohead.biHeight << endl;    
	cout << "\t位图的平面数: " << infohead.biPlanes << endl;    
	cout << "\t采用颜色位数: " << infohead.biBitCount << endl;    
	cout << "\t压缩的方式  : " << infohead.biCompression << endl;    
	cout << "\t位图数据大小: " << infohead.biSizeImage << endl;    
	cout << "\tX 方向分辨率: " << infohead.biXPelsPerMeter << endl;    
	cout << "\tY 方向分辨率: " << infohead.biYPelsPerMeter << endl;    
	cout << "\t使用的颜色数: " << infohead.biClrUsed << endl;    
	cout << "\t重要颜色数  : " << infohead.biClrImportant << endl;    
}
bool Rbmp::out_range_error(PIXELS pixel)
{
	int x = pixel.getX();
	int y = pixel.getY();
	if((x >= bmpWidth) | (y >= bmpHeight) | (x < 0) | (y < 0) )
		return true;
	else
		return false;
}
