/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <bitset>
#include <sstream>

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
class SimKey
{
    public:
        SimKey(CirGate*gate, bool inv)
        {
            if(inv)
                value = ~(gate -> getSimValue());
            else
                value = gate -> getSimValue();
        }
        size_t operator() () const{return value;}
        bool operator == (const SimKey& k) const{return k.value == value;}
    private:
        unsigned value;
};

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/

void
CirMgr::randomSim()
{
    //construct DFSlist
    buildDfsList();
    vector<unsigned> pattern;
    size_t max_fail = 0;
    FecGrpInit(firstSim);
    size_t patternCount = 0;
    //construct inputgate randomly
    while(max_fail < 256)
    {
        patternCount += 32;
        //change from Const Number Simulation to Size Dependent
        pattern.clear();
        for(size_t i = 0 ; i < header[1] ; i++)
        {
            unsigned rn = rnGen(INT_MAX);
            pattern.push_back(rn);
        }
        //make all the pattren in AIG gates
        simGateValue(pattern);
        // throw all gates into FECGroup(from Original group)
        vector<FECGroup*> tmpGrps;
        for(size_t i = 0 ; i < _fecGroups.size() ; i++)
        {
            HashMap<FecKey,FECGroup*> newFecGrp (_fecGroups[i]->gatelistSize());
            FECGroup* fecGrp = _fecGroups[i];
            for(size_t j = 0 ; j < fecGrp -> gatelistSize() ; j++)
            {
                FECGroup* find = 0;
                CirGate* gate = fecGrp -> getGate(j);
                FecKey key (gate -> getSimValue());
                ;
                if(newFecGrp.query(key,find))
                {
                    find -> addGate(gate);
                    newFecGrp.update(key,find);
                }
                else
                {
                    find = new FECGroup();
                    find -> addGate(gate);
                    newFecGrp.insert(key,find);
                }
            }
            // to find usable grp constructed and push into tmpgrps
            for(HashMap<FecKey,FECGroup*>::iterator it = newFecGrp.begin(); it!= newFecGrp.end() ;it++)
                if((*it).second->gatelistSize() >= 2)
                tmpGrps.push_back((*it).second);
        }
        size_t last = _fecGroups.size();
        resetFecGrps();
        for(size_t i = 0 ; i < tmpGrps.size() ; i++)
            _fecGroups.push_back(tmpGrps[i]);
        if(_fecGroups.size() == last)
            max_fail ++;
    }
    cout << "Total # of FEC groups:"<<_fecGroups.size() <<endl;
    cout << patternCount <<" pattern simluated"<<endl;

}

void
CirMgr::fileSim(ifstream& patternFile)
{
//    vector<FECGroup*>& FecGroups = _fecGroups;
    //construct DFSlist
    buildDfsList();
    //read file
    string str;
    unsigned pat[header[1]];
    memset(pat,0,sizeof(pat));
    vector<unsigned> pattern;
    size_t patternCount = 0;
    FecGrpInit(firstSim);
    //read line
    while(patternFile >> str)
    {
        if(str.size() != header[1])//num != input number
        {
            cerr <<"Error: Pattern(" << str << ") length(" << str.length() << ") does not match";
            cerr <<" the number of inputs";
            return;
        }
        for(size_t i = 0 ; i < header[1] ;i++) //check if illegal input( not bool )
        {
            if(str[i] != '0' && str[i] != '1')
            {
                cerr <<"Error: Pattern(" <<str <<") contains a non-0/1 characters(\'" <<str[i] <<"\')."<<endl;
                return;
            }
            //shift and store pattern in
            pat[i] = (pat[i] << 1) | (str[i] - '0');
        }
        ++ patternCount;
        //push out the pack
        if(patternCount % 32 == 0)
        {   
            pattern.clear();
            for(size_t i = 0 ; i < header[1] ; i++)
            {
                pattern.push_back(pat[i]);
            }
            //make all the pattren in AIG gates
            simGateValue(pattern);
            // throw all gates into FECGroup(from Original group)
            vector<FECGroup*> tmpGrps;
            for(size_t i = 0 ; i < _fecGroups.size() ; i++)
            {
                HashMap<FecKey,FECGroup*> newFecGrp (_fecGroups[i]->gatelistSize());
                FECGroup* fecGrp = _fecGroups[i];
                for(size_t j = 0 ; j < fecGrp -> gatelistSize() ; j++)
                {
                    FECGroup* find = 0;
                    CirGate* gate = fecGrp -> getGate(j);
                    FecKey key (gate -> getSimValue());
                    if(newFecGrp.query(key,find))
                    {
                        find -> addGate(gate);
                        newFecGrp.update(key,find);
                    }
                    else
                    {
                        find = new FECGroup();
                        find -> addGate(gate);
                        newFecGrp.insert(key,find);
                    }
                }
                // to find usable grp constructed and push into tmpgrps
                for(HashMap<FecKey,FECGroup*>::iterator it = newFecGrp.begin(); it!= newFecGrp.end() ;it++)
                      if((*it).second->gatelistSize() >= 2)
                          tmpGrps.push_back((*it).second);
            }
            resetFecGrps(); 
            for(size_t i = 0 ; i < tmpGrps.size() ; i++)
                _fecGroups.push_back(tmpGrps[i]);
   //         cout << _fecGroups.size() <<endl;
       }
    }
    //if not enough 32 bit
    if(patternCount % 32 != 0)
    {
        pattern.clear();
        size_t cpl = 32 - patternCount % 32;
        for(size_t i = 0 ; i < header[1] ; i++)
            pat[i] = pat[i] << cpl;
        for(size_t i = 0 ; i < header[1] ; i++)
            pattern.push_back(pat[i]);
        //make all the pattren in AIG gates
        simGateValue(pattern);
        memset(pat,0,sizeof(pat));

        // throw all gates into FECGroup(from Original group)
        vector<FECGroup*> tmpGrps;
        for(size_t i = 0 ; i < _fecGroups.size() ; i++)
        {
            HashMap<FecKey,FECGroup*> newFecGrp (_fecGroups[i] -> gatelistSize());
            FECGroup* fecGrp = _fecGroups[i];
            for(size_t j = 0 ; j < fecGrp -> gatelistSize() ; j++)
            {
                FECGroup* find;
                CirGate* gate = fecGrp -> getGate(j);
                FecKey key (gate -> getSimValue());
                if(newFecGrp.query(key,find))
                {
                    find -> addGate(gate);
                    newFecGrp.update(key,find);
                  //      cout << gate -> getId() << " " << endl;
                }
                else
                {
                    find = new FECGroup();
                    find -> addGate(gate);
                    newFecGrp.insert(key,find);
                //    cout << gate -> getId() << endl;
                }
            }
                // to find usable grp constructed and push into tmpgrps
            for(HashMap<FecKey,FECGroup*>::iterator it = newFecGrp.begin(); it!= newFecGrp.end() ;it++)
                  if((*it).second->gatelistSize() >= 2)
                      tmpGrps.push_back((*it).second);
        }
        resetFecGrps();
        for(size_t i = 0 ; i < tmpGrps.size() ; i++)
            push_grp(tmpGrps[i]);
        
    }
    cout << "Total # of FEC groups:"<<_fecGroups.size() <<endl;
    cout << patternCount <<" pattern simluated"<<endl;
}

