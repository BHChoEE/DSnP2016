/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"
#include <sstream>

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(size_t i = 0;i<r.size();i++)
   {
        DBTable::printData(os,r[i]);
	os<<' ';
   }
   os<<'\b';
    return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   
    for(size_t i = 0; i <t.nRows(); i++)
    {
        for(size_t j= 0; j <t.nCols(); j++)
        {
            os<< setw(6)<<right;
	    DBTable::printData(os,t[i][j]);
        }
        os<<endl;
    }
    os<<endl;
    return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
   string rowstring;
   while(getline(ifs,rowstring,'\r'))
   {
        if(rowstring=="")
            break;
        DBRow row;  //make a constructor
        stringstream rowstream(rowstring);
        string unit;
        while(getline(rowstream,unit,','))
        {
            if(unit =="")
		row.addData(INT_MAX);
	    else
		{
	    		int value = atoi(unit.c_str());
        	        row.addData(value);
 		}
        }
        getline(rowstream,unit);
        if(unit =="")
	row.addData(INT_MAX);
	else
	{
  		int value = atoi(unit.c_str());
       	        row.addData(value);
	}
        t.addRow(row);
   }
    return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
    _data.erase(_data.begin() +c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   
    for(size_t i = 0 ; i <_sortOrder.size(); i++)
    {
        if(r1[_sortOrder[i]] < r2 [_sortOrder[i]])
            return true;
        else if(r1[_sortOrder[i]] > r2[_sortOrder[i]])
            return false;
    }
    
    return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
    _table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
    for(size_t i = 0 ; i <_table.size(); i++)
        _table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
    _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   float max = INT_MIN;
   size_t count = 0;
   for(size_t i =0 ; i <_table.size();i++)
   {
        if(_table[i][c] ==INT_MAX)
            count++;
        else
            if(_table[i][c] > max)
                max = _table[i][c];
   }
   if(count ==_table.size())
       return NAN;
   else
       return max;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   float min = INT_MAX;
   size_t count = 0;
   for(size_t i = 0; i <_table.size();i++)
   {
        if(_table[i][c] == INT_MAX)
            count ++;
        else
            if(_table[i][c] <min)
                min = _table[i][c];
   }
   if(count == _table.size())
       return NAN;
   else
       return min;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   float sum =0.0;
   size_t count = 0;
   for(size_t i = 0; i <_table.size();i++)
   {
        if(_table[i][c] ==INT_MAX)
            count++;
        else
            sum += _table[i][c];
   }
   if(count== _table.size())
       return NAN;
   else
       return sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   vector<int > temp;
   for(size_t i = 0;i<_table.size(); i++)
   {
        if(_table[i][c] !=INT_MAX)
        {
            bool repeat = false;
            for(size_t j = 0; j< temp.size();j++)
                if(_table[i][c] == temp[j])
                    repeat = true;
            if(!repeat)
                temp.push_back(_table[i][c]);
        }
   }
   return (int) temp.size();
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   
    int count = 0;
    for(size_t i = 0; i< _table.size();i++)
   {
       if(_table[i][c] == INT_MAX)
           count++;
   }
    if(count ==0)
        return NAN;
    else
        return getSum(c) / count;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
    stable_sort(_table.begin(),_table.end(),s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
    for(size_t i = 0; i< _table.size();i++)
        cout<<_table[i][c]<<' ';
    cout<<'\b';
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

