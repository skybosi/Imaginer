#include <unistd.h>
#include "Rbmp.h"
static int globalI = 0;
Rbmp::Rbmp(const char* bmpname):fp(NULL),fpo(NULL),bmppath(bmpname),pBmpBuf(NULL),pColorTable(NULL)
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
	//int lineByte = (bmpWidth * biBitCount/8+3)/4*4;
	//int lineByte = (bmpWidth * biBitCount + 31)/32*4;
	int lineByte;
	lineByte = (bmpWidth * biBitCount + 31)/32*4;

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
			printf("$%d %3d ",i,pColorTable[i]);
			printf("r:%3d ",pColorTable[i].rgbRed);
			printf("g:%3d ",pColorTable[i].rgbGreen);
			printf("b:%3d\n",pColorTable[i].rgbBlue);
			i++;
		}
		*/
	}
	//printf("here1111:%ld\n",ftell(fp));
	//fseek(fp,bfOffBits,0); //左上原点
	BYTE8 *linedata = new BYTE8[lineByte];
	allData = new PIXELS*[bmpWidth];
	int k = 0, x = 0 ,y = bmpHeight - 1;
	int tablesite;
	for(; y >= 0; y--)//y is row number
	{
		allData[y] = new PIXELS[bmpWidth];
		fread(linedata,1,lineByte,fp);
		for(;x < bmpWidth/*k < lineByte*/; k++,x++)
		{
			//printf("%03d ",linedata[k]);
			switch(biBitCount)
			{
				case 24:
					allData[y][x].setRGB(linedata[k],linedata[k+1],linedata[k+2]);
					//allData[y][x].setRGB(linedata[k+2],linedata[k+1],linedata[k]);
					allData[y][x].setXY(x,y);
					allData[y][x].isEdge(bmpWidth,bmpHeight);
					k+=2;
					break;
				case 8:
					tablesite = linedata[k];
					//printf("%03d\n",linedata[k]);
					//printf("颜色表位置:%d\n",tablesite);
					allData[y][x].setRGB(pColorTable[tablesite]);
					allData[y][x].setXY(x,y);
					allData[y][x].isEdge(bmpWidth,bmpHeight);
					break;
				default:
					break;
			}
			//allData[y][x].show_PIXELS();
			//printf("\n");
		}
		k = x = 0;
	}
	delete []linedata;
	rewind(fp);
	return true;
}
bool Rbmp::initWimage()
{
	if(NULL == fp)
		return false;
	if(biBitCount == 24)
	{
		allhead = new U8[bfOffBits];
		//printf("bfOffBits:%d\n",bfOffBits);
		fread(allhead, bfOffBits,1,fp);
		rewind(fp);
	}
	else if(biBitCount == 8)
	{
		allhead = new U8[54];
		fread(allhead, 54,1,fp);
	}
	else;

	return true;
}

Rbmp::~Rbmp()
{
	if(pBmpBuf)
	{
		delete []pBmpBuf;
		pBmpBuf = NULL;
	}
	if(biBitCount == 8 && pColorTable)
	{
		delete []pColorTable;
		pColorTable = NULL;
	}
	if(allData)
	{
		for(int i = 0; i < bmpHeight; i++)   
			delete []allData[i];   
		delete []allData; 
		allData = NULL;
	}
	if(allhead)
	{
		delete []allhead;
		allhead = NULL;
	}
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if(fpo)
	{
		fclose(fpo);
		fpo = NULL;
	}
	cout << "delete a Rbmp ...." << endl;
}

