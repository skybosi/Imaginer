#include "Rbmp.h"
#include "base.h"
#include <math.h>

bool Rbmp::getBoundaryLine(int& x, int& y)
{
	limitXXY footprint;
	int beforeX;
	beforeX = x;
	size_t bsize = boundarys.size();
	x = trackDown(allData[y][x]);
	if( boundarys.size() != bsize)
	{
		//footprint.add(allData[y][beforeX],allData[y][x],skipTable);
		return true;
	}
	return false;
	//printf("next footprint'x value:%d\n",x+1);
	//printf("trackDown.... insert\n");
}

void Rbmp::getBoundarys()
{
#define debug1
	PIXELS tmp;
	for (int y = 0;y < bmpHeight; y++)
	{
		for (int x = 0; x < bmpWidth; x++)
		{
			if(isBoundaryPoint(x,y))
			{
				tmp = allData[y][x];
				if(tmp.getEdge() >= 0)
				{
					if(!getBoundaryLine(x,y))
					{
						//printf("getBoundaryLine flase\n");
					}
				}
				else
				{
				}
			}
		}
	}
#ifdef debug
here:	printf("OOOOOOKKKKK!\n");
#endif
	printf("granularity: %u boundarys size:%ld\n",granularity,boundarys.size());
	printf("granularity: %u boundarys size:%ld\n",granularity,boundarys.size());
	printf("skip Table size:%ld\n",trackTables.size());
	/*
	for (size_t i =0; i < boundarys.size(); i++)
	{
		printf("$[%ld]: close or open status: %s ;boundary line len: %ld,(%u)\n",
				i,CLOSEOPEN(isCloseOpen(boundarys[i])),boundarys[i].size(),granularity);
		show_line(boundarys[i]);
	}
	vector<limitXXY>::const_iterator it;
	for(it = skipTable.begin(); it != skipTable.end();++it)
	{
		printf("start x:%d end x:%d communal y:%d\n", it->sttx,it->endx,it->ally);
	}

	printf("Frames point:%ld\n",frames.size());
	vector<FramePoint>::const_iterator itPoint;
	for(itPoint = frames.begin(); itPoint != frames.end();++itPoint)
	{
		printf("UPy:%d Downy:%d Leftx:%d Rightx:%d\n",
				(*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3]);
	}
	*/

}

bool Rbmp::isCloseOpen(dPIXELS boundaryline)
{
	if(boundaryline.empty())
		return false;
	else
		return (boundaryline.front() == boundaryline.back());
}

