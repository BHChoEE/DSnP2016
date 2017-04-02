/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
       
      //constructor
        iterator(size_t b = 0,size_t d = 0, size_t nBuc = 0 , vector<Data>* p=0 ):
            _b(b), _d(d), numBuckets(nBuc), _buckets(p) {}
        iterator(const iterator& i):
            _b(i._b), _d(i._d), numBuckets(i.numBuckets), _buckets(i._buckets) {}
        ~iterator() {}

      //operator overloading
        const Data& operator * () const { return _buckets[_b][_d]; }
        Data& operator * () {return _buckets[_b][_d];}
        iterator& operator ++ ()
        {
            if(_d < _buckets[_b].size() -1)    //in the same line
                ++ _d;
            else // to the next line
            {
                if (_b + 1 == numBuckets)
                    ++ _d;
                else
                {
                   while( _b + 1 < numBuckets)
                   {
                       ++ _b;
                       if(!_buckets[_b].empty())
                           break;
                   }
                   _d = 0;
                }
            }
            return (*this);
        }
        iterator operator ++ (int) 
        {
            iterator tmp = (*this);
            ++ (*this);
            return tmp;
        }
        iterator& operator -- ()
        {
            if(_b < 0)
                return (*this);
            else if(_d > 0)
                -- _d;
            else
            {
                while(-- _b >= 0)
                    if(_buckets[_b].size() > 0)
                        break;
                _d = _buckets[_b].size() -1;
            }
            return (*this);
        }
        iterator operator -- (int) 
        {
            iterator tmp = (*this);
            -- (*this);
            return tmp;
        }
        iterator& operator = (const iterator& i)
        {
            _b = i._b;
            _d = i._d;
            numBuckets = i.numBuckets;
            _buckets = i._buckets;
            return (*this);
        }
        bool operator != (const iterator& i)
        {return (_b != i._b || _d != i._d );}
        bool operator == (const iterator& i)
        {return (_b == i._b && _d == i._d);}

   private:
   size_t _b;
   size_t _d;
   size_t numBuckets;
   vector<Data>* _buckets;
    
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const
   {
       size_t count = 0;
       for(; count < _numBuckets ; count++)
       { 
           if(!_buckets[count].empty())
                break;
       }
       return iterator(count , 0,numBuckets() , _buckets);
   }
   // Pass the end
   iterator end() const
   {
       return iterator(numBuckets()-1,_buckets[numBuckets()-1].size(),numBuckets(),_buckets);
   }
   // return true if no valid data
   bool empty() const
   {
       for(size_t i = 0 ; i < _numBuckets ; i++)
       {
            if(!_buckets[i].empty())
                return false;
       }
       return true;
   }
   // number of valid data
   size_t size() const 
   {
       size_t s = 0;
       for(size_t i = 0; i < _numBuckets; i++)
           s = s + _buckets[i].size();
       return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const
   {
       for(size_t i = 0; i <_buckets[bucketNum(d)].size(); i++)
            if(d == _buckets[bucketNum(d)][i])
                return true;
       return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const
   {
       for(size_t i = 0 ; i < _buckets[bucketNum(d)].size() ; i++)
            if(_buckets[bucketNum(d)][i] == d)
            {
                d = _buckets[bucketNum(d)][i];
                return true;
            }
       return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) 
   {
       for(size_t i = 0 ; i < _buckets[bucketNum(d)].size(); i ++)
            if(_buckets[bucketNum(d)][i] == d)
            {
                _buckets[bucketNum(d)][i] = d;
                return true;
            }
       return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d)
   {
       // alrready inserted , return false
       for(size_t i = 0 ; i < _buckets[bucketNum(d)].size() ; i++)
            if(_buckets[bucketNum(d)][i] == d)
                return false;
       //push in 
       _buckets[bucketNum(d)].push_back(d); 
       return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) 
   {
       //removed successfully
       for(size_t i = 0 ; i < _buckets[bucketNum(d)].size() ; i++)
           if(_buckets[bucketNum(d)][i] == d)
           {
                size_t size = _buckets[bucketNum(d)].size();
                _buckets[bucketNum(d)][i] = _buckets[bucketNum(d)].back();
                _buckets[bucketNum(d)].resize(size -1);
                return true;
           }
       //nothing is removed
       return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;
    
   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