PIXELS Rbmp::get_pix(int x,int y)
{
	printf("In get_pix(x,y)\n");
	PIXELS ppot;
	ppot.setXY(x,y);
	try
	{
		if(out_range_error(ppot))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n",ppot.getX(),ppot.getY());
		return ppot;
	}
	ppot.setRGB(allData[y][x]);
	ppot.show_PIXELS();
	printf("\n");
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
	ppot.setXY(pixel.getX(),pixel.getY());
	ppot.setRGB(allData[pixel.getY()][pixel.getX()]);
	ppot.show_PIXELS();
	printf("\n");
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
	/*
	 * put the point and the 8 point arond into a array;
	 */
	//get the 8 point right position(x,y),if isEdge will be fixup
	pos8 = pots8.get_pos8(pixel,pos8,bmpWidth,bmpHeight);
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
	//pots8.show_PIXPOT();
	if(pos8)
	{
		delete []pos8;
	}
	return pots8;
}
void Rbmp::show_allData()
{
	for(int y = 0; y < bmpHeight;y++)
	{
		for(int x = 0;x < bmpWidth;x++)
		{
			allData[y][x].show_PIXELS();
			printf("\n");
		}
	}
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
PIXELS** Rbmp::readIline(int beginY,int rows)
{
#define DEAL(ROWS,Y) ((ROWS)>0?(Y++):(Y--))
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
		return allData;
	}
	else
	{
		int rows2 = ABS(rows);
		if(beginY + rows <= 0)
		{
			rows2 = beginY+1;//dealwith rows2
			printf("You set rows2 change %d\n",rows2);
		}
		if(beginY + rows >= bmpHeight)
		{
			rows2 = bmpHeight-beginY;//dealwith rows2
			printf("You set rows2 change %d\n",rows2);
		}
		int x,y = beginY;
		PIXELS** lineppot = new PIXELS*[rows2];
		for(;rows2--;DEAL(rows,y))//y is row number
		{
			lineppot[y] = new PIXELS[bmpWidth];
			for(x = 0;x < bmpWidth;x++)
			{
				lineppot[y][x] = allData[y][x];
				lineppot[y][x].show_PIXELS();
				printf("\n");
			}
			x = 0;
		}
		return lineppot;
	}
}
void Rbmp::delReadIline(PIXELS** lineppot,int rows)
{
	if(lineppot)
	{
		for(int i = 0; i < ABS(rows); i++)
			delete []lineppot[i];   
		delete []lineppot; 
	}
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
		switch(biBitCount)
		{
			case 24:
				fwrite(allhead,bfOffBits,1,fpo);
				break;
			case 8:
				//申请颜色表所需要的空间，读颜色表进内存
				//fread(pColorTable,sizeof(RGBQUAD),256,fp);
				fwrite(allhead,54,1,fpo);
				fwrite(pColorTable,sizeof(RGBQUAD)*256,1,fpo);
				break;
			default:
				break;
		}
		deal_image();
	}
	return true;
}
bool Rbmp::deal_image()
{
	PIXELS** imageData = allData;
	imageData = imageTransfer(UD);
	//imageData = imageMove(10,10);
	int lineByte;
	lineByte = (bmpWidth * biBitCount + 31)/32*4;
	BYTE8 *linedata = new BYTE8[lineByte];
	if(pColorTable)
	{
		memset(pColorTable,0,sizeof(RGBQUAD)*256);
	}
	int state;
	int x , k;
	for(int y = (bmpHeight-1); y >= 0;y--)
	{
		for(x = 0,k = 0;x < bmpWidth;x++,k++)
		{
			switch(biBitCount)
			{
				case 24:
					imageData[y][x].get3Color(Red);
					imageData[y][x].setData(linedata[k],linedata[k+1],linedata[k+2]);
					k+=2;
					break;
				case 8:
					imageData[y][x].get3Color(Red);
					state = addColorTable(imageData[y][x],linedata[k]);
					if(state != -1)
						linedata[k] = state;
					break;
				default:
					break;
			}
		}
		fwrite(linedata,lineByte,1,fpo);
	}
	//free memory
	if(imageData && imageData != allData)
	{
		for(int i = 0; i < bmpHeight; i++)
			delete []imageData[i];
		delete []imageData;
	}
	if(linedata)
		delete []linedata;
	return true;
}
//return value = -1:the color is exist
int Rbmp::addColorTable(PIXELS pixel,BYTE8& linedata)
{
	int i = 0;
	while(i < 256)
	{
		//pColorTable[i++] ==  pixel.getRGB()
		if(pColorTable[i].rgbBlue == pixel.getRGB().rgbBlue &&
				pColorTable[i].rgbRed == pixel.getRGB().rgbRed &&
				pColorTable[i].rgbGreen == pixel.getRGB().rgbGreen)
		{
			linedata = i;
			return -1;
		}
		i++;
	}
	/*
	pColorTable[globalI] = pixel.getRGB();
	printf("new颜色表:%d ",globalI);
	pixel.show_PIXELS();
	printf("\n");
	globalI++;*/
	return globalI-1;
}
PIXELS** Rbmp::imageTransfer(Method method)
{
	PIXELS** imageData;
	imageData = new PIXELS*[bmpWidth];
	switch(method)
	{
		case UD://up down change
			for(int y = 0; y < bmpHeight;y++)
			{
				imageData[y] = new PIXELS[bmpWidth];
				for(int x = 0;x < bmpWidth;x++)
				{
					imageData[y][x] = allData[bmpHeight-1-y][x];
				}
			}
			break;
		case LR://left right change
			for(int y = 0; y < bmpHeight;y++)
			{
				imageData[y] = new PIXELS[bmpWidth];
				for(int x = 0;x < bmpWidth;x++)
				{
					imageData[y][x] = allData[y][bmpWidth-1-x];
				}
			}
			break;
		case UR://up down & left right change
			for(int y = 0; y < bmpHeight;y++)
			{
				imageData[y] = new PIXELS[bmpWidth];
				for(int x = 0;x < bmpWidth;x++)
				{
					imageData[y][x] = allData[bmpHeight-1-y][bmpWidth-1-x];
				}
			}
			break;
		default://NONE
			for(int y = 0; y < bmpHeight;y++)
			{
				imageData[y] = new PIXELS[bmpWidth];
				for(int x = 0;x < bmpWidth;x++)
				{
					imageData[y][x] = allData[y][x];
				}
			}
			break;
	}
	return imageData;
}
//move the image:x>0,y>0 ->right down ;x<0 y<0 -> left up
PIXELS** Rbmp::imageMove(int mx,int my)
{
#define MOVEX(Mx,x) ((Mx)>0 ? (x <= Mx) : (x > bmpWidth-1+Mx))
#define MOVEY(My,y) ((My)>0 ? (y <= My) : (y > bmpHeight-1+My))
	PIXELS** imageData;
	imageData = new PIXELS*[bmpWidth];
	for(int y = 0; y < bmpHeight;y++)
	{
		imageData[y] = new PIXELS[bmpWidth];
		for(int x = 0;x < bmpWidth;x++)
		{
			if((MOVEY(my,y) || MOVEX(mx,x)))
				imageData[y][x].setRGB(255,255,255);
			else
				imageData[y][x] = allData[y-my][x-mx];
		}
	}
	return imageData;
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