//start track down by following clues(顺藤摸瓜)
int  Rbmp::trackDown(PIXELS& startPoint)
{
	int sx = startPoint.getX();
	int x = sx;
	int sy = startPoint.getY();
	int y = sy;
	//make sure not trackDown again
	if(!boundarys.empty() && testStartP(startPoint))
		return sx;
	int nextx = 0;
	dPIXELS boundaryline;
	SkipTable skipTable2;
	Position direction = Right;
	//each direction relative to the image
	if(getRpoint(direction,x,y))
	{
		//make sure not only one point
		if (x < sx || y < sy)
		{
			sx--;
			return sx;
		}
	}
	else //when at first line,maybe out-of-range
		return sx++;
	//    //startPoint.setEdge(-1);//cannnot modify the x,y and rgb value
	//    SETCURR(Down,startPoint);
	//    startPoint.setpPosStatus();
	//    boundaryline.push_back(startPoint);
	//    /*
	//    printf("push s: ");
	//    startPoint.show_PIXELS();
	//    printf("\n");
	// */
	//    SETCURR(direction,allData[y][x]);
	//    /*
	//    if(y != sy)
	//    {
	//        allData[y][x].setEdge(-1);
	//    }
	//    else
	//    {
	//        allData[y][x].setEdge(-2);
	//    }
	//    */
	//    boundaryline.push_back(allData[y][x]);

	//SETCURR(Down,startPoint);
	//startPoint.setpPosStatus();
	//boundaryline.push_back(startPoint);
	Position prevDiret = direction;
	FramePoint framePoint(bmpHeight,bmpWidth);
	x = sx,y = sy;
	int rx = sx,ry = sy;
	int lx = sx,ly = sy;
	Position ldirection = direction;
	Position rdirection = direction;
	bool rEdge = false;
	bool lEdge = false;

	//anticlockwise(getRpoint)
	x = rx;//rollback lx to x
	y = ry;//rollback ly to y
	direction = rdirection;//rollback rdirection to direction
	while(1)//getRpoint
	{
		PIXELS& prevPoint = allData[y][x];
		prevDiret = direction;
		//printf("rdirection:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
		if(getRpoint(direction,x,y))
		{
			if(x == rx && y == ry)//back to the start point
			{
				if(ISH(prevDiret))
				{
					prevPoint.setEdge(-2);
				}
				else
				{
					prevPoint.setEdge(-1);
					genSkipTable(prevPoint,skipTable2);
				}
				//set frame Point
				framePoint.setframePoint(direction,allData[y][x]);
				//set boundary line point
				boundaryline.push_back(prevPoint);
				break;
			}
			if(prevDiret == direction)
			{
				if(ISV(prevDiret))
				{
					prevPoint.setEdge(-1);
					genSkipTable(prevPoint,skipTable2);
				}
				else
				{
					prevPoint.setEdge(-2);
				}
			}
			else//direction is change
			{
				prevPoint.setpPosStatus();//set Corner point
				int direSum = prevDiret + direction;
				switch(direSum)
				{
					case 3://reset direction
						prevDiret = direction;
						if(ISV(direction))
						{
							prevPoint.setEdge(-1);
							genSkipTable(prevPoint,skipTable2);
						}
						else
						{
							prevPoint.setEdge(-2);
						}
						break;
					case 2:
					case 4:
						if(ISV(direction))
						{
							prevPoint.setEdge(-2);
						}
						else
						{
							prevPoint.setEdge(-1);
							genSkipTable(prevPoint,skipTable2);
						}
						break;
					case 5://LR or RL
						prevPoint.setEdge(-1);
						genSkipTable(prevPoint,skipTable2);
						if(prevDiret > direction)
							prevPoint.setpPos(Up);
						else
							prevPoint.setpPos(Down);
						break;
					default://case 1:
						prevPoint.setEdge(-1);
						genSkipTable(prevPoint,skipTable2);
						genSkipTable(prevPoint,skipTable2);
						break;
				}
			}
			//set frame Point
			framePoint.setframePoint(direction,allData[y][x]);
			//set boundary line point
			boundaryline.push_back(prevPoint);
			/*
			   printf("push a: ");
			   get_pix(x,y).show_PIXELS();
			   printf("\n");
			   */
		}
		else //at Edge,Over loop
		{
			rEdge = true;
			break;
		}
	}

	//clockwise(getLpoint),if anticlockwise at Edge
	x = lx;//rollback lx to x
	y = ly;//rollback ly to y
	direction = ldirection;//rollback ldirection to direction
	while(rEdge)//getLpoint
	{
		PIXELS& prevPoint = allData[y][x];
		prevDiret = direction;
		//printf("rdirection:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
		if(getLpoint(direction,x,y))
		{
			if(x == lx && y == ly)//back to the start point
			{
				if(ISH(prevDiret))
				{
					prevPoint.setEdge(-2);
				}
				else
				{
					prevPoint.setEdge(-1);
					genSkipTable(prevPoint,skipTable2);
				}
				//set frame Point
				framePoint.setframePoint(direction,allData[y][x]);
				//set boundary line point
				boundaryline.push_back(prevPoint);
				/*
				   printf("push a: ");
				   get_pix(x,y).show_PIXELS();
				   printf("\n");
				   */
				break;
			}
			if(prevDiret == direction)
			{
				if(ISV(prevDiret))
				{
					prevPoint.setEdge(-1);
					genSkipTable(prevPoint,skipTable2);
				}
				else
				{
					prevPoint.setEdge(-2);
				}
			}
			else//direction is change
			{
				prevPoint.setpPosStatus();//set Corner point
				int direSum = prevDiret + direction;
				switch(direSum)
				{
					//reset direction
					case 2:
					case 4:
						prevDiret = direction;
						if(ISV(direction))//?
						{
							prevPoint.setEdge(-1);
							genSkipTable(prevPoint,skipTable2);
						}
						else
						{
							prevPoint.setEdge(-2);
						}
						break;
					case 3:
						if(ISV(direction))
						{
							prevPoint.setEdge(-2);
						}
						else
						{
							prevPoint.setEdge(-1);
							genSkipTable(prevPoint,skipTable2);
						}
						break;
					case 5://LR or RL
						prevPoint.setEdge(-1);
						genSkipTable(prevPoint,skipTable2);
						if(prevDiret < direction)
							prevPoint.setpPos(Up);
						else
							prevPoint.setpPos(Down);
						break;
					default://case 1:
						prevPoint.setEdge(-1);
						genSkipTable(prevPoint,skipTable2);
						genSkipTable(prevPoint,skipTable2);
						break;
				}
			}
			//set frame Point
			framePoint.setframePoint(direction,allData[y][x]);
			//set boundary line point
			boundaryline.push_back(prevPoint);
			/*
			   printf("push a: ");
			   get_pix(x,y).show_PIXELS();
			   printf("\n");
			   */
		}
		else //at Edge,Over loop
		{
			lEdge = true;
			break;
		}
	}
	TrackTable tracktable;
	skipTables.push_back(skipTable2);
	genTrackTable(skipTable2,tracktable);
	trackTables.push_back(tracktable);
	//startPoint.setEdge(-1);//cannnot modify the x,y and rgb value
#define GRANOPERATION(size) (granOpeartor)?(size > granularity):(size <= granularity)
	if(GRANOPERATION(boundaryline.size()))
	{
		boundarys.push_back(boundaryline);
		framePoint.setBindNum(boundarys.size());
		frames.push_back(framePoint);
		nextx = boundaryline.size();
	}
	else
	{
		nextx = 0;
		int xx = 0,yy = 0;
		while(!boundaryline.empty())//clean the smaller than the granularity data
		{
			xx = boundaryline.front().getX();
			yy = boundaryline.front().getY();
			allData[yy][xx].setEdge(0);
			allData[yy][xx].initpPos();
			boundaryline.pop_front();
		}
	}
	//printf("$[%d]> close or open status: %s Track down by following clues(顺藤摸瓜) OK... len:%ld(%u)\n",
	//globalI,CLOSEOPEN(isCloseOpen(boundaryline)),boundaryline.size(),granularity);
	//get next point's x value
	int NextX = sx;
	while(--nextx >= 0 && boundaryline[nextx].getY() == sy
			&& boundaryline[nextx].getX() >= NextX)
	{
		NextX = boundaryline[nextx].getX();
		//printf("%d mx %d\n",nextx,maxX);
	}
	//printf("%d mx %d\n",nextx,maxX);
	//printf("The max x %d will be nextpoint\n",maxX);
	return NextX;
}

