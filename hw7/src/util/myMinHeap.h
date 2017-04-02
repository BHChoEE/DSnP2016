/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }
   #define idx(t)  t-1
   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) 
   {
        //down to top
        _data.push_back(d);
        size_t n = _data.size();  //last element
        while(n > 0)
        {
            size_t p = n / 2;//p is n's parent
            if(!(_data[idx(n)] <  _data[idx(p)]))
                break;
            swap(_data[idx(p)],_data[idx(n)]);
            n = p;
        }
   }
   void delMin() 
   {
       size_t n = _data.size();
       swap(_data[0],_data[idx(n)]);
       _data.resize(n-1);
       minHeapify(1);
   }
   void delData(size_t i)
   {
        size_t n = _data.size() ;
        swap(_data[i],_data[idx(n)]);
        _data.resize(n-1);
        minHeapify(i+1);
   }

   void minHeapify(size_t i)
   {
        size_t n = _data.size();
        size_t left = 2 * i;
        size_t right = 2 * i + 1;
        size_t parent = i / 2;
        size_t tmp;
        // if i <  p, swap i,p
        if( _data[idx(i)] < _data[idx(parent)] && parent > 1)
        {
            swap(_data[idx(i)],_data[idx(parent)]);
            minHeapify(i);
        }
        
        if(left <= n && _data[idx(left)] < _data[idx(i)])
            tmp = left;
        else
            tmp = i;
        if(right <= n && _data[idx(right)] < _data[idx(tmp)])
            tmp = right;
        if(tmp != i)
        {
            swap(_data[idx(i)],_data[idx(tmp)]);
            minHeapify(tmp);
        }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