void
CirMgr::simGateValue(vector<unsigned> pattern)
{
    buildDfsList();
    //reset Sim Value
    for(size_t i = 0 ; i < gatelist.size() ; i++)
    {
        CirGate* gate = gatelist[i];
        gate -> resetSimValue();
    }
    //push pack into inputgate
    for(size_t i = 0 ; i < header[1] ; i++)
    {
        CirGate* input = getGate(ingate[i]);
        input -> setSimValue(pattern[i]);
    }
    //bulidDfsList
    buildDfsList();
    //get value of AIG_GATE and PO_GATE by dfs order
    for(size_t i = 0 ; i < DfsList.size() ; i++)
    {
        CirGate* gate = DfsList[i];
        if(gate -> getType() == AIG_GATE)
        {
            CirGate* fanin[2] = {gate -> getInputGate(0), gate -> getInputGate(1)};
            bool faninInv[2] =  {gate -> faninInv(0), gate -> faninInv(1)};
            unsigned faninSimValue[2] = {fanin[0] -> getSimValue(), fanin[1] -> getSimValue()};
            if(faninInv[0] == 0 && faninInv[1] == 0)
                gate -> setSimValue( faninSimValue[0] & faninSimValue[1]);
            else if(faninInv[0] == 0 && faninInv[1] == 1)
                gate -> setSimValue( faninSimValue[0] & ~faninSimValue[1]);
            else if(faninInv[0] == 1 && faninInv[1] == 0)
                gate -> setSimValue( ~faninSimValue[0] & faninSimValue[1]);
            else if(faninInv[0] == 1 && faninInv[1] == 1)
                gate -> setSimValue( ~faninSimValue[0] & ~faninSimValue[1]);
        }
        else if(gate -> getType() == PO_GATE)
        {
            unsigned faninSimValue = gate -> getInputGate(0) -> getSimValue();
            if(gate -> faninInv(0))
                gate -> setSimValue( ~faninSimValue);
            else if(!gate -> faninInv(0))
                gate -> setSimValue(  faninSimValue);
        }
    }

}
void
CirMgr::FecGrpInit(bool& firstSim)
{
    _fecGroups.clear();
    buildDfsList();
    vector<CirGate*> group;
    FECGroup* fecGrp = new FECGroup();
    for(size_t i = 0 ; i < DfsList.size() ; i++)
        if(DfsList[i] -> getType() == AIG_GATE || DfsList[i] -> getType() == CONST_GATE)
            fecGrp -> addGate(DfsList[i]);
    _fecGroups.push_back(fecGrp);
//    for(size_t i = 0 ; i < _fecGroups[0] -> gatelistSize() ; i++)
//        cout << _fecGroups[0] ->getGate(i) -> getId() <<endl;
    firstSim = false;
}
/*
void
CirMgr::simulate(vector<FECGroup*> &fecGrps, HashMap<FecKey,FECGroup>& newFecGrps)
{
    for(size_t i = 0 ; i < fecGrps.size() ; i++)
    {
        FECGroup fecGrp = fecGrps[i];
        newFecGrps.init(fecGrp.size());
        for(size_t j = 0 ; j < fecGrp -> gatelistSize() ; j++)
        {
            CirGate* gate = fecGrp -> getGate(j);
            FecKey key(gate -> getSimValue());
            FECGroup* grp;
            if(newFecGrps.query)
        }
    }
}*/
/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
