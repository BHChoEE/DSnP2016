/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
class FECGroup;

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr()
   {
        for(int i = 0 ; i < 5 ; i++)
            header[i] = 0;
   }
   ~CirMgr() {} 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const
   {
       if(gid > header[0] + header[3])
           return 0;
       return gatelist[gid]; 
   }
   FECGroup* getGroup(size_t i) const {return _fecGroups[i];}
   size_t groupSize() const {return _fecGroups.size();}
   void resetFecGrps() {_fecGroups.clear();}
   void push_grp(FECGroup* i) {_fecGroups.push_back(i);}
   //util func
   bool lexOptions(const string& option, vector<string>& tokens,size_t nOpts) const;
   bool myStr2Int(unsigned& num, const string& str);
   
   // Member functions about circuit construction
   bool readCircuit(const string&);
   void setComment(string& str){comment.clear();comment = str;}
   string getComment() const {return comment;}
   void resetMark() const;
   void pushGate(CirGate*gate) {DfsList.push_back(gate);} 
   // Member functions about circuit optimization
   void buildDfsList();
   void sweep();
   void optimize();
   void buildDfs(CirGate*) ; 
   void replaceGate_same(CirGate*, bool);
   void replaceGate_0(CirGate*);

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   void simGateValue(vector<unsigned>);
   void FecGrpInit(bool&);
//   void simulate(vector<FECGroup*>&, HashMap<FecKey,FECGroup>&);
//   static bool grpOrderSort(FECGroup* i, FECGroup* j)
//   {return (i -> getFirstGate() -> getId() < j -> getFirstGate() -> getId());}
//   void sortFecGrps(vector<FECGroup*>& _l) {std::sort(_l.begin(),_l.end(),grpOrderSort);}
   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void Dfs(CirGate*, vector<string>&, bool) const;
   void writeGate(ostream&, CirGate*) const;
   void buildWriteDfs(CirGate*,vector<CirGate*>) const;
   vector<unsigned> ingate;
private:
   ofstream           *_simLog;
   vector<CirGate*>   DfsList;
   unsigned header[5];
   GateList gatelist;
   vector<FECGroup*> _fecGroups;
   string comment;
   bool firstSim ;
};

class FECGroup
{
public:
    FECGroup() {}
    ~FECGroup() {}

    //access function

    //manipulate function
    void addGate(CirGate* gate)
    {
        group_gatelist.push_back(gate);
    }
    void eraseGate(size_t i) {group_gatelist.erase(group_gatelist.begin() +i);}
    CirGate* getFirstGate() {return group_gatelist[0];}
    CirGate* getLastGate() {return group_gatelist[group_gatelist.size()-1];}
    vector<CirGate*> getGatelist() {return group_gatelist;}
    CirGate* getGate(size_t i){return group_gatelist[i];}
    size_t gatelistSize() {return group_gatelist.size();}
    bool isInv(unsigned i) const {return((size_t)group_gatelist[i]& 0x1); }
private:
    vector<CirGate*> group_gatelist;
};
#endif // CIR_MGR_H