int Rbmp::alikeBackground(PIXELS pixel)
{
	if(pixel.empty())
		return -1;
	/*
	RGBQUAD rgb = pixel.getRGB();
	if(EQUALBackGround(rgb))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	*/
	PIXELS backGroundpixel(backGround);
	if(getSimilarity(backGroundpixel,pixel) > baseSmlrty)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int Rbmp::alikeBackground(int x,int y)
{
	PIXELS tmp = get_pix(x,y);
	if(tmp.empty())
		return -1;
	/*
	RGBQUAD rgb = tmp.getRGB();
	if(EQUALBackGround(rgb))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	*/
	PIXELS backGroundpixel(backGround);
	if(getSimilarity(backGroundpixel,tmp) > baseSmlrty)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool Rbmp::alikeBackground(RGBQUAD rgb)
{
	if(EQUALBackGround(rgb))
		return true;
	else
		return false;
}

float Rbmp::getSimilarity(PIXELS backPoint, PIXELS currPoint)
{
	float Similarity = 0;
	PIXELS diff = backPoint - currPoint;
	/*
	   float SmlrtyR = 0,SmlrtyG = 0,SmlrtyB = 0;
	   SmlrtyR = 1 - diff.getRed()/255.0;
	   SmlrtyG = 1 - diff.getGreen()/255.0;
	   SmlrtyB = 1 - diff.getBlue()/255.0;
	   */
	//Similarity = 1- abs(diff.getRed() + diff.getGreen() + diff.getBlue())/765.0;
	diff = ~diff;
	Similarity = (diff.getRed() + diff.getGreen() + diff.getBlue())/765.0;
	//printf("curr x: %2d y: %2d\tSimilarity: %.3f\tSmlrtyR: %.3f\tSmlrtyG: %.3f\tSmlrtyB: %.3f\n",
	//       currPoint.getX(),currPoint.getY(),Similarity,SmlrtyR,SmlrtyG,SmlrtyB);
	//return TMAX(SmlrtyR,SmlrtyB,SmlrtyG);
	return Similarity;
}

float Rbmp::getSimilarity(Position direction,int x,int y,int step)
{
	float Similarity = 0;
	float SmlrtyR = 0,SmlrtyG = 0,SmlrtyB = 0;
	if(get_pix(x,y).empty())
		return -1;
	PIXELS potCurnt;
	PIXELS potRight;
	switch(direction)
	{
		case Down:
			if(y+step >= bmpHeight)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y+step][x];
			break;
		case Right:
			if(x+step >= bmpWidth)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y][x+step];
			break;
		case Up:
			if(y-step < 0)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y-step][x];
			break;
		case Left:
			if(x-step < 0)
				return -1;
			potCurnt = allData[y][x];
			potRight = allData[y][x-step];
			break;
		default:
			break;
	}
	PIXELS diff = potCurnt - potRight;
	SmlrtyR = 1 - diff.getRed()/255.0;
	SmlrtyG = 1 - diff.getGreen()/255.0;
	SmlrtyB = 1 - diff.getBlue()/255.0;
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
	/*
	   printf("x: %2d y: %2d Similarity: %.3f\n",x,y,Similarity);
	   if(Similarity != 1 && Similarity > baseSmlrty)
	   baseSmlrty = Similarity;
	   printf("curr x: %2d y: %2d\tSimilarity: %.3f\tSmlrtyR: %.3f\tSmlrtyG: %.3f\tSmlrtyB: %.3f\n",
	   x,y,Similarity,SmlrtyR,SmlrtyG,SmlrtyB);
	   */
	//return TMAX(SmlrtyR,SmlrtyB,SmlrtyG);
	return Similarity;
}

