#include <unistd.h>
#include <math.h>
#define PI 3.14159
#define R2D(R) R*180/PI
#define D2R(D) D*PI/180
#include "Rbmp.h"
static int globalI = 0;
Rbmp::Rbmp(const char *bmpname):fp(NULL), fpo(NULL), bmppath(bmpname),pBmpBuf(NULL), allData(NULL), pColorTable(NULL)
{
	// 二进制读方式打开指定的图像文件
	fp = fopen(bmppath.c_str(), "rb");
	if (init_image())
	{
		cout << "init bmp image is OK!" << endl;
	}
	else
	{
		cout << "init bmp image is fair!" << endl;
		return;
	}
	cout << "create a Rbmp ....\n" << endl;
}
Rbmp::Rbmp(const char **bmpnamel):fp(NULL), fpo(NULL),bmppathl(bmpnamel),pBmpBuf(NULL),allData(NULL),pColorTable(NULL)
{
	cout << "create a Rbmp list ....\n" << endl;
}
bool Rbmp::init_image()
{
	if (NULL == fp)
		return false;
	U16 BM;
	fread(&BM, sizeof(U16), 1, fp);
	// printf("BBBBBBB:%0X\n",BM);
	if (BM != 0x4d42)
	{
		printf("is not bmp image!");
		return false;
	}
	rewind(fp);
	// 跳过位图文件头结构BITMAPFILEHEADER
	fread(&head, sizeof(BITMAPFILEHEADER), 1, fp);
	// printf("BITMAPFILEHEADER:%ld\n",sizeof(BITMAPFILEHEADER));
	bfOffBits = head.bfOffBits;

	// 位图信息头结构变量,存放在变量head中
	fread(&infohead, sizeof(BITMAPINFOHEADER), 1, fp);
	// printf("BITMAPINFOHEADER:%ld\n",sizeof(BITMAPINFOHEADER));
	// 获取图像宽、高、每像素所占位数等信息
	bmpWidth = infohead.biWidth;
	bmpHeight = infohead.biHeight;
	biBitCount = infohead.biBitCount;
	allhead.bmpHead = head;
	allhead.infoHead = infohead;
	// 定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	// int lineByte = (bmpWidth * biBitCount/8+3)/4*4;
	// int lineByte = (bmpWidth * biBitCount + 31)/32*4;
	int lineByte;
	lineByte = (bmpWidth * biBitCount + 31) / 32 * 4;

	// 灰度图像有颜色表，且颜色表表项为256
	if (biBitCount == 8)
	{
		// 申请颜色表所需要的空间，读颜色表进内存
		pColorTable = new RGBQUAD[256];
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
		/* 
			 int i = 0; printf("颜色表:\n"); while(i < 256) { printf("$%d %3d 
			 ",i,pColorTable[i]); printf("r:%3d ",pColorTable[i].rgbRed);
			 printf("g:%3d ",pColorTable[i].rgbGreen);
			 printf("b:%3d\n",pColorTable[i].rgbBlue); i++; } */
	}
	// printf("here1111:%ld\n",ftell(fp));
	// fseek(fp,bfOffBits,0); //左上原点
	BYTE8 *linedata = new BYTE8[lineByte];
	// allData = new pPIXELS[bmpWidth];
	allData = (ppPIXELS)calloc(bmpWidth, sizeof(PIXELS));
	int k = 0, x = 0, y = bmpHeight - 1;
	int tablesite;
	for (; y >= 0; y--)			// y is row number
	{
		// allData[y] = new PIXELS[bmpWidth];
		allData[y] = (pPIXELS)calloc(bmpWidth, sizeof(PIXELS));
		fread(linedata, 1, lineByte, fp);
		for (; x < bmpWidth /* k < lineByte */ ; k++, x++)
		{
			// printf("%03d ",linedata[k]);
			switch (biBitCount)
			{
				case 24:
					allData[y][x].setRGB(linedata[k], linedata[k + 1], linedata[k + 2]);
					// allData[y][x].setRGB(linedata[k+2],linedata[k+1],linedata[k]);
					allData[y][x].setXY(x, y);
					allData[y][x].isEdge(bmpWidth, bmpHeight);
					k += 2;
					break;
				case 8:
					tablesite = linedata[k];
					// printf("%03d\n",linedata[k]);
					// printf("颜色表位置:%d\n",tablesite);
					allData[y][x].setRGB(pColorTable[tablesite]);
					allData[y][x].setXY(x, y);
					allData[y][x].isEdge(bmpWidth, bmpHeight);
					break;
				default:
					break;
			}
			// allData[y][x].show_PIXELS();
			// printf("\n");
		}
		k = x = 0;
	}
	delete[]linedata;
	rewind(fp);
	return true;
}

