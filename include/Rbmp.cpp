#include <unistd.h>
#include <math.h>
#include "Rbmp.h"
#define PI 3.14159
#define R2D(R) R*180/PI
#define D2R(D) D*PI/180
#define OUTRANGE(P) ((P > 255) || (P < 0))
#define EQUALBackGround(rgb) ( !(rgb.rgbRed ^ backGround.rgbRed) && \
		                           !(rgb.rgbGreen ^ backGround.rgbGreen) && \
		                           !(rgb.rgbBlue ^ backGround.rgbBlue) )
#define CLOSEOPEN(var)       ((var) ? ("close") : ("open"))
/*
#define DIRECTION(var)       switch(var) {\
                               case Right: direction = Right; x++; break;\
                               case Down:  direction = Down;  y++; break;\
                               case Left:  direction = Left;  x--; break;\
                               case Up:    direction = Up;    y--; break;\
                               default:    break; }
															 */
#define DIRECTION_Up       {direction = Up;    y--;}
#define DIRECTION_Down     {direction = Down;  y++;}
#define DIRECTION_Right    {direction = Right; x++;}
#define DIRECTION_Left     {direction = Left;  x--;}
static int globalI = 0;
Rbmp::Rbmp(const char *bmpname):fp(NULL), fpo(NULL), bmppath(bmpname), allData(NULL), pColorTable(NULL),granularity(10),pixelTrend(true),granOpeartor(true),baseSmlrty(1.0)
{
	// 二进制读方式打开指定的图像文件
	fp = fopen(bmppath.c_str(), "rb");
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
					allData[y][x].setempty(true);
					k += 2;
					break;
				case 8:
					tablesite = linedata[k];
					// printf("%03d\n",linedata[k]);
					// printf("颜色表位置:%d\n",tablesite);
					allData[y][x].setRGB(pColorTable[tablesite]);
					allData[y][x].setXY(x, y);
					allData[y][x].isEdge(bmpWidth, bmpHeight);
					allData[y][x].setempty(true);
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
	if(alikeBackground(allData[0][0]) == 1)
	{
		printf("This image maybe is lonely island!\n");
		pixelTrend = false;
	}
	return true;
}
int Rbmp::alikeBackground(PIXELS pixel)
{
	if(pixel.empty())
		return -1;
	RGBQUAD rgb = pixel.getRGB();
	if(EQUALBackGround(rgb))
	{
		if(pixelTrend)
			return 1;
		else
			return 0;
	}
	else
	{
		if(pixelTrend)
			return 0;
		else
			return 1;
	}
}
int Rbmp::alikeBackground(int x,int y)
{
	PIXELS tmp = get_pix(x,y);
	if(tmp.empty())
		return -1;
	RGBQUAD rgb = tmp.getRGB();
	if(EQUALBackGround(rgb))
	{
		if(pixelTrend)
			return 1;
		else
			return 0;
	}
	else
	{
		if(pixelTrend)
			return 0;
		else
			return 1;
	}
}

bool Rbmp::alikeBackground(RGBQUAD rgb)
{
	if(EQUALBackGround(rgb))
		return true;
	else
		return false;
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

bool Rbmp::boundarysHL()
{
	int color;
	if(pixelTrend)
		color = 128;
	else
		color = 0;
	//printf("+++++++++++++++++++++++++\n");
	if(boundarys.empty())
		return false;
	int x = 0,y = 0;
	for (size_t i =0; i < boundarys.size(); i++)
	{
		for (size_t j =0; j < boundarys[i].size(); j ++)
		{
			y = boundarys[i][j].getY();
			x = boundarys[i][j].getX();
			//if(allData[y][x].getEdge() == -2)
				//allData[y][x].setRGB(0,0,255);
			//if(allData[y][x].getEdge() == -1)
				//allData[y][x].setRGB(color,color,color);
                allData[y][x].setRGB(0,255,0);
		}
	}
    /*
	vector<xx_y>::const_iterator it;
	for(it = skipTable.begin(); it != skipTable.end();++it)
	{
		allData[it->ally][it->sttx].setRGB(0,255,0);
		allData[it->ally][it->endx].setRGB(0,255,0);
    }*/
	return true;
}
bool Rbmp::imageCutOut()
{
	setBackground(0,0,0);
	if(skipTable.empty())
		return false;
	vector<xx_y>::const_iterator it = skipTable.begin();
	for (int y = 0; y < bmpHeight; y++)
	{
		if(it->ally != y)
		{
			for (int x = 0; x < bmpWidth; x++)
			{
				allData[y][x].setRGB(backGround);
			}
		}
		else
		{
			for (int x = 0; x < bmpWidth; x++)
			{
				//set other part to backGround
				if(x < it->sttx || x > it->endx)
					allData[y][x].setRGB(backGround);
				//set the cutOut part to you color that you want
				//if(x >= it->sttx && x <= it->endx)
				//	allData[y][x].setRGB(0,0,255);
				//use and contrl the skipTable
				if(x == it->endx && (it + 1)->ally == y)
					++it;
			}
			++it;
		}
	}
	return true;
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
			// delete []imageData[i];
			free(allData[i]);
		}
		// delete []imageData;
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
	//printf("In get_pix(x,y)\n");
	PIXELS ppot;
	ppot.setXY(x, y);
	if (out_range_error(ppot))
	{
		printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", ppot.getX(), ppot.getY());
		ppot.setempty();
		return ppot;
	}
	ppot.setRGB(allData[y][x]);
	/*
	if(alikeBackground(ppot))
	{
		ppot.show_PIXELS();
		printf("\n");
	}*/
	return ppot;
}