bool Rbmp::getRpoint(Position& direction,int& x,int& y)
{
	int flagxy = 0;
	//printf("direction:%s x:%d y:%d\t",Pos2str(direction).c_str(),x,y);
	switch (direction)
	{
		case Right:
			flagxy = alikeBackground(x,y + 1);
			if (flagxy == 1)
			{
				//direction = Down;
				//y++;
				DIRECTION_Down;
			}
			else if (flagxy == 0)
			{
				flagxy = alikeBackground(x + 1,y);
				if (flagxy == 1)
				{
					x++;
				}
				else if(flagxy == 0)
				{
					flagxy = alikeBackground(x,y - 1);
					if (flagxy == 1)
					{
						//direction = Up;
						//y--;
						DIRECTION_Up;
					}
					else if (flagxy == 0)
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
			flagxy = alikeBackground(x - 1,y);
			if (flagxy == 1)
			{
				//direction = Left;
				//x--;
				DIRECTION_Left;
			}
			else if (flagxy == 0)
			{
				flagxy = alikeBackground(x,y + 1);
				if (flagxy == 1)
				{
					y++;
				}
				else if(flagxy == 0)
				{
					flagxy = alikeBackground(x + 1,y);
					if (flagxy == 1)
					{
						//direction = Right;
						//x++;
						DIRECTION_Right;
					}
					else if (flagxy == 0)
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
			flagxy = alikeBackground(x + 1,y);
			if (flagxy == 1)
			{
				//direction = Right;
				//x++;
				DIRECTION_Right;
			}
			else if (flagxy == 0)
			{
				flagxy = alikeBackground(x,y - 1);
				if (flagxy == 1)
				{
					y--;
				}
				else if(flagxy == 0)
				{
					flagxy = alikeBackground(x - 1,y);
					if (flagxy == 1)
					{
						//direction = Left;
						//x--;
						DIRECTION_Left;
					}
					else if (flagxy == 0)
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
			flagxy = alikeBackground(x ,y - 1);
			if (flagxy == 1)
			{
				//direction = Up;
				//y--;
				DIRECTION_Up;
			}
			else if (flagxy == 0)
			{
				flagxy = alikeBackground(x - 1,y);
				if (flagxy == 1)
				{
					x--;
				}
				else if(flagxy == 0)
				{
					flagxy = alikeBackground(x,y + 1);
					if (flagxy == 1)
					{
						//direction = Down;
						//y++;
						DIRECTION_Down;
					}
					else if(flagxy == 0)
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
	int flagxy = 0;
	PIXELS pot;
	//printf("direction:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
	switch (direction)
	{
		case Right:
			flagxy = alikeBackground(x ,y - 1);
			if (flagxy == 1)
			{
				// direction = Up;
				// y--;
				DIRECTION_Up;
			}
			else if (flagxy == 0)
			{
				flagxy = alikeBackground(x + 1,y);
				if (flagxy == 1)
				{
					x++;
				}
				else if(flagxy == 0)
				{
					flagxy = alikeBackground(x,y + 1);
					if (flagxy == 1)
					{
						// direction = Down;
						// y++;
						DIRECTION_Down;
					}
					else if (flagxy == 0)
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
			flagxy = alikeBackground(x + 1,y);
			if (flagxy == 1)
			{
				// direction = Right;
				// x++;
				DIRECTION_Right;
			}
			else if (flagxy == 0)
			{
				flagxy = alikeBackground(x,y + 1);
				if (flagxy == 1)
				{
					y++;
				}
				else if(flagxy == 0)
				{
					flagxy = alikeBackground(x - 1,y);
					if (flagxy == 1)
					{
						// direction = Left;
						// x--;
						DIRECTION_Left;
					}
					else if (flagxy == 0)
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
			flagxy = alikeBackground(x - 1,y);
			if (flagxy == 1)
			{
				// direction = Left;
				// x--;
				DIRECTION_Left;
			}
			else if (flagxy == 0)
			{
				flagxy = alikeBackground(x,y - 1);
				if (flagxy == 1)
				{
					y--;
				}
				else if(flagxy == 0)
				{
					flagxy = alikeBackground(x + 1,y);
					if (flagxy == 1)
					{
						// direction = Right;
						// x++;
						DIRECTION_Right;
					}
					else if (flagxy == 0)
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
			flagxy = alikeBackground(x ,y + 1);
			if (flagxy == 1)
			{
				// direction = Down;
				// y++;
				DIRECTION_Down;
			}
			else if (flagxy == 0)
			{
				flagxy = alikeBackground(x - 1,y);
				if (flagxy == 1)
				{
					x--;
				}
				else if(flagxy == 0)
				{
					flagxy = alikeBackground(x,y - 1);
					if (flagxy == 1)
					{
						//direction = Up;
						//y--;
						DIRECTION_Up;
					}
					else if (flagxy == 0)
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

void Rbmp::getNext(Position& pos, int &x,int& y,int& nexts,int& step)
{
	nexts++;
	switch(pos)
	{
		case Left:
			x--;
			if(nexts==step)
			{
				pos = Down;
				nexts = 0;
			}
			break;
		case Down:
			y++;
			if(nexts==step)
			{
				step++;
				pos = Right;
				nexts = 0;
			}
			break;
		case Right:
			x++;
			if(nexts==step)
			{
				pos = Up;
				nexts = 0;
			}
			break;
		case Up:
			y--;
			if(nexts==step)
			{
				step++;
				pos = Left;
				nexts = 0;
			}
			break;
		default:
			break;
	}
}

int Rbmp::Bsearch(dSkipLine& line,int indexX)
{
	int len = line.size();
	int high = len -1,low = 0;//假设数组是从小到大排列的
	int midle = len >> 1;
	int curX = 0;
	while(high >= low)
	{
		midle = (high + low) >> 1;
		curX = line[midle].getX();
		if(curX  == indexX)
			return midle + 1;
		if(curX > indexX)
		{
			high = midle - 1;
		}
		else if(curX < indexX )
		{
			low = midle + 1;
		}
	}
	return low;
}

void Rbmp::genSkipTable(int x,int y,SkipTable& skipTable2)
{
	//allData[y][x].setRGB(0,0,255);
	dSkipLine line;
	dSkipLine::iterator it;
	int deltaY = -1;
	int pos = -1;
	if(!skipTable2.empty())
	{
		deltaY = y - skipTable2[0][0].getY();
	}
	if(deltaY >= 0 && deltaY < skipTable2.size())
	{
		it = skipTable2[deltaY].begin();
		pos = Bsearch(skipTable2[deltaY],x);
		skipTable2[deltaY].insert(it+pos,allData[y][x]);
	}
	else // a new watch line
	{
		line.push_back(allData[y][x]);
		skipTable2.push_back(line);
	}
}

void Rbmp::genSkipTable(const PIXELS& pixel,SkipTable& skipTable2)
{
	int x = pixel.getX();
	int y = pixel.getY();
	genSkipTable(x,y,skipTable2);
}

void Rbmp::genTrackTable(SkipTable& skipTable2,TrackTable& tracktable)
{
	size_t skptlen = skipTable2.size();
	dSkipLine line;
	vTrackLine lline;
	limitXXY lxxy;
	size_t j = 0;
	for (size_t i = 0; i < skptlen; i ++)
	{
		line = skipTable2[i];
		size_t len = line.size();
		j = 0;
		while(j < len && lxxy.add(line[j++],line[j++],lline));
		tracktable.push_back(lline);
		lline.clear();
	}
}

// is Boundary
bool Rbmp::isBoundaryPoint(PIXELS pot)
{
	/*
	PIXPOT tmp;
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
	}
	*/

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
	float similarity = 1;
	//float checkSmlrty = 0;
	float avgSimi = 0;
	float diffSim = 0;
	for (i = 0; x < bmpWidth-1; ++i,++x)
	{
		diffSim  = similarity - avgSimi;
		similarity = getSimilarity(Right,x,y);
		//printf("%3d: num:%lf\tavg:%lf\tdiff:%lf\n", i + 1, similarity, avgSimi, fabs(diffSim));
		avgSimi += diffSim / (i + 1);
		if (fabs(similarity - avgSimi) > 0.1)
		{
			++x;
			setBackground(allData[y][x]);
			//printf("finded :%lf\n", similarity);
			//make sure the edge point is not a shade
			//work is not stable, need TODO
			/*
			if(allData[y][x].getEdge() >= 0)
			{
				checkSmlrty = getSimilarity(Right,x,y);
				if(checkSmlrty != 1 && checkSmlrty != similarity)
				{
					++x;
					setBackground(allData[y][x]);
					similarity = checkSmlrty;
				}
			}
			*/
			baseSmlrty = similarity;
			// printf("baseSmlrty:%lf\n",baseSmlrty);
			break;
		}
	}
	if(x < bmpWidth-1)
	{
		return true;
	}
	else
		return false;
}

//#define FramesRelation()
void Rbmp::linker(const Frames& frame)
{
	printf("Frames point:%ld\n",frame.size());
	vector<FramePoint>::const_iterator itPoint;
	for(itPoint = frame.begin(); itPoint != frame.end();++itPoint)
	{
		printf("UPy:%d Downy:%d Leftx:%d Rightx:%d\n",
				(*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3]);
	}
}

/*
//return: true trackDown again
bool Rbmp::testStartP(PIXELS& pixel,int range)
{
    range = testRange;
    int x = pixel.getX();
    int y = pixel.getY();
    if(x < range || x > bmpWidth - 1 - range)
        return true;
    int i = 1;
    while(i <= range)
    {
        if(allData[y][x-i].getEdge() < 0 || allData[y][x+i].getEdge() < 0)
        {
            testRange = std::max(i,(int)testRange);
            //printf("testRange:%d\n",testRange);
            return true;
        }
        i++;
    }
    return false;

//	if(allData[y][x-3].getEdge() < 0 || allData[y][x-2].getEdge() < 0 ||
//			allData[y][x-1].getEdge() < 0 || allData[y][x].getEdge() < 0 ||
//			allData[y][x+1].getEdge() < 0 || allData[y][x-1].getEdge() < 0)
//		return true;
//	else
//		return false;

}*/

bool Rbmp::testStartP(PIXELS& pixel)
{
	int step = 1;
	int x = pixel.getX();
	int y = pixel.getY();
	Position nextPos = Left;
	int numPoint = 1;
	int nexts = 0;
	//    pixel.show_PIXELS();
	//    printf("\n");
	getNext(nextPos,x,y,nexts,step);
	//    allData[y][x].show_PIXELS();
	//    printf("\n");
	while( !isEdge(x,y) && numPoint < 25 && allData[y][x].getEdge() >= 0)
	{
		getNext(nextPos,x,y,nexts,step);
		//        allData[y][x].show_PIXELS();
		//        printf("\n");
		numPoint++;
	}
	//printf("%d\t",numPoint);
	if(numPoint >=25)
		return false;
	return true;
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
}
*/