bool Rbmp::initSpatialize(const char** imagePath)
{
	if(NULL == imagePath)
		return  false;
	for(int i = 0; i < 6; ++i)
	{
		//int to enum Position 
		if(strcmp(imagePath[i],""))
		{
			rbmp.insert(pair<Position, string>((Position)i, imagePath[i]));
		}
	}
	return true;
}

Rbmp::~Rbmp()
{
	if (pBmpBuf)
	{
		delete[]pBmpBuf;
		pBmpBuf = NULL;
	}
	if (pColorTable && biBitCount == 8)
	{
		delete[]pColorTable;
		pColorTable = NULL;
	}
	if (allData)
	{
		for (int i = 0; i < bmpHeight; i++)
		{
			// delete []imageData[i];
			free(allData[i]);
		}
		// delete []imageData;
		free(allData);
		allData = NULL;
	}
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if (fpo)
	{
		fclose(fpo);
		fpo = NULL;
	}
	cout << "delete a Rbmp ...." <<  endl;
}
bool Rbmp::deleteAll()
{
	if (pBmpBuf)
	{
		delete[]pBmpBuf;
		pBmpBuf = NULL;
	}
	if (pColorTable && biBitCount == 8)
	{
		delete[]pColorTable;
		pColorTable = NULL;
	}
	if (allData)
	{
		for (int i = 0; i < bmpHeight; i++)
		{
			// delete []imageData[i];
			free(allData[i]);
		}
		// delete []imageData;
		free(allData);
		allData = NULL;
	}
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if (fpo)
	{
		fclose(fpo);
		fpo = NULL;
	}
	cout << "delete all data...." <<  endl;
	return true;
}
int Rbmp::getW()
{
	return bmpWidth;
}

int Rbmp::getH()
{
	return bmpHeight;
}

PIXELS Rbmp::get_pix(int x, int y)
{
	printf("In get_pix(x,y)\n");
	PIXELS ppot;
	ppot.setXY(x, y);
	try
	{
		if (out_range_error(ppot))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", ppot.getX(),
				ppot.getY());
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
		if (out_range_error(pixel))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", pixel.getX(),
				pixel.getY());
		return ppot;
	}
	ppot.setXY(pixel.getX(), pixel.getY());
	ppot.setRGB(allData[pixel.getY()][pixel.getX()]);
	ppot.show_PIXELS();
	printf("\n");
	return ppot;
}

// get the 8 point rgb value
PIXPOT Rbmp::get_pot(PIXELS pixel)
{
	PIXPOT pots8;
	pPIXELS pos8;
	try
	{
		if (out_range_error(pixel))
			throw 0;
	}
	catch(...)
	{
		printf("In get_pot ,You set (x,y):(%d,%d) is out Range!\n", pixel.getX(), pixel.getY());
		return pots8;
	}
	pos8 = new PIXELS[9];
	/* 
	 * put the point and the 8 point arond into a array;
	 */
	// get the 8 point right position(x,y),if isEdge will be fixup
	pos8 = pots8.get_pos8(pixel, pos8, bmpWidth, bmpHeight);
	pos8[0] = get_pix(pos8[0]);
	// get the 8 point rgb value
	int i = 0;
	while (i < 4)
	{
		pos8[i + 1] = get_pix(pos8[i + 1]);
		pos8[i + 5] = get_pix(pos8[i + 5]);
		i++;
	}
	// set the 8 point message
	pots8.set_pots8(pos8);
	// pots8.show_PIXPOT();
	if (pos8)
	{
		delete[]pos8;
	}
	return pots8;
}

void Rbmp::show_allData()
{
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			allData[y][x].show_PIXELS();
			printf("\n");
		}
	}
}

// is Boundary 
// PIXPOT *lineppot = new PIXPOT[bmpWidth];
bool Rbmp::isBoundary(pPIXELS  lineppot)
{
	PIXPOT pots8;
	int x = 0;
	for (; x < bmpWidth; x++)
	{
		pots8 = get_pot(lineppot[x]);
		printf("=======================\n");
	}

	return true;
}

