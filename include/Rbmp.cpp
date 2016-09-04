#include <unistd.h>
#include "Rbmp.h"

Rbmp::Rbmp(const char *bmpname):fp(NULL), fpo(NULL), bmppath(bmpname), allData(NULL), pColorTable(NULL),granularity(10),granOpeartor(true),baseSmlrty(1.0),testRange(2)
{
	// 二进制读方式打开指定的图像文件
	fp = fopen(bmppath.c_str(), "rb");
	if(!fp)
	{
		printf("image file cannot find!\n");
		exit(-1);
	}
	if (init_image())
	{
		printf("In %s init bmp image is OK!\n",__FUNCTION__);
	}
	else
	{
		printf("In %s init bmp image is failed!\n",__FUNCTION__);
		return;
	}
	cout << "create a Rbmp ....\n" << endl;
}

Rbmp::Rbmp(const char **bmpnamel):fp(NULL), fpo(NULL),bmppathl(bmpnamel),allData(NULL),pColorTable(NULL)
{
	cout << "create a Rbmp list ....\n" << endl;
}

Rbmp::~Rbmp()
{
	if (pColorTable && biBitCount == 8)
	{
		delete[]pColorTable;
		pColorTable = NULL;
	}
	/*
	if (allData)
	{
		for (int i = 0; i < bmpHeight; i++)
		{
			delete []imageData[i];
			free(allData[i]);
		}
		delete []imageData;
		free(allData);
		allData = NULL;
	}
	*/

	delImageData(allData,bmpHeight);
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

bool Rbmp::init_image()
{
	if (NULL == fp)
	{
		printf("In %s open image is FAILED!\n",__FUNCTION__);
		return false;
	}
	U16 BM;
	fread(&BM, sizeof(U16), 1, fp);
	// printf("BBBBBBB:%0X\n",BM);
	if (BM != 0x4d42)
	{
		printf("In %s is not bmp image!\n",__FUNCTION__);
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
					allData[y][x].setempty();
					allData[y][x].initpPos();
					k += 2;
					break;
				case 8:
					tablesite = linedata[k];
					// printf("%03d\n",linedata[k]);
					// printf("颜色表位置:%d\n",tablesite);
					allData[y][x].setRGB(pColorTable[tablesite]);
					allData[y][x].setXY(x, y);
					allData[y][x].isEdge(bmpWidth, bmpHeight);
					allData[y][x].setempty();
					allData[y][x].initpPos();
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
	if(!setBackground())//use default value
		printf("In %s set Background error!\n",__FUNCTION__);
	return true;
}

bool Rbmp::initSpatialize(const char** imagePath)
{
	if(NULL == imagePath)
	{
		printf("In %s imagePath is NULL!\n",__FUNCTION__);
		return  false;
	}
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

ppPIXELS Rbmp::newImageData(ppPIXELS &imageData, int W, int H)
{
	// malloc some memroy
	// imageData = new pPIXELS[W];
	imageData = (ppPIXELS)calloc(W, sizeof(PIXELS));
	if(!imageData)
		printf("In %s new imageData wrong!\n",__FUNCTION__);
	// printf("new:(W,H):%d,%d\n",W,H);
	int y;
	for (y = 0; y < H; y++)
	{
		// imageData[y] = new PIXELS[W];
		imageData[y] = (pPIXELS)calloc(W, sizeof(PIXELS));
		if (!imageData[y])
			printf("In %s new imageData[y] wrong!\n",__FUNCTION__);
	}
	return imageData;
}

ppPIXELS Rbmp::imageDatadup2(ppPIXELS imageData, ppPIXELS &tmpimageData)
{
	// tmpimageData = new pPIXELS[bmpWidth];
	tmpimageData = (ppPIXELS)calloc(bmpWidth, sizeof(PIXELS));
	if(!tmpimageData)
	{
		printf("In %s calloc memory is failed\n",__FUNCTION__);
	}
	for (int y = 0; y < bmpHeight; y++)
	{
		// tmpimageData[y] = new PIXELS[bmpWidth];
		tmpimageData[y] = (pPIXELS)calloc(bmpWidth, sizeof(PIXELS));
		if(!tmpimageData[y])
		{
			printf("In %s[y] calloc memory is failedp\n",__FUNCTION__);
		}
		memcpy(tmpimageData[y], imageData[y], sizeof(PIXELS) * bmpWidth);
	}
	return tmpimageData;
}

bool Rbmp::deleteAll()
{
	if (pColorTable && biBitCount == 8)
	{
		delete[]pColorTable;
		pColorTable = NULL;
	}
	if (allData)
	{
		for (int i = 0; i < bmpHeight; i++)
		{
			//delete []imageData[i];
			free(allData[i]);
		}
		//delete []imageData;
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

void Rbmp::delreadNline(ppPIXELS  lineppot, int rows)
{
	if (lineppot)
	{
		for (int i = 0; i < ABS(rows); i++)
			delete[]lineppot[i];
		delete[]lineppot;
		lineppot = NULL;
	}
}

bool Rbmp::delImageData(ppPIXELS& imageData, int H)
{
	//printf("H:%d %p\n",H,imageData);
	if (imageData)
	{
		for (int i = 0; i < H; i++)
		{
			// delete []imageData[i];
			free(imageData[i]);
		}
		// delete []imageData;
		free(imageData);
		imageData = NULL;
	}
	else
	{
		//printf("In delImageData imageData = NULL\n");
	}
	return true;
}

ppPIXELS Rbmp::readNline(int beginY, int rows)
{
#define DEAL(ROWS,Y) ((ROWS)>0?(Y++):(Y--))
	try
	{
		if ((beginY < 0) | (beginY >= bmpHeight))
			throw 0;
	}
	catch(...)
	{
		printf("In readNline ,You set beginY:%d is out Range!\n", beginY);
		//return NULL;
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
#undef DEAL
}

RecImage Rbmp::readRect(int beginX,int beginY,int width,int height)
{
	RecImage recimage;
#define DEAL(ROWS,Y) ((ROWS)>0?(++Y):(--Y))
	try
	{
		if ((beginY < 0) | (beginY >= bmpHeight))
			throw 0;
	}
	catch(...)
	{
		printf("In readNline ,You set beginY:%d is out Range!\n", beginY);
		//return NULL;
	}
	if (height == 0)
	{
		recimage.imageDatas = allData;
		recimage.height = bmpHeight;
		recimage.width = bmpWidth;
		return recimage;
	}
	else
	{
		int height2 = ABS(height);
		int width2 = width;
		if(width >= bmpWidth)width = bmpWidth;
		if(height >= bmpHeight)height = bmpHeight;
		ppPIXELS lineppot = new pPIXELS[height2];
		int line = 0;
		for (int y = beginY; height2--; DEAL(height, y))	// y is row number
		{
			line = y - beginY;
			lineppot[line] = new PIXELS[width2];
			width = width2;
			for (int x = beginX; width--; ++x)
			{
				lineppot[line][x-beginX] = allData[y][x];
				//lineppot[y][x].show_PIXELS();
				//printf("\n");
			}
		}
		recimage.imageDatas = lineppot;
		recimage.width = width2;
		recimage.height = height;
		return recimage;
	}
#undef DEAL
}

bool Rbmp::write_image(const char *outpath,RecImage imageData,const char* dealType)
{
	int H = 0,W = 0;
	BMPALLHEAD imagehead;
	ppPIXELS data;
	if(imageData.imageDatas)
	{
		H = imageData.height;
		W = imageData.width;
		//memcpy(&imagehead,&allhead,sizeof(BMPALLHEAD));
		imagehead = allhead;
		data = imageData.imageDatas;
		setHead(imagehead,W,H);
	}
	else
	{
		H = bmpHeight;
		W = bmpWidth;
		data = allData;
		imagehead = allhead;
	}
	fpo = fopen(outpath, "wb");
	fseek(fpo, 54, 0);		// 跳过allhead
	//ppPIXELS imageData = NULL;
	//imageData = newImageData(imageData, bmpWidth, bmpHeight);
	if (deal_image(dealType))
	{
		writeAllData(data,W,H);
	}
	rewind(fpo);
	switch (biBitCount)
	{
		case 24:
			fwrite(&imagehead, bfOffBits, 1, fpo);
			break;
		case 8:
			// 申请颜色表所需要的空间，读颜色表进内存
			// fread(pColorTable,sizeof(RGBQUAD),256,fp);
			fwrite(&imagehead, 54, 1, fpo);
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

bool Rbmp::setHead(BMPALLHEAD& allhead,int W,int H)
{
	if(W <= 0 || H <= 0)
	{
		printf("In %s W is %d H is %d!\n",__FUNCTION__,W,H);
		return false;
	}
	bmpWidth = W;
	bmpHeight = H;
	allhead.infoHead.biWidth = W;
	allhead.infoHead.biHeight = H;
	int lineByte = (W * biBitCount + 31) / 32 * 4;
	allhead.infoHead.biSizeImage = H * lineByte;
	allhead.bmpHead.bfSize = H * lineByte + 54;
	//printf("allhead:%p,lineByte:%d\n",&allhead,lineByte);
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
			",globalI); pixel.show_PIXELS(); printf("\n"); globalI++;
	*/
	return globalI - 1;
}

bool Rbmp::writeAllData(ppPIXELS& imageData,int width,int height)
{
	int H = 0;
	int W = 0;
	if(imageData == allData)
	{
		H = allhead.infoHead.biHeight;
		W = allhead.infoHead.biWidth;
	}
	else
	{
		H = height;
		W = width;
	}
	int lineByte;
	lineByte = (W * biBitCount + 31) / 32 * 4;
	//setHead(lineByte,W,H);
	//printf("lineByte:%d\n",lineByte);
	BYTE8 *linedata = new BYTE8[lineByte];
	memset(linedata, 0, lineByte);
	if (pColorTable)
	{
		memset(pColorTable, 0, sizeof(RGBQUAD) * 256);
	}
	int state;
	int x, k;
	//printf("WWW:%d HHH:%d\n", W, H);
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

bool Rbmp::deal_image(const char* dealType)
{
	if(!dealType)
	{
		printf("Not deal with!,will output itself\n");
		return true;
	}
	while(*dealType)
	{
		switch(*dealType)
		{
			case 'T':
				cout << "  -T     imageTranspose  : Transpose a iamge\n";
				imageTranspose(allData);
				break;
			case 'R':
				cout << "  -R     imageRevolution : Revolution a image\n";
				imageRevolution(allData,bmpWidth/2,bmpHeight/2,-45);
				break;
			case 's':
				cout << "  -s     imageSpherize   : Spherize a image\n";
				imageSpherize(allData);
				break;
			case 'Z':
				cout << "  -Z     imageZoom       : zoom a image\n";
				imageZoom(allData,0.5,0.5);
				break;
			case 'M':
				cout << "  -M     imageMirror     : Mirror a image\n";
				imageMirror(allData,UD);
				break;
			case 'S':
				cout << "  -S     imageShear      : Shear a image\n";
				imageShear(allData,true,-45.0);
				break;
			case 'm':
				cout << "  -m     imageMove       : move a image\n";
				imageMove(allData,100,100);
				break;
			case 'C':
				cout << "  -C     getImage3Color  : get a image's 3(R,G,B) color image\n";
				getImage3Color(allData,Green);
				break;
			case 'H':
				cout << "  -H     genHistogram    : get a image's 3(R,G,B) color Histogram\n";
				genBardiagram(allData,Red);
				genBardiagram(allData,Green);
				genBardiagram(allData,Blue);
				genBardiagram(allData);
				break;
			case 'B':
				cout << "  -B     genBardiagram   : get a image's 3(R,G,B) color Bar diagram\n";
				genHistogram(allData,Red);
				genHistogram(allData,Green);
				genHistogram(allData,Blue);
				genHistogram(allData);
				break;
			case 'b':
				cout << "  -b     backGround_ize   : get a image's part of backGround\n";
				backGround_ize();
				break;
			case 'h':
				cout << "  -h     boundarysHL      : change boundarys line to HightLight\n";
				boundarysHL(trackTables);
				break;
			case 'c':
				cout << "  -c     imageCutOut      : cutout the effective part of the image\n";
				imageCutOut();
				break;
			case 'd':
				cout << "  -d     imageDensity     :  Change a image each pixel's Idensity\n";
				imageDensity(allData,100);
				break;
			default:
				printf("Not deal with!\n");
				break;
		}
		dealType++;
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

void Rbmp::show_6path(map<Position,string> pathl)
{
	map<Position,string>::iterator it;
	for(it = pathl.begin(); it != pathl.end() ; ++it)
	{
		cout<<"key: " << it->first <<" (" << Pos2str(it->first)
			<< ")" << "\tvalue: " << it->second << endl;
	}
}

void Rbmp::show_line(dPIXELS boundaryline)
{
	for (size_t i =0; i < boundaryline.size(); i ++)
	{
		boundaryline[i].show_PIXELS();
		printf("\n");
	}
}

bool Rbmp::isEdge(int x,int y)
{
	if(x <= 0 || x >= bmpWidth ||
			y <= 0 || y >= bmpHeight)
		return true;
	else
		return false;
}

bool Rbmp::backGround_ize()
{
	for (int y = 0;y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			if(!alikeBackground(allData[y][x]))
			{
				allData[y][x].setRGB(0,0,0);
			}
		}
	}
	return true;
}
//note: index not safe
PIXELS* Rbmp::getBLpixel(dPIXELS& boundaryline,int index)
{
	int bsize = boundaryline.size();
	if(ABS(index) > bsize)
		return NULL;
	if(index < 0)
		index = bsize + index;
	return &boundaryline[index];
}
