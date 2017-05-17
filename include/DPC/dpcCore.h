/*
 * Data Processing Center Core >> DPCcore  (P is the most important)
 * This class provide some core Process method,It is the meaning and purpose of the project.
 * So this class is core ,Optimization and improvement are here!!!
 */
#ifndef DPCCORE_H
#define DPCCORE_H
#include "dataPcer.h"

namespace Imaginer
{
namespace DPC
{

class dpcCore :public dataPcer
{

/////////////////////////////part of dpcCore/////////////////////////////

struct limitXXY
{
    limitXXY():sttx(0),endx(0),ally(0){}
    limitXXY(const limitXXY& rhs){
        sttx = rhs.sttx;
        endx = rhs.endx;
        ally = rhs.ally;
    }
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
    bool in(int x)
    {
        return x >= sttx && x < endx;
    }
    bool lout(int x)
    {
        return x < sttx;
    }
    bool rout(int x)
    {
        return x > endx;
    }
    limitXXY& operator=(const limitXXY& rhs){
        sttx = rhs.sttx;
        endx = rhs.endx;
        ally = rhs.ally;
        return *this;
    }
    limitXXY operator+(const iVector& mv){
        limitXXY rt(*this);
        rt.sttx += mv.X();
        rt.endx += mv.X();
        rt.ally += mv.Y();
        return rt;
    }
    limitXXY& operator+=(const iVector& mv){
        sttx += mv.X();
        endx += mv.X();
        ally += mv.Y();
        return *this;
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
    FramePoint(){FramePoint(-1,-1);}
    FramePoint(int H,int W):bindNum(-1)
    {
        framePoint[0] = H;
        framePoint[1] = 0;
        framePoint[2] = W;
        framePoint[3] = 0;
    }
    FramePoint(const FramePoint& rh)
    {
        bindNum = rh.bindNum;
        framePoint[0] = rh.framePoint[0];
        framePoint[1] = rh.framePoint[1];
        framePoint[2] = rh.framePoint[2];
        framePoint[3] = rh.framePoint[3];
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
    //NOTE: this bindNum is first set,then readonly
    //It's use to bind a boundarys,even though the boundarys is move
    void setBindNum(int num){if(bindNum == -1)bindNum = num;}
    int getBindNum()const{ return bindNum;}
    FramePoint operator+(const iVector& mv){
        FramePoint rt(*this);
        rt.framePoint[0] += mv.Y();
        rt.framePoint[1] += mv.Y() ;
        rt.framePoint[2] += mv.X();
        rt.framePoint[3] += mv.X();
        return rt;
    }
    FramePoint operator+(const int& mvXorY){
        FramePoint rt(*this);
        rt.framePoint[0] += mvXorY;
        rt.framePoint[1] += mvXorY;
        rt.framePoint[2] += mvXorY;
        rt.framePoint[3] += mvXorY;
        return rt;
    }
    FramePoint& operator+=(const iVector& mv){
        framePoint[0] += mv.Y();
        framePoint[1] += mv.Y() ;
        framePoint[2] += mv.X();
        framePoint[3] += mv.X();
        return *this;
    }
    const FramePoint& operator=(const FramePoint& rhs){
        framePoint[0] = rhs[0];
        framePoint[1] = rhs[1];
        framePoint[2] = rhs[2];
        framePoint[3] = rhs[3];
        bindNum       = rhs.bindNum;
        return *this;
    }
    bool operator>(const FramePoint& rhs){
        if(framePoint[Up] > rhs[Up])
            return true;
        else if(framePoint[Up] == rhs[Up] && framePoint[Left] > rhs[Left])
            return true;
        else return false;
    }
    bool operator<(const FramePoint& rhs){
        if(framePoint[Up] < rhs[Up])
            return true;
        else if(framePoint[Up] == rhs[Up] && framePoint[Left] < rhs[Left])
            return true;
        else return false;
    }
    bool operator==(const FramePoint& rhs){
        return  framePoint[0] == rhs[0] &&
                framePoint[1] == rhs[1] &&
                framePoint[2] == rhs[2] &&
                framePoint[3] == rhs[3];
    }
    bool operator!=(const FramePoint& rhs){
        return  framePoint[0] != rhs[0] ||
                framePoint[1] != rhs[1] ||
                framePoint[2] != rhs[2] ||
                framePoint[3] != rhs[3];
    }
    bool checkPosition(iVector where,const FramePoint& Curr)
    {
        int x = where.X();
        int y = where.Y();
        int CurrX = 0;
        int CurrY = 0;
        //由于移动的方向确定了，以被移动的一个角点为原点，来判断被检测碰撞的框子在移动的向量的那一
        //侧与该角点坐标被忽略(其实已经被减去)，及可直接由移动向量的坐标(x,y),具体原理详见（《关于边界移动时碰撞的原理讨论》）
        switch(where.getFofA())
        {
        case RU://(Curr[Left]-this[Right],Curr[Down]-this[Up]) * (x,y)
            //return (((Curr[Left]-framePoint[Right])*y-
            //         (Curr[Down]-framePoint[Up])*x) >= 0);
            CurrX = Curr[Left]-framePoint[Right];
            CurrY = Curr[Down]-framePoint[Up];
            break;
        case LU://(Curr[Right]-this[Left],Curr[Down]-this[Up]) * (x,y)
            //return (((Curr[Right]-framePoint[Left])*y+
            //        (Curr[Down]-framePoint[Up])*x) >=0);
            CurrX = Curr[Right]-framePoint[Left];
            CurrY = Curr[Down]-framePoint[Up];
            break;
        case LD://(Curr[Right]-this[Left],Curr[Up]-this[Down]) * (x,y)
            //return (((Curr[Right]-framePoint[Left])*y-
            //         (Curr[Up]-framePoint[Down])*x)>=0);
            CurrX = Curr[Right]-framePoint[Left];
            CurrY = Curr[Up]-framePoint[Down];
            break;
        case RD://(Curr[Left]-this[Right],Curr[Up]-this[Down]) * (x,y)
            //return (((Curr[Left]-framePoint[Right])*y+
            //         (Curr[Up]-framePoint[Down])*x) >=0);
            CurrX = Curr[Left]-framePoint[Right];
            CurrY = Curr[Up]-framePoint[Down];
            break;
        default:
            return false;
            break;
        }
        return ((CurrX*y-CurrY*x)<=0);
    }
    bool in(int x,int y){ return ((y <= framePoint[Down]  && y >= framePoint[Up]) &&
                                  (x <= framePoint[Right] && x >= framePoint[Left])); }
    //a line is cross the FramePoint or not:line in
    bool lin(int xORy,int index){ return ((index > Down) ?
                                  (xORy >= framePoint[Left] && xORy <= framePoint[Right]) :
                                  (xORy >= framePoint[Up]   && xORy <= framePoint[Down])); }
    //check this FramePoint whether out of image after move
    FofA out(const int& width,const int& height){
        if(framePoint[Up] < 0)
        {
            if(framePoint[Left] < 0)
            {
                return LU;
            }
            if(framePoint[Right] > width-1)
            {
                return RU;
            }
            return U;
        }
        if(framePoint[Left] < 0)
        {
            if(framePoint[Down] > height-1)
            {
                return LD;
            }
            if(framePoint[Up] < 0)
            {
                return LU;
            }
            return L;
        }
        if(framePoint[Down] > height-1)
        {
            if(framePoint[Right] > width-1)
            {
                return RD;
            }
            if(framePoint[Left] < 0)
            {
                return LD;
            }
            return D;
        }
        if(framePoint[Right] > width-1)
        {
            if(framePoint[Up] < 0)
            {
                return RU;
            }
            if(framePoint[Down] > height-1)
            {
                return RD;
            }
            return R;
        }
        return NO;
    }
    void checkout(iVector& where,int width,int height)
    {
        FofA fofa = (*this+where).out(width,height);
        switch(fofa)
        {
        case U:
            where.Y() = 1-framePoint[Up];
            break;
        case D:
            where.Y() = height-2-framePoint[Down];
            break;
        case L:
            where.X() = 1-framePoint[Left] ;
            break;
        case R:
            where.X() = width-2-framePoint[Right] ;
            break;
        case RU:
            where.X() = width-2-framePoint[Right] ;
            where.Y() = 1-framePoint[Up] ;
            break;
        case LU:
            where.X() = 1-framePoint[Left] ;
            where.Y() = 1-framePoint[Up] ;
            break;
        case LD:
            where.X() = 1-framePoint[Left] ;
            where.Y() = height-2-framePoint[Down];
            break;
        case RD:
            where.X() = width-2-framePoint[Right] ;
            where.Y() = height-2-framePoint[Down];
            break;
        default:
            break;
        }
    }
    void getCenter(int& x,int& y)
    {
        x = (framePoint[Right] + framePoint[Left]) >> 1;
        y = (framePoint[Down] + framePoint[Up]) >> 1;
    }
    private:
    int framePoint[4];
    int bindNum;
};

//this position is first relation to second,but NOTE IN2
enum PRs// position relation
{
    IN,   //first in the second
    IN2,  //second in the first NOTE!
    CROSS,//include inscribe
    HERE, //Circumscribe
    NEAR, //>0 && <5 //5 is a example,but it's a small number
    OUT,
    EVER
};

typedef deque<FramePoint>  Frames;
typedef deque<PIXELS>      dSkipLine;
typedef vector<dSkipLine>  SkipTable;
typedef vector<SkipTable>  vSkipTable;

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


/////////////////////////////part of dpcCore/////////////////////////////
friend class dpcCons;

public:
    dpcCore(ppPIXELS& data,int& width,int& height):
        dataPcer(data,width,height),granularity(10),granOpeartor(true),baseSmlrty(1.0),testRange(2),LGranlarty(3){}
    dpcCore(DperMum* dp):
        dataPcer(dp),granularity(10),granOpeartor(true),baseSmlrty(1.0),testRange(2),LGranlarty(3){}
public:
    /**
     * @brief dealManager
     *        all deal way open API
     * @param dealType  ctrlon deal way
     * @return
     */
    virtual bool dealManager(int argc, char* argv[]);
    const char*  doc();
    /**
     * @brief MultiProces
     *        Multiple processing use one or more processing way
     * @param data
     * @param dealType
     * @param deep
     * @return
     */
    ppPIXELS MultiProces(int argc, char* argv[],ppPIXELS data,int deep = 2);
    ppPIXELS MultiProces(int argc, char* argv[],int deep = 2);
    /**
     * @brief autoMove
     * @param x
     * @param y
     * @param mx
     * @param my
     * @return
     */
    bool     autoMove(int x,int y,int mx,int my);
private:
    iColor      backGround;
    vdPIXELS    boundarys; //record all boundary line
    vSkipTable  skipTables;
    vTrackTable trackTables;//用于抠出轨迹内的部分
    //record the trackdown's result,just for cutout the image
    Frames      frames;    //record all boundaryline's frame Point
    U32         granularity;//图像碎片边缘最少像素
    bool        granOpeartor;//contrl the granularity opeartor method
    //granOpeartor: boundarys will save only largger than granularity value's boundaryline :true
    //              boundarys will save only smaller than granularity value's boundaryline :false
    float       baseSmlrty;//base Similarity,use to judge is boundary point or not
    U8          testRange;//for use to set the test Range,that can sure a Edge Point is not trackdown again
    int         LGranlarty;//linear Granularity,Flag for measuring the degree of linearity
private://core function
    /**
     * @brief getBoundarys
     *        Gets all of the border(boundary) line
     */
    void   getBoundarys();
    /**
     * @brief getBoundaryLine
     *        Gets the border(boundary) line
     * @param x start point x
     * @param y start point y
     * @return  if get Boundary return ture else return false
     */
    bool   getBoundaryLine(int& x, int& y);
    /**
     * @brief isCloseOpen
     *        is a close/open boundary line or not
     * @param boundaryline
     * @return close:true open:false
     */
    bool   isCloseOpen(dPIXELS boundaryline);
    /**
     * @brief trackDown
     *        from a start point will get next util image Edege or start point again
     * @param startPoint
     * @return next point's x value
     */
    int    trackDown(PIXELS& startPoint);
    /**
     * @brief deburrTrack
     *        delete The Burr on the track,so that make boundaryline is smooth
     * @param boundaryline
     * @return
     */
    bool   deburrTrack(dPIXELS& boundaryline);
    /**
     * @brief deburrTrack
     *        Remove small fluctuations in the boundary line, so that the characteristics
     *        of the boundary line is more obvious, (Make the framework use LGranlarty)
     * @param boundaryline
     * @return
     */
    dPIXELS  deburrTrack(dPIXELS& boundaryline,int lgranlarty);
    /**
     * @brief boundaryFrame
     *        combine near pixel with LGranlarty at all boundary
     * @param bondry
     * @return
     */
    vdPIXELS boundaryFrame(vdPIXELS& bondry);
    /**
     * @brief boundarysHL
     *        Boundary  highlight
     * @return
     */
    bool   boundarysHL();
    /**
     * @brief boundarysHL
     *        Boundary  highlight with vTrackTable
     * @param Tables,will call function: boundarylineHL
     * @return
     */
    bool   boundarysHL(vTrackTable& Tables);
    /**
     * @brief boundarylineHL
     *         Boundary  highlight with a TrackTable of vTrackTable
     * @param table
     * @return
     */
    bool   boundarylineHL(TrackTable& table);
    /**
     * @brief boundaryer
     *        After clear iamge, then just draw boundarys
     * @param bondry
     * @return
     */
    bool   boundaryer(const vdPIXELS& bondry);
    /**
     * @brief boundary2p
     *        After clear iamge, then just draw boundarys point
     * @param bondry
     * @return
     */
    bool   boundary2p(const vdPIXELS& bondry);
    /**
     * @brief ImageHL
     *        highlight all point after getBoundarys()
     * @return
     */
    bool   ImageHL();
    /**
     * @brief cutOut
     *        cut out part of Boundary with cut point(skip table) vTrackTable
     * @param Tables
     * @return
     */
    bool   cutOut(vTrackTable& Tables);
    /**
     * @brief cutAOut
     *        cut out part of Boundary with cut point(skip table) with a TrackTable
     * @param imageData
     * @param table
     * @param FP        use to contrl the Boundary
     * @return
     */
    bool   cutAOut(ppPIXELS& imageData,TrackTable& table,FramePoint& FP);
    /**
     * @brief inTrackLine
     *        use to check a point if in the vTrackLine or not
     * @param line
     * @param x
     * @return in: true out :false
     */
    bool   inTrackLine(vTrackLine line,int x);
	/**
	 *
     * @brief recBoundarys
     *        record the boundarys's datas to a file, Especially record the meaningful symbol,
	 *        example Languages, the first test is to do this.
     * @param ch    character
     * @param mode  open file mode "r/w b" default "wb+"
	 *
	 */
	void   recBoundarys(const char* fpo, int ch, char* mode = "wb+");
	/**
	 *
     * @brief encodeBoundarys
     *        encode the file record the boundarys's datas, and save to the Structure Boundary,
	 *        And draw on the picture
     * @param ch    character
     * @param mode  open file mode "r/w b", default "rb"
	 *
	 */
	void   decBoundarys(const char* fpi, int ch, char* mode = "rb");
private://core function get message
    /**
     * @brief setBackground
     *        set BackGround with RGBQUAD
     * @param rgb
     * @return
     */
    bool   setBackground(iColor rgb);
    /**
     * @brief setBackground
     *        set BackGround with R,G,B,maybe add alpha
     * @param r
     * @param g
     * @param b
     * @return
     */
    bool   setBackground(U8 r = 255,U8 g = 255,U8 b = 255);
    /**
     * @brief setBackground
     *        set BackGround with PIXELS,will trackDown(),is very inportant
     * @param pixel
     * @return
     */
    bool   setBackground(const PIXELS& pixel);
    /**
     * @brief setGranularity
     *        set image granularity,will use to contrl the boundary line is vaild or not
     * @param gran
     * @param opeartor
     */
    void   setGranularity(U32 gran,bool opeartor=false);
    /**
     * @brief setLGranlarty
     *        linear Granularity,Flag for measuring the degree of linearity,unit: Number of pixels
     * @param lgranlarty
     */
    void   setLGranlarty(int lgranlarty);
    /**
     * @brief alikeBackground
     *        alike background or not,Mean and the same color as
     *        the background color
     *        NOTE: But not necessarily background
     * @param pixel
     * @return -1 is empty pixel, 1 alike background,0 not alike background
     */
    int    alikeBackground(PIXELS pixel);
    /**
     * @brief alikeBackground
     *        just like up function,x,y just to get this PIXELS,maybe add z
     * @param x
     * @param y
     * @return
     */
    int    alikeBackground(int x,int y);
    /**
     * @brief getSimilarity
     *       test two border upon point similarity
     *       NOTE: is very important
     * @param direction contrl test direction,default:Right
     * @param x
     * @param y
     * @param step      contrl next point step,default:1
     * @return
     */
    float  getSimilarity(Position direction,int x,int y,int step = 1);
    /**
     * @brief getSimilarity
     *        test curr point with background point's similarity
     * @param backPoint next point
     * @param currPoint current point
     * @return Similar: true else false
     */
    float  getSimilarity(PIXELS backPoint, PIXELS currPoint);
    /**
     * @brief getRpoint
     *        just want to get the right point of one point,if get x,y will be reset
     *        NOTE: this is very import function to get boundary line
     * @param direction will be reset to Next direction
     * @param x
     * @param y
     * @return get next point:true, out of Edge false
     */
    bool   getRpoint(Position& direction,int& x,int& y);
    /**
     * @brief getLpoint
     *        the same as getRpoint,just left
     * @param direction
     * @param x
     * @param y
     * @return
     */
    bool   getLpoint(Position& direction,int& x,int& y);
    /**
     * @brief getNext
     *        Used to get around a point,you can read 关于周边点的思考.bmp
     * @param pos
     * @param x
     * @param y
     * @param nexts
     * @param step
     */
    void   getNext(Position& pos, int &x,int& y,int& nexts,int& step);
    /**
     * @brief Bsearch
     *        is a utils for getNext
     * @param line
     * @param indexX
     * @return
     */
    int    Bsearch(dSkipLine& line,int indexX);
    /**
     * @brief genSkipTable
     *        while get a boundary point will add to Skip Table,if can
     * @param x
     * @param y
     * @param skipTable2
     */
    void   genSkipTable(int x,int y,SkipTable& skipTable2);
    /**
     * @brief genSkipTable
     *        the same as up
     * @param pixel
     * @param skipTable2
     */
    void   genSkipTable(const PIXELS& pixel,SkipTable& skipTable2);
    /**
     * @brief genTrackTable
     *        after get a boundary line and genSkipTable,will to generation a Track Table
     * @param skipTable2
     * @param tracktable
     */
    void   genTrackTable(SkipTable& skipTable2,TrackTable& tracktable);
    /**
     * @brief isBoundaryPoint
     * @param pot
     * @return
     */
    bool   isBoundaryPoint(PIXELS pot);
    /**
     * @brief isBoundaryPoint
     *        check a point is a Boundary Point or not
     * @param x
     * @param y
     * @return
     */
    bool   isBoundaryPoint(int& x,int& y);
    /**
     * @brief linker
     *        link each Boundary Line
     * @param frame
     */
    void   linker(const Frames& frame);
    /**
     * @brief testStartP
     *        Test whether around the start point has been visited
     * @param pixel
     * @return
     */
    //bool testStartP(PIXELS& pixel,int range = 2);
    bool   testStartP(PIXELS& pixel);
    /**
     * @brief getPix
     *        get a point RGB value from the (x,y)
     *        if the x,y is out range ,return empty PIXELS
     *        get PIXELS with a point(x,y),maybe add z
     * @param x
     * @param y
     * @return
     */
    PIXELS getPix(int x,int y)throw(IException);
private://core function dealwith message
    /**
     * @brief checkPR
     *        test two Frame postion relation
     *        this position is first relation to second,but NOTE IN2
     * @param A
     * @param B
     * @return
     */
    PRs     checkPR(const FramePoint& A,const FramePoint& B);
    /**
     * @brief genMultiMvect
     * @param pixels
     * @return
     */
    dmvect  genMultiMvect(dPIXELS pixels);
    /**
     * @brief genMultiMvect
     * @param pixels
     * @return
     */
    dmvect  genMultiMvect(dPoint pixels);
    /**
     * @brief testMultiMvect
     * @return
     */
    dmvect  testMultiMvect();
    /**
     * @brief checkCollision
     *        check collision algorithm;
     * @param fms
     * @param pos
     * @param where
     * @param interval
     * @return
     */
    bool    checkCollision(Frames& fms,int pos,iVector& where,int interval = 1);
    /**
     * @brief updateFrames
     *        after move a boundary ,the Frames must update
     *        NOTE: just update the Position in the Frames,not modify the bindNum
     * @param fms
     * @param pos
     * @param where
     * @return
     */
    bool    updateFrames(Frames& fms,int& pos,iVector& where);
    /**
     * @brief updateBoundary
     *        after move a boundary ,the new Boundary must update
     * @param bondry
     * @param pos
     * @param where
     * @return
     */
    bool    updateBoundary(vdPIXELS& bondry,int pos,iVector& where);
    /**
     * @brief getNearFrame
     *        get a point's near Frame
     * @param fms
     * @param x
     * @param y
     * @return
     */
    int     getNearFrame(Frames& fms,int x,int y)const;
    /**
     * @brief locateFrame
     *        locate a boundary and highlight the Frame
     * @return
     */
    bool    locateFrame();
    /**
     * @brief locateMove
     * @return
     */
    bool    locateMove();
    /**
     * @brief BrainRepair
     * @param A
     * @param B
     * @return
     */
    bool    BrainRepair(PIXELS& A,PIXELS& B);
    /**
     * @brief BrainRepair
     * @param A
     * @param VorH
     * @param range
     * @return
     */
    bool    BrainRepair(PIXELS& A,bool VorH,float range = 1.0);
    /**
     * @brief moveLine
     *        move a line ,line data from dPIXELS line
     * @param line
     * @param where    move vector
     * @param isOverWrite
     * @return
     */
    bool    moveLine(dPIXELS& line,iVector where,bool isOverWrite = false);
    /**
     * @brief moveLine
     *        just move a straight line
     * @param sx
     * @param xy
     * @param ex
     * @param ey
     * @param where
     * @param isOverWrite
     * @return
     */
    bool    moveLine(int sx,int xy,int ex,int ey,iVector where,bool isOverWrite = false);
    /**
     * @brief moveBoundary
     *        MoveBoundary with a iVector
     *        NOTE: must updateFrames and updateBoundary
     * @param pos
     * @param where
     * @param isOverWrite
     * @return
     */
    int     moveBoundary(int pos,iVector where,bool isOverWrite = false);
    /**
     * @brief MultiMoveBoundary
     *        Multi MoveBoundary with a series of iVector
     * @param pos
     * @param wheres
     * @param isOverWrite
     * @return
     */
    bool    MultiMoveBoundary(int pos,dmvect wheres,bool isOverWrite = false);
    /**
     * @brief zoomBoundary
     *        zoom boundary
     * @param boundry
     * @param step
     * @param method
     * @param isOverWrite
     * @return
     */
    bool    zoomBoundary(dPIXELS& boundry,int step,Method method = UR,bool isOverWrite = false);
private://utils
    //just draw a line
    /**
     * @brief drawLine
     *        draw a line with start poinr and slope
     * @param x
     * @param y
     * @param delta
     * @param length
     * @param size  line width
     * @param rgb   line color
     * @return
     */
    bool    drawLine(int x,int y,double delta,double length,iColor rgb,int size = 1)throw(IException);
    //void    drawLine(int startX, int startY, int endX,int endY,int size = 1,RGBQUAD* rgb = NULL)throw(IException);
    /**
     * @brief drawLine
     *        draw a line with start poinr and end point
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param size
     * @param rgb
     */
    void    drawLine(int x1, int y1, int x2, int y2, iColor rgb,int size = 1) throw(IException);
    void    drawLine(const iPoint& start,const iPoint& end,int rgb,int size/* = 1*/) throw(IException);
    /**
     * @brief gradLine
     *        draw a line with start poinr and end point, with add avg rgb,gradient color
     * @param startX
     * @param startY
     * @param endX
     * @param endY
     * @param avgrgb line color
     * @param size   line width
     */
    void    gradLine(int startX, int startY, int endX,int endY,iColor avgrgb,int size = 1)throw(IException);
    //just draw Horizontal and Vertical Rect.
    /**
     * @brief drawRect
     *        draw a rectange with a point and width/height
     * @param x
     * @param y
     * @param width
     * @param height
     * @param size  line width
     * @return
     */
    bool    drawRect(int x,int y,int width,int height,iColor rgb,int size = 1);
    //just draw Horizontal and Vertical Rect.
    /**
     * @brief drawRect
     *        draw a rectange with FramePoint
     * @param FP
     * @param size line width
     * @param rgb  line color
     * @return
     */
    bool    drawRect(const FramePoint& FP ,iColor rgb,int size = 1);
    /**
     * @brief near
     *        test two PIXELS is near with LGranlarty or not
     * @param A
     * @param B
     * @param lgranlarty
     * @return <= LGranlarty true;else false
     */
    bool    near(PIXELS& A,PIXELS& B,int lgranlarty)const;
};

}//namespace DPC
}//namespace Imaginer

#endif // DPCCORE_H