/* 函数名称readIline() */
ppPIXELS Rbmp::readIline(int beginY, int rows)
{
#define DEAL(ROWS,Y) ((ROWS)>0?(Y++):(Y--))
	try
	{
		if ((beginY < 0) | (beginY >= bmpHeight))
			throw 0;
	}
	catch(...)
	{
		printf("In readIline ,You set beginY:%d is out Range!\n", beginY);
		return NULL;
	}
	if (rows == 0)
	{
		return allData;
	}
	else
	{
		int rows2 = ABS(rows);
		if (beginY + rows <= 0)
		{
			rows2 = beginY + 1;	// dealwith rows2
			printf("You set rows2 change %d\n", rows2);
		}
		if (beginY + rows >= bmpHeight)
		{
			rows2 = bmpHeight - beginY;	// dealwith rows2
			printf("You set rows2 change %d\n", rows2);
		}
		int x, y = beginY;
		ppPIXELS lineppot = new pPIXELS[rows2];
		for (; rows2--; DEAL(rows, y))	// y is row number
		{
			lineppot[y] = new PIXELS[bmpWidth];
			for (x = 0; x < bmpWidth; x++)
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

void Rbmp::delReadIline(ppPIXELS  lineppot, int rows)
{
	if (lineppot)
	{
		for (int i = 0; i < ABS(rows); i++)
			delete[]lineppot[i];
		delete[]lineppot;
		lineppot = NULL;
	}
}

bool Rbmp::write_image(const char *outpath)
{
	// FILE* fpo = fopen(outpath,"wb");
	fpo = fopen(outpath, "wb");
	fseek(fpo, 54, 0);		// 跳过allhead
	//ppPIXELS imageData = NULL;
	imageData = newImageData(imageData, bmpWidth, bmpHeight);
	if (deal_image(imageData))
	{
		writeAllData(imageData);
	}
	rewind(fpo);
	switch (biBitCount)
	{
		case 24:
			fwrite(&allhead, bfOffBits, 1, fpo);
			break;
		case 8:
			// 申请颜色表所需要的空间，读颜色表进内存
			// fread(pColorTable,sizeof(RGBQUAD),256,fp);
			fwrite(&allhead, 54, 1, fpo);
			fwrite(pColorTable, sizeof(RGBQUAD) * 256, 1, fpo);
			break;
		default:
			break;
	}
	return true;
}

bool Rbmp::isNew(ppPIXELS imageData)
{
	if(imageData == NULL)
	{
		return true;
	}
	if ((imageData[0][1].getY() != 0) || (imageData[0][1].getY() != 0) ||
			(imageData[0][1].getEdge() != 0) || (imageData[0][1].getRGB().rgbRed != 0) ||
			(imageData[0][1].getRGB().rgbGreen != 0) || (imageData[0][1].getRGB().rgbBlue != 0))
	{
		return false;
	}
	return true;
}
bool Rbmp::setHead(int& lineByte,int W,int H)
{
	if(W <= 0 || H <= 0)
		return false;
	allhead.infoHead.biWidth = W;
	allhead.infoHead.biHeight = H;
	lineByte = (W * biBitCount + 31) / 32 * 4;
	allhead.infoHead.biSizeImage = H * lineByte;
	allhead.bmpHead.bfSize = H * lineByte + 54;
	printf("allhead:%p,lineByte:%d\n",&allhead,lineByte);
	return true;
}

bool Rbmp::writeAllData(ppPIXELS imageData)
{
	int H = allhead.infoHead.biHeight;
	int W = allhead.infoHead.biWidth;
	int lineByte;
	lineByte = (W * biBitCount + 31) / 32 * 4;
	printf("lineByte:%d\n",lineByte);
	BYTE8 *linedata = new BYTE8[lineByte];
	memset(linedata, 0, lineByte);
	if (pColorTable)
	{
		memset(pColorTable, 0, sizeof(RGBQUAD) * 256);
	}
	int state;
	int x, k;
	printf("WWW:%d HHH:%d\n", W, H);
	for (int y = (H - 1); y >= 0; y--)
	{
		for (x = 0, k = 0; x < W; x++, k++)
		{
			//printf("x:%d\n",x);
			switch (biBitCount)
			{
				case 24:
					imageData[y][x].setData(linedata[k], linedata[k + 1], linedata[k + 2]);
					k += 2;
					break;
				case 8:
					state = addColorTable(imageData[y][x], linedata[k]);
					if (state != -1)
						linedata[k] = state;
					break;
				default:
					break;
			}
		}
		//printf("???????????fpo:%p linedata:%p\n",fpo,linedata);
		fwrite(linedata, lineByte, 1, fpo);
		 //printf("y:%d\n",y);
	}
	// free memory
	if (linedata)
		delete[]linedata;
	delImageData(imageData, H);
	return true;

}

// return value = -1:the color is exist
int Rbmp::addColorTable(PIXELS pixel, BYTE8 & linedata)
{
	int i = 0;
	while (i < 256)
	{
		// pColorTable[i++] == pixel.getRGB()
		if (pColorTable[i].rgbBlue == pixel.getRGB().rgbBlue &&
				pColorTable[i].rgbRed == pixel.getRGB().rgbRed &&
				pColorTable[i].rgbGreen == pixel.getRGB().rgbGreen)
		{
			linedata = i;
			return -1;
		}
		i++;
	}
	/* 
		 pColorTable[globalI] = pixel.getRGB(); printf("new颜色表:%d
		 ",globalI); pixel.show_PIXELS(); printf("\n"); globalI++; */
	return globalI - 1;
}

bool Rbmp::deal_image(ppPIXELS  &imageData)
{
	//int Chose = 0;
	//switch(Chose)
	//{
		//imageData = imageShear(imageData,true,-45.0);
		imageData = imageTranspose(imageData);
		//imageData = imageRevolution(imageData,bmpWidth/2,bmpHeight/2,-45);
		//imageData = imageSpherize(imageData, bmpHeight / 2);
		//imageData = imageZoom(imageData,0.5,0.5);
		//imageData = imageMirror(imageData,UR);
		//imageData = imageMove(imageData,100,100);
		//imageData = getImage3Color(imageData,Green);
	//}
	return true;
}

bool Rbmp::genHistogram(colorType color)
{
	if(NULL == allData)
		return false;
	int RED[256] = {0};
	int GRE[256] = {0};
	int BLU[256] = {0};
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			RED[allData[y][x].getRed()]++;
			GRE[allData[y][x].getGreen()]++;
			BLU[allData[y][x].getBlue()]++;
		}
	}
	float Rmax = 0,Gmax = 0,Bmax = 0;
	for(int i = 0; i < 256; i++)
	{
		if(RED[i] > Rmax)
			Rmax=RED[i];
		if(GRE[i] > Gmax)
			Gmax=GRE[i];
		if(BLU[i] > Bmax)
			Bmax=BLU[i];
		//printf("i:%d R:%d G:%d B:%d\n",i,RED[i],GRE[i],BLU[i]);
	}
	//set allhead
	//set Histogram's frame
	int border = 1;
	int Lmargin = 20, Rmargin = 20;
	int Dmargin = 20, Umargin = 20;
	int Lb = Lmargin + border;
	int Rb = Rmargin + border;
	int Ub = Umargin + border;
	int Db = Dmargin + border;
#define Hh 158
	int W = 255 + Lb + Rb;
	int H = Hh + Ub + Db;// 158 = 256*0.618 golden section
	int H_D = H - Dmargin;
	int W_R = W - Rmargin;
	int H_Db = H - Db;
	int W_Rb = W - Rb;
	float Rscale = Hh/Rmax;
	float Gscale = Hh/Gmax;
	float Bscale = Hh/Bmax;
	int x, k;
	int lineByte;
	FILE* fpg = NULL;
	size_t pos = bmppath.rfind(".bmp");
	string outpath = bmppath.substr(0,pos) + color2str(color) +"_htg.bmp";
	//printf("genHistogram :%s Rmax:%f Gmax:%f Blue:%f\n",outpath.c_str() ,Rmax,Gmax,Bmax);
	fpg = fopen(outpath.c_str(),"wb");
	if(setHead(lineByte,W,H))
	{
		//write allhead
		fwrite(&allhead, 54, 1, fpg);
	}
	//write allData
	BYTE8 *linedata = new BYTE8[lineByte];
	memset(linedata,0,lineByte);
	bool flag = true;
	for (int y = 0; y < H; y++)
	{
		for (x = 0, k = 0; x < W; x++, k++)
		{
			if(y < Umargin || y > H_D || x < Lmargin || x > W_R)
			{
				linedata[k++] = 255, linedata[k++] = 255, linedata[k] = 255;
			}
			else if((y >= Umargin && y < Ub) || (y <= H_D && y > H_Db)
					|| (x >= Lmargin && x < Lb) || (x > W_Rb && x <= W_R))
			{
				linedata[k++] = 0, linedata[k++] = 0, linedata[k] = 0;
			}
			else
			{
				switch(color)
				{
					case Red:
					if(RED[x-Rb]*Rscale > y-Db)
					{
						flag = false;
						linedata[k+2] = 255;
					}
					else
						flag = true;
					break;
					case Green:
					if(GRE[x-Rb]*Gscale > y-Db)
					{
						flag = false;
						linedata[k+1] = 255;
					}
					else
						flag = true;
					break;
					case Blue:
					if(BLU[x-Rb]*Bscale > y-Db)
					{
						flag = false;
						linedata[k] = 255;
					}
					else
						flag = true;
					break;
					default:
					printf("NO THIS COLOR!\n");
					break;
				}
				if(flag)
				{
					linedata[k] = 255, linedata[k+1] = 255, linedata[k+2] = 255;
				}
				k+=2;
			}
		}
		fwrite(linedata, lineByte, 1, fpg);
	}
#undef  Hh
	if (linedata)
	{
		delete[]linedata;
		linedata = NULL;
	}
	if(fpg)
	{
		fclose(fpg);
		fpg = NULL;
	}
	return true;
}
ppPIXELS Rbmp::imageMirror(ppPIXELS &imageData, Method method)
{
	ppPIXELS tmpimageData;
	if (isNew(imageData))
	{
		tmpimageData = imageDatadup2(allData, tmpimageData);
	}
	else
	{
		tmpimageData = imageDatadup2(imageData, tmpimageData);
	}
	switch (method)
	{
		case UD:					// up down change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					imageData[y][x] = tmpimageData[bmpHeight - 1 - y][x];
				}
			}
			break;
		case LR:					// left right change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					imageData[y][x] = tmpimageData[y][bmpWidth - 1 - x];
				}
			}
			break;
		case UR:					// up down & left right change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					imageData[y][x] = tmpimageData[bmpHeight - 1 - y][bmpWidth - 1 - x];
				}
			}
			break;
		default:					// NONE
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					imageData[y][x] = tmpimageData[y][x];
				}
			}
			break;
	}
	delImageData(tmpimageData, bmpHeight);
	return imageData;
}

