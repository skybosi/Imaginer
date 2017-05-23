#include "dpcCore.h"
#include <math.h>
#include <float.h>
#include "iVector.h"
#include "iFonts.h"

using namespace Imaginer::Utils;

namespace Imaginer
{
namespace DPC
{

#define OUTRANGE(P) ((P > 255) || (P < 0))
#define EQUALBackGround(rgb) ( !(rgb.rgbRed ^ backGround.rgbRed) && \
                                   !(rgb.rgbGreen ^ backGround.rgbGreen) && \
                                   !(rgb.rgbBlue ^ backGround.rgbBlue) )
/*
 * dpcCore core function
 *
 */

bool     dpcCore::getBoundaryLine(int& x, int& y)
{
    size_t bsize = boundarys.size();
    x = trackDown(_Data[y][x]);
    if( boundarys.size() != bsize)
    {
        return true;
    }
    return false;
    //printf("next footprint'x value:%d\n",x+1);
}

void     dpcCore::getBoundarys()
{
    if(!frames.empty())
    {
        return;
    }
    _beginX = _Data[0][0].getX();_beginY = _Data[0][0].getY();//矫正值
#define debug1
    PIXELS tmp;
    for (int y = 0;y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            if(isBoundaryPoint(x,y))
            {
                tmp = _Data[y][x];
                if(tmp.getEdge() >= IBASE)
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

bool     dpcCore::isCloseOpen(dPIXELS boundaryline)
{
    if(boundaryline.empty())
        return false;
    else
        return (boundaryline.front() == boundaryline.back());
}

//start track down by following clues(顺藤摸瓜)
int      dpcCore::trackDown(PIXELS& startPoint)
{
    int sx = startPoint.getX()-_beginX;
    int x = sx;
    int sy = startPoint.getY()-_beginY;
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
    {
        return sx++;
    }
    startPoint.setPostion(direction,true);
    //boundaryline.push_back(startPoint);
    Position prevDiret = direction;
    FramePoint framePoint(_height+_beginY,_width+_beginX);//此处的入参要注意
    x = sx,y = sy;
    int rx = sx,ry = sy;
    int lx = sx,ly = sy;
    Position ldirection = direction;
    Position rdirection = direction;
    bool rEdge = false;
    //bool lEdge = false;

    //anticlockwise(getRpoint)
    x = rx;//rollback lx to x
    y = ry;//rollback ly to y
    direction = rdirection;//rollback rdirection to direction
    while(1)//getRpoint
    {
        PIXELS& prevPoint = _Data[y][x];
        prevDiret = direction;
        //printf("rdirection:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
        if(getRpoint(direction,x,y))
        {
            if(x == rx && y == ry)//back to the start point
            {
                prevPoint.setPostion(prevDiret);//set postion point
                if(ISH(prevDiret))
                {
                    prevPoint.setEdge(IEDGE);
                }
                else
                {
                    prevPoint.setEdge(IPOLE);
                    //genSkipTable(prevPoint,skipTable2);
                }
                //set frame Point
                framePoint.setframePoint(direction,_Data[y][x]);
                //set boundary line point
                boundaryline.push_back(prevPoint);
                boundaryline.push_back(startPoint);
                break;
            }
            if(prevDiret == direction)
            {
                prevPoint.setPostion(prevDiret);//save postion point
                if(ISV(prevDiret))
                {
                    prevPoint.setEdge(IPOLE);
                    genSkipTable(prevPoint,skipTable2);
                }
                else
                {
                    prevPoint.setEdge(IEDGE);
                }
            }
            else//direction is change
            {
                int direSum = prevDiret + direction;
                switch(direSum)
                {
                    case 3://reset direction
                        prevDiret = direction;
                        if(ISV(direction))
                        {
                            prevPoint.setEdge(IPOLE);
                            genSkipTable(prevPoint,skipTable2);
                        }
                        else
                        {
                            prevPoint.setEdge(IEDGE);
                        }
                        break;
                    case 2:
                    case 4:
                        if(ISV(direction))
                        {
                            prevPoint.setEdge(IEDGE);
                        }
                        else
                        {
                            prevPoint.setEdge(IPOLE);
                            genSkipTable(prevPoint,skipTable2);
                        }
                        break;
                    case 5://LR or RL
                        prevPoint.setEdge(IPOLE);
                        genSkipTable(prevPoint,skipTable2);
                        if(prevDiret > direction)
                            prevPoint.setPostion(Up,true);
                        else
                            prevPoint.setPostion(Down,true);
                        break;
                    default://case 1:
                        prevPoint.setEdge(IPOLE);
                        genSkipTable(prevPoint,skipTable2);
                        genSkipTable(prevPoint,skipTable2);
                        break;
                }
                prevPoint.setPostion(prevDiret,true);//save Corner point
            }
            //set frame Point
            framePoint.setframePoint(direction,_Data[y][x]);
            //set boundary line point
            boundaryline.push_back(prevPoint);
            /*
               printf("push a: ");
               cout << getPix(x,y);
               printf("\n");
               */
        }
        else //at Edge,Over loop
        {
            rEdge = true;
            //if(ISH(direction))
            {
                boundaryline.push_back(prevPoint);
                genSkipTable(prevPoint,skipTable2);
            }
            break;
        }
    }

    //clockwise(getLpoint),if anticlockwise at Edge
    x = lx;//rollback lx to x
    y = ly;//rollback ly to y
    direction = ldirection;//rollback ldirection to direction
    while(rEdge)//getLpoint
    {
        PIXELS& prevPoint = _Data[y][x];
        prevDiret = direction;
        //printf("rdirection:%s x:%d y:%d\n",Pos2str(direction).c_str(),x,y);
        if(getLpoint(direction,x,y))
        {
            if(x == lx && y == ly)//back to the start point
            {
                prevPoint.setPostion(prevDiret);//set postion point
                if(ISH(prevDiret))
                {
                    prevPoint.setEdge(IEDGE);
                }
                else
                {
                    prevPoint.setEdge(IPOLE);
                    genSkipTable(prevPoint,skipTable2);
                }
                //set frame Point
                framePoint.setframePoint(direction,_Data[y][x]);
                //set boundary line point
                boundaryline.push_front(prevPoint);
                boundaryline.push_front(startPoint);
                /*
                   printf("push a: ");
                   cout << getPix(x,y);
                   printf("\n");
                   */
                break;
            }
            if(prevDiret == direction)
            {
                prevPoint.setPostion(prevDiret);//set postion point
                if(ISV(prevDiret))
                {
                    prevPoint.setEdge(IPOLE);
                    genSkipTable(prevPoint,skipTable2);
                }
                else
                {
                    prevPoint.setEdge(IEDGE);
                }
            }
            else//direction is change
            {
                int direSum = prevDiret + direction;
                switch(direSum)
                {
                    //reset direction
                    case 2:
                    case 4:
                        prevDiret = direction;
                        if(ISV(direction))//?
                        {
                            prevPoint.setEdge(IPOLE);
                            genSkipTable(prevPoint,skipTable2);
                        }
                        else
                        {
                            prevPoint.setEdge(IEDGE);
                        }
                        break;
                    case 3:
                        if(ISV(direction))
                        {
                            prevPoint.setEdge(IEDGE);
                        }
                        else
                        {
                            prevPoint.setEdge(IPOLE);
                            genSkipTable(prevPoint,skipTable2);
                        }
                        break;
                    case 5://LR or RL
                        prevPoint.setEdge(IPOLE);
                        genSkipTable(prevPoint,skipTable2);
                        if(prevDiret < direction)
                            prevPoint.setPostion(Up,true);
                        else
                            prevPoint.setPostion(Down,true);
                        break;
                    default://case 1:
                        prevPoint.setEdge(IPOLE);
                        genSkipTable(prevPoint,skipTable2);
                        genSkipTable(prevPoint,skipTable2);
                        break;
                }
                prevPoint.setPostion(prevDiret,true);//save Corner point
            }
            //set frame Point
            framePoint.setframePoint(direction,_Data[y][x]);
            //set boundary line point
            boundaryline.push_front(prevPoint);
            /*
               printf("push a: ");
               cout << getPix(x,y);
               printf("\n");
               */
        }
        else //at Edge,Over loop
        {
            //lEdge = true;
            //if(ISH(direction))
            {
                boundaryline.push_front(prevPoint);
                genSkipTable(prevPoint,skipTable2);
            }
            break;
        }
    }

    if(!isCloseOpen(boundaryline))//open
    {
        PIXELS head = boundaryline.front();
        PIXELS tail = boundaryline.back();
        if(tail.getX() == head.getX())//Touch Left/Right
        {
            int dy = tail.getY() - head.getY();
            tail = ((dy > 0) ? (tail) : (dy=-dy,head));
            int x = tail.getX();
            while(dy--)
            {
                PIXELS& curr = _Data[tail.getY()-dy][x];
                curr.setEdge(IPOLE);
                boundaryline.push_front(curr);
                genSkipTable(curr,skipTable2);
            }
        }
        if(tail.getY() == head.getY())//Touch Up/Down
        {
            int dx = tail.getX() - head.getX();
            tail = ((dx > 0) ? (tail) : (dx=-dx,head));
            int y = tail.getY();
            while(dx--)
            {
                PIXELS& curr = _Data[y][tail.getX()-dx];
                curr.setEdge(IEDGE);
                boundaryline.push_front(curr);
            }
        }
    }
    //startPoint.setEdge(IPOLE);//cannnot modify the x,y and rgb value
#define GRANOPERATION(size) (granOpeartor)?(size > granularity):(size <= granularity)
    if(GRANOPERATION(boundaryline.size()))
    {
        //deburrTrack(boundaryline);
        TrackTable tracktable;
        skipTables.push_back(skipTable2);
        genTrackTable(skipTable2,tracktable);
        trackTables.push_back(tracktable);
        boundarys.push_back(boundaryline);
        framePoint.setBindNum(boundarys.size()-1);
        frames.push_back(framePoint);
        nextx = boundaryline.size();
    }
    else
    {
        nextx = 0;
        int xx = 0,yy = 0;
        while(!boundaryline.empty())//clean the smaller than the granularity data
        {
            xx = boundaryline.front().getX()-_beginX;
            yy = boundaryline.front().getY()-_beginY;
            _Data[yy][xx].setEdge(IBASE);
            _Data[yy][xx].initpPos();
            boundaryline.pop_front();
        }
    }
    //printf("$[%d]> close or open status: %s Track down by following clues(顺藤摸瓜) OK... len:%ld(%u)\n",
    //globalI,CLOSEOPEN(isCloseOpen(boundaryline)),boundaryline.size(),granularity);
    //get next point's x value
    int NextX = sx;
    while(--nextx >= 0 && boundaryline[nextx].getY()-_beginY == sy
            && boundaryline[nextx].getX()-_beginX >= NextX)
    {
        NextX = boundaryline[nextx].getX()-_beginX;
        //printf("%d mx %d\n",nextx,maxX);
    }
    //printf("%d mx %d\n",nextx,maxX);
    //printf("The max x %d will be nextpoint\n",maxX);
    return NextX;
}

bool     dpcCore::setBackground(iColor rgb)
{
    /*
    if(OUTRANGE(rgb.rgbRed) || OUTRANGE(rgb.rgbGreen) || OUTRANGE(rgb.rgbBlue))
        return false;
        */
    backGround = rgb;
    return true;
}

bool     dpcCore::setBackground(U8 r,U8 g,U8 b)
{
    /*
    if(OUTRANGE(r) || OUTRANGE(g) || OUTRANGE(b))
        return false;
        */
    backGround.setRGBA(r,g,b);
    return true;
}

bool     dpcCore::setBackground(const PIXELS& pixel)
{
    return setBackground(pixel.getColor());
}

void     dpcCore::setGranularity(U32 gran,bool opeartor)
{
    granularity = gran;
    granOpeartor = opeartor;
}

void     dpcCore::setLGranlarty(int lgranlarty)
{
    LGranlarty = lgranlarty;
}

int      dpcCore::alikeBackground(PIXELS pixel)
{
    PIXELS backGroundpixel(backGround);
    if(getSimilarity(backGroundpixel,pixel) > baseSmlrty)
        return 1;
    return 0;
}

int      dpcCore::alikeBackground(int x,int y)
{
    PIXELS pixle;
    try{
        pixle = getPix(x,y);
    }catch(IException& e)
    {
        //printf("%s\n",e.msg.c_str());
        return -1;
    }catch(...)
    {
        return -1;
    }
    return alikeBackground(pixle);
}

float    dpcCore::getSimilarity(PIXELS backPoint, PIXELS currPoint)
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
    //       currPoint.getX()-_beginX,currPoint.getY()-_beginY,Similarity,SmlrtyR,SmlrtyG,SmlrtyB);
    //return TMAX(SmlrtyR,SmlrtyB,SmlrtyG);
    return Similarity;
}

float    dpcCore::getSimilarity(Position direction,int x,int y,int step)
{
    float Similarity = 0;
    //float SmlrtyR = 0,SmlrtyG = 0,SmlrtyB = 0;
    try
    {
        getPix(x,y);
    }
    catch(IException& e)
    {
        return -1;
    }
    PIXELS potCurnt;
    PIXELS potRight;
    switch(direction)
    {
        case Down:
            if(y+step >= _height)
                return -1;
            potCurnt = _Data[y][x];
            potRight = _Data[y+step][x];
            break;
        case Right:
            if(x+step >= _width)
                return -1;
            potCurnt = _Data[y][x];
            potRight = _Data[y][x+step];
            break;
        case Up:
            if(y-step < 0)
                return -1;
            potCurnt = _Data[y][x];
            potRight = _Data[y-step][x];
            break;
        case Left:
            if(x-step < 0)
                return -1;
            potCurnt = _Data[y][x];
            potRight = _Data[y][x-step];
            break;
        default:
            break;
    }
    //cout << potCurnt;
    //cout << potRight;
    PIXELS diff = potCurnt - potRight;
    //SmlrtyR = 1 - diff.getRed()/255.0;
    //SmlrtyG = 1 - diff.getGreen()/255.0;
    //SmlrtyB = 1 - diff.getBlue()/255.0;
    /*
       printf("curr:\n");
       cout << potCurnt;
       printf("\n");
       printf("Right:\n");
       cout << potRight;
       printf("\n");
       printf("diff:\n");
       cout << diff;
       printf("\n");
       printf("~diff:\n");
       */
    diff = ~diff;
    /*
       cout << diff;
       printf("\n");
       */
    U8 red = diff.getRed();
    U8 green =  diff.getGreen();
    U8 blue =  diff.getBlue();
    Similarity = (red + green + blue)/765.0;
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

bool     dpcCore::getRpoint(Position& direction,int& x,int& y)
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

bool     dpcCore::getLpoint(Position& direction,int& x,int& y)
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

void     dpcCore::getNext(Position& pos, int &x,int& y,int& nexts,int& step)
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

int      dpcCore::Bsearch(dSkipLine& line,int indexX)
{
    int len = line.size();
    int high = len -1,low = 0;//假设数组是从小到大排列的
    int midle = len >> 1;
    int curX = 0;
    while(high >= low)
    {
        midle = (high + low) >> 1;
        curX = line[midle].getX()-_beginX;
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

void     dpcCore::genSkipTable(int x,int y,SkipTable& skipTable2)
{
    //allData[y][x].setRGBA(0,0,255);
    dSkipLine line;
    dSkipLine::iterator it;
    int deltaY = -1;
    int pos = -1;
    if(!skipTable2.empty())
    {
        deltaY = (y +_beginY) - skipTable2[0][0].getY();
    }
    if(deltaY >= 0 && deltaY < (int)skipTable2.size())
    {
        it = skipTable2[deltaY].begin();
        pos = Bsearch(skipTable2[deltaY],x);
        skipTable2[deltaY].insert(it+pos,_Data[y][x]);
    }
    else // a new watch line
    {
        line.push_back(_Data[y][x]);
        skipTable2.push_back(line);
    }
}

void     dpcCore::genSkipTable(const PIXELS& pixel,SkipTable& skipTable2)
{
    int x = pixel.getX()-_beginX;
    int y = pixel.getY()-_beginY;
    genSkipTable(x,y,skipTable2);
}

void     dpcCore::genTrackTable(SkipTable& skipTable2,TrackTable& tracktable)
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
        if(len == 1)
        {
            lxxy.add(line[0],line[0],lline);
        }
        else
        {
            while(j < len)
            {
                lxxy.add(line[j],line[j+1],lline);
                j+=2;
            }
        }
        tracktable.push_back(lline);
        lline.clear();
    }
}

bool     dpcCore::isBoundaryPoint(PIXELS pot)
{
    if(alikeBackground(pot) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool     dpcCore::isBoundaryPoint(int& x,int& y)
{
    int i = 0;
    float similarity = 1;
    //float checkSmlrty = 0;
    float avgSimi = 0;
    float diffSim = 0;
    for (i = 0; x < _width-1; ++i,++x)
    {
        diffSim  = similarity - avgSimi;
        similarity = getSimilarity(Right,x,y);
        //printf("%3d: num:%lf\tavg:%lf\tdiff:%lf\n", i + 1, similarity, avgSimi, fabs(diffSim));
        avgSimi += diffSim / (i + 1);
        if (fabs(similarity - avgSimi) > 0.11)
        {
            ++x;
            setBackground(_Data[y][x]);
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
    if(x < _width-1)
    {
        return true;
    }
    else
        return false;
}

void     dpcCore::linker(const Frames& frame)
{
    printf("Frames point:%ld\n",frame.size());
    deque<FramePoint>::const_iterator itPoint;
    for(itPoint = frame.begin(); itPoint != frame.end();++itPoint)
    {
        printf("UPy:%d Downy:%d Leftx:%d Rightx:%d\n",
                (*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3]);
    }
}

/*
//return: true trackDown again
bool dpcCore::testStartP(PIXELS& pixel,int range)
{
    range = testRange;
    int x = pixel.getX()-_beginX;
    int y = pixel.getY()-_beginY;
    if(x < range || x > _width - 1 - range)
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

bool     dpcCore::testStartP(PIXELS& pixel)
{
    int step = 1;
    int x = pixel.getX()-_beginX;
    int y = pixel.getY()-_beginY;
    Position nextPos = Left;
    int numPoint = 1;
    int nexts = 0;
    //    cout << pixel;
    //    printf("\n");
    getNext(nextPos,x,y,nexts,step);
    //    cout << allData[y][x];
    //    printf("\n");
    while( !isEdge(x,y) && numPoint < 25 && _Data[y][x].getEdge() >= 0)
    {
        getNext(nextPos,x,y,nexts,step);
        //        cout << allData[y][x];
        //        printf("\n");
        numPoint++;
    }
    //printf("%d\t",numPoint);
    if(numPoint >=25)
        return false;
    return true;
}

PIXELS   dpcCore::getPix(int x, int y) throw(IException)
{
#define OUTIMAGE(_X,_Y,_W,_H) ((_X >= _W) || (_Y >= _H) || (_X < 0) || (_Y < 0))
    if(! OUTIMAGE(x,y,_width,_height))
        return _Data[y][x];
    else
    {
        throw IException("out of image RANGE!");
    }
#undef  OUTIMAGE
}

/*
bool dpcCore::deburrTrack(dPIXELS& boundaryline)
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
                y = boundaryline[i-j].getY()-_beginY;
                x = boundaryline[i-j].getX()-_beginX;
                printf("find back point! j:%d\n",j);
                boundaryline[i-j].setEdge(-3);
                allData[y][x].setEdge(-3);
                cout << boundaryline[i-j];
                printf("\n===============\n");
                y = boundaryline[i+j].getY()-_beginY;
                x = boundaryline[i+j].getX()-_beginX;
                boundaryline[i+j].setEdge(-3);
                allData[y][x].setEdge(-3);
                cout << boundaryline[i+j];
                printf("\n");
                j++;
            }
            j = 1;
        }
    }
    return true;
}
*/

bool     dpcCore::deburrTrack(dPIXELS& boundaryline)
{
    printf("in deburrTrack...\n");
    /*
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
                y = (itPoint-j)->getY()-_beginY;
                x = (itPoint-j)->getX()-_beginX;
                printf("find back point! j:%d\n",j);
                //(itPoint-j)->setEdge(-3);
                boundaryline.erase(itPoint-j);
                _Data[y][x].setEdge(-3);
                cout << *(itPoint-j);
                printf("\n===============\n");
                y = (itPoint+j)->getY()-_beginY;
                x = (itPoint+j)->getX()-_beginX;
                (itPoint+j)->setEdge(-3);
                _Data[y][x].setEdge(-3);
                cout << *(itPoint+j);
                printf("\n");
                j++;
            }
            j = 1;
        }
    }*/

    if (boundaryline.empty())
        return false;
    //cornerSize++;//size must add 1,
    int cornerSize = 2;
    int corner = 0;
    dPIXELS::iterator iter = boundaryline.begin();
    dPIXELS::iterator fast, low;
    for (;iter != boundaryline.end();)
    {
        fast = iter + cornerSize;
        low =  iter;
        if(*fast == *low)
        {
            do
            {
                ++corner,++iter;
                fast = iter + cornerSize;
                low = iter -(corner<<1);
            }while (*fast == *low);
            (corner<<=1)+=(cornerSize-1);

            //insert value
            //boundaryline.insert(low+1,pointer(low->getX(),low->getY()+1));
            iter = boundaryline.erase(low+1,fast);
            corner = 0;
        }
        else
             ++iter;
    }
    return true;
}

dPIXELS  dpcCore::deburrTrack(dPIXELS& boundaryline,int lgranlarty)
{
    dPIXELS rtBoundary;
    if (boundaryline.empty())
        return rtBoundary;
    PIXELS curpos;
    bool   noempty =  false;
    for (size_t i=0,j=0;i < boundaryline.size();++i)
    {
        curpos = boundaryline[i];
        if(curpos.isCorner())
        {
            if(noempty && !isnear(curpos,rtBoundary.back(),lgranlarty))
            {
                rtBoundary.push_back(curpos);
                ++j;
            }
            if(!noempty)
            {
                noempty = true;
                rtBoundary.push_back(curpos);
                ++j;
            }
        }
    }
    return rtBoundary;
}

bool     dpcCore::boundarysHL()
{
        getBoundarys();
        iColor color[7] = { iColor::RED,
                            iColor::ORANGE,
                            iColor::YELLOW,
                            iColor::GREEN,
                            iColor::BLUE,
                            iColor::INDIGO,
                            iColor::VIOLET};
#define RAND
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
            y = boundarys[i][j].getY()-_beginY;
            x = boundarys[i][j].getX()-_beginX;
#ifdef FINGERPRINT
            U8 tmpnum = _Data[y][x].getpPosStatus();
            if(tmpnum)
                _Data[y][x].setRGBA(iColor::GREEN);
#endif
#ifdef FOUR
            switch (boundarys[i][j].getpPosPostion())
            {
            case Up:
                _Data[y][x].setRGBA(iColor::BLUE);
                break;
            case Down:
                _Data[y][x].setRGBA(iColor::GREEN);
                break;
            case Left:
                _Data[y][x].setRGBA(iColor::RED);
                break;
            case Right:
                _Data[y][x].setRGBA(iColor::BLACK);
                break;
            default:
                _Data[y][x].setRGBA(iColor::GRAY);
                break;
            }
#endif
#ifdef  ONE
            //if(allData[y][x].getEdge() == -2)
            //allData[y][x].setRGBA(255,0,0);
            if(_Data[y][x].getEdge() == -1)
            {
                /*
                PIXELS::pix_p tmp = allData[y][x].getpPos();
                U8 tmpnum = allData[y][x].getpPosStatus();
                if(tmp.first == Down)//入点
                    allData[y][x].setRGBA(0,255,0);
                else if(tmp.first == Up)//出点
                    allData[y][x].setRGBA(255,0,0);
                else
                */
                _Data[y][x].setRGBA(iColor::BLUE);
            }
            if(_Data[y][x].getEdge() == -2)
                _Data[y][x].setRGBA(iColor::GREEN);
#endif
            /*
            int color = 128;
            allData[y][x].setRGBA(color,color,color);
            int v = allData[y][x].getpPosValues();
            bool s = allData[y][x].getpPosStatus();
            if(s)
                allData[y][x].setRGBA(iColor::GREEN);
            */
#ifdef  AVG
            _Data[y][x].setRGBA(0,avg*(i+2),0);
#endif
#ifdef RAND
            _Data[y][x].setRGBA(color[i%7]);
#endif
        }
#ifdef  START
        //get first point
        y = boundarys[i][0].getY()-_beginY;
        x = boundarys[i][0].getX()-_beginX;
        _Data[y][x].setRGBA(iColor::WHITE);
#endif
    }
#ifdef SKIP
    vector<limitXXY>::const_iterator it;
    for(it = skipTable.begin(); it != skipTable.end();++it)
    {
        allData[it->ally][it->sttx].setRGBA(iColor::GREEN);
        allData[it->ally][it->endx].setRGBA(iColor::GREEN);
    }
#endif

#endif
    return true;
}

bool     dpcCore::cutOut(vTrackTable& Tables)
{
    getBoundarys();
    ppPIXELS tmpData = NULL;
    if(dataDup2(_Data,tmpData))
    {
        clearData(_Data,_height);
    }
    size_t skipTablesLen  = frames.size();//boundary line's number
    for (size_t i = 0; i < skipTablesLen ; i++)
    {
        cutAOut(tmpData,Tables[i],frames[i]);
    }
    delData(tmpData,_height);
    return true;
}

bool     dpcCore::locateFrame()
{
    iColor red(255,0,0);
    iColor green(0,255,0);
    //drawRect(20,20,100,100,green);
    //drawLine(45,189,33,279,1);
    //drawLine(0,0,D2R(45),100);
    //drawLine(1,2,100,2,2);
    //drawLine(0,0,D2R(45),100);
    getBoundarys();
    printf("Frames point:%ld\n",frames.size());
    deque<FramePoint>::const_iterator itPoint;
    for(itPoint = frames.begin(); itPoint != frames.end();++itPoint)
    {
        printf("UPy:%d\tDowny:%d\tLeftx:%d\tRightx:%d\n",
                (*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3]);
    }
    clearData(_Data,_height);
    if(frames.empty())
        return false;
    size_t skipTablesLen  = frames.size();//boundary line's number
    PRs prs = EVER;
    for (size_t i = 0; i < skipTablesLen ; i++)
    {
        prs = checkPR(frames[i],frames[i+1]);
        //printf("checkPR:%d\n",prs);
        if(i&1)
        {
            drawRect(frames[i],red);
        }else
        {
            drawRect(frames[i],green);
        }
    }
    return true;
}

bool     dpcCore::locateMove()
{
    //autoMove(108,84,1000,1000);
    getBoundarys();
    dmvect vm = testMultiMvect();
    MultiMoveBoundary(5,vm);
    /*
    size_t len = trackTables.size();
    for(int i = 0;i < len ;++i)
    {
        //moveLine(boundarys[0],mv);
        checkCollision(frames,i,mv);
        moveBoundary(i,mv);
        mv.set(1000,1000);
        //zoomBoundary(boundarys[i],2);
    }*/
    return true;
}

bool     dpcCore::inTrackLine(vTrackLine line,int x)
{
    size_t len  = line.size();
    if(line.empty())
        return false;
    if(x < line.front().sttx || x > line.back().endx)
    {
        return false;
    }
    int high = len -1,low = 0;//假设数组是从小到大排列的
    int midle = len >> 1;
    while(high >= low)
    {
        midle = (high + low)>>1;
        if(line[midle].in(x))
            return true;
        else if(line[midle].lout(x))
            high = midle - 1;
        else
            low = midle + 1;
    }
    return false;
}

bool     dpcCore::cutAOut(ppPIXELS& imageData, TrackTable& table,FramePoint& FP)
{
    int len = table.size();
    vTrackLine line;
    for (int y = FP[0],i = 0; y <= FP[1] && i < len; y++)
    {
        line = table[i++];
        for (int x = FP[2]; x <= FP[3]; x++)
        {
            if(inTrackLine(line,x))//此处不需要传入y值得原因是：FramePoint的特性
            {
                _Data[y-_beginY][x-_beginX] = imageData[y-_beginY][x-_beginX];
            }
        }
    }
    return true;
}

bool     dpcCore::boundarysHL(vTrackTable& Tables)
{
    getBoundarys();
    size_t skipTablesLen  = Tables.size();//boundary line's number
    for (size_t i = 0; i < skipTablesLen; i++)
    {
        boundarylineHL(Tables[i]);
    }
    return true;
}

bool     dpcCore::boundarylineHL(TrackTable& table)
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
                _Data[y-_beginY][sx-_beginX].setRGBA(iColor::GREEN);
            else
            {
                _Data[y-_beginY][sx-_beginX].setRGBA(iColor::RED);
                _Data[y-_beginY][ex-_beginX].setRGBA(iColor::BLUE);
            }
        }
    }
    return true;
}

vdPIXELS dpcCore::boundaryFrame(vdPIXELS& bondry)
{
    vdPIXELS vboundarys;
    if(bondry.empty())
        return vboundarys;
    size_t  size = bondry.size();
    dPIXELS aBoundary;
    for (size_t i =0; i < size; i++)
    {
        aBoundary = bondry[i];
        vboundarys.push_back(deburrTrack(aBoundary,LGranlarty));
    }
    return vboundarys;
}

bool     dpcCore::boundaryer(const vdPIXELS& bondry)
{
    if(bondry.empty())
        return false;
    clearData(_Data,_height);
    int size = bondry.size();
    int asize = 0;
    int prevx = 0,prevy = 0;
    int currx = 0,curry = 0;
    iColor color[7] = { iColor::RED,
                        iColor::ORANGE,
                        iColor::YELLOW,
                        iColor::GREEN,
                        iColor::BLUE,
                        iColor::INDIGO,
                        iColor::VIOLET};
    for(int i = 0; i < size; ++i)
    {
        asize = bondry[i].size();
        for(int j = 0 ; j < asize-1; ++j)
        {
            prevx = bondry[i][j].getX() - _beginX;
            prevy = bondry[i][j].getY() - _beginY;
            currx = bondry[i][j+1].getX() - _beginX;
            curry = bondry[i][j+1].getY() - _beginY;
            drawLine(prevx,prevy,currx,curry,color[i%7]);
        }
    }
    return true;
}

bool     dpcCore::boundary2p(const vdPIXELS& bondry)
{
    if(bondry.empty())
        return false;
    clearData(_Data,_height);
    int size = bondry.size();
    int asize = 0;
    int x = 0,y = 0;
    for(int i = 0; i < size; ++i)
    {
        asize = bondry[i].size();
        for(int j = 0 ; j < asize; ++j)
        {
            x = bondry[i][j].getX() - _beginX;
            y = bondry[i][j].getY() - _beginY;
            _Data[y][x] = bondry[i][j];
        }
    }
    return true;
}

bool     dpcCore::ImageHL()
{
    for (int i = 0; i < _width; i++)
    {
        for (int j = 0; j < _height; j ++)
        {
            if(_Data[j][i].getEdge() < 0)
                _Data[j][i].setRGBA(iColor::GREEN);
        }
    }
    return true;
}

bool     dpcCore::autoMove(int x,int y,int mx,int my)
{
    iVector mv(mx,my);
    if(frames.empty())
    {
        getBoundarys();
    }
    int i = getNearFrame(frames,x,y);
    if(checkCollision(frames,i,mv))
    {
        if(moveBoundary(i,mv))
        {
            return true;
        }
        return false;
    }
    return false;
}

bool     dpcCore::updateFrames(Frames& fms,int& pos,iVector& where)
{
    if(fms.empty() || pos < 0 || pos >= fms.size())
        return false;
    printf("Frames point:%ld\n",frames.size());
    deque<FramePoint>::const_iterator itPoint;
    for(itPoint = fms.begin(); itPoint != fms.end();++itPoint)
    {
        printf("UPy:%d Downy:%d Leftx:%d Rightx:%d bindNum:%d\n",
                (*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3],itPoint->getBindNum());
    }
    printf("============================================\n");
    int fmsize = fms.size();
    int oldpos =  pos;
    FramePoint Move = fms[pos];
    FramePoint newMove = Move + where;
    if(where.Y() >= 0)//向下
    {
        while(++pos < fmsize && newMove > fms[pos]);
        --pos;
        //printf("pos:%d\n",pos);
    }
    else
    {
        while(--pos >= 0 && newMove < fms[pos]);
        ++pos;
        //printf("pos:%d\n",pos);
    }
    Frames::iterator itPointOld = fms.begin() + oldpos;
    fms.erase(itPointOld);
    Frames::iterator itPointNew = fms.begin() + pos;
    fms.insert(itPointNew,newMove);
    for(itPoint = fms.begin(); itPoint != fms.end();++itPoint)
    {
        printf("UPy:%d Downy:%d Leftx:%d Rightx:%d bindNum:%d\n",
                (*itPoint)[0],(*itPoint)[1],(*itPoint)[2],(*itPoint)[3],itPoint->getBindNum());
    }
    return true;
}

/*
 * dataPcer utils for deal with function
 *
 */

bool     dpcCore::drawLine(int x,int y,double delta,double length,iColor rgb,int size/* = 1*/)throw(IException)
{
    try
    {
        int ey = y + length*sin(delta);
        int ex = x + length*cos(delta);
        drawLine(x,y,ex,ey,rgb,size);
    }
    catch(IException& e)
    {
        return false;
    }
    return true;
}

bool     dpcCore::drawRect(int x,int y,int width,int height,iColor rgb,int size/* = 1*/)
{
//#define RESET(_T,_FLAG) (((_T) < 0) ? 0 : (((_T) > (_FLAG)) ? (_FLAG) : (_T)))
    int newXY = 0;
    try
    {
        drawLine(x,y,x+width,y,rgb,size);
        drawLine(x,y,x,y+height,rgb,size);
        newXY = y + height;
        newXY = REPAIR(newXY,_height-1);
        drawLine(x,newXY,x+width,newXY,rgb,size);
        newXY = x + width;
        newXY = REPAIR(newXY,_width-1);
        drawLine(newXY,y,newXY,y+height,rgb,size);
    }
    catch(IException& e)
    {
        return false;
    }
    return true;
//#undef RESET
}

bool     dpcCore::drawRect(const FramePoint& FP,iColor rgb,int size/* = 1*/)
{
    try
    {
        drawLine(FP[2],FP[0],FP[3],FP[0],rgb,size);
        drawLine(FP[2],FP[1],FP[3],FP[1],rgb,size);
        drawLine(FP[2],FP[0],FP[2],FP[1],rgb,size);
        drawLine(FP[3],FP[0],FP[3],FP[1],rgb,size);
    }
    catch(IException& e)
    {
        return false;
    }
    return true;
}

inline   bool dpcCore::isnear(PIXELS& A,PIXELS& B,int lgranlarty)const
{
    int iX = abs(A.getX() - B.getX());
    int iY = abs(A.getY() - B.getY());
    if(iX + iY < lgranlarty)
        return true;
    return false;
}

//void     dataPcer::drawLine(int startX, int startY, int endX,int endY,int size,iColor* rgb /*= NULL*/)throw(IException)
/*{
    if(size < 0)
    {
        throw("error paint size\n");
    }
#define SWAP(I,J) (I=I+J,J=I-J,I=I-J)
    iColor tmp;
    if(rgb)
    {
        tmp = *rgb;
    }else
    {
        tmp.setRGBA(0,255,0);
    }
    if(endX < startX)
    {
        SWAP(endX,startX);
        SWAP(endY,startY);
    }
    int x = startX,y = startY;
    if(startY == endY)
    {
        //平行x轴
        while(x <= endX)
        {
            _Data[y][x].setRGBA(tmp);
            x++;
        }
        return ;
    }
    if(startX == endX)
    {
        //平行y轴
        while (y <= endY)
        {
            _Data[y][x].setRGBA(tmp);
            y++;
        }
        return ;
    }
    int deltaX = endX - startX,deltaY = endY - startY;
    int p = 0;
    int twoDy = deltaY << 1,twoMinusDx = (deltaY-deltaX) << 1,
        twoDx = deltaX << 1,twoMinusDy = (deltaX-deltaY) << 1;
    int twoSum= (deltaY + deltaX) << 1;
    double k = (double)deltaY / (double)deltaX;

    //0 < k < 1的情况
    if(k < 1.0 && k > 0.0)
    {
        p = (deltaY << 1) - deltaX;
        while(x < endX)
        {
            _Data[y][x].setRGBA(tmp);
            x++;
            if(p < 0)
                p += twoDy;
            else
            {
                y++;
                p += twoMinusDx;
            }
        }
    }
    //k>=1的情况
    if(k >= 1.0)
    {
        p = deltaY;
        while(y < endY)
        {
            _Data[y][x].setRGBA(tmp);
            y++;
            if(p < 0)
                p += twoDx;
            else
            {
                x++;
                p += twoMinusDy;
            }
        }
    }
    //0>k>-1的情况
    if(k > -1 && k < 0)
    {
        p = (deltaY << 1) + deltaX;
        while(x < endX)
        {
            _Data[y][x].setRGBA(tmp);
            x++;
            if(p>=0)
                p += twoDy;
            else
            {
                y--;
                p += twoSum;
            }
        }
    }
    //k < -1的情况
    if(k <= -1)
    {
        p = (deltaX << 1) - deltaY;
        while(y > endY)
        {
            _Data[y][x].setRGBA(tmp);
            y--;
            if(p >= 0)
                p -= twoDx;
            else
            {
                x++;
                p -= twoSum;
            }
        }
    }
#undef SWAP
}*/

void     dpcCore::drawLine(int x1, int y1, int x2, int y2, iColor rgb,int size/* = 1*/) throw(IException)
{
    if(size < 0)
    {
        throw("error paint size\n");
    }
    //(!rgb) ? (tmp = *rgb) : (tmp.setRGBA(0,255,0));
    int x = x1,y = y1;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int s1 = (x2 > x1) ? 1 : -1;
    int s2 = (y2 > y1) ? 1 : -1;
    bool interchange = false;	// 默认不互换 dx、dy
    if (dy > dx)				// 当斜率大于 1 时，dx、dy 互换
    {
        int temp = dx;
        dx = dy;
        dy = temp;
        interchange = true;
    }
    int p = (dy << 1) - dx;
    for(int i = 0;i < size ;++i)
    {
        if(interchange)
        {
            (dx)?(++x1):(++y1);//参考上一版本的设计思路
        }else
        {
            (dx)?(++y1):(++x1);//参考上一版本的设计思路
        }
        for(int j = 0; j < dx; ++j)
        {
            _Data[y][x].setRGBA(rgb);
            if (p >= 0)
            {
                if (!interchange)		// 当斜率 < 1 时，选取上下象素点
                    y += s2;
                else					// 当斜率 > 1 时，选取左右象素点
                    x += s1;
                p -= dx << 1;
            }
            if (!interchange)
                x += s1;				// 当斜率 < 1 时，选取 x 为步长
            else
                y += s2;				// 当斜率 > 1 时，选取 y 为步长
            p += dy << 1;
        }
        x = x1,y = y1;
    }
}

void     dpcCore::drawLine(const iPoint& start,const iPoint& end,int rgb,int size/* = 1*/) throw(IException)
{
    int sx = start.X();int ex = start.Y();
    int sy = end.X();int ey = end.Y();
    drawLine(sx,ex,sy,ey,rgb,size);
}

void     dpcCore::gradLine(int startX, int startY, int endX,int endY,iColor avgrgb,int size/* = 1*/)throw(IException)
{
    if(size < 0)
    {
        throw("error paint size\n");
    }
#define SWAP(I,J) ((I>J)?(I=I+J,J=I-J,I=I-J):(I=I,J=J))
    SWAP(startX,endX);
    SWAP(startY,endY);
    int deltaX = endX - startX;
    int deltaY = endY - startY;
    int e = 0;
    for(int i = 0;i < size ;++i)
    {
        if(!deltaX)//Vertical ,x not change
        {
            for (int y = startY; y <= endY; ++y)
            {
                _Data[y][startX] = _Data[y][startX] + avgrgb;
            }
            startX++;
        }
        else if(!deltaY)//Horizontal ,y not change
        {
            for (int x = startX; x <= endX; ++x)
            {
                _Data[startY][x] =  _Data[startY][x] + avgrgb;
            }
            startY++;
        }
        else
        {
            for (int x = startX, y = startY; x <= endX; ++x)
            {
                //e2 =  e1 + 2 * deltaY – 2 * deltaX * (y2 – y1)
                //当e1 + 2 * deltaY > deltaX时y2 = y1 + 1, 否则y2 = y1
                e += (deltaY << 1);
                if (e > deltaX)
                {
                    e -= (deltaX << 1);
                    ++y;
                }
                _Data[y][x] =  _Data[y][x] + avgrgb;
            }
            startY++;
        }
    }
}

/*
 * core function for deal with image message
 *
 */

dpcCore::PRs dpcCore::checkPR(const FramePoint& A,const FramePoint& B)
{
      int a_cx,a_cy; /* 第一个中心点*/
      int a_cw,a_ch; /* 第一个矩形宽高的一半*/
      int b_cx,b_cy; /* 第二个中心点*/
      int b_cw,b_ch; /* 第二个矩形宽高的一半*/
      a_cy = (A[0] + A[1]) >> 1;
      a_ch = (A[1] - A[0]) >> 1;
      a_cx = (A[2] + A[3]) >> 1;
      a_cw = (A[3] - A[2]) >> 1;

      b_cy = (B[0] + B[1]) >> 1;
      b_ch = (B[1] - B[0]) >> 1;
      b_cx = (B[2] + B[3]) >> 1;
      b_cw = (B[3] - B[2]) >> 1;

      int DisX = abs(a_cx - b_cx);
      int DisY = abs(a_cy - b_cy);
      if(DisX <= (a_cw + b_cw) || DisY <= (a_ch + b_ch)) //cross
      {
          if(A[0] <= B[0])
          {
              if(A[1] >= B[1] && A[2] <= B[2] && A[3] >= B[3])
                  return IN; //A in the B
              else if(A[1] == B[0] || A[2] == B[3] || A[3] == B[2])
                  return HERE;
              else return CROSS;
          }else
          {
              if(A[1] <= B[1] && A[2] >= B[2] && A[3] <= B[3])
                  return IN2;//B in the A
              else if(A[0] == B[1] || A[2] == B[3] || A[3] == B[2])
                  return HERE;
              else
                  return CROSS;
          }
      }else
      {
          if((DisX - a_cw - b_cw) < 5 || (DisY - a_ch - b_ch) < 5)
              return NEAR;
          return OUT;
      }
      return EVER;
}

dmvect   dpcCore::genMultiMvect(dPoint points)
{
    dmvect vm;
    if(points.empty())
        return vm;
    iVector mv;
    iPoint cur, next;
    while(1)
    {
        cur = points[0];
        points.pop_front();
        if(points.empty())
            break;
        next = points[0];
        mv.set(next-cur);
        vm.push_back(mv);
    }
    return vm;
}

dmvect   dpcCore::genMultiMvect(dPIXELS pixels)
{
    dmvect vm;
    if(pixels.empty())
        return vm;
    iVector mv;
    iPoint cur, next;
    while(1)
    {
        cur = pixels[0].toPoint();
        pixels.pop_front();
        if(pixels.empty())
            break;
        next = pixels[0].toPoint();
        mv.set(next-cur);
        vm.push_back(mv);
    }
    return vm;
}

dmvect   dpcCore::testMultiMvect()
{
    dPoint dp;
    dmvect vm;
    iPoint pv(1000,1000);
    iPoint pv2(-1000,-1000);
    iPoint pv3(-1000,1000);
    iPoint pv4(1000,-1000);
    dp.push_back(pv);
    dp.push_back(pv2);
    dp.push_back(pv3);
    dp.push_back(pv4);
    iVector mv(pv);
    iVector mv2(pv2);
    iVector mv3(pv3);
    iVector mv4(pv4);
    vm.push_back(mv);
    vm.push_back(mv2);
    vm.push_back(mv3);
    vm.push_back(mv4);
    return vm;
    return genMultiMvect(dp);
    return genMultiMvect(boundarys[0]);
}

bool     dpcCore::updateBoundary(vdPIXELS& bondry,int pos,iVector& where)
{
    if(bondry.empty() || pos < 0 || pos >= bondry.size())
        return false;
    dPIXELS& aBoundary = bondry[pos];
    size_t  size = aBoundary.size();
    for (size_t i =0; i < size; i++)
    {
        aBoundary[i] += where;
    }
    return true;
}

int      dpcCore::getNearFrame(Frames& fms,int x,int y)const
{
    if(fms.empty())
        return -1;
    int len = fms.size();
    int high = len -1,low = 0;//假设数组是从小到大排列的
    int midle = len >> 1;
    int cur = 0;
    while(high >= low)
    {
        midle = (high + low) >> 1;
        cur = fms[midle][Up];
        if(cur  == y)
            return fms[midle].getBindNum();
        if(cur < y)
        {
            low = midle + 1;
        }
        else if(cur > y )
        {
            high = midle-1;
        }
    }
    if(low > (len>>1))
    {
        cur = low-1;
        while (cur >= 0)
        {
            if(fms[cur].in(x,y))
                return fms[cur].getBindNum();
            cur--;
        }
    }else
    {
        cur = 0;
        while (cur < low)
        {
            if(fms[cur].in(x,y))
                return fms[cur].getBindNum();
            cur++;
        }
    }
    return -1;
}

bool     dpcCore::checkCollision(Frames& fms,int pos,iVector& where,int interval /*=0*/)
{
    if(fms.empty() || pos < 0 || pos >= fms.size())
        return false;
    if(where.getFofA() == O)
        return true;
    int newpos = 0;
    FramePoint& Move = fms[pos];
    FramePoint Movetmp = Move;
    FramePoint Curr = fms[newpos];
    bool eachPos = true;
    int _y = fabs(where.Y());
    int _x = fabs(where.X());
    int minMvH = _y;
    int MH = 0;
    int MW = 0;
    int move = 0;//
    FofA fofa = where.getFofA();
    if(fofa < R)
    {
        while(minMvH <= _y && newpos < fms.size())
        {
            if(newpos == pos)
            {
                goto NEXTX;
            }
            switch(fofa)
            {
            case RU:
                if(Move[Left] > Curr[Right] || Move[Down] < Curr[Up])
                    goto NEXTX;
                //获取待测框角点在移动向量的哪一侧(上方false，下方或向量上true)
                eachPos = Move.checkPosition(where,Curr);
                if(eachPos)
                {
                    MW = Move[Right]-Curr[Left];
                }
                else
                {
                    MH = Move[Up]-Curr[Down];
                }
                break;
            case LU:
                if(Move[Right] < Curr[Left] || Move[Down] < Curr[Up])
                    goto NEXTX;
                //获取待测框角点在移动向量的哪一侧(上方false，下方或向量上true)
                eachPos = Move.checkPosition(where,Curr);
                if(eachPos)
                {
                    MH = Move[Up]-Curr[Down];
                }
                else
                {
                    MW = Move[Left]-Curr[Right];
                }
                break;
            case LD:
                if(Move[Right] < Curr[Left] || Move[Up] > Curr[Down])
                    goto NEXTX;
                //获取待测框角点在移动向量的哪一侧(上方false，下方或向量上true)
                eachPos = Move.checkPosition(where,Curr);
                if(eachPos)
                {
                    MW = Move[Left]-Curr[Right];
                }
                else
                {
                    MH = Move[Down]-Curr[Up];
                }
                break;
            case RD:
                if(Move[Left] > Curr[Right] || Move[Up] > Curr[Down])
                    goto NEXTX;
                //获取待测框角点在移动向量的哪一侧(上方false，下方或向量上true)
                eachPos = Move.checkPosition(where,Curr);
                if(eachPos)
                {
                    MH = Move[Down]-Curr[Up];
                }
                else
                {
                    MW = Move[Right]-Curr[Left];
                }
                break;
            default:
                break;
            }
            if(MW)
            {
                MH = (MW * where.Y())/where.X();
                MW = 0;
            }
            Movetmp = Move + where.reset(MH);//获取临时移动的框子的位置
            if((MH = abs(MH)) <= minMvH && checkPR(Movetmp,Curr) < NEAR)
            {
                minMvH = MH;
            }
NEXTX:      Curr = fms[++newpos];
        }
        if(minMvH != where.Y())
        {
            where = where.reset(minMvH-(1+interval));
        }
    }
    else
    {
        if(!_y)
        {
            minMvH = _x;
            _y = _x;//just for loop
        }
        while(minMvH <= _y && newpos < fms.size())
        {
            if(newpos == pos)
            {
                goto NEXT;
            }
            switch (fofa)
            {
            case R:
                if(Move[Up] > Curr[Down] || Move[Down] < Curr[Up] || Move[Left] > Curr[Right])
                    goto NEXT;
                move = Curr[Left] - Move[Right];
                break;
            case U:
                if(Move[Right] < Curr[Left] || Move[Left] > Curr[Right] || Move[Down] < Curr[Up])
                    goto NEXT;
                move = Move[Up] - Curr[Down];
                break;
            case L:
                if(Move[Up] > Curr[Down] || Move[Down] < Curr[Up] || Move[Right] < Curr[Left])
                    goto NEXT;
                move = Move[Left] - Curr[Right];

                break;
            case D:
                if(Move[Right] < Curr[Left] || Move[Left] > Curr[Right] || Move[Up] > Curr[Down])
                    goto NEXT;
                move = Curr[Up] - Move[Down];
                break;
            default:
                break;
            }
            if( move < minMvH)
                minMvH = move;
NEXT:       Curr = fms[++newpos];
        }
        if(where.X())
        {
            int meta_x = where.X()/_x;
            where.X() = meta_x * abs(minMvH-(1+interval));
        }else
        {
            int meta_y = where.Y()/_y;
            where.Y() = meta_y * abs(minMvH-(1+interval));
        }
    }
    //make sure the Move not out image,so should reset where again
    Move.checkout(where,_width,_height);
    return true;
}

bool     dpcCore::BrainRepair(PIXELS& A,PIXELS& B)
{
    int range = min(abs(A.getX()-B.getX()),abs(A.getY()-B.getY()));
    iColor avg = ((A - B)/(float)range).getColor();
    drawLine(A.getX(),A.getY(),B.getX(),B.getY(),avg,3);
    return true;
}

bool     dpcCore::BrainRepair(PIXELS& A,bool VorH,float range/* = 1*/)
{
    if(!range)
        return false;
    int x = A.getX();
    int y = A.getY();
    iColor avg;
    int range2 = (int)range;
    if(VorH)
    {
        avg = ((A - _Data[y+range2][x])/range).getColor();
        drawLine(x,y,x,y+range2,avg,1);
    }
    else
    {
        avg = ((A - _Data[y][x+range2])/range).getColor();
        drawLine(x,y,x+range2,y,avg,1);
    }
    return true;
}

bool     dpcCore::moveLine(dPIXELS& line,iVector where,bool isOverWrite/* = false*/)
{
    if(line.empty())
        return false;
    int len = line.size();
    if(!isOverWrite)
    {
        int xx = 0,yy = 0;
        for(int i = 0;i < len;++i)
        {
            xx = REPAIR(line[i].getX()-_beginX + where.X(), _width);
            yy = REPAIR(line[i].getY()-_beginY + where.Y(), _height);
            _Data[yy][xx] = line[i];
            line[i].setRGBA(0,255,0);
        }
        return true;
    }
    else
    {
        int xx = 0,yy = 0;
        int x = 0, y = 0;
        iColor color;
        //int i = 0;
        for(int i = 0;i < len;++i)
        {
            color = line[i].getColor();
            x = line[i].getX();
            y = line[i].getY();
            xx = REPAIR(x -_beginX + where.X(), _width);
            yy = REPAIR(y -_beginY + where.Y(), _height);
            drawLine(x,y,xx,yy,color);
            //_Data[yy][xx] = line[i];
            //line[i].setRGBA(0,255,0);
        }
        return true;
    }
}

bool     dpcCore::moveLine(int sx,int sy,int ex,int ey,iVector where,bool isOverWrite/* = false*/)
{
    //(x-sx)/(ex-sx)=(y-sy)/(ey-sy)
    float K = 0;
    float B = 0;
    if(ex == sx)
    {
        K = FLT_MAX;
        B = ey - sy;
    }
    else if(ey == sy)
    {
        K = 0;
        B = ex - sx;
    }
    else
    {
        K =(ey -sy)/(float)(ex -sx);
        B = sy - K * sx;
    }
    int xx = 0,yy = 0,y = 0;
    if(!isOverWrite)
    {
        for(int x = sx;x <= ex;++x)
        {
            y = K * x + B;
            xx = REPAIR(x -_beginX + where.X(), _width);
            yy = REPAIR(y -_beginY + where.Y(), _height);
            _Data[yy][xx] = _Data[y][x];
            _Data[y][x].setRGBA(0,255,0);
        }
        return true;
    }
    else
    {
        iColor color;
        //int i = 0;
        for(int x = sx;x <= ex;++x)
        {
            y = K * x + B;
            color = _Data[y][x].getColor();
            xx = REPAIR(x -_beginX + where.X(), _width);
            yy = REPAIR(y -_beginY + where.Y(), _height);
            drawLine(x,y,xx,yy,color);
            //_Data[yy][xx] = line[i];
            //line[i].setRGBA(0,255,0);
        }
        return true;
    }
}

int      dpcCore::moveBoundary(int pos,iVector where,bool isOverWrite/* = false*/)
{
    if(pos < 0)
        return -1;
    int newpos = frames[pos].getBindNum();
    TrackTable& table = trackTables[newpos];
    if(table.empty())
        return -1;
    int len = table.size();
    int linelen = 0;
    vTrackLine line;
    limitXXY   range;
    limitXXY   newRane;
    int x = 0,y = 0;
    int xx = 0,yy = 0;
    int coverLine = 0;
    int moveY = fabs(where.Y());
    if(!isOverWrite)
    {
        for(int i = 0;i < len; ++i)//要移动的边界线总共的行数
        {
/*
 * 坐标系：右下(Right:Down)为正方向
 * _y < 0:向上移动，必须从上到下移动每一行像素
 * _y > 0:向下移动，必须从下到上移动每一行像素
 * 不按此顺序将覆盖部分数据
 */
            if(where.Y() <= 0)
            {
                line = table[i];
                linelen = line.size();
                for(int j = 0;j < linelen;++j)//每一行可能有多列，凹边界线时
                {
                    range = line[j];
                    if(moveY >= linelen)//由于移动的够远，所以不会与原来的边界重合
                        coverLine = 0;
                    else
                    {
                        coverLine = linelen - moveY;
                        newRane = line[j+moveY] + where;
                    }
                    for(x = range.sttx,y = range.ally;x <= range.endx;++x)
                    {
                        xx = REPAIR(x -_beginX + where.X(), _width);
                        yy = REPAIR(y -_beginY + where.Y(), _height);
                        _Data[yy][xx]  = _Data[y][x];
                        if(!coverLine || !newRane.in(x))//this check can be improve
                        {
                            _Data[y][x].setRGBA(_Data[range.ally][range.endx+1]);//it's a bad method or a bug
                            _Data[y][x].setEdge(IBASE);//???
                        }
                    }
                    table[i][j] += where;
                }
            }
            else
            {
                line = table[len-1-i];
                linelen = line.size();
                for(int j = 0;j < linelen;++j)//每一行可能有多列，凹边界线时
                {
                    range = line[j];
                    if(moveY >= linelen)//由于移动的够远，所以不会与原来的边界重合
                        coverLine = 0;
                    else
                    {
                        coverLine = linelen - moveY;
                        newRane = line[j-moveY] + where;
                    }
                    for(x = range.sttx,y = range.ally;x <= range.endx;++x)
                    {
                        xx = REPAIR(x -_beginX + where.X(), _width);
                        yy = REPAIR(y -_beginY + where.Y(), _height);
                        _Data[yy][xx] = _Data[y][x];
                        if(!coverLine || !newRane.in(x))//this check can be improve
                        {
                            _Data[y][x].setRGBA(_Data[range.ally][range.endx+1]);//it's a bad method or a bug
                            _Data[y][x].setEdge(IBASE);//???
                        }
                    }
                    table[len-1-i][j] += where;
                }

            }
        }

    }
    else
    {
        return true;
    }
    //maybe need update position of the frames
    updateBoundary(boundarys,newpos,where);
    updateFrames(frames,pos,where);
    return pos;
}

bool     dpcCore::MultiMoveBoundary(int pos,dmvect wheres,bool isOverWrite/* = false*/)
{
    //int i = getNearFrame(frames,108,84);
    if(wheres.empty() || pos < 0)
        return false;
    size_t curMvect = 0;
    int rtPos = pos;
    while(curMvect < wheres.size())
    {
        checkCollision(frames,rtPos,wheres[curMvect]);
        rtPos = moveBoundary(rtPos,wheres[curMvect]);
        curMvect++;
    }
    return true;
}

bool     dpcCore::zoomBoundary(dPIXELS& boundary,int step,Method method/* = UR*/,bool isOverWrite/* = false*/)
{
    if(boundary.empty())
        return false;
    size_t len  = boundary.size();
    int x = 0,y = 0;
    int n = 0;
    if(!isOverWrite)
    {
        for (size_t i = 0; i < len; ++i)
        {
            y = boundary[i].getY()-_beginY;
            x = boundary[i].getX()-_beginX;
            Position prepos = boundary[i].getPostion();
            Position postpos = boundary[(i+1)].getPostion();
            //if(prepos == postpos)
            {
                while(++n <= step)
                {
                    switch (prepos)
                    {//use getRpoint
                    case Down://move Left
                        if(method)//method != UD
                            _Data[y][x-n] = _Data[y][x];
                        break;
                    case Left://move Up
                        if(method != LR)
                            _Data[y-n][x] = _Data[y][x];
                        break;
                    case Right://move Down
                        if(method != LR)
                            _Data[y+n][x] = _Data[y][x];
                        break;
                    case Up://move Right
                        if(method)//method != UD
                            _Data[y][x+n] = _Data[y][x];
                        break;
                    default:
                        break;
                    }
                }
                n = 0;
            }
            /*
            else
            {
                while(n++ <= step)
                {
                    switch (position)
                    {//use getRpoint
                    case Down://move Left
                        if(method)//method != UD
                            _Data[y-n][x-step] = _Data[y][x];
                        break;
                    case Left://move Up
                        if(method != LR)
                            _Data[y-step][x] = _Data[y][x];
                        break;
                    case Right://move Down
                        if(method != LR)
                            _Data[y+step][x-n] = _Data[y][x];
                        break;
                    case Up://move Right
                        if(method)//method != UD
                            _Data[y][x+step] = _Data[y][x];
                        break;
                    default:
                        break;
                    }
                }
            }*/
        }
    }
    else
    {
        for (size_t i = 0; i < len; ++i)
        {
            y = boundary[i].getY()-_beginY;
            x = boundary[i].getX()-_beginX;
        }
    }
    return true;
}

void     dpcCore::encBoundarys(const char* fpo, int ch, char* mode)
{
    if(NULL == fpo)
    {
        printf("file path is invalid!");
        return;
    }
    if(boundarys.empty())
        getBoundarys();
    std::cout << "character: " << ch << " boundary: " << std::endl;
    for(size_t i = 0; i < boundarys.size(); ++i)
    {
        for(size_t j = 0; j < boundarys[i].size(); ++j)
        {
            printf("$%2d: x: %2d; y: %2d\n", j, boundarys[i][j].getX(), boundarys[i][j].getY());
        }
    }
    iFonts  ifont(fpo, mode);
    ifont.encoder(ch, boundarys);
}

void     dpcCore::decBoundarys(const char* fpi, int ch, char* mode)
{
    if(NULL == fpi)
    {
        printf("file path is invalid!");
        return;
    }
    iFonts ifont;
    ifont.loader(fpi, mode);
    cfont c = ifont.decoder(ch);
    vdPIXELS vna;
    int ot = 0;
    c.decode(ot, vna, 12, 4);
    std::cout << c << std::endl;
    std::cout << "decode cfont ..." << std::endl;
    for(size_t i = 0; i < vna.size(); ++i)
    {
        for(size_t j = 0; j < vna[i].size(); ++j)
        {
            _Data[vna[i][j].getY()][vna[i][j].getX()].setRGBA(iColor::BLACK);
            printf("$%2d: x: %2d; y: %2d\n", j, vna[i][j].getX(), vna[i][j].getY());
        }
    }


}

bool     dpcCore::dealManager(int argc, char* argv[])
{
    if(!_Data)
    {
        printf("cannot deal with,there are not Data!\n");
        return false;
    }
    if(argc < 1)
    {
        printf("No option error!\n");
        return false;
    }
    OPt opt(argc, argv, "bhcLldpE:D:", (char*)doc());
    if(!opt.getOpt())
    {
        printf("deal with option error!\n");
        return false;
    }
    char dealType = 0;
    size_t i = 0;
    OPt::vvstr sop = opt.getOptSarry();
    while (i < sop.size())
    {
        dealType = sop[i][0][1];
        switch (dealType)
        {
        case 'b':
            cout << "  -b     backGround_ize   : get a image's part of backGround\n";
            //backGround_ize();
            break;
        case 'h':
            cout << "  -h     boundarysHL      : change boundarys line to HightLight\n";
            //boundarysHL(trackTables);
            boundarysHL();
            break;
        case 'c':
            cout << "  -c     imageCutOut      : cutout the effective part of the image\n";
            cutOut(trackTables);
            break;
        case 'l':
            cout << "  -l     locateFrame      : HightLight the every frames\n";
            locateFrame();
            break;
        case 'L':
            cout << "  -L     locateMove       : Move the part of image:like boundarysline or boundary\n";
            locateMove();
            //BrainRepair(_Data[48][66],_Data[88][96]);
            break;
        case 'd':
            cout << "  -d     boundaryer       : just draw boundary with drawline\n";
            getBoundarys();
            boundaryer(boundarys);
            //setLGranlarty(3);
            //boundaryer(boundaryFrame(boundarys));
            break;
        case 'p':
            cout << "  -p     boundary2p       : just draw boundary point\n";
            getBoundarys();
            //boundary2p(boundarys);
            //setLGranlarty(3);
            boundary2p(boundaryFrame(boundarys));
            break;
        case 'E':
            cout << "  -E     encBoundarys     : record boundarys's data to a file\n";
            encBoundarys(sop[i][1].c_str(), atoi(sop[i][2].c_str()));
            break;
        case 'D':
            cout << "  -D     decBoundarys   : encode file and get boundarys's data\n";
            decBoundarys(sop[i][1].c_str(), atoi(sop[i][2].c_str()));
            break;
        default:
            printf("Not deal with!\n");
            break;
        }
        i++;
    }
    //set data
    if(_dp)
    {
        _dp->setData(_Data);
        _dp->setWidth(_width);
        _dp->setHeight(_height);
    }
    return true;
}

const char*  dpcCore::doc()
{
    string doc =  string("Usage: \n") +
            "  -b  backGround_ize\t: get a image's part of backGround\n" +
            "  -h  boundarysHL\t: change boundarys line to HightLight\n" +
            "  -c  imageCutOut\t: cutout the effective part of the image\n" +
            "  -l  locateFrame\t: HightLight the every frames\n" +
            "  -L  locateMove\t: Move the part of image:like boundarysline or boundary\n" +
            "  -d  boundaryer\t: just draw boundary with drawline\n" +
            "  -p  boundary2p\t: just draw boundary point\n" +
            "  -E: (outpath,character,mode=wb)\trecBoundarys\t: record boundarys's data to a file\n" +
            "  -D: (inpath,character,mode=wb))\tdecBoundarys\t: encode file and get boundarys's data\n";
    return doc.c_str();
}

ppPIXELS dpcCore::MultiProces(int argc, char* argv[],ppPIXELS data,int deep)
{
    if(!data)
        return NULL;
    _Data = data;
    while(deep--)
    {
        dealManager(argc, argv);
        frames.clear();
        boundarys.clear();
        skipTables.clear();
        trackTables.clear();
    }
    return _Data;
}

ppPIXELS dpcCore::MultiProces(int argc, char* argv[],int deep)
{
    return MultiProces(argc, argv, _Data, deep);
}

}//namespace DPC
}//namespace Imaginer
