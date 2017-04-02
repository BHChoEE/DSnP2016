#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

using namespace std;

class Row
{
public:
   const int operator[] (size_t i) const 
   {return _data[i]; } // TODO
   int& operator[] (size_t i)
   {return _data[i]; } // TODO
    int getColumn()
    {return column;}

    Row(int);//constructor

private:
   int  *_data;
   int column;

};

class Table
{
public:
   const vector<Row>& operator[] (size_t i) const;
   vector<Row>& operator[] (size_t i);

   bool read(const string&);
   void PRINT();
   int SUM(int);
   double AVE(int);
   int MAX(int);
   int MIN(int);
   int COUNT(int);
   void ADD(string);
private:
   vector<Row>  _rows;
};


#endif // P2_TABLE_H
