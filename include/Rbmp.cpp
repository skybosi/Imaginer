#include <unistd.h>
#include "Rbmp.h"

Rbmp::Rbmp(const char* bmpname):bmppath(bmpname),pBmpBuf(NULL),pColorTable(NULL)
{
	//二进制读方式打开指定的图像文件
	fp = fopen(bmppath.c_str(),"rb");
	if(init_image())
		cout << "init bmp image is OK!" << endl;
	else
	{
		cout << "init bmp image is fair!" << endl;
		return;
	}
	cout << "create a Rbmp ....\n" << endl;
}
bool Rbmp::init_image()
{
	if(fp ==  NULL) 
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
		printf("In get_pix, You set (x,y) is out Range!\n");
		return ppot;
	}
	ppot.setXY(pixel);

	int lineByte = (bmpWidth * biBitCount + 31)/32*4;
	fseek(fp,bfOffBits + lineByte*(bmpHeight-pixel.getX()-1),0); //左上原点
	//	printf("坐标为(%d,%d)的像素\n",pixel.pix_X,pixel.pix_Y);

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
			ppot.setRGB(linedata[rgbsite+2],linedata[rgbsite+1],linedata[rgbsite]);
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
	//	show_PIXELS(PIXELS);
	delete []linedata;
	return ppot;
}
//get 8 point position(x,y)
PIXPOT Rbmp::get_pos8(PIXPOT& pots8, PIXELS pixel)
{
	try
	{
		if(out_range_error(pixel))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pos8 ,You set (x,y) is out Range!\n");
		return pots8;
	}
	//get 4 side point position(x,y)
	memcpy(&pots8.pot4s[0],&pixel,sizeof(PIXELS));
	pots8.pot4s[0].pix_Y-=1;
	memcpy(&pots8.pot4s[1],&pixel,sizeof(PIXELS));
	pots8.pot4s[1].pix_X+=1;
	memcpy(&pots8.pot4s[2],&pixel,sizeof(PIXELS));
	pots8.pot4s[2].pix_Y+=1;
	memcpy(&pots8.pot4s[3],&pixel,sizeof(PIXELS));
	pots8.pot4s[3].pix_X-=1;
	//get 4 angle point position(x,y)
	memcpy(&pots8.pot4a[0],&pixel,sizeof(PIXELS));
	pots8.pot4a[0].pix_X-=1;
	pots8.pot4a[0].pix_Y-=1;
	memcpy(&pots8.pot4a[1],&pixel,sizeof(PIXELS));
	pots8.pot4a[1].pix_X+=1;
	pots8.pot4a[1].pix_Y-=1;
	memcpy(&pots8.pot4a[2],&pixel,sizeof(PIXELS));
	pots8.pot4a[2].pix_X+=1;
	pots8.pot4a[2].pix_Y+=1;
	memcpy(&pots8.pot4a[3],&pixel,sizeof(PIXELS));
	pots8.pot4a[3].pix_X-=1;
	pots8.pot4a[3].pix_Y+=1;
	/*
		 int i = 0;
		 while(i<4)
		 {
		 show_PIXPOT(pots8.pot4s[i]);
		 show_PIXPOT(pots8.pot4a[i]);
		 i++;
		 }
		 */
	//fix the 8 point's position
	pots8.fix_PIXPOT(pots8,bmpWidth,bmpHeight);
	/*
		 printf("===========================\n");
		 int i = 0;
		 while(i<4)
		 {
		 show_PIXPOT(pots8.pot4s[i]);
		 show_PIXPOT(pots8.pot4a[i]);
		 i++;
		 }
		 */
	return pots8;
}
//get the 8 point rgb value
PIXPOT Rbmp::get_pot(PIXELS pixel)
{
	PIXPOT pots8;
	memset(&pots8,0,sizeof(PIXPOT));
	try
	{
		if(out_range_error(pixel))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pot ,You set (x,y) is out Range!\n");
		return pots8;
	}
	//get the 8 point right position(x,y)
	get_pos8(pots8,pixel);
	pots8.pot   = get_pix(pixel);
	//get the 8 point rgb value
	int i = 0;
	while(i<4)
	{
		pots8.pot4s[i] = get_pix(pots8.pot4s[i]);
		pots8.diff4s[i] = pots8.get_diff8RGB(pots8.pot,pots8.pot4s[i]);

		pots8.pot4a[i]  = get_pix(pots8.pot4a[i]);
		pots8.diff4a[i] = pots8.get_diff8RGB(pots8.pot,pots8.pot4a[i]);
		i++;
	}
	pots8.show_PIXPOT();
	return pots8;
}
/*函数名称readIline()*/
bool Rbmp::read_image()
{
	if(fp ==  NULL)
		return false;
	int lineByte;
	lineByte = (bmpWidth * biBitCount + 31)/32*4;
	printf("kkkkk:%d\n",lineByte);
	BYTE8 *linedata = new BYTE8[lineByte];
	PIXELS *lineppot = new PIXELS[bmpWidth];
	fseek(fp,bfOffBits,0); //左上原点
	int k, x = 0 ,y = bmpHeight - 1;
	int tablesite;
//	for(;y >= 0;y--)//左上为原点
		//	for(int i = 0; i < bmpHeight; i++)//左下为原点
	{
		fread(linedata,1,lineByte,fp);
		for(k = 0;x < bmpWidth/*k < lineByte*/; k++)
		{
			printf("%03d ",linedata[k]);
			switch(biBitCount)
			{
				case 24:
					lineppot[x].prgb.rgbBlue  = linedata[k];
					lineppot[x].prgb.rgbGreen = linedata[++k];
					lineppot[x].prgb.rgbRed   = linedata[++k];
					lineppot[x].pix_X = x;
					lineppot[x].pix_Y = y;
					lineppot[x].isEdge(bmpWidth,bmpHeight);
					break;
				case 8:
					tablesite = linedata[k];
					//printf("%03d\n",linedata[k]);
					//printf("颜色表位置:%d\n",tablesite);
					lineppot[x].prgb.rgbRed   = pColorTable[tablesite].rgbRed;
					lineppot[x].prgb.rgbGreen = pColorTable[tablesite].rgbGreen;
					lineppot[x].prgb.rgbBlue  = pColorTable[tablesite].rgbBlue;
					lineppot[x].pix_X = x;
					lineppot[x].pix_Y = y;
					lineppot[x].isEdge(bmpWidth,bmpHeight);
					break;
				default:
					break;
			}
			lineppot[x].show_PIXELS();
			x++;
			printf("\n");
		}
	//	isBoundary(lineppot);
		x = 0;
		cout << endl;
	}
	delete []linedata;
	delete []lineppot;
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
	if((pixel.pix_X >= bmpWidth) |
			(pixel.pix_Y >= bmpHeight) |
			(pixel.pix_X < 0) |
			(pixel.pix_Y < 0) )
		return true;
	else
		return false;

}
