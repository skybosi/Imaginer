#include "Rbmp.h"
#include <math.h>

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
		//printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", ppot.getX(), ppot.getY());
		ppot.setempty(true);
		return ppot;
	}
	ppot.setRGB(allData[y][x]);
    /*
    if(alikeBackground(ppot))
    {
        ppot.show_PIXELS();
        printf("\n");
    }
 */
	return ppot;
}

PIXELS Rbmp::get_pix(PIXELS pixel)
{
	PIXELS ppot;
	if (out_range_error(pixel))
	{
		//printf("In get_pix(pixel) ,You set (x,y):(%d,%d) is out Range!\n", pixel.getX(), pixel.getY());
		ppot.setempty(true);
		return ppot;
	}
	ppot.setXY(pixel.getX(), pixel.getY());
	ppot.setRGB(allData[pixel.getY()][pixel.getX()]);
	//ppot.show_PIXELS();
	//printf("\n");
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
	//put the point and the 8 point arond into a array;
	//get the 8 point right position(x,y),if isEdge will be fixup
	pos8 = pots8.get_pos8(pixel, pos8, bmpWidth, bmpHeight);
	pos8[0] = get_pix(pos8[0]);
	//get the 8 point rgb value
	int i = 0;
	while (i < 4)
	{
		pos8[i + 1] = get_pix(pos8[i + 1]);
		pos8[i + 5] = get_pix(pos8[i + 5]);
		i++;
	}
	//set the 8 point message
	pots8.set_pots8(pos8);
	//pots8.show_PIXPOT();
	if (pos8)
	{
		delete[]pos8;
	}
	return pots8;
}

bool Rbmp::ImageHL()
{
	for (size_t i = 0; i < bmpWidth; i++)
	{
		for (size_t j = 0; j < bmpHeight; j ++)
		{
			if(allData[j][i].getEdge() < 0)
				allData[j][i].setRGB(0,255,0);
		}
	}
}

bool Rbmp::boundarysHL(vTrackTable& Tables)
{
	size_t skipTablesLen  = Tables.size();//boundary line's number
	for (size_t i = 0; i < skipTablesLen; i++)
	{
		boundarylineHL(Tables[i]);
	}
}

bool Rbmp::boundarylineHL(TrackTable& table)
{
	size_t tablelen = table.size();
	vTrackLine line;
	size_t linelen = 0;
	int sx = 0,ex = 0,y = 0;
	for (size_t i = 0; i < tablelen; ++i)
	{
		line = table[i];
		linelen = line.size();
		for (size_t j = 0; j < linelen; ++j)
		{
			//line[k].show();
			sx = line[j].sttx;
			ex = line[j].endx;
			y = line[j].ally;
			if(sx == ex)
				allData[y][sx].setRGB(0,255,0);
			else
			{
				allData[y][sx].setRGB(255,0,0);
				allData[y][ex].setRGB(0,0,255);
			}
		}
	}
	return true;
}

bool Rbmp::boundarysHL()
{
#define ONE

#ifdef EDGE
	ImageHL();
#else

	if(boundarys.empty())
		return false;
	int x = 0,y = 0;
	int avg  = 0;
	size_t boundarysLen  = boundarys.size();
	if(boundarysLen)
		avg = 255/ boundarysLen;
	for (size_t i = 0; i < boundarysLen; i++)
	{
		for (size_t j = 0; j < boundarys[i].size(); j ++)
		{
			y = boundarys[i][j].getY();
			x = boundarys[i][j].getX();
#ifdef  ONE
			//if(allData[y][x].getEdge() == -2)
			//allData[y][x].setRGB(255,0,0);
			if(allData[y][x].getEdge() == -1)
			{
				/*
				PIXELS::pix_p tmp = allData[y][x].getpPos();
				U8 tmpnum = allData[y][x].getpPosStatus();
				if(tmp.first == Down)//入点
					allData[y][x].setRGB(0,255,0);
				else if(tmp.first == Up)//出点
					allData[y][x].setRGB(255,0,0);
				else
				*/
				allData[y][x].setRGB(0,0,255);
			}
			if(allData[y][x].getEdge() == -2)
				allData[y][x].setRGB(0,255,0);
#endif
			/*
			int color = 128;
			allData[y][x].setRGB(color,color,color);
			int v = allData[y][x].getpPosValues();
			bool s = allData[y][x].getpPosStatus();
			if(s)
				allData[y][x].setRGB(0,255,0);
			*/
#ifdef  AVG
			allData[y][x].setRGB(0,255,i*avg);
#endif
		}
#ifdef  START
		//get first point
		y = boundarys[i][0].getY();
		x = boundarys[i][0].getX();
		allData[y][x].setRGB(255,255,255);
#endif
	}
#ifdef SKIP
	vector<limitXXY>::const_iterator it;
	for(it = skipTable.begin(); it != skipTable.end();++it)
	{
		allData[it->ally][it->sttx].setRGB(0,255,0);
		allData[it->ally][it->endx].setRGB(0,255,0);
	}
#endif
#endif
	return true;
}