ppPIXELS Rbmp::getImage3Color(ppPIXELS&  imageData, colorType color)
{
	ppPIXELS tmpimageData;
	if (isNew(imageData))
	{
		tmpimageData = imageDatadup2(allData, tmpimageData);
	}
	else
	{
		tmpimageData = imageDatadup2(imageData, tmpimageData);
	}
	switch (color)
	{
		case Red:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					imageData[y][x] = tmpimageData[y][x].get3Color(Red);
				}
			}
			break;
		case Green:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					imageData[y][x] = tmpimageData[y][x].get3Color(Green);
				}
			}
			break;
		case Blue:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					imageData[y][x] = tmpimageData[y][x].get3Color(Blue);
				}
			}
			break;
		default:
			break;
	}
	delImageData(tmpimageData, bmpHeight);
	return imageData;
}

// move the image:x>0,y>0 ->right down ;x<0 y<0 -> left up
ppPIXELS Rbmp::imageMove(ppPIXELS &imageData, int mx, int my)
{
#define MOVEX(Mx,x) ((Mx)>0 ? (x <= Mx) : (x > bmpWidth-1+Mx))
#define MOVEY(My,y) ((My)>0 ? (y <= My) : (y > bmpHeight-1+My))
	ppPIXELS tmpimageData;
	if (isNew(imageData))
	{
		tmpimageData = imageDatadup2(allData, tmpimageData);
	}
	else
	{
		tmpimageData = imageDatadup2(imageData, tmpimageData);
	}
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			if ((MOVEY(my, y) || MOVEX(mx, x)))
				imageData[y][x].setRGB(255, 255, 255);
			else
				imageData[y][x] = tmpimageData[y - my][x - mx];
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return imageData;
}

