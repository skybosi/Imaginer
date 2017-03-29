/*
 * This is a utils class for coordinate transformation,provide
 * some base Matrix operation
 * Vector is simple Matrix,but scalar is not
 */
#ifndef IMATRIX_H
#define IMATRIX_H
#include <iostream>
#include <typeinfo>
#include "iCommons.h"
#define random(x) (rand() % (x))
using namespace Imaginer::Commons;

namespace Imaginer {
namespace Utils {
typedef DaTp    MeT;//Matrix element Type
typedef MeT*    pMeT;//Matrix element Type pointer
typedef pMeT*   ppMeT;//Matrix element Type pointer's pointer

class MatrixException
{
public:
    MatrixException():_msg("MatrixException HaHa :(\n"){}
    MatrixException(std::string str):_msg(str){}
    MatrixException(const MatrixException& rhs):_msg(rhs._msg){}
    virtual ~ MatrixException() throw(){}
    std::string what()const{return _msg;}
private:
    std::string _msg;
};

class iMatrix
{
protected:
    ppMeT _Mvs;
    int   _rows;
    int   _cols;
public:    
    iMatrix(int rows,int cols);
    iMatrix(int rc);
    template<typename T>
    iMatrix(int rows,int cols,int size,T* values):_rows(rows),_cols(cols)
    {
        std::string type = typeid(T).name();
        //std::cout << typeid(T).name() << std::endl;
        if(type.find('_') != -1)//Must base type
            throw MatrixException("Type Not Match,Not Base Type");
        _Mvs = new pMeT[rows];
        //_Mvs = (ppMeT)calloc(rows, sizeof(pMeT));
        int sum = 0;
        for (int x = 0; x < rows; ++x)
        {
            _Mvs[x] = new MeT[cols];
            //_Mvs[x] = (pMeT)calloc(cols, sizeof(MeT));
            if(!_Mvs[x])
            {
               std::cout << "calloc " << __FUNCTION__;
            }
            for (int y = 0; y < cols; ++y,++sum)
            {
                if(sum < size)
                {
                    _Mvs[x][y] = *values++;
                }else
                {
                    _Mvs[x][y] = 0;
                }
            }
            //memset(_Mvs[x],n,cols*sizeof(MeT));
        }
    }
    template<typename T>
    iMatrix(int rc,int size,T* values):iMatrix(rc,rc,size,values){}
    iMatrix();
    iMatrix(const iMatrix& rhs);
    ~iMatrix();
public:
    inline pMeT& operator[](const int& r)const
    {
        return _Mvs[r];
    }
    inline MeT&  operator()(const int &r, const int & c)const
    {
        return _Mvs[r][c];
    }
    inline MeT&  operator()(const int &r, const int & c)
    {
        return _Mvs[r][c];
    }
    iMatrix&     operator=(const iMatrix &rhs);
    bool         operator==(const iMatrix &rhs);
    friend  std::ostream& operator<<(std::ostream &os,const iMatrix& rhs);
public:
    iMatrix  operator+(const iMatrix& rhs);
    iMatrix  operator-(const iMatrix& rhs);
    iMatrix  operator*(const iMatrix& rhs);
    iMatrix  operator*(const MeT& rhs);
    friend iMatrix operator *(const MeT& lhs,iMatrix& rhs);
public:
    iMatrix  transposit();
    MeT      det();
private:
    bool init(int rows,int cols,int seed = -1);
    bool dele(int rows);
};

}//namespace Utils
}//namespace Imaginer
#endif // IMATRIX_H
