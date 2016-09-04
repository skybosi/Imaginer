#ifndef BASE
#define BASE
#include <vector>
struct limitXXY
{
	limitXXY():sttx(0),endx(0),ally(0){}
	bool change(){return (sttx != endx); }
	bool add(PIXELS begin,PIXELS end,vector<limitXXY>& vtrackLine)
	{
		if(begin.getY() != end.getY())
			return false;
		sttx = begin.getX();
		endx = end.getX();
		ally = begin.getY();
		vtrackLine.push_back(*this);
		return true;
	}
	void show()
	{
		printf("ally:%d\tsttx:%d\tendx:%d\n",ally,sttx,endx);
	}
	int sttx; //start point x
	int endx; //end point x
	int ally; //communal y
};
typedef vector<limitXXY>   vTrackLine;
typedef vector<vTrackLine> TrackTable;
typedef vector<TrackTable> vTrackTable;

struct FramePoint
{
	FramePoint(int H,int W):bindNum(-1)
	{
		framePoint[0] = H;
		framePoint[1] = 0;
		framePoint[2] = W;
		framePoint[3] = 0;
	}
	bool setframePoint(Position index,const PIXELS& xORy)
	{
		if(index < Up || index > Right)
			return false;
		switch(index)
		{
			case Up:
				framePoint[Up] = std::min(framePoint[Up],xORy.getY());
				break;
			case Down:
				framePoint[Down] = std::max(framePoint[Down],xORy.getY());
				break;
			case Left:
				framePoint[Left] = std::min(framePoint[Left],xORy.getX());
				break;
			case Right:
				framePoint[Right] = std::max(framePoint[Right],xORy.getX());
				break;
			default:
				break;
		}
		return true;
	}
	int operator[](int index)const
	{
		if(index < Up || index > Right)
			return -1;
		else
			return framePoint[index];
	}
	void setBindNum(int num){bindNum = num;}
	int getBindNum()const{ return bindNum;}
	private:
	int framePoint[4];
	int bindNum;
};
typedef vector<FramePoint> Frames;

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
#define ISV(_var) (_var <= Down)   //vertically
#define ISH(_var) (_var >= Left)//  horizontally
#define SETSKIP(_pot)     (ISV(_pot.getpPos().first) ? (_pot.setEdge(-3)) : (_pot.setEdge(-1)))
#define SETPREV(_v,_prev) (_prev.setpPos(_v),(ISV(_v) ? SETSKIP(_prev) : _prev.setEdge(-2)))
#define SETCURR(_v,_curr) (_curr.setpPos(_v),(ISV(_v) ? SETSKIP(_curr) : _curr.setEdge(-2)))


/*
#define SETPREV(_v,_prev) (_prev.setpPos(_v),(ISV(_v) ? _prev.setEdge(-1) : _prev.setEdge(-2)))
#define SETCURR(_v,_curr) (_curr.setpPos(_v),(ISV(_v) ? _curr.setEdge(-1) : _curr.setEdge(-2)))
*/
#define TMAX(_v1,_v2,_v3) (_v1 > _v2) ? ((_v1 > _v3) ? (_v1) : (_v3)) : ((_v2 > _v3) ? (_v2) : (_v3))
#define TMIN(_v1,_v2,_v3) (_v1 < _v2) ? ((_v1 < _v3) ? (_v1) : (_v3)) : ((_v2 < _v3) ? (_v2) : (_v3))
static int globalI = 0;
#endif // BASE