bool Rbmp::imageCutOut()
{
	size_t skipTablesLen  = trackTables.size();
	int sx = 0,ex = 0,y = 0;
	TrackTable aline;
	vTrackLine line;
	for (size_t i = 0; i < skipTablesLen; i++)
	{
		aline = trackTables[i];
		for (size_t j = 0; j < aline.size(); j ++)
		{
			line = aline[j];
			for (size_t k = 0; k < line.size(); k++)
			{
				//line[k].show();
				sx = line[k].sttx;
				ex = line[k].endx;
				y = line[k].ally;
				if(sx == ex)
					allData[y][sx].setRGB(0,255,0);
				else
				{
					allData[y][sx].setRGB(255,0,0);
					allData[y][ex].setRGB(0,0,255);
				}
			}
		}
	}
	return true;
}

bool Rbmp::genBardiagram(ppPIXELS& allData,colorType color)
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

bool Rbmp::genHistogram(ppPIXELS& allData,colorType color)
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

ppPIXELS Rbmp::imageMirror(ppPIXELS& allData,Method method)
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

ppPIXELS Rbmp::getImage3Color(ppPIXELS& allData,colorType color)
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
ppPIXELS Rbmp::imageMove(ppPIXELS& allData,int mx, int my)
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

ppPIXELS Rbmp::imageZoom(ppPIXELS& allData,float scalex, float scaley)
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

ppPIXELS Rbmp::imageTranspose(ppPIXELS& allData,bool AR)
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

ppPIXELS Rbmp::imageShear(ppPIXELS& allData,bool XorY,float angle)
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
			/*
			//right up
			nx = XorY ? x + tan(angle) * (y - bmpHeight): x;
			ny = XorY ? y : y + tan(angle) * (x - bmpWidth);
			//left down
			nx = XorY ? x - tan(angle) * (y - bmpHeight)- bmpHeight: x;
			ny = XorY ? y : y - tan(angle) * (x - bmpWidth) - bmpWidth;
			*/

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

ppPIXELS Rbmp::imageRevolution(ppPIXELS& allData,int px,int py,float angle)
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

ppPIXELS Rbmp::imageSpherize(ppPIXELS& allData,float radius)
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

	else// circle
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
				imageShear(allData,false,45.0);
				H = allhead.infoHead.biHeight;
				LH = H;
				break;
			case Right:
				imageShear(allData,false,-45.0);
				H = allhead.infoHead.biHeight;
				RH = H;
				break;
			case Up:
				imageShear(allData,true,45.0);
				W = allhead.infoHead.biWidth;
				UH = allhead.infoHead.biHeight;
				break;
			case Down:
				imageShear(allData,true,-45.0);
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

ppPIXELS Rbmp::imageDensity(ppPIXELS& allData,float scale)
{
	ppPIXELS tmpimageData;
	tmpimageData = imageDatadup2(allData, tmpimageData);
	if(scale <= 0)
	{
		return NULL;
	}
	for (int y = 0; y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			allData[y][x] = tmpimageData[y][x]*scale;
		}
	}
	delImageData(tmpimageData, bmpHeight);
	return allData;
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

bool Rbmp::setBackground(RGBQUAD rgb)
{
	if(OUTRANGE(rgb.rgbRed) || OUTRANGE(rgb.rgbGreen) || OUTRANGE(rgb.rgbBlue))
		return false;
	backGround = rgb;
	return true;
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

bool Rbmp::setBackground(const PIXELS& pixel)
{
	RGBQUAD rgb = pixel.getRGB();
	return setBackground(rgb);
}

U32 Rbmp::setGranularity(U32 gran,bool opeartor)
{
	granularity = gran;
	granOpeartor = opeartor;
	return gran;
}