PIXELS Rbmp::get_pix(PIXELS pixel)
{
	PIXELS ppot;
	if (out_range_error(pixel))
	{
		printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", pixel.getX(), pixel.getY());
		ppot.setempty();
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
		//return pots8;
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
void Rbmp::getBoundaryLine()
{
#define debug1
	vector<xx_y>::const_iterator it;
	xx_y footprint;
	for (int y = 0;y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			//isBoundaryPoint(x,y);
			//if(isBoundaryPoint(allData[y][x]))
			if(isBoundaryPoint(x,y))
			{
				if(allData[y][x].getEdge() >= 0)
				{
					//start track down by following clues(顺藤摸瓜)
					footprint.sttx = x;
					x = trackDown(allData[y][x]);
					footprint.endx = x;
					footprint.ally = y;
					skipTable.push_back(footprint);
					baseSmlrty = 1.0;
					//printf("next footprint'x value:%d\n",x+1);
					//printf("trackDown.... insert\n");
#ifdef debug
					goto here;
#endif
				}
				else
				{
					footprint.sttx = x;
					x++;
					//while(isBoundaryPoint(allData[y][x]))
					while(isBoundaryPoint(x,y))
					{
						if(allData[y][x].getEdge() != -1)
							x++;
						else
						{
							break;
						}
					}
					footprint.endx = x;
					footprint.ally = y;
					skipTable.push_back(footprint);
					//printf("skip table.... insert\n");
				}
			}
		}
	}
#ifdef debug
here:	printf("OOOOOOKKKKK!\n");
#endif
	printf("granularity: %u boundarys size:%ld\n",granularity,boundarys.size());
	for (size_t i =0; i < boundarys.size(); i++)
	{
		printf("$[%ld]: close or open status: %s ;boundary line len: %ld,(%u)\n",
				i,CLOSEOPEN(isCloseOpen(boundarys[i])),boundarys[i].size(),granularity);
		show_line(boundarys[i]);
	}
	/*
	printf("skip Table size:%ld\n",skipTable.size());
	for(it = skipTable.begin(); it != skipTable.end();++it)
	{
		printf("start x:%d end x:%d communal y:%d\n", it->sttx,it->endx,it->ally);
	}*/
}
void Rbmp::show_line(dPIXELS boundaryline)
{
	for (size_t i =0; i < boundaryline.size(); i ++)
	{
		boundaryline[i].show_PIXELS();
		printf("\n");
	}
}
bool Rbmp::isCloseOpen(dPIXELS boundaryline)
{
	if(boundaryline.empty())
		return false;
	else
		return (boundaryline.front() == boundaryline.back());
}
//start track down by following clues(顺藤摸瓜)
int Rbmp::trackDown(PIXELS& startPoint)
{
	globalI++;
	bool openstatus = true;
	if(startPoint.getEdge() != 1)
		openstatus = !openstatus;
	int sx = startPoint.getX();
	int x = sx;
	int sy = startPoint.getY();
	int y = sy;
	int nextx = 0;
	bool downs = true;
	dPIXELS boundaryline;
	boundaryline.push_back(startPoint);
		 printf("push s: ");
		 startPoint.show_PIXELS();
		 printf("\n");
	Position direction = Right;
	//each direction relative to the image
	do
	{
		//if(getRpoint(direction,x,y)&& !isEdge(x,y))
		PIXELS& prevPoint = allData[y][x];
		if(getRpoint(direction,x,y))
		{
			//printf("x:%d y:%d\n",x,y);
			//printf("direction:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
			//if(alikeBackground(x,y) == 1)
			if ( getSimilarity(direction,x,y) > baseSmlrty)
			{
				allData[y][x].setEdge(-1);
				switch(direction)
				{
					case Left:
						if(downs)
							prevPoint.setEdge(-2);
						else
							allData[y][x].setEdge(-2);
						break;
					case Right:
						if(downs)
							allData[y][x].setEdge(-2);
						else
							prevPoint.setEdge(-2);
						break;
					case Up:
						if(downs)
						{
							prevPoint.setEdge(-1);
							downs = false;
						}
						break;
					case Down:
						if(!downs)
						{
							prevPoint.setEdge(-2);
							downs = true;
						}
						break;
					default:
						break;
                }
				boundaryline.push_back(allData[y][x]);
				/*
					 printf("push a: ");
					 get_pix(x,y).show_PIXELS();
					 printf("\n");
					 */
			}
			else
				break;
		}
		else
		{
			openstatus = !openstatus;
			break;
		}
	}while (x != sx || y != sy);
	if(openstatus)
	{
		direction = Right;
		x = sx;
		y = sy;
		do
		{
			PIXELS& prevPoint = allData[y][x];
			//printf("direction:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
			if(getLpoint(direction,x,y))
			{
				//if(alikeBackground(x,y) == 1)
				if (getSimilarity(direction,x,y) > baseSmlrty)
				{
					allData[y][x].setEdge(-1);
					switch(direction)
					{
						case Right:
							if(downs)
								prevPoint.setEdge(-2);
							else
								allData[y][x].setEdge(-2);
							break;
						case Left:
							if(downs)
								allData[y][x].setEdge(-2);
							else
								prevPoint.setEdge(-2);
							break;
						case Up:
							if(downs)
							{
								prevPoint.setEdge(-2);
								downs = false;
							}
							break;
						case Down:
							if(!downs)
							{
								prevPoint.setEdge(-1);
								downs = true;
							}
							break;
						default:
							break;
					}
					//allData[y][x].setRGB(0,0,0);
					boundaryline.push_front(allData[y][x]);
					/*
						 printf("push a: ");
						 get_pix(x,y).show_PIXELS();
						 printf("\n");
						 */
					nextx++;
				}
				else
					break;
			}
			else
				break;
		}while (x != sx || y != sy);
	}
	else
	{
		nextx =  boundaryline.size() - 1;
	}
	startPoint.setEdge(-1);//cannnot modify the x,y and rgb value
	if(granOpeartor)
	{
		if(boundaryline.size() > granularity)
		{
			show_line(boundaryline);
			//deburrTrack(boundaryline);
			boundarys.push_back(boundaryline);
		}
		else
		{
			nextx = 0;
			while(!boundaryline.empty())
			{
				boundaryline.front().setEdge(0);
				boundaryline.pop_front();
			}
		}
	}
	else
	{
		if(boundaryline.size() <= granularity)
		{
			show_line(boundaryline);
			//deburrTrack(boundaryline);
			boundarys.push_back(boundaryline);
		}
		else
		{
			nextx = 0;
			while(!boundaryline.empty())
			{
				boundaryline.front().setEdge(0);
				boundaryline.pop_front();
			}
		}
	}
	//printf("$[%d]> close or open status: %s Track down by following clues(顺藤摸瓜) OK... len:%ld(%u)\n",
			//globalI,CLOSEOPEN(isCloseOpen(boundaryline)),boundaryline.size(),granularity);
	//get next point's x value
	int maxX = sx;
	while(--nextx >= 0 && boundaryline[nextx].getY() == sy
			&& boundaryline[nextx].getX() >= maxX)
	{
		maxX = boundaryline[nextx].getX();
		//printf("%d mx %d\n",nextx,maxX);
	}
	//printf("The max x %d will be nextpoint\n",maxX);
	return maxX;
}
bool Rbmp::deburrTrack(dPIXELS& boundaryline)
{
	printf("in deburrTrack...\n");
	if(boundaryline.empty())
		return false;
	int j = 1;
	int x ,y;
	dPIXELS::iterator itPoint = boundaryline.begin();
	for (;itPoint != boundaryline.end(); ++itPoint)
	{
		if(itPoint->getEdge() == -1)
		{
			while((itPoint-j != boundaryline.begin())
					&& (*(itPoint-j) == *(itPoint+j)))
			{
				y = (itPoint-j)->getY();
				x = (itPoint-j)->getX();
				printf("find back point! j:%d\n",j);
				//(itPoint-j)->setEdge(-3);
				boundaryline.erase(itPoint-j);
				allData[y][x].setEdge(-3);
				(itPoint-j)->show_PIXELS();
				printf("\n===============\n");
				y = (itPoint+j)->getY();
				x = (itPoint+j)->getX();
				(itPoint+j)->setEdge(-3);
				allData[y][x].setEdge(-3);
				(itPoint+j)->show_PIXELS();
				printf("\n");
				j++;
			}
			j = 1;
		}
	}
	return true;
}
/*
bool Rbmp::deburrTrack(dPIXELS& boundaryline)
{
	printf("in deburrTrack...\n");
	if(boundaryline.empty())
		return false;
	int j = 1;
	int x ,y;
	for (size_t i = 1; i < boundaryline.size(); i ++)
	{
		if(boundaryline[i].getEdge() == -1)
		{
			while((i-j > 0) && (boundaryline[i-j] == boundaryline[i+j]))
			{
				y = boundaryline[i-j].getY();
				x = boundaryline[i-j].getX();
				printf("find back point! j:%d\n",j);
				boundaryline[i-j].setEdge(-3);
				allData[y][x].setEdge(-3);
				boundaryline[i-j].show_PIXELS();
				printf("\n===============\n");
				y = boundaryline[i+j].getY();
				x = boundaryline[i+j].getX();
				boundaryline[i+j].setEdge(-3);
				allData[y][x].setEdge(-3);
				boundaryline[i+j].show_PIXELS();
				printf("\n");
				j++;
			}
			j = 1;
		}
	}
	return true;
}*/
// is Boundary 
bool Rbmp::isBoundaryPoint(PIXELS pot)
{
	/*PIXPOT tmp;
	if(alikeBackground(pot) == 1)
	{
		tmp = get_pot(pot);
		if(tmp.pixelSimilar())
		{
			tmp.show_PIXPOT();
			printf("相似度极高\n");
			printf("\n");
			return false;
		}
		else
		{
			tmp.show_PIXPOT();
			printf("相似度一般\n");
			printf("\n");
			return true;
		}
	}*/
	if(alikeBackground(pot) == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
// is Boundary 
bool Rbmp::isBoundaryPoint(int& x,int& y)
{
	int i = 0;
	float similarity = getSimilarity(Right,x,y);
	float avgSimi = 0;
	float diffSim = 0;
	for (i = 0; x < bmpWidth-1; ++i,++x)
	{
		//allData[y][x].show_PIXELS();
		//printf("\n");
		diffSim  = similarity - avgSimi;
		similarity = getSimilarity(Right,x,y);
		//printf("%3d: num:%lf\tavg:%lf\tdiff:%lf\n", i + 1, similarity, avgSimi, fabs(diffSim));
		avgSimi += diffSim / (i + 1);
		if (fabs(similarity - avgSimi) > 0.1)
		{
			printf("finded :%lf\n", similarity);
			++x;
			break;
		}
	}
	baseSmlrty = similarity;
	printf("baseSmlrty:%lf\n",baseSmlrty);
    if(x < bmpWidth-1)
		return true;
	else
		return false;
}
float Rbmp::getSimilarity(Position direction,int x,int y)
{
	float Similarity = 0;
	if(get_pix(x,y).empty())
		return -1;
	PIXELS potCurnt;
	PIXELS potRight;
	switch(direction)
	{
		case Down:
			if(y+1 >= bmpHeight)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y+1][x];
			break;
		case Right:
			if(x+1 >= bmpWidth)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y][x+1];
			break;
		case Up:
			if(y-1 < 0)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y-1][x];
			break;
		case Left:
			if(x-1 < 0)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y][x-1];
			break;
		default:
			break;
	}
	PIXELS diff = potCurnt - potRight;
	/*
	printf("curr:\n");
	potCurnt.show_PIXELS();
	printf("\n");
	printf("Right:\n");
	potRight.show_PIXELS();
	printf("\n");
	printf("diff:\n");
	diff.show_PIXELS();
	printf("\n");
	printf("~diff:\n");
	*/
	diff = ~diff;
	/*
	diff.show_PIXELS();
	printf("\n");
	*/
	Similarity = (diff.getRed() + diff.getGreen() + diff.getBlue())/765.0;
	//printf("x: %2d y: %2d Similarity: %.3f\n",x,y,Similarity);
	return Similarity;
}
/* 函数名称readNline() */
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

