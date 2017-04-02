/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;

    BSTreeNode(const T& d, BSTreeNode<T>* l = 0,BSTreeNode<T>* r = 0, BSTreeNode<T>* p = 0):            _data(d), _left(l), _right(r), _parent(p){}

    T           _data;
    BSTreeNode<T>*  _left;
    BSTreeNode<T>*  _right;
    BSTreeNode<T>*  _parent;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
   //consturctor/destructor of BSTree
public:
   BSTree()
   {
        _dummy = new BSTreeNode<T>(T(),0,0,0);//dummy
        _root = NULL;
        _size = 0;
   }
   ~BSTree(){clear(); delete _dummy;}
   // define iterator and overloading symbols for iterators  
   class iterator
   {
        friend class BSTree;

    public:
        iterator(BSTreeNode<T>* n=0): _node(n){}
        iterator(const iterator& i) :_node(i._node) {}
        ~iterator() {}
    //operator overloading
        const T& operator * () const{return(*this); }
        T& operator * () {return _node -> _data;}
        iterator& operator ++ ()
        {
            if(_node -> _right == 0)
            {
                BSTreeNode <T>* tmp= _node -> _parent;
                while(tmp != 0 && _node == tmp -> _right)
                {
                    _node = tmp;
                    tmp = tmp -> _parent;
                }
                _node = tmp;
                return *(this);
            }
            else
            {
                _node = _node -> _right;
                while(_node -> _left != 0)
                    _node = _node -> _left;
                return *(this);
            }
        }
        iterator operator ++ (int)
        {
            iterator tmp = *this;
           ++ *(this);
            return tmp; 
        }
        iterator& operator -- () 
        {  
            if(_node -> _left == 0)
            {
                BSTreeNode <T>* tmp= _node -> _parent;
                while(tmp != 0 && _node == tmp -> _left)
                {
                    _node = tmp;
                    tmp = tmp -> _parent;
                }
                _node = tmp;
                return *(this);
            }
            else
            {
                _node = _node -> _left;
                while(_node -> _right != 0)
                    _node = _node -> _right;
                return *(this);
            }  
        }
        iterator operator -- (int) 
        {
            iterator tmp = *this;
            -- *(this);
            return tmp;
        }
        iterator& operator = (const iterator& i)  
        {
            _node = i._node;
            return *(this);
        }
        
        bool operator != (const iterator& i) const {return(_node!=i._node);}
        bool operator == (const iterator& i) const {return(_node==i._node);}
    private:
        BSTreeNode<T>* _node;
   };
   // other functions of BSTree
   iterator begin() const
   {
       if(_size ==0)
           return _dummy;
       else
       {
            BSTreeNode<T>* tmp = _root;
            while(tmp -> _left != 0)
                tmp = tmp -> _left;
            return tmp;
       }
   }
   iterator end() const
   {
       return _dummy;
   }
   
   bool empty() const
   {
        if(_size == 0)
            return true;
        else
            return false;
   }

   size_t size() const
   {
        return _size;
   }
   
   void insert(const T& x)
   {
       //constructor of new node
       BSTreeNode<T>* newNode = new BSTreeNode<T>(x, 0, 0, 0);
       //algo
       BSTreeNode<T>* nodeX = _root;
       BSTreeNode<T>* nodeY = NULL;
       //check if the tree is empty
       if(nodeX == NULL)
           nodeX = _dummy;
       while(nodeX != NULL && nodeX != _dummy)  //X != NIL
       {
            nodeY = nodeX;
            if(newNode -> _data < nodeX -> _data)
                nodeX = nodeX -> _left;
            else
                nodeX = nodeX -> _right;
       }
       newNode -> _parent = nodeY;
       if(nodeY == NULL)
           _root = newNode;
       else if(newNode -> _data < nodeY -> _data)
           nodeY -> _left = newNode;
       else
           nodeY -> _right = newNode;
       //deal with empty case
       if(nodeX == _dummy)
       {
           _dummy -> _parent = newNode;
           newNode -> _right = _dummy;
       }
       _size ++;
   }

   void pop_front()
   {
      erase(begin()); 
   }

   void pop_back()
   {
       erase(--end()); 
   }

   bool erase(iterator pos)
   {
      if(pos == 0 || pos._node == 0 || empty())
          return false;
      
      if(pos._node == NULL)
          return false;
      BSTreeNode <T>* nodeZ = pos._node;
      //case A
      if(nodeZ-> _left == NULL)
          transplant(nodeZ, nodeZ ->_right);
      //case B
      else if(nodeZ -> _right == NULL)
          transplant(nodeZ, nodeZ -> _left);
      //case B - dummy type
      else if(nodeZ -> _right == _dummy)
      {
            transplant(nodeZ , nodeZ -> _left);
            BSTreeNode<T>* maxNode = nodeZ -> _left;
            while(maxNode -> _right != NULL && maxNode -> _right != _dummy)
                maxNode = maxNode-> _right;
            maxNode -> _right = _dummy;    //repair dummy relationship
            _dummy -> _parent = maxNode;
      }
      else
      {
            //minNode = tree-minimum(nodeZ.right)
            BSTreeNode<T>* minNode = nodeZ -> _right;
            while(minNode -> _left != NULL)
                minNode = minNode -> _left;
            //case D
            if(minNode -> _parent != nodeZ)
            {
                transplant(minNode,minNode -> _right);
                minNode -> _right = nodeZ -> _right;
                minNode -> _right -> _parent = minNode;
            }
            //case C
            transplant(nodeZ,minNode);
            minNode -> _left = nodeZ -> _left;
            minNode -> _left -> _parent = minNode;
      }
      delete nodeZ;
      _size --;
      return true;
   }
   bool erase(const T& x)
   {
       iterator tmp = end();
       for(iterator iter = begin(); iter!= end(); iter++)
       {
            if(iter._node -> _data == x)
            {
                tmp = iter;
                break;
            }
       }
       if(tmp == end())
           return false;
       erase(tmp);
       _size --;
       return true;
   }

   void clear()
   {
        if(empty())
            return;
        if(_root != 0)
            deleteNode(_root);
        _size = 0;
        _dummy -> _parent = 0;
        _root = 0;
   }
   
   void deleteNode(BSTreeNode<T>*n)
   {
        if(n->_left != 0)
            deleteNode(n -> _left);
        else if(n-> _right!= 0 && n->_right != _dummy)
            deleteNode (n->_right);
        delete n;
   }

   void transplant(BSTreeNode<T>* u,BSTreeNode<T>* v)//for erase
   {
        if(u -> _parent == NULL)
            _root = v;
        else if(u == u ->_parent -> _left)
            u -> _parent -> _left = v;
        else
            u -> _parent -> _right = v;
        if(v != NULL)
            v -> _parent = u -> _parent;
   }
        

   void sort() const
   {

   }

   void print() const
   {

   }
    
   private:
        BSTreeNode<T>*   _root;     // the very original node of every tree
        BSTreeNode<T>*   _dummy;    //dummy node
        size_t _size ; //to indicate the array is sorted
};

#endif // BST_H