ppPIXELS Rbmp::imageDatadup2(ppPIXELS imageData, ppPIXELS &tmpimageData)
{
	// tmpimageData = new pPIXELS[bmpWidth];
	tmpimageData = (ppPIXELS)calloc(bmpWidth, sizeof(PIXELS));
	for (int y = 0; y < bmpHeight; y++)
	{
		// tmpimageData[y] = new PIXELS[bmpWidth];
		tmpimageData[y] = (pPIXELS)calloc(bmpWidth, sizeof(PIXELS));
		memcpy(tmpimageData[y], imageData[y], sizeof(PIXELS) * bmpWidth);
	}
	return tmpimageData;
}

ppPIXELS Rbmp::imageZoom(ppPIXELS& imageData, float scalex, float scaley)
{
	ppPIXELS tmpimageData;
	if (isNew(imageData))
	{
		tmpimageData = imageDatadup2(allData, tmpimageData);
	}
	else
	{
		tmpimageData = imageDatadup2(imageData, tmpimageData);
	}
	if (scalex <= 0.0 || scaley <= 0.0)
	{
		printf("zoom number is <= 0 ,is wrong");
		return NULL;
	}
	else
	{
		int W = bmpWidth * scalex;
		int H = bmpHeight * scaley;
		delImageData(imageData, bmpHeight);	// free
		if (imageData == NULL)	// renew
		{
			newImageData(imageData, W, H);
			// printf("new is ok\n");
		}
		// printf("WWW:%d HHH:%d\n",W,H);
		for (int y = 0; y < H; y++)
		{
			for (int x = 0; x < W; x++)
			{
				int nx = (int)(x / scalex);
				int ny = (int)(y / scaley);
				imageData[y][x] = tmpimageData[ny][nx];
				imageData[y][x].setXY(x, y);
				// imageData[y][x].show_PIXELS();
				// tmpimageData[y/scaley][x/scalex].show_PIXELS();
				// printf("\n");
			}
		}
		// first free tmpimageData
		delImageData(tmpimageData, bmpHeight);
		// then set allhead
		int lineByte;
		setHead(lineByte,W,H);
	}
	return imageData;
}

