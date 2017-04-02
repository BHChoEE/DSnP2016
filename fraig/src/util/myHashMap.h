/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//


class FecKey
{
public:
    FecKey(unsigned v):simValue(v) {}
    size_t operator() () const 
    {
        if(~simValue < simValue)
            return ~simValue;
        else
            return simValue;
    }
    bool operator == (const FecKey& k) const 
    { if((simValue == k.simValue) ||(~simValue == k.simValue) ) 
        return true;
      else
        return false;
    }
    bool inv (const FecKey& k) const
    {return simValue == ~k.simValue;}
private:
    unsigned simValue;
};

class HashKey
{
public:
   HashKey(size_t x0,size_t x1):lhs0(x0),lhs1(x1) {}

   size_t operator() () const
   {
       size_t x = lhs0 % INT_MAX;
       size_t y = lhs1 % INT_MAX;
       if(x < y)
           swap(x,y);
       size_t p = 0;
       size_t i = 0;
       while( x||y )
       {
           p |= ((size_t)(x & 1) << i);
           x >>= 1;
           p |= ((size_t)(y & 1) << (i+1));
           y >>= 1;
           i += 2;
       }
       return p;
   }

   bool operator == (const HashKey& k) const
   {
       if(lhs0 == k.getlhs0() && lhs1 == k.getlhs1() )
           return true;
       else if(lhs0 == k.getlhs1() && lhs1 == k.getlhs0())
           return true;
       else
           return false;
   }
   size_t getlhs0() const{return lhs0;}
   size_t getlhs1() const{return lhs1;}
private:
   size_t lhs0;
   size_t lhs1;
};
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(vector<HashNode>* hash = 0,size_t b = 0,size_t n = 0,size_t num = 0):
          _hash(hash),_b(b),_n(n),numBuckets(num) { }
      iterator(const iterator& i):
          _hash(i._hash),_b(i._b),_n(i._n) { }
      ~iterator () { }

      const HashNode& operator * () const { return _hash[_b][_n]; }
      HashNode& operator * () { return _hash[_b][_n]; }
      iterator& operator ++ ()
      {
          if(_b ==  numBuckets)
              return *(this);
          else if(_n < _hash[_b].size() -1)
              ++ _n;
          else
          {
              while( ++ _b < numBuckets)
                  if(!_hash[_b].empty())
                      break;
              _n = 0;
          }
          return *(this);
      }
      iterator operator ++ (int)
      {
          iterator tmp = iterator(*this);
          ++(*this);
          return tmp;
      }
      iterator& operator -- ()
      {
          if(_hash == 0)
              return *(this);
          if(_n > 0)
          {
              -- _n;
              return *(this);
          }
          size_t t = _b;
          while(t > 0)
          {
              -- t;
              if(!_hash[t].empty())
              {
                  _n =_hash[t].size() -1;
                  _b = t;
                  break;
              }
          }
          return *(this);
      }
      iterator operator -- (int)
      {
          iterator tmp = iterator(*this);
          --(*this);
          return tmp;
      }
      iterator& operator = (const iterator& i)
      {
          _hash = i._hash;
          _b = i._b;
          _n = i._n; 
          numBuckets = i .numBuckets;
          return *(this);
      }
      bool operator == (const iterator& i)
      {
          return ((_b == i._b) && (_n == i._n) && (_hash == i._hash));
      }
      bool operator != (const iterator& i)
      {
          return !(*(this) == i);
      }

   private:
      vector<HashNode>* _hash;
      size_t _b,_n;
      size_t numBuckets;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const
   {
       for(size_t i = 0 ; i < _numBuckets ; i++)
           if(!_buckets[i].empty())
               return iterator(_buckets,i, 0,_numBuckets);
       return end();
   }
   // Pass the end
   iterator end() const
   {
       return iterator(_buckets,_numBuckets,0,_numBuckets);
   }
   // return true if no valid data
   bool empty() const
   {
       for(size_t i = 0 ; i < _numBuckets ; i++)
           if(!_buckets[i].empty())
               return false;
       return true;
   }
   // number of valid data
   size_t size() const
   {
       size_t s = 0;
       for(size_t i = 0 ; i < _numBuckets ; i++)
           s += _buckets[i].size();
       return s;
   }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const
   {
       size_t num = bucketNum(k);
       for(size_t i = 0 ; i < _buckets[num].size() ; i++)
           if(_buckets[num][i].first == k)
               return true;
       return false;
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const
   {
       size_t num = bucketNum(k);
       for(size_t i = 0 ; i < _buckets[num].size() ; i++)
           if(_buckets[num][i].first == k)
           {
               d = _buckets[num][i].second;
               return true;
           }
       return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d)
   {
       size_t num = bucketNum(k);
       for(size_t i = 0 ; i < _buckets[num].size() ; i++)
           if(_buckets[num][i].first == k)
           {
               _buckets[num][i].second = d;
               return true;
           }
       _buckets[num].push_back(HashNode(k, d));
       return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d)
   {
       size_t num = bucketNum(k);
       for(size_t i = 0 ; i < _buckets[num].size() ; i++)
           if(_buckets[num][i].first == k)
               return false;
       _buckets[num].push_back(HashNode(k, d));
       return true;
   }

   void forceInsert(const HashKey& k, const HashData& d)
   {
       size_t num = bucketNum(k);
       _buckets[num].push_back(HashNode(k, d));
   }
   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k)
   {
       size_t num = bucketNum(k);
       for(size_t i = 0 ; i < _buckets[num].size() ; i++)
           if(_buckets[num][i].first == k)
           {
               _buckets[num][i] = _buckets[num].back();
               _buckets[num].resize(_buckets[num].size() -1);
               return true;
           }
       return false;
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