bool Rbmp::write_image(const char *outpath,const char* dealType)
{
	// FILE* fpo = fopen(outpath,"wb");
	fpo = fopen(outpath, "wb");
	fseek(fpo, 54, 0);		// 跳过allhead
	//ppPIXELS imageData = NULL;
	//imageData = newImageData(imageData, bmpWidth, bmpHeight);
	if (deal_image(dealType))
	{
		writeAllData(allData);
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

bool Rbmp::writeAllData(ppPIXELS& imageData)
{
	int H = allhead.infoHead.biHeight;
	int W = allhead.infoHead.biWidth;
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

bool Rbmp::deal_image(const char* dealType)
{
	if(!dealType)
	{
		printf("Not deal with!\n");
		return true;
	}
	while(*dealType)
	{
		switch(*dealType)
		{
			case 'T':
				cout << "  -T     imageTranspose  : Transpose a iamge\n";
				imageTranspose();
				break;
			case 'R':
				cout << "  -R     imageRevolution : Revolution a image\n";
				imageRevolution(bmpWidth/2,bmpHeight/2,-45);
				break;
			case 's':
				cout << "  -s     imageSpherize   : Spherize a image\n";
				imageSpherize();
				break;
			case 'Z':
				cout << "  -Z     imageZoom       : zoom a image\n";
				imageZoom(0.5,0.5);
				break;
			case 'M':
				cout << "  -M     imageMirror     : Mirror a image\n";
				imageMirror(UD);
				break;
			case 'S':
				cout << "  -S     imageShear      : Shear a image\n";
				imageShear(true,-45.0);
				break;
			case 'm':
				cout << "  -m     imageMove       : move a image\n";
				imageMove(100,100);
				break;
			case 'C':
				cout << "  -C     getImage3Color  : get a image's 3(R,G,B) color image\n";
				getImage3Color(Green);
				break;
			case 'H':
				cout << "  -H     genHistogram    : get a image's 3(R,G,B) color Histogram\n";
				genBardiagram(Red);
				genBardiagram(Green);
				genBardiagram(Blue);
				genBardiagram();
				break;
			case 'B':
				cout << "  -B     genBardiagram   : get a image's 3(R,G,B) color Bar diagram\n";
				genHistogram(Red);
				genHistogram(Green);
				genHistogram(Blue);
				genHistogram();
				break;
			case 'b':
				cout << "  -b     backGround_ize   : get a image's part of backGround\n";
				backGround_ize();
				break;
			case 'h':
				cout << "  -h     boundarysHL      : change boundarys line to HightLight\n";
				boundarysHL();
				break;
			case 'c':
				cout << "  -c     imageCutOut      : cutout the effective part of the image\n";
				imageCutOut();
				break;

			default:
				printf("Not deal with!\n");
				break;
		}
		dealType++;
	}
	return true;
}

bool Rbmp::genBardiagram(colorType color)
{
	if(NULL == allData)
	{
		printf("In %s allData is NULL\n",__FUNCTION__);
		return false;
	}
	int RED[256] = {0};
	int GRE[256] = {0};
	int BLU[256] = {0};
	int ALL[256] = {0};
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			RED[allData[y][x].getRed()]++;
			GRE[allData[y][x].getGreen()]++;
			BLU[allData[y][x].getBlue()]++;
		}
	}
	float Rmax = 0,Gmax = 0,Bmax = 0,Amax = 0;
	for(int i = 0; i < 256; i++)
	{
		ALL[i] = RED[i] + GRE[i] + BLU[i];
		if(RED[i] > Rmax)
			Rmax=RED[i];
		if(GRE[i] > Gmax)
			Gmax=GRE[i];
		if(BLU[i] > Bmax)
			Bmax=BLU[i];
		//printf("i:%d R:%d G:%d B:%d\n",i,RED[i],GRE[i],BLU[i]);
	}
	Amax = Rmax + Gmax + Bmax;
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
	float Ascale = Hh/Amax;
	int x, k;
	FILE* fpg = NULL;
	size_t pos = bmppath.rfind(".bmp");
	string outpath;
	if(pos != string::npos)
		outpath = bmppath.substr(0,pos) + color2str(color) +"_bar.bmp";
	else
		outpath = bmppath + color2str(color) +"_htg.bmp";
	//printf("genHistogram :%s Rmax:%f Gmax:%f Blue:%f\n",outpath.c_str() ,Rmax,Gmax,Bmax);
	fpg = fopen(outpath.c_str(),"wb");
	BMPALLHEAD HallHead;
	memcpy(&HallHead,&allhead,sizeof(BMPALLHEAD));
	if(setHead(HallHead,W,H))
	{
		fwrite(&HallHead, 54, 1, fpg);
	}
	int lineByte;
	lineByte = (W * biBitCount + 31) / 32 * 4;
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
					case Pricolor:
					if(ALL[x-Rb]*Ascale > y-Db)
					{
						flag = false;
						linedata[k] = 57,linedata[k+1] = 77,linedata[k+2] = 118;
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
bool Rbmp::genHistogram(colorType color)
{
	if(NULL == allData)
	{
		printf("In %s allData is NULL\n",__FUNCTION__);
		return false;
	}
	float PIXELSCOUNT = bmpWidth * bmpHeight;
	int RED[256] = {0};
	float FRED[256] = {0};
	int GRE[256] = {0};
	float FGRE[256] = {0};
	int BLU[256] = {0};
	float FBLU[256] = {0};
	int ALL[256] = {0};
	float FALL[256] = {0};
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			RED[allData[y][x].getRed()]++;
			GRE[allData[y][x].getGreen()]++;
			BLU[allData[y][x].getBlue()]++;
		}
	}
	for(int i = 0; i < 256; i++)
	{
		FRED[i] = RED[i] / PIXELSCOUNT;
		FGRE[i] = GRE[i] / PIXELSCOUNT;
		FBLU[i] = BLU[i] / PIXELSCOUNT;
		ALL[i] = RED[i] + GRE[i] + BLU[i];
		FALL[i] = ALL[i] / PIXELSCOUNT;
		//FALL[i] = FRED[i] + FGRE[i] + FBLU[i];
		//printf("i:%d R:%d(%f) G:%d(%f) B:%d(%f)\n",i,RED[i],FRED[i],GRE[i],FGRE[i],BLU[i],FBLU[i]);
		printf("%d %d %d %d %d\n",i,RED[i],GRE[i],BLU[i],ALL[i]);
	}
	float Rmax = 0,Gmax = 0,Bmax = 0,Amax = 0;
	for(int i = 0; i < 256; i++)
	{
		if(FRED[i] > Rmax)
			Rmax=FRED[i];
		if(FGRE[i] > Gmax)
			Gmax=FGRE[i];
		if(FBLU[i] > Bmax)
			Bmax=FBLU[i];
	}
	Amax  = Rmax + Gmax + Bmax;
	printf("PIXELSCOUNT：%f\n",PIXELSCOUNT);
	printf("R:%f G:%f B:%f A:%f\n",Rmax,Gmax,Bmax,Amax);
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
	float Ascale = Hh/Amax;
	printf("R:%f G:%f B:%f A:%f\n",Rscale,Gscale,Bscale,Ascale);
	int x, k;
	FILE* fpg = NULL;
	size_t pos = bmppath.rfind(".bmp");
	string outpath;
	if(pos != string::npos)
		outpath = bmppath.substr(0,pos) + color2str(color) +"_htg.bmp";
	else
		outpath = bmppath + color2str(color) +"_htg.bmp";
	//printf("genHistogram :%s Rmax:%f Gmax:%f Blue:%f\n",outpath.c_str() ,Rmax,Gmax,Bmax);
	fpg = fopen(outpath.c_str(),"wb");
	BMPALLHEAD HallHead;
	memcpy(&HallHead,&allhead,sizeof(BMPALLHEAD));
	if(setHead(HallHead,W,H))
	{
		fwrite(&HallHead, 54, 1, fpg);
	}
	int lineByte;
	lineByte = (W * biBitCount + 31) / 32 * 4;
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
					if(FRED[x-Rb]*Rscale > y-Db)
					{
						flag = false;
						linedata[k+2] = 255;
					}
					else
						flag = true;
					break;
					case Green:
					if(FGRE[x-Rb]*Gscale > y-Db)
					{
						flag = false;
						linedata[k+1] = 255;
					}
					else
						flag = true;
					break;
					case Blue:
					if(FBLU[x-Rb]*Bscale > y-Db)
					{
						flag = false;
						linedata[k] = 255;
					}
					else
						flag = true;
					break;
					case Pricolor:
					if(FALL[x-Rb]*Ascale > y-Db)
					{
						flag = false;
						linedata[k] = 57,linedata[k+1] = 77,linedata[k+2] = 118;
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
ppPIXELS Rbmp::imageMirror(Method method)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	switch (method)
	{
		case UD:					// up down change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[bmpHeight - 1 - y][x];
				}
			}
			break;
		case LR:					// left right change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][bmpWidth - 1 - x];
				}
			}
			break;
		case UR:					// up down & left right change
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[bmpHeight - 1 - y][bmpWidth - 1 - x];
				}
			}
			break;
		default:					// NONE
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][x];
				}
			}
			break;
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

