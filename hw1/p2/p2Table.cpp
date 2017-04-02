#include "p2Table.h"
#include<fstream>
#include<iomanip>
#include<iostream>
#include<sstream>

using namespace std;

// Implement member functions of class Row and Table here

Row::Row (int col)    
{
    _data = new int [col];
    column = col; 
    for(int i =0;i<col;i++)
        _data[i]=5487;
   }

    
bool
Table::read(const string& csvFile)
{
    //READ_IN_csvFile
    ifstream file;      //declare file stream
    file.open(csvFile.c_str()); //string to char*

    string rowstring;
    string unit;
    
    while(file>>rowstring)//read a string until ^M
    {
        int col=1;       //find comma num in per row
        for( int i =0;i<rowstring.length();i++)
        {
            if(rowstring.c_str()[i] == ',')
                col++;
        }
        Row row(col);    //make a constructor
        
        stringstream rowstream(rowstring); //string to stream
        int column=0;
        
        while(getline(rowstream,unit,','))//read a row until comma
        {
            stringstream value(unit);          //fill in value of each unit
            value >> row[column];
            column++;
        }
        _rows.push_back(row);
    }
    
    file.close();

    return true; // TODO
}

void Table::PRINT()
{
    for(int i =0; i <_rows.size();i++)
    {
        for(int j =0 ; j< _rows[i].getColumn(); j++)
        {
            if(_rows[i][j]==5487)
                cout<<setw(4)<<right<<"";
            else
                cout<<setw(4)<<right<<_rows[i][j];
        }
        cout<<endl;
    }
    
}

int Table::SUM(int col)
{
    int sum=0;
    for(int i =0;i<_rows.size();i++)
    {
        if(_rows[i][col]==5487)
            continue;
        else
            sum = sum + _rows[i][col];
    }
    return sum;
}

double Table::AVE(int col)
{
    double ave=0;
    double sum ,count =0;
    for(int i =0;i<_rows.size();i++)
    {
        if(_rows[i][col]==5487)
            continue;
        else
        {
            count ++;
            sum = sum + _rows[i][col];
        }
    }
    ave = sum/count;
    return ave;
}

int Table::MAX(int col)
{
    int max = -5487;
    for(int i =0;i<_rows.size();i++)
    {
        if(_rows[i][col]==5487 || _rows[i][col] <= max)
            continue;
        else if(_rows[i][col]> max)
            max = _rows[i][col];
    }
    return max;
}

int Table::MIN(int col)
{
    int min = 5488;
    for(int i=0;i<_rows.size();i++)
    {
           if( _rows[i][col]==5487 || _rows[i][col] >= min)
               continue;
           else if(_rows[i][col] < min)
               min = _rows[i][col];
    }

    return min;
}

int Table::COUNT(int col)
{
    int count =0;
    
    vector<int> copy;
    for(int i =0;i<_rows.size();i++)
    {
        if(_rows[i][col]==5487)
            continue;
        else
        {
            copy.push_back(_rows[i][col]);
            count++;
        }
    }
    int total =count;
    for(int i =1;i<total;i++)
    {
        for(int j=0;j<i;j++)
        {
            if(copy[i]==copy[j])
            {
                count--;
                break;
            }
            else
                continue;
        }
    }
    return count;
}

void Table::ADD(string add)
{
    string unit;
    stringstream Add(add);
    int num=0;
    Row row(_rows[0].getColumn());
 
    while(getline(Add,unit,' '))
    {
        if(unit !="-")
        {
            stringstream col(unit);
            col>>row[num];
        }
        num++;
    }   
    _rows.push_back(row);
}
