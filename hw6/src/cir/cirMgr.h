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

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   //constructor & destructor
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
   //util func
   bool lexOptions(const string& option, vector<string>& tokens,size_t nOpts) const;
  // void getToken(const string& option, vector<string>& options);
   bool myStr2Int(unsigned& num, const string& str);

   // Member functions about circuit construction
   bool readCircuit(const string&);
   void setComment(string & str){comment.clear();comment = str;};
   string getComment() const {return comment;};
   
   void resetMark() const;
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   void Dfs(CirGate*, vector<string>&, bool) const;
   vector<unsigned> ingate; 
private:
   //aag M I L O A == aag header[0][1][2][3][4]
   unsigned header[5];
   GateList gatelist;//gate between
   string comment;//tailing comment
};

#endif // CIR_MGR_H
