/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head;// _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node =_node -> _next ;return *(this); }
      iterator operator ++ (int) 
      {iterator tmp = *this; ++(*this); return tmp; }
      iterator& operator -- () { _node = _node -> _prev;return *(this); }
      iterator operator -- (int) 
      { iterator tmp = *this; --(*this); return tmp; }

      iterator& operator = (const iterator& i) { *this = i;return *(this); }

      bool operator != (const iterator& i) const { return (_node !=i._node); }
      bool operator == (const iterator& i) const { return (_node ==i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return _head -> _next; }
   iterator end() const { return _head; }
   bool empty() const 
   { 
       if(_head ->_prev == _head && _head == _head ->_next)
           return true;
       else
           return false;
   }
   size_t size() const 
   {  
        size_t count =0;
        for( iterator it = begin(); it != end();it++)
            count++;
        return count; 
   }

   void push_back(const T& x) 
   {
        if(empty())
        {
            DListNode<T>* tmp = new DListNode<T>(x,_head , _head);
            _head -> _prev = tmp;
            _head -> _next = tmp;
            _isSorted = true;
        }
        else
        {
            DListNode<T>* tmp = new DListNode<T>(x,_head -> _prev, _head);
            DListNode<T>* originLast = _head -> _prev;
            originLast -> _next = tmp;
            _head -> _prev = tmp;
            _isSorted = false;
        }
   }
   void pop_front() 
   {
        erase(begin()); 
   }
   void pop_back()
   {
        erase(--end());
   }

   // return false if nothing to erase
   bool erase(iterator pos) 
   {
       if(!pos._node && pos == end())
            return false;
       (pos._node -> _prev) -> _next = pos._node -> _next;
       (pos._node -> _next) -> _prev = pos._node -> _prev;
       return true;
   }
   bool erase(const T& x)
   {
       for(iterator it = begin(); it != end(); it++)
       {
            if(x == it._node -> _data)
            {
               (it._node -> _prev) -> _next = it._node -> _next;
               (it._node -> _next) -> _prev = it._node -> _prev;
               return true;
            }
       }
       return false;
   }

   void clear()
   {
        for(iterator it = begin(); it != end(); it++)
        {
            erase(it._node);
        }
        assert(_head = _head -> _prev = _head -> _next);
   }  // delete all nodes except for the dummy node

   void sort() const
   {
        for(iterator it1 = begin()++; it1 != end(); it1++)
        {
            iterator it2 = it1;
            T tmp = it1._node -> _data;
            for(;it2 != begin() && tmp < it2._node -> _prev -> _data; it2--)
                it2._node -> _data = it2._node -> _prev -> _data;
            it2._node -> _data = tmp;
        }
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
