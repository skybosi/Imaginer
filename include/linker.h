#ifndef _LINKER_H_
#define _LINKER_H_
#include <iostream>;
#include <map>;
//#include "Rbmp.h"
template <typename ElementypeA,typename ElementypeB>;
class linker
{
  public:
    typedef std::pair<std::string,float>; LinkType;
    typedef std::map<std::string,float>; LinkBridge;
    typedef LinkBridge::iterator LinkBridgeOne;
    //map<string LinkBridge_type,float linkDegree>;;
    //LinkBridge type : from a thing create the link,it's the name of the thing
    //linkDegree      : 0 ->; 1,link Degree more and more strong
  public:
    linker();
    ~linker();
    //create a linker
    const linker& create(LinkType a_b)
    {
      A_B.insert(a_b);
    }
    //test have a link or not
    const LinkType isLink()
    {
      if(A_B.empty())
        return LinkType("NOLINK",0);
      //TODO : want return link Degree strongest pair
      return A_B.end();
    }
  protected:
    //set the link degree
    bool setLinkDegree(LinkType a_b,float degree)
    {
      LinkBridgeOne linkOne = A_B.find(a_b.first);
      if(linkOne != A_B.end())
      {
        linkOne->;second = degree;
      }
      else
      {
        return false;
      }
    }

  private:
    //ElementypeA and ElementypeB is the same or not
    //same : create a link from a LinkBridge_type
    //Not  : forse create two difference Element link
    ElementypeA linkElementA;
    ElementypeB linkElementB;
    //link bridge: create A - B link from this or those
    LinkBridge A_B;
};



#endif //linker.h