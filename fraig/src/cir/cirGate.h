/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.
class FECGroup;
class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   CirGate(GateType _t, unsigned _id, unsigned _n):
    type(_t), lineNo(_n), id(_id), mark(0){}
   virtual ~CirGate() {}

   // Basic access methods
   GateType getType() const {return type;}
   string getTypeStr() const
   {
       if(type == PI_GATE)
           return "PI";
       else if(type == PO_GATE)
           return "PO";
       else if(type == AIG_GATE)
           return "AIG";
       else if(type == CONST_GATE)
           return "CONST";
       else if(type == UNDEF_GATE)
           return "UNDEF";
       return "UNDEF";
   }
   unsigned getLineNo() const { return lineNo; }
   unsigned getId() const {return id;}
   virtual bool isAig() const {return false; }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void faninDfs(int level, bool inv, int levelconst) const;
   void reportFanin(int level) const;
   void fanoutDfs(int level, bool inv, int levelconst) const;
   void reportFanout(int level) const;
   void setMark(size_t m) const {mark = m;}
   size_t getMark () {return mark;}
   bool faninInv(unsigned i) const{return (faninList[i] % 2);}
   bool fanoutInv(unsigned i) const{return(fanoutList[i] % 2);}
   size_t faninSize() const{return faninList.size();}
   size_t fanoutSize() const{return fanoutList.size();}
   unsigned getSimValue() const  { return simValue;}
   void resetSimValue()  { simValue = 0;}
   void setSimValue(unsigned i)  {simValue = i;}

   //gate function
   void addInput(CirGate* gate, bool sign = false)
   {
        //if inverted gate ++
       size_t g = (size_t) gate;
       if(sign)
           ++g;
       //push in gate number
       faninList.push_back(g);
   }

   void addOutput(CirGate* gate, bool sign = false)
   {
        size_t g = (size_t) gate;
        if(sign)
            ++g;
        //push in gate number
        fanoutList.push_back(g);
   }
   
   CirGate* getInputGate(size_t gate) const
   {
        //illegal case
       if(gate >= faninList.size())
           return 0;
       return (CirGate*)((size_t)faninList[gate]& ~(0x1));
   }

   CirGate* getOutputGate(size_t gate) const
   {
        //illegal case
       if(gate >= fanoutList.size())
           return 0;
       return (CirGate*)((size_t)fanoutList[gate]& ~(0x1));
   }
   
   void eraseInGate (size_t gate)
   {
       //illegal case
       if(gate >= faninList.size())
           return ;
       swap(faninList[gate],faninList[faninList.size()-1]);
       faninList.resize(faninList.size()-1);
   }

   void eraseOutGate(size_t gate)
   {
       //illegal case
       if(gate >= fanoutList.size())
           return ;
       swap(fanoutList[gate],fanoutList[fanoutList.size()-1]);
       fanoutList.resize(fanoutList.size()-1);
   }
   void resetGlobalref() const { _global = 0; }
   void setGlobalRef() const { _global++; }
   bool isGlobalRef() const { return mark == _global; }
   void setToGlobalRef() const { mark = _global; }
   static unsigned _global;
private:

protected:
   GateType type;
   unsigned lineNo;
   unsigned id;
   vector<size_t> faninList;
   vector<size_t> fanoutList;
   FECGroup* fecGrp;
   unsigned simValue;
   mutable size_t mark;
};

class CirPoGate : public CirGate
{
    public:
        //constructor & destructor
        CirPoGate(unsigned _id, unsigned _n):
            CirGate(PO_GATE, _id, _n),name("") {}
        virtual ~CirPoGate() {}
        //function
        void printGate() const;
        void setName(string str){if(name.empty()) name = str;}
        string getName() const {return name;}
    protected:
        string name;
};

class CirPiGate : public CirGate
{
    public:
        //constructor & destructor
        CirPiGate(unsigned _id, unsigned _n):
            CirGate(PI_GATE, _id, _n),name("") {}
        virtual ~CirPiGate() {}
        //function
        void printGate() const;
        void setName(string str){if(name.empty()) name = str;}
        string getName() const {return name;}
    protected:
        string name;
};

class CirAigGate : public CirGate
{
    public:
        //constructor & destructor
        CirAigGate(unsigned _id, unsigned _n):
            CirGate(AIG_GATE, _id, _n) {}
        virtual ~CirAigGate() {}
        //function
        void printGate() const;
};

class CirConstGate : public CirGate
{
    public:
        //constructor & destructor
        CirConstGate(unsigned _n):
            CirGate(CONST_GATE, 0, _n) {}
        virtual ~CirConstGate() {}
        //function
        void printGate() const;
};

class CirUndefGate : public CirGate
{
    public:
        //constructor & destructor
        CirUndefGate(unsigned _id, unsigned _n):
            CirGate(UNDEF_GATE, _id, _n) {}
        virtual ~CirUndefGate() {}
        //function
        void printGate() const;
};
#endif //CIR_GATE_H