ppPIXELS Rbmp::getImage3Color(colorType color)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	switch (color)
	{
		case Red:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][x].get3Color(Red);
				}
			}
			break;
		case Green:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][x].get3Color(Green);
				}
			}
			break;
		case Blue:
			for (int y = 0; y < bmpHeight; y++)
			{
				for (int x = 0; x < bmpWidth; x++)
				{
					allData[y][x] = tmpimageData[y][x].get3Color(Blue);
				}
			}
			break;
		default:
			break;
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

// move the image:x>0,y>0 ->right down ;x<0 y<0 -> left up
ppPIXELS Rbmp::imageMove(int mx, int my)
{
#define MOVEX(Mx,x) ((Mx)>0 ? (x <= Mx) : (x > bmpWidth-1+Mx))
#define MOVEY(My,y) ((My)>0 ? (y <= My) : (y > bmpHeight-1+My))
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			if ((MOVEY(my, y) || MOVEX(mx, x)))
				allData[y][x].setRGB(255, 255, 255);
			else
				allData[y][x] = tmpimageData[y - my][x - mx];
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
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

ppPIXELS Rbmp::imageZoom(float scalex, float scaley)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	if (scalex <= 0.0 || scaley <= 0.0)
	{
		printf("In %s zoom number is <= 0 ,is wrong",__FUNCTION__);
		return NULL;
	}
	else
	{
		int W = bmpWidth * scalex;
		int H = bmpHeight * scaley;
		delImageData(allData, bmpHeight);	// free
		if (allData == NULL)	// renew
		{
			newImageData(allData, W, H);
			// printf("new is ok\n");
		}
		//printf("imageZoom: WWW:%d HHH:%d\n",W,H);
		for (int y = 0; y < H; y++)
		{
			for (int x = 0; x < W; x++)
			{
				int nx = (int)(x / scalex);
				int ny = (int)(y / scaley);
				allData[y][x] = tmpimageData[ny][nx];
				allData[y][x].setXY(x, y);
				// imageData[y][x].show_PIXELS();
				// tmpimageData[y/scaley][x/scalex].show_PIXELS();
				// printf("\n");
			}
		}
		// first free tmpimageData
		delImageData(tmpimageData, bmpHeight);
		// then set allhead
		setHead(allhead,W,H);
	}
	return allData;
}

