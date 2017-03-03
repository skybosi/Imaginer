#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "iMatrix.h"
namespace Imaginer {
namespace Utils {

iMatrix::iMatrix(int rows,int cols):_rows(rows),_cols(cols)
{
    init(_rows,_cols);
}

iMatrix::iMatrix(int rc):_rows(rc),_cols(rc)
{
    init(_rows,_cols);
}

/* //error
template<typename T>
iMatrix::iMatrix(int rows,int cols,T values):_rows(rows),_cols(cols)
{
    std::string type = typeid(T).name();
    std::cout << typeid(T).name() << std::endl;
    if(type.size() > 1)
        throw MatrixException("Type Not Match");
    _Mvs = new pMeT[rows];
    //_Mvs = (ppMeT)calloc(rows, sizeof(pMeT));
    for (int x = 0; x < rows; ++x)
    {
        _Mvs[x] = new MeT[cols];
        //_Mvs[x] = (pMeT)calloc(cols, sizeof(MeT));
        if(!_Mvs[x])
        {
            std::cout << "calloc " << __FUNCTION__;
        }
        for (int y = 0; y < cols; ++y)
        {
            _Mvs[x][y] = *values++;
        }
        //memset(_Mvs[x],n,cols*sizeof(MeT));
    }
}
*/

iMatrix::iMatrix():_rows(3),_cols(3)
{
    init(3,3,(int)time(NULL));
}

iMatrix::iMatrix(const iMatrix& rhs):_rows(rhs._rows),_cols(rhs._cols)
{
    init(_rows,_cols);
    for(int x = 0;x < _rows;++x)
    {
        for(int y = 0;y < _cols;++y)
        {
            //memcpy(_Mvs,rhs._Mvs,_rows*_cols*sizeof(MeT));
            _Mvs[x][y] = rhs._Mvs[x][y];
        }
    }
}

iMatrix::~iMatrix()
{
    dele(_rows);
}

bool iMatrix::init(int rows,int cols,int seed)
{
    _Mvs = new pMeT[rows];
    //_Mvs = (ppMeT)calloc(rows, sizeof(pMeT));
    for (int x = 0; x < rows; ++x)
    {
        _Mvs[x] = new MeT[cols];
        //_Mvs[x] = (pMeT)calloc(cols, sizeof(MeT));
        if(!_Mvs[x])
        {
           std::cout << "calloc " << __FUNCTION__;
           return false;
        }
        for (int y = 0; y < cols; ++y)
        {
            if(seed == -1)
                _Mvs[x][y] = 0;
            else
                _Mvs[x][y] = random(seed)%100;
        }
        //memset(_Mvs[x],n,cols*sizeof(MeT));
    }
    return true;
}

bool iMatrix::dele(int rows)
{
    if (_Mvs && rows > 0)
    {
        for (int i = 0; i < rows; i++)
        {
            delete []_Mvs[i];
            //free(_Mvs[i]);
        }
        delete []_Mvs;
        //free(_Mvs);
        _Mvs = NULL;
    }
    else
    {
        return false;
    }
    return true;
}

iMatrix& iMatrix::operator=(const iMatrix &rhs)
{
    if(rhs._rows != _rows || rhs._cols != _cols)
        throw MatrixException("operator =: size not Match\n");
    if (this != &rhs)
    {
        init(rhs._rows,rhs._cols);
        for(int x = 0;x < rhs._rows;++x)
        {
            for(int y = 0;y <rhs._cols;++y)
            {
                _Mvs[x][y] = rhs._Mvs[x][y];
            }
        }
        //memcpy(_Mvs,rhs._Mvs,rhs._rows*rhs._cols);
    }
    return *this;
}

bool     iMatrix::operator==(const iMatrix &rhs)
{
    if(rhs._rows != _rows || rhs._cols != _cols)
        return false;
    if(this == &rhs)
        return true;
    for(int x = 0;x < rhs._rows;++x)
    {
        for(int y = 0;y <rhs._cols;++y)
        {
            if(_Mvs[x][y] != rhs._Mvs[x][y])
                return false;
        }
    }
    return true;
}

std::ostream & operator<<(std::ostream &os,const iMatrix& rhs)
{
    for(int x = 0;x < rhs._rows;++x)
    {
        for(int y = 0;y <rhs._cols;++y)
        {
            os << rhs._Mvs[x][y] << "\t";
        }
        os << std::endl;
    }
    return os;
}

iMatrix  iMatrix::operator+(const iMatrix& rhs)
{
    if(rhs._rows != _rows || rhs._cols != _cols)
        throw MatrixException("operator +: size not Match\n");
    iMatrix rt(_rows,_cols);
    for(int x = 0;x < rhs._rows;++x)
    {
        for(int y = 0;y <rhs._cols;++y)
        {
            rt._Mvs[x][y] = _Mvs[x][y] + rhs._Mvs[x][y];
        }
    }
    return rt;
}

iMatrix  iMatrix::operator-(const iMatrix& rhs)
{
    if(rhs._rows != _rows || rhs._cols != _cols)
        throw MatrixException("operator -: size not Match\n");
    iMatrix rt(_rows,_cols);
    for(int x = 0;x < rhs._rows;++x)
    {
        for(int y = 0;y <rhs._cols;++y)
        {
            rt._Mvs[x][y] = _Mvs[x][y] - rhs._Mvs[x][y];
        }
    }
    return rt;
}

iMatrix  iMatrix::operator*(const iMatrix& rhs)
{
    if(_cols != rhs._rows)
        throw MatrixException("operator *: size not Match\n");
    iMatrix rt(_rows,rhs._cols);
    for(int i = 0; i < _rows; ++i)
    {
        for(int k = 0; k < rhs._cols; ++k)
        {
            for(int j = 0; j < rhs._rows; ++j)
            {
                rt._Mvs[i][k] += _Mvs[i][j] * rhs._Mvs[j][k];
            }
        }
    }
    return rt;
}

iMatrix  iMatrix::operator*(const MeT& rhs)
{
    iMatrix rt(*this);
    for(int i = 0; i < _rows; ++i)
    {
        for(int j = 0; j < _cols; ++j)
        {
            rt._Mvs[i][j] *= rhs;
        }
    }
    return rt;
}

iMatrix operator *(const MeT& lhs,iMatrix& rhs)
{
    return rhs.operator*(lhs);
}

iMatrix  iMatrix::transposit()
{
    iMatrix rt(_cols,_rows);
    for(int x = 0;x < _rows;++x)
    {
        for(int y = 0;y <_cols;++y)
        {
            rt._Mvs[y][x] = _Mvs[x][y];
        }
    }
    return rt;
}

MeT      iMatrix::det()
{
    if(_cols != _rows)
        throw MatrixException("operator det: row and col not Match\n");
    return 0;
}

}//namespace Utils
}//namespace Imaginer
