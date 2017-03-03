/*
 *  This is a template class, implement a stack structure, And provide the necessary pop push
 *  and other operations, does not support multi-threaded(thread not safe)
 *
 */
#ifndef ISTACK
#define ISTACK
#include <string.h>
namespace Imaginer{
namespace Utils {

template<typename Type>
class iStack
{
private:
    Type*   _data;
    int     _size;
    int     _top;
public:
    iStack(const int& size = 1024):_size(size),_top(-1)
    {
        _data = new Type[_size];
    }
    iStack(iStack<Type>& rhs)
    {
        if(this != &rhs)
        {
            _size = rhs._size;
            _data = new Type[_size];
            memcpy(_data,rhs._data,_size);
        }
    }
    ~iStack()
    {
        //(_data) ? (delete[] _data,_data = NULL) : (_data);
        if(_data)
        {
            delete[] _data;
            _data = NULL;
        }
    }
public:
    inline Type& top(){ return _data[_top];}
    inline Type& bottom() {return _data[0];}
    inline Type& pop(){ if(_top==-1) std::cout << "stack is empty" << std::endl;return _data[_top--];}
    inline void  push(const Type& value){ _data[++_top] = value;}
    inline bool  empty(){ return (_top == -1);}
    inline bool  full() { return (_top == _size);}
    inline int   size() {return _top;}
    inline void  clear(){/*memset(_data,0,1024);*/_top = -1;}
    inline void  destroy() {_top = -1,delete[] _data,_data = NULL;}
    inline bool  swap(int first,int second)
    {
        if(first < 0 || first > _top ||
                second < 0 || second > _top)
            return false;
        Type tmp = _data[first];
        _data[first] = _data[second];
        _data[second] = tmp;
        return true;
    }
    iStack<Type>& reverse()
    {
        Type tmp;
        int head = 0;
        int tail = _top;
        while(head < tail)
        {
            tmp = _data[head];
            _data[head] = _data[tail];
            _data[tail] = tmp;
            head++;
            tail--;
        }
        return *this;
    }
    iStack<Type>  copy(int start = 0,int end = 1024)
    {
        iStack<Type> rt;
        (end > size())?(end = size()+1):(end);
        int i = start;
        while(i < end)
        {
            rt.push(_data[i++]);
        }
        return rt;
    }
    iStack<Type>  cut(int start = 0,int end = 1024)
    {
        iStack<Type> rt;
        (end > size())?(end = size()+1):(end);
        int i = start;
        while(i < end)
        {
            rt.push(_data[i++]);
        }
        _data = _data+i;
        _top-=i;
        return rt;
    }
    void copy(iStack<Type>& rt,int start = 0,int end = 1024)
    {
        (end > size())?(end = size()+1):(end);
        int i = start;
        while(i < end)
        {
            rt.push(_data[i++]);
        }
        //return rt;
    }
    void cut(iStack<Type>& rt,int start = 0,int end = 1024)
    {
        (end > size())?(end = size()+1):(end);
        int i = start;
        while(i < end)
        {
            rt.push(_data[i++]);
        }
        _data = _data+i;
        _top-=i;
    }
public:
    iStack<Type>& operator=(const iStack<Type>& rhs)
    {
        if(this != &rhs)
        {
            _size = rhs._size;
            _top  = rhs._top;
            if(!_data)
            {
                _data = new Type[_size];
            }
            memcpy(_data,rhs._data,_size);
        }
        return *this;
    }
    inline  Type&  operator[](int index){ return ((index < 0)?(_data[_top+1+index]):(_data[index]));}
    friend  std::ostream& operator<<(std::ostream &os,iStack<Type>& stack)
    {
        int size = stack.size();
        int i = 0;
        while(i <= size)
        {
            os << stack[i++] << "  ";
        }
        return os;
    }
};

}//namespace Utils
}//namespace Imaginer
#endif // ISTACK