ppPIXELS Rbmp::imageTranspose(bool AR)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	int W = bmpHeight;
	int H = bmpWidth;
	delImageData(allData, bmpHeight);	// free
	if (allData == NULL)		// renew
	{
		newImageData(allData, W, H);
		// printf("new is ok\n");
	}
	// printf("WWW:%d HHH:%d\n",W,H);
#define AorR(x,y) ((AR) ? (tmpimageData[x][H-1-y]) : (tmpimageData[W-1-x][y]))
	for (int y = 0;y < H;y++)
	{
		for (int x = 0;x < W;x++)
		{
			allData[y][x] = AorR(x,y);
		}
	}
	// first free tmpimageData
	delImageData(tmpimageData, bmpHeight);
	// then set allhead
	setHead(allhead,W,H);
	return allData;
}

ppPIXELS Rbmp::imageShear(bool XorY,float angle)
{
	angle = D2R(angle);
	ppPIXELS tmpimageData = NULL;
	int nx, ny;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	int H = XorY ? bmpHeight : (int)(bmpHeight + bmpWidth * ABS(tan(angle)));
	int W = XorY ? (int)(bmpWidth + bmpHeight * ABS(tan(angle))) : bmpWidth;
	//printf("Shear after: W:%d H:%d\n",W,H);
	delImageData(allData, bmpHeight);	// free
	if (allData == NULL)	// renew
	{
		newImageData(allData, W, H);
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
				allData[y][x].setRGB(255, 255, 255);
			else
				allData[y][x] = tmpimageData[ny][nx];
		}
	}	// then set allhead
	//delImageData(imageData,H);
	setHead(allhead,W,H);
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

ppPIXELS Rbmp::imageRevolution(int px,int py,float angle)
{
	angle = D2R(angle);
	ppPIXELS tmpimageData;
	int nx, ny;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{

			nx = (x - px) * cos(angle) - (y - py) * sin(angle) + px;
			ny = (x - px) * sin(angle) + (y - py) * cos(angle) + py;
			//printf("angle: %f cos:%f X:%d nx:%d\t Y:%d ny:%d\n",angle, cos(angle),x,nx,y,ny);
			if (nx < 0 || ny < 0 || nx >= bmpWidth || ny >= bmpHeight)
				allData[y][x].setRGB(255, 255, 255);
			else
				allData[y][x] = tmpimageData[ny][nx];
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}

ppPIXELS Rbmp::imageSpherize(float radius)
{
	ppPIXELS tmpimageData;
	float w = bmpWidth / 2.0;
	float h = bmpHeight / 2.0;
	float wh = w / h;
	float mx1, mx2;
	int nx;
	float radius2 = radius * radius;
	float dealt;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	if (radius <= 0.0)			// oval
	{
		for (int y = 0; y < bmpHeight; y++)
		{
			//mx1 = w + (w * sqrt(2 * h * y - y * y)) / h;
			//mx2 = w - (w * sqrt(2 * h * y - y * y)) / h;
			mx1 = w + wh * sqrt(2 * h * y - y * y);
			mx2 = w - wh * sqrt(2 * h * y - y * y);
			dealt = (mx1 - mx2) / bmpWidth;
			//printf("Y:%d, %d - %d %d ;%f\n", y, mx1, mx2, mx1 - mx2, dealt);
			for (int x = 0; x < bmpWidth; x++)
			{
				if (x < mx2 || x > mx1 || dealt == 0)
					allData[y][x].setRGB(255, 255, 255);
				else
				{
					nx = (x - mx2) / dealt;
					if (nx >= bmpWidth)
					{
						nx = bmpWidth - 1;
					}
					allData[y][x] = tmpimageData[y][nx];
				}
			}
		}
	}

	else						// circle
	{
		for (int y = 0; y < bmpHeight; y++)
		{
			mx1 = w + sqrt(radius2 - (y - h) * (y - h));
			// mx1 = w + sqrt(2*h*y-y*y);
			mx2 = w - sqrt(radius2 - (y - h) * (y - h));
			// mx2 = w - sqrt(2*h*y-y*y);
			dealt = (mx1 - mx2) / (bmpWidth * 1.0);
			//printf("Y:%d, %d - %d ;%f\n", y, mx1, mx2, dealt);
			for (int x = 0; x < bmpWidth; x++)
			{
				// if(x < mx2 || x > mx1)
				if (x < mx2 || x > mx1 || dealt == 0)
					allData[y][x].setRGB(255, 255, 255);
				else
				{
					nx = (x - mx2) / dealt;
					if (nx >= bmpWidth)
					{
						nx = bmpWidth - 1;
					}
					allData[y][x] = tmpimageData[y][nx];
				}
			}
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
}
ppPIXELS Rbmp::imageSpatialize(string outpath)
{
	if (initSpatialize(bmppathl))
	{
		printf("In %s init bmp list image is OK!\n",__FUNCTION__);
		//show_6path(rbmp);
	}
	else
	{
		cout << "" << endl;
		printf("In %s init bmp list image is fair!\n",__FUNCTION__);
		return NULL;
	}
	map<Position,string>::iterator it;
	ppPIXELS imageData[6] = {NULL};
	int i;
	int LH = 0,RH = 0, UH = 0,DH = 0, FH = 0,BH = 0, H = 0,W = 0;
	for(i = 0,it = rbmp.begin(); it != rbmp.end() ; ++it,++i)
	{
		cout<<"key: " << it->first <<" (" << Pos2str(it->first) 
			<< ")" << "\tvalue: " << it->second << endl;
		bmppath = it->second;
		fp = fopen(bmppath.c_str(), "rb");
		if (!init_image())
		{
			printf("init_image %s is failed\n",(it->second).c_str() );
		}
		printf("it->first:%d\n",it->first);
		switch(it->first)
		{
			case Left:
				imageShear(false,45.0);
				H = allhead.infoHead.biHeight;
				LH = H;
				break;
			case Right:
				imageShear(false,-45.0);
				H = allhead.infoHead.biHeight;
				RH = H;
				break;
			case Up:
				imageShear(true,45.0);
				W = allhead.infoHead.biWidth;
				UH = allhead.infoHead.biHeight;
				break;
			case Down:
				imageShear(true,-45.0);
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
	//open OutFile
	size_t pos = bmppath.rfind(".bmp");
	if(pos != string::npos)
		outpath = bmppath.substr(0,pos) +"_Spl.bmp";
	else
		outpath = bmppath +"_Spl.bmp";
	fpo = fopen(outpath.c_str(), "wb");
	if (!fpo)
	{
		printf("In %s open out image is FAILED!\n",__FUNCTION__);
	}
	//write image
	setHead(allhead,W,H);
	//write head
	fwrite(&allhead, 54, 1, fpo);
	//write body
	ppPIXELS newImage = NULL;
	newImageData(newImage,W,H);
	//memset(newImage,255,sizeof(PIXELS)*W*H);
	//printf("imageData:%p,fpo:%p",imageData[Down],fpo);
	//imageData[Down][0][0].show_PIXELS();
	writeAllData(newImage);
	//free a image memory!
	if(LH)
	{
		//printf("Lwwww%d\n",LH);
		delImageData(imageData[Left],LH);
	}
	if(RH)
	{
		//printf("Rwwww%d\n",RH);
		delImageData(imageData[Right],RH);
	}
	if(UH)
	{
		//printf("Uwwww%d\n",UH);
		delImageData(imageData[Up],UH);
	}
	if(DH)
	{
		//printf("Dwwww%d ",DH);
		delImageData(imageData[Down],DH);
	}
	if(FH)
	{
		//printf("Fwwww%d\n",FH);
		delImageData(imageData[Front],FH);
	}
	if(BH)
	{
		//printf("Bwwww%d\n",BH);
		delImageData(imageData[Back],BH);
	}
	return NULL;
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
	if ((x >= bmpWidth) || (y >= bmpHeight) || (x < 0) || (y < 0))
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
			return "Up   ";
			break;
		case Down:
			return "Down ";
			break;
		case Left:
			return "Left ";
			break;
		case Right:
			return "Right";
			break;
		case Front:
			return "Front";
			break;
		case Back:
			return "Back ";
			break;
		default:
			return "None ";
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
bool Rbmp::setBackground(RGBQUAD rgb)
{
	if(OUTRANGE(rgb.rgbRed) || OUTRANGE(rgb.rgbGreen) || OUTRANGE(rgb.rgbBlue))
		return false;
	backGround = rgb;
	return true;
}
U32 Rbmp::setGranularity(U32 gran,bool opeartor)
{
	granularity = gran;
	granOpeartor = opeartor;
	return gran;
}
bool Rbmp::setBackground(U8 r,U8 g,U8 b)
{
	if(OUTRANGE(r) || OUTRANGE(g) || OUTRANGE(b))
		return false;
	backGround.rgbRed = r;
	backGround.rgbGreen = g;
	backGround.rgbBlue = b;
	return true;
}
bool Rbmp::isEdge(int x,int y)
{
	if(x <= 0 || x >= bmpWidth ||
			y <= 0 || y >= bmpHeight)
		return true;
	else
		return false;
}
bool Rbmp::getRpoint(Position& direction,int& x,int& y)
{
	float flagxy = 0;
	PIXELS pot;
	//printf("direction:%s x:%d y:%d\t",Pos2str(direction).c_str(),x,y);
	switch (direction)
	{
		case Right:
			//flagxy = alikeBackground(x,y + 1);
			flagxy = getSimilarity(Down,x,y);
			//if (flagxy == 1)
			if (flagxy > baseSmlrty)
			{
				//direction = Down;
				//y++;
				DIRECTION_Down;
			}
			//else if (flagxy == 0)
			else if (flagxy <= baseSmlrty)
			{
				//flagxy = alikeBackground(x + 1,y);
				flagxy = getSimilarity(Right,x,y);
				//if (flagxy == 1)
				if (flagxy > baseSmlrty)
				{
					x++;
				}
				//else if(flagxy == 0)
				else if (flagxy <= baseSmlrty)
				{
					//flagxy = alikeBackground(x,y - 1);
					flagxy = getSimilarity(Up,x,y);
					//if (flagxy == 1)
					if (flagxy > baseSmlrty)
					{
						//direction = Up;
						//y--;
						DIRECTION_Up;
					}
					//else if (flagxy == 0)
					else if (flagxy <= baseSmlrty)
					{
						//direction = Left;
						//x--;
						DIRECTION_Left;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
			break;
		case Down:
			//flagxy = alikeBackground(x - 1,y);
			flagxy = getSimilarity(Left,x,y);
			//if (flagxy == 1)
			if (flagxy > baseSmlrty)
			{
				//direction = Left;
				//x--;
				DIRECTION_Left;
			}
			//else if (flagxy == 0)
			else if (flagxy <= baseSmlrty)
			{
				//flagxy = alikeBackground(x,y + 1);
				flagxy = getSimilarity(Down,x,y);
				//if (flagxy == 1)
				if (flagxy > baseSmlrty)
				{
					y++;
				}
				//else if(flagxy == 0)
				else if (flagxy <= baseSmlrty)
				{
					//flagxy = alikeBackground(x + 1,y);
					flagxy = getSimilarity(Right,x,y);
					//if (flagxy == 1)
					if (flagxy > baseSmlrty)
					{
						//direction = Right;
						//x++;
						DIRECTION_Right;
					}
					//else if (flagxy == 0)
					else if (flagxy <= baseSmlrty)
					{
						//direction = Up;
						//y--;
						DIRECTION_Up;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
			break;
		case Up:
			//flagxy = alikeBackground(x + 1,y);
			flagxy = getSimilarity(Right,x,y);
			//if (flagxy == 1)
			if (flagxy > baseSmlrty)
			{
				//direction = Right;
				//x++;
				DIRECTION_Right;
			}
			//else if (flagxy == 0)
			else if (flagxy <= baseSmlrty)
			{
				//flagxy = alikeBackground(x,y - 1);
				flagxy = getSimilarity(Up,x,y);
				//if (flagxy == 1)
				if (flagxy > baseSmlrty)
				{
					y--;
				}
				//else if(flagxy == 0)
				else if (flagxy <= baseSmlrty)
				{
					//flagxy = alikeBackground(x - 1,y);
					flagxy = getSimilarity(Left,x,y);
					//if (flagxy == 1)
					if (flagxy > baseSmlrty)
					{
						//direction = Left;
						//x--;
						DIRECTION_Left;
					}
					//else if (flagxy == 0)
					else if (flagxy <= baseSmlrty)
					{
						//direction = Down;
						//y++;
						DIRECTION_Down;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
			break;
		case Left:
			//flagxy = alikeBackground(x ,y - 1);
			flagxy = getSimilarity(Up,x,y);
			//if (flagxy == 1)
			if (flagxy > baseSmlrty)
			{
				//direction = Up;
				//y--;
				DIRECTION_Up;
			}
			//else if (flagxy == 0)
			else if (flagxy <= baseSmlrty)
			{
				//flagxy = alikeBackground(x - 1,y);
				flagxy = getSimilarity(Left,x,y);
				//if (flagxy == 1)
				if (flagxy > baseSmlrty)
				{
					x--;
				}
				//else if(flagxy == 0)
				else if (flagxy <= baseSmlrty)
				{
					//flagxy = alikeBackground(x,y + 1);
					flagxy = getSimilarity(Down,x,y);
					//if (flagxy == 1)
					if (flagxy > baseSmlrty)
					{
						//direction = Down;
						//y++;
						DIRECTION_Down;
					}
					//else if(flagxy == 0)
					else if (flagxy <= baseSmlrty)
					{
						//direction = Right;
						//x++;
						DIRECTION_Right;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
			break;
		default:
			break;
	}
	return true;
}

bool Rbmp::getLpoint(Position& direction,int& x,int& y)
{
	float flagxy = 0;
	PIXELS pot;
	//printf("direction:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
	switch (direction)
	{
		case Right:
			//flagxy = alikeBackground(x ,y - 1);
			flagxy = getSimilarity(Up,x,y);
			//if (flagxy == 1)
			if (flagxy > baseSmlrty)
			{
				// direction = Up;
				// y--;
				DIRECTION_Up;
			}
			//else if (flagxy == 0)
			else if (flagxy <= baseSmlrty)
			{
				//flagxy = alikeBackground(x + 1,y);
				flagxy = getSimilarity(Right,x,y);
				//if (flagxy == 1)
				if (flagxy > baseSmlrty)
				{
					x++;
				}
				//else if(flagxy == 0)
				else if (flagxy <= baseSmlrty)
				{
					//flagxy = alikeBackground(x,y + 1);
					flagxy = getSimilarity(Down,x,y);
					//if (flagxy == 1)
					if (flagxy > baseSmlrty)
					{
						// direction = Down;
						// y++;
						DIRECTION_Down;
					}
					//else if (flagxy == 0)
					else if (flagxy <= baseSmlrty)
					{
						//direction = Left;
						//x--;
						DIRECTION_Left;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
			break;
		case Down:
			//flagxy = alikeBackground(x + 1,y);
			flagxy = getSimilarity(Right,x,y);
			//if (flagxy == 1)
			if (flagxy > baseSmlrty)
			{
				// direction = Right;
				// x++;
				DIRECTION_Right;
			}
			//else if (flagxy == 0)
			else if (flagxy <= baseSmlrty)
			{
				//flagxy = alikeBackground(x,y + 1);
				flagxy = getSimilarity(Down,x,y);
				//if (flagxy == 1)
				if (flagxy > baseSmlrty)
				{
					y++;
				}
				//else if(flagxy == 0)
				else if (flagxy <= baseSmlrty)
				{
					//flagxy = alikeBackground(x - 1,y);
					flagxy = getSimilarity(Up,x,y);
					//if (flagxy == 1)
					if (flagxy > baseSmlrty)
					{
						// direction = Left;
						// x--;
						DIRECTION_Left;
					}
					//else if (flagxy == 0)
					else if (flagxy <= baseSmlrty)
					{
						//direction = Up;
						//y--;
						DIRECTION_Up;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
			break;
		case Up:
			//flagxy = alikeBackground(x - 1,y);
			flagxy = getSimilarity(Left,x,y);
			//if (flagxy == 1)
			if (flagxy > baseSmlrty)
			{
				// direction = Left;
				// x--;
				DIRECTION_Left;
			}
			//else if (flagxy == 0)
			else if (flagxy <= baseSmlrty)
			{
				//flagxy = alikeBackground(x,y - 1);
				flagxy = getSimilarity(Down,x,y);
				//if (flagxy == 1)
				if (flagxy > baseSmlrty)
				{
					y--;
				}
				//else if(flagxy == 0)
				else if (flagxy <= baseSmlrty)
				{
					//flagxy = alikeBackground(x + 1,y);
					flagxy = getSimilarity(Right,x,y);
					//if (flagxy == 1)
					if (flagxy > baseSmlrty)
					{
						// direction = Right;
						// x++;
						DIRECTION_Right;
					}
					//else if (flagxy == 0)
					else if (flagxy <= baseSmlrty)
					{
						//direction = Down;
						//y++;
						DIRECTION_Down;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
			break;
		case Left:
			//flagxy = alikeBackground(x ,y + 1);
			flagxy = getSimilarity(Down,x,y);
			//if (flagxy == 1)
			if (flagxy > baseSmlrty)
			{
				// direction = Down;
				// y++;
				DIRECTION_Down;
			}
			//else if (flagxy == 0)
			else if (flagxy <= baseSmlrty)
			{
				//flagxy = alikeBackground(x - 1,y);
				flagxy = getSimilarity(Left,x,y);
				//if (flagxy == 1)
				if (flagxy > baseSmlrty)
				{
					x--;
				}
				//else if(flagxy == 0)
				else if (flagxy <= baseSmlrty)
				{
					//flagxy = alikeBackground(x,y - 1);
					flagxy = getSimilarity(Up,x,y);
					//if (flagxy == 1)
					if (flagxy > baseSmlrty)
					{
						//direction = Up;
						//y--;
						DIRECTION_Up;
					}
					//else if (flagxy == 0)
					else if (flagxy <= baseSmlrty)
					{
						//direction = Right;
						//x++;
						DIRECTION_Right;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
			break;
		default:
			break;
	}
	return true;
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
