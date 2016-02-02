#include "Rbmp.h"
Rbmp::Rbmp(const char* bmpname):bmppath(bmpname),pBmpBuf(NULL),pColorTable(NULL)
{
	cout << "create a Rbmp ...." << endl;
	//二进制读方式打开指定的图像文件
	fp = fopen(bmppath.c_str(),"rb");
}
Rbmp::~Rbmp()
{
	if(pBmpBuf)
		delete []pBmpBuf;
	if(biBitCount == 8 && pColorTable)
		delete []pColorTable;
	cout << "delete a Rbmp ...." << endl;
}
/*函数名称read_image()*/
bool Rbmp::read_image()
{
	if(fp ==  NULL) 
		return false;
	get_image_msg();
	int lineByte;
	lineByte = (bmpWidth * biBitCount + 31)/32*4;
	BYTE8 *data = new BYTE8[lineByte * bmpHeight];
	BYTE8 Color[1300][3];
	fseek(fp,54,0);//sizeof(BITMAPFILEHEADER + BITMAPINFOHEADER)
	int k, m;
	for(int i = bmpHeight-1; i >= 0; i--)//左上为原点
		//for(int i = 0; i < bmpHeight; i++)//左下为原点
	{  
		fread(data,1,lineByte,fp);  
		cout<<"第"<< i <<"行[B, G, R]:" << endl;
		for(k = 0;k < bmpWidth*3; k++)
		{
			printf("%03d ",data[k]);
			if(k%3 == 2)
			{
				m = k/3;	
				Color[m][0] = data[k-2];//b
				Color[m][1] = data[k-1];//g
				Color[m][2] = data[k-0];//r
				printf("[%03d,%03d,%03d]\n",Color[m][0], Color[m][1], Color[m][2]);
			}
		}
		cout << endl;
	}
	//关闭文件
	fclose(fp);
	delete []data;
	return 1;
}
bool Rbmp::get_image_msg()
{
	if(fp ==  NULL) 
		return false;
	//跳过位图文件头结构BITMAPFILEHEADER
	BITMAPFILEHEADER head;
	fread(&head, sizeof(BITMAPFILEHEADER),1,fp);
	printf("BITMAPFILEHEADER:%ld\n",sizeof(BITMAPFILEHEADER));
	show_bmp_head(head);
	bfOffBits  = head.bfOffBits;

	//定义位图信息头结构变量,存放在变量head中
	BITMAPINFOHEADER infohead;
	fread(&infohead, sizeof(BITMAPINFOHEADER), 1,fp);
	printf("BITMAPINFOHEADER:%ld\n",sizeof(BITMAPINFOHEADER));
	show_info_head(infohead);
	//获取图像宽、高、每像素所占位数等信息
	bmpWidth   = infohead.biWidth;
	bmpHeight  = infohead.biHeight;
	biBitCount = infohead.biBitCount;

	//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	//int lineBYTE = (bmpWidth * biBitCount/8+3)/4*4;
	//int lineBYTE = (bmpWidth * biBitCount + 31)/32*4;

	//申请位图数据所需要的空间，读位图数据进内存
	//pBmpBuf = new unsigned char[lineBYTE * bmpHeight];
	//fread(pBmpBuf,1,lineBYTE * bmpHeight,fp);

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

PIXPOT Rbmp::get_pot(PIXPOS pixel)
{
	PIXPOT pixpot;
	get_image_msg();
	pixpot.pot.pix_X = pixel.pix_X;
	pixpot.pot.pix_Y = pixel.pix_Y;
	pixpot.pot.isEdge = false;

	int lineByte = (bmpWidth * biBitCount + 31)/32*4;
	fseek(fp,bfOffBits + lineByte*(bmpHeight-pixel.pix_Y-1),0); //左上原点
	printf("坐标为(%d,%d)的像素\n",pixel.pix_X,pixel.pix_Y);

	BYTE8 *data = new BYTE8[lineByte];//一行像素的字节数(一个像素4个字节)
	printf("lineByte:%d\n",lineByte);
	int k;
	int tablesite;
	switch(biBitCount)
	{
		case 24://OK
			printf("24位图\n");
			k = pixel.pix_X*3;
			fread(data,lineByte,1,fp);//读取一行的所有数据
			//fread(data,1,lineByte,fp);//读取一行的所有数据
			printf("B G R:");
			while(k < pixel.pix_X*3 + 3 )
			{
				printf("%03d ",data[k]);
				pixpot.prgb.rgbBlue  = data[k-pixel.pix_X*3];
				pixpot.prgb.rgbGreen = data[k-pixel.pix_X*3 + 1];
				pixpot.prgb.rgbRed   = data[k-pixel.pix_X*3 + 2];
				k++;
			}
			break;
		case 8:
			printf("8位图\n");
			k = pixel.pix_X;
			fread(data,lineByte,1,fp);//读取一行的所有数据
			printf("%03d\n",data[k]);
			tablesite = data[k];
			printf("颜色表位置:%d\n",tablesite);
			printf("B G R:");
			printf("%d ",pColorTable[tablesite].rgbRed);
			printf("%d ",pColorTable[tablesite].rgbGreen);
			printf("%d ",pColorTable[tablesite].rgbBlue);
			pixpot.prgb.rgbBlue  = pColorTable[tablesite].rgbBlue;
			pixpot.prgb.rgbGreen = pColorTable[tablesite].rgbGreen;
			pixpot.prgb.rgbRed   = pColorTable[tablesite].rgbRed;
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
	cout << endl;
	return pixpot;
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