ppPIXELS Rbmp::imageTranspose(ppPIXELS& imageData,bool AR)
{
	ppPIXELS tmpimageData;
	if (isNew(imageData))
	{
		tmpimageData = imageDatadup2(allData, tmpimageData);
	}
	else
	{
		tmpimageData = imageDatadup2(imageData, tmpimageData);
	}
	int W = bmpHeight;
	int H = bmpWidth;
	delImageData(imageData, bmpHeight);	// free
	if (imageData == NULL)		// renew
	{
		newImageData(imageData, W, H);
		// printf("new is ok\n");
	}
	// printf("WWW:%d HHH:%d\n",W,H);
#define AorR(x,y) ((AR) ? (tmpimageData[x][H-1-y]) : (tmpimageData[W-1-x][y]))
	for (int y = 0;y < H;y++)
	{
		for (int x = 0;x < W;x++)
		{
			imageData[y][x] = AorR(x,y);
		}
	}
	/*
		 {
		 for (int y = 0;y < H;y++)
		 {
		 for (int x = 0;x < W;x++)
		 {
		 imageData[y][x] = tmpimageData[x][H-1-y];
		 }
		 }
		 }
		 else
		 {
		 for (int y = 0;y < H;y++)
		 {
		 for (int x = 0;x < W;x++)
		 {
		 imageData[y][x] = tmpimageData[W-1-x][y];
		 }
		 }

		 }
		 */
	// first free tmpimageData
	delImageData(tmpimageData, bmpHeight);
	// then set allhead
	int lineByte;
	setHead(lineByte,W,H);
	return imageData;
}

ppPIXELS Rbmp::imageShear(ppPIXELS& imageData,bool XorY,float angle)
{
	angle = D2R(angle);
	ppPIXELS tmpimageData;
	int nx, ny;
	if (isNew(imageData))
	{
		tmpimageData = imageDatadup2(allData, tmpimageData);
	}
	else
	{
		printf("!!!!!!!!!!!\n");
		tmpimageData = imageDatadup2(imageData, tmpimageData);
	}
	int H = XorY ? bmpHeight : (int)(bmpHeight + bmpWidth * ABS(tan(angle)));
	int W = XorY ? (int)(bmpWidth + bmpHeight * ABS(tan(angle))) : bmpWidth;
	printf("Shear after: W:%d H:%d\n",W,H);
	delImageData(imageData, bmpHeight);	// free
	if (imageData == NULL)	// renew
	{
		newImageData(imageData, W, H);
		// printf("new is ok\n");
	}
	for (int y = 0; y < H; y++)
	{	    
		for (int x = 0; x < W; x++)
		{
			//right up
			//nx = XorY ? x + tan(angle) * (y - bmpHeight): x;
			//ny = XorY ? y : y + tan(angle) * (x - bmpWidth);
			//left down 
			//nx = XorY ? x - tan(angle) * (y - bmpHeight)- bmpHeight: x;
			//ny = XorY ? y : y - tan(angle) * (x - bmpWidth) - bmpWidth;
			nx = (XorY ? ((angle > 0) ? (x + tan(angle) * (y - bmpHeight)) : (x + tan(angle) * (y - bmpHeight) - bmpHeight)) : x);
			ny = (XorY ? y : ((angle > 0) ? (y + tan(angle) * (x - bmpWidth)) :(y + tan(angle) * (x - bmpWidth) - bmpWidth)));
			//printf("Shear after: x:%d nx:%d y:%d ny:%d\n",x,nx,y,ny);
			if (nx < 0 || ny < 0 || nx >= bmpWidth || ny >= bmpHeight)
				imageData[y][x].setRGB(255, 255, 255);
			else
				imageData[y][x] = tmpimageData[ny][nx];
		}
	}	// then set allhead
	//delImageData(imageData,H);
	int lineByte;
	setHead(lineByte,W,H);
	delImageData(tmpimageData, bmpHeight);
	printf("INNNNN: H:%d %p\n",H,imageData);
	return imageData;
}

