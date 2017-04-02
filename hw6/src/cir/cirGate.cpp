/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"
#include <sstream>

using namespace std;

extern CirMgr *cirMgr;
unsigned CirGate::_global = 0;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    stringstream outp;
    cout<<"=================================================="<<'\n';
    
    string name;
    string typestr = getTypeStr();
    unsigned id = getId();
    unsigned lineNo = getLineNo();
    outp<<" =" << typestr <<'('<< id << ')';
    if(type == PI_GATE)
    {
        const CirPiGate* gate = (CirPiGate*)this;
        if(gate -> getName() != "")
        {
            name = gate -> getName();
            outp<<'\"'<< name <<'\"';
        }
    }
    else if(type == PO_GATE)
    {
        const CirPoGate* gate = (CirPoGate*)this;
        if(gate -> getName() != "")
        {    
            name = gate -> getName();
            outp<<'\"'<< name <<'\"'; 
        }
    }
    outp<<", line " << lineNo;
    cout<<setw(49) << left <<outp.str()<<"=\n";
    cout<<"=================================================="<<'\n'; 
}

void
CirGate::faninDfs(int level, bool inv,int levelconst) const
{
    if(level < 0)
        return;
    for(unsigned i = 0 ; i < levelconst -level ; i++)
        cout<<"  ";
//    for(unsigned i = 0 ; i < level; i++ )
//        cout<<'\b'<<'\b';
    if(inv)
        cout<<"!";
    cout<<getTypeStr()<<" "<<id;
//    if(level == 0)
 //       cout<<'\n';
    if(mark != 0)
        cout<<" (*)"<<endl;
    else
    {
        cout<<'\n';
        for(unsigned j = 0 ; j < faninList.size() ; j++)
        {
            _global++;
            CirGate* gate = getInputGate(j);
            gate -> faninDfs(level-1,faninInv(j),levelconst);
            setMark(1);
        }
    }
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   cirMgr ->resetMark();
   faninDfs(level,0,level);
   _global = 0;
}

void
CirGate::fanoutDfs(int level, bool inv,int levelconst) const
{
    if(level < 0)
        return;
    for(unsigned i = 0 ; i < levelconst - level; i++)
        cout<<"  ";
//    for(unsigned i = 0 ; i <level ; i++)
//        cout<<'\b'<<'\b';
    if(inv)
        cout<<"!";
    cout<<getTypeStr()<<" "<<id;
    if(mark != 0)
        cout<<" (*)" << endl;
    else
    {
        cout<<'\n';
        for(unsigned j = 0 ; j < fanoutList.size() ; j++)
        {
            _global++;
            CirGate* gate = getOutputGate(j);
            gate -> fanoutDfs(level-1,fanoutInv(j),levelconst);
            setMark(1);
        }
    }
    /*else
    {
       if(mark)
            cout << " (*)"<<endl;
   i    //construct 
       vector<unsigned> order;
       vector<unsigned> index;
       for(unsigned i = 0 ; i < fanoutList.size() ; i++)
       {
            CirGate* gate = getOutputGate(i);
            order.push_back(gate -> getId());
            index.push_back(i);
       }
       //sort
       for(unsigned i = 0; i < fanoutList.size(); i++)
       {
            for(unsigned j = i ; j < fanoutList.size() ;j++)
            {
                if(order[j] < order[i])
                {
                    swap(order[i],order[j]);
                    swap(index[i],index[j]);
                }
            }
       }
        //print
       cout<<endl;
       for(unsigned i = 0; i < fanoutList.size() ; i++)
       {
            _global ++;
            CirGate* gate1 = getOutputGate(index[i]);
            bool inv = false;
            for(unsigned j = 0 ; j < gate1 -> faninList.size(); j++)
            {
                CirGate* gate2 = getInputGate(j);
                if(gate2 == 0)
                    break;
                else if(gate2 == this)
                {
                    inv = gate1 -> Inv(j);
                    gate1 -> fanoutDfs(level -1, inv,levelconst);
                    break;
                }

            }
       }
       if(fanoutList.size() != 0)
           setMark(1);
    }
    _global --;*/
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   cirMgr -> resetMark();
   fanoutDfs(level,0,level);
   _global = 0;
}

void
CirPiGate::printGate() const
{
    //[9] PI  7(7GAT)
    if(mark != 0)
        return;
    cout<<"["<< _global <<"] "<<setw(4)<<left<< getTypeStr() << getId();
    _global++;
    if(getName().empty())
        cout<<endl;
    else
        cout<<" (" << getName() << ")"<<endl;
    setMark(1);
}

void
CirPoGate::printGate() const
{
   //[8] PO  24  !22(22GAT$PO)
   if(mark != 0)
       return;
   for(unsigned i = 0; i<faninList.size();i++)
       getInputGate(i) -> printGate();
   cout<<"["<<_global<<"] "<< setw(4) << left << getTypeStr() <<getId()<<" ";
   CirGate* input = getInputGate(0);
   //stringstream outp;
   if(input -> getType() == UNDEF_GATE)
       cout << "*";
   else if(faninInv(0))
       cout << "!";
   cout << input -> getId();

   _global ++;
   if(getName().empty())
       cout<<endl;
   else
       cout<<" ("<< getName() <<")" <<endl;
   setMark(1);
}

void
CirAigGate::printGate() const
{
    //[7] AIG 22 !10 !16
    if(mark != 0)
        return;
    for(unsigned i = 0 ; i < faninList.size() ; i++)
        getInputGate(i) -> printGate();
    cout<<"["<<_global<<"] "<< setw(4) << left <<getTypeStr() <<getId()<<" ";
    _global ++;
    
    CirGate* gate1 = getInputGate(0);
    CirGate* gate2 = getInputGate(1);
    stringstream outp1;
    stringstream outp2;
    
  //  if(gate1 == 0 || gate2 == 0)
  //      return;
    if(gate1 -> getType() == UNDEF_GATE)
        outp1 << "*";
    else if(faninInv(0))
        outp1 <<"!";
    outp1 << (gate1 -> getId()) <<" ";
    
    if(gate2 -> getType() == UNDEF_GATE)
        outp2 << "*";
    else if(faninInv(1))
        outp2 << "!";
    outp2 << (gate2 -> getId()) ;
    cout << outp1.str() <<  outp2.str() << endl;
    
    setMark(1);
}

void
CirConstGate::printGate() const
{
    //[1] CONST0
    if(mark != 0)
        return;
    cout<<"["<<_global<<"] "<<setw(4) <<left << getTypeStr() <<getId();
    cout<<'\n';
    _global ++;
    setMark(1);
}

void
CirUndefGate::printGate() const
{
    // do nothing
}