ppPIXELS Rbmp::imageRevolution(ppPIXELS&  imageData,int px,int py,float angle)
{
	angle = D2R(angle);
	ppPIXELS tmpimageData;
	int nx, ny;
	if (isNew(imageData))
	{
		tmpimageData = imageDatadup2(allData, tmpimageData);
	}
	else
	{
		tmpimageData = imageDatadup2(imageData, tmpimageData);
	}
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{

			nx = (x - px) * cos(angle) - (y - py) * sin(angle) + px;
			ny = (x - px) * sin(angle) + (y - py) * cos(angle) + py;
			//printf("angle: %f cos:%f X:%d nx:%d\t Y:%d ny:%d\n",angle, cos(angle),x,nx,y,ny);
			if (nx < 0 || ny < 0 || nx >= bmpWidth || ny >= bmpHeight)
				imageData[y][x].setRGB(255, 255, 255);
			else
				imageData[y][x] = tmpimageData[ny][nx];
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return imageData;
}

ppPIXELS Rbmp::imageSpherize(ppPIXELS& imageData, float radius)
{
	ppPIXELS tmpimageData;
	int w = bmpWidth / 2;
	int h = bmpHeight / 2;
	int mx1, mx2, nx;
	float dealt;
	if (isNew(imageData))
	{
		tmpimageData = imageDatadup2(allData, tmpimageData);
	}
	else
	{
		tmpimageData = imageDatadup2(imageData, tmpimageData);
	}
	if (radius <= 0.0)			// oval
	{
		for (int y = 0; y < bmpHeight; y++)
		{
			mx1 = w + (w * sqrt(2 * h * y - y * y)) / h;
			mx2 = w - (w * sqrt(2 * h * y - y * y)) / h;
			dealt = (mx1 - mx2) / (bmpWidth * 1.0);
			printf("Y:%d, %d - %d %d ;%f\n", y, mx1, mx2, mx1 - mx2, dealt);
			for (int x = 0; x < bmpWidth; x++)
			{
				if (x < mx2 || x > mx1 || dealt == 0)
					imageData[y][x].setRGB(255, 255, 255);
				else
				{
					nx = (x - mx2) / dealt;
					if (nx >= bmpWidth)
					{
						nx = bmpWidth - 1;
					}
					imageData[y][x] = tmpimageData[y][nx];
				}
			}
		}
	}

	else						// circle
	{
		for (int y = 0; y < bmpHeight; y++)
		{
			mx1 = w + sqrt(radius * radius - (y - h) * (y - h));
			// mx1 = w + sqrt(2*h*y-y*y);
			mx2 = w - sqrt(radius * radius - (y - h) * (y - h));
			// mx2 = w - sqrt(2*h*y-y*y);
			dealt = (mx1 - mx2) / (bmpWidth * 1.0);
			printf("Y:%d, %d - %d ;%f\n", y, mx1, mx2, dealt);
			for (int x = 0; x < bmpWidth; x++)
			{
				// if(x < mx2 || x > mx1)
				if (x < mx2 || x > mx1 || dealt == 0)
					imageData[y][x].setRGB(255, 255, 255);
				else
				{
					nx = (x - mx2) / dealt;
					if (nx >= bmpWidth)
					{
						nx = bmpWidth - 1;
					}
					imageData[y][x] = tmpimageData[y][nx];
				}
			}
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return imageData;
}
ppPIXELS Rbmp::imageSpatialize(string outpath)
{
	if (initSpatialize(bmppathl))
	{
		cout << "init bmp list image is OK!" << endl;
		//show_6path(rbmp);
	}
	else
	{
		cout << "init bmp list image is fair!" << endl;
		return NULL;
	}
	map<Position,string>::iterator it;
	ppPIXELS imageData[6] = {NULL};
	int i;
	int LH = 0,RH = 0, UH = 0,DH = 0, FH = 0,BH = 0, H = 0,W = 0;
	size_t pos = bmppath.rfind(".bmp");
	if(pos == string::npos)
		outpath += "_Spl.bmp";
	fpo = fopen(outpath.c_str(), "wb");
	for(i = 0,it = rbmp.begin(); it != rbmp.end() ; ++it,++i)
	{
		cout<<"key: " << it->first <<" (" << Pos2str(it->first) 
			<< ")" << "\tvalue: " << it->second << endl;
		bmppath = it->second;
		fpo = fopen(bmppath.c_str(), "rb");
		if (init_image())
		{
			cout << "init bmp image is OK!" << endl;
		}
		printf("it->first:%d\n",it->first);
		switch(it->first)
		{
			case Left:
				imageShear(imageData[Left],false,45.0);
				H = allhead.infoHead.biHeight;
				LH = H;
				break;
			case Right:
				imageShear(imageData[Right],false,-45.0);
				H = allhead.infoHead.biHeight;
				RH = H;
				break;
			case Up:
				imageShear(imageData[Up],true,45.0);
				W = allhead.infoHead.biWidth;
				UH = allhead.infoHead.biHeight;
				break;
			case Down:
				imageShear(imageData[Down],true,-45.0);
				W = allhead.infoHead.biWidth;
				DH = allhead.infoHead.biHeight;
				break;
			case Front:
				FH = allhead.infoHead.biHeight;
				break;
			case Back:
				BH = allhead.infoHead.biHeight;
				break;
			default:	
				printf("why:%d\n",it->first);
				break;
		}
		printf("shear is ok... W:%d H:%d\n",W,H);
		get_image_msg();
		deleteAll();
	}
	//write image
	int lineByte = 0;
	setHead(lineByte,W,H);
	ppPIXELS newImage = NULL;
	newImageData(newImage,W,H);
	printf("imageData:%p,fpo:%p",imageData[Down],fpo);
	//imageData[Down][0][0].show_PIXELS();
	writeAllData(newImage);
	if(LH)
	{
		printf("Lwwww%d\n",LH);
		delImageData(imageData[Left],LH);
	}
	if(RH)
	{
		printf("Rwwww%d\n",RH);
		delImageData(imageData[Right],RH);
	}
	if(UH)
	{
		printf("Uwwww%d\n",UH);
		delImageData(imageData[Up],UH);
	}
	if(DH)
	{
		printf("Dwwww%d ",DH);
		delImageData(imageData[Down],DH);
	}
	if(FH)
	{
		printf("Fwwww%d\n",FH);
		delImageData(imageData[Front],FH);
	}
	if(BH)
	{
		printf("Bwwww%d\n",BH);
		delImageData(imageData[Back],BH);
	}
	return NULL;
}

ppPIXELS Rbmp::newImageData(ppPIXELS &imageData, int W, int H)
{
	// malloc some memroy
	// imageData = new pPIXELS[W];
	imageData = (ppPIXELS)calloc(W, sizeof(PIXELS));
	// printf("new:(W,H):%d,%d\n",W,H);
	int y;
	for (y = 0; y < H; y++)
	{
		// imageData[y] = new PIXELS[W];
		imageData[y] = (pPIXELS)calloc(W, sizeof(PIXELS));
		if (imageData[y] == NULL)
			printf("new wrong!\n");
	}
	return imageData;
}

bool Rbmp::delImageData(ppPIXELS& imageData, int H)
{
	printf("H:%d %p\n",H,imageData);
	if (imageData)
	{
		// printf("del:%d\n",H);
		for (int i = 0; i < H; i++)
		{
			// printf("del:%d ",i);
			// delete []imageData[i];
			free(imageData[i]);
		}
		// delete []imageData;
		free(imageData);
		imageData = NULL;
	}
	else
	{
		printf("imageData = NULL\n");
	}
	return true;
}

void Rbmp::get_image_msg()
{
	// 显示位图文件头结构BITMAPFILEHEADER
	show_bmp_head(head);
	// 显示位图信息头结构变量,存放在变量head中
	show_info_head(infohead);
}

void Rbmp::show_bmp_head(BITMAPFILEHEADER & head)
{
	cout << "位图文件头:" << endl;
	cout << "\t文件头类型  : " << head.bfType << endl;
	cout << "\t文件的大小  : " << head.bfSize << endl;
	cout << "\t保留字 _1   : " << head.bfReserved1 << endl;
	cout << "\t保留字 _2   : " << head.bfReserved2 << endl;
	cout << "\tRGB偏移字节 : " << head.bfOffBits << endl << endl;
}

void Rbmp::show_info_head(BITMAPINFOHEADER & infohead)
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
	if ((x >= bmpWidth) | (y >= bmpHeight) | (x < 0) | (y < 0))
		return true;
	else
		return false;
}
void Rbmp::show_6path(map<Position,string> pathl)
{
	map<Position,string>::iterator it;
	for(it = pathl.begin(); it != pathl.end() ; ++it)
	{
		cout<<"key: " << it->first <<" (" << Pos2str(it->first) 
			<< ")" << "\tvalue: " << it->second << endl;
	}
}
string Rbmp::Pos2str(Position pos)
{
	switch(pos)
	{
		case Up:
			return "Up";
			break;
		case Down:
			return "Down";
			break;
		case Left:
			return "Left";
			break;
		case Right:
			return "Right";
			break;
		case Front:
			return "Front";
			break;
		case Back:
			return "Back";
			break;
		default:
			return "None";
			break;
	}
}
string Rbmp::color2str(colorType color)
{
	switch(color)
	{
		case Pricolor:
			return "All";
			break;
		case Red:
			return "Red";
			break;
		case Green:
			return "Green";
			break;
		case Blue:
			return "Blue";
			break;
		default:
			return "all";
			break;
	}
}
