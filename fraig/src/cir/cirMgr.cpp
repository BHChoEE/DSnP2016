/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <sstream>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   utility function   */
/**************************************************************/
bool
CirMgr::myStr2Int(unsigned& num, const string& str)
{
    bool valid = false; 
    num = atoi(str.c_str());
    valid = true;
    return valid;
}

bool
CirMgr::lexOptions(const string& option, vector<string>& tokens, size_t nOpts) const
{
    string token;
    size_t n = myStrGetTok(option, token);
    while(token.size())
    {
        tokens.push_back(token);
        n = myStrGetTok(option, token, n);
    }
    return true;
}

void
CirMgr::resetMark() const
{
    for(unsigned i = 0; i < header[0] + header[3] + 1; i++)
    {
        CirGate* gate = getGate(i);
        if(gate != 0)
            gate -> setMark(0);            
    }
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
    bool err = true;
    
    //get all command out of file
    ifstream file(fileName.c_str());
    if(!file.is_open())
    {
        cerr<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
        return false;
    }
    string tmp;
    vector<string> all;
    while(getline(file,tmp,'\n'))
        all.push_back(tmp);
    file.close();

    //get first line aag
    vector<string> headline;
    err = lexOptions(all[0], headline,6);
    if(headline[0][0] != 'a')
        return false;
    for(size_t i = 1 ; i < 6 ;i++)
    {
        unsigned num = 1;
        err = myStr2Int(num,headline[i]);
        header[i-1] = num;
    }
    
    //reset environment
    gatelist.clear();
    gatelist.resize(header[0] + header[3] + 1, 0);

    //construct const gate
    gatelist[0] = new CirConstGate(0);
    
    //construct input gate
    for(unsigned i = 0 ; i < header[1] ; i++)
    {
        unsigned lineNo = 0;
        err = myStr2Int(lineNo , all[i+1]);
        unsigned var = (((unsigned)(lineNo)) >> 1);
        ingate.push_back(var);
        gatelist[var] = new CirPiGate(var, i+2);
    }

    //construct AIG gate
    for(unsigned i = 0; i <header[4]; i++)
    {
         unsigned lineNo = i + header[1] + header[3] + 2;
         string tmpo = all[lineNo -1];
         unsigned x[3] = {0,0,0};
         vector<string> tmp;
         err = lexOptions(tmpo,tmp,3);
         for(unsigned j = 0 ; j < 3;j++)
         {
             err = myStr2Int(x[j],tmp[j]);
             x[j] = (((unsigned)(x[j])) >> 1); //literal to variable
          }
          CirGate* gate = new CirAigGate(x[0],lineNo);
          gatelist[x[0]] = gate;
     }

    //check and construct every floating gate as UNDEF gate
    for(unsigned i = 0 ; i < header[4] ; i++)
    {
        string tempstr = all[i+header[1]+header[3]+1];
        unsigned literal[3] = {0,0,0};
        unsigned x[3] = {0,0,0};
        vector<string> tmp;
        err = lexOptions(tempstr,tmp,3);
        for(int i = 0 ; i < 3; i++)
        {
            err = myStr2Int(literal[i],tmp[i]);
            x[i] = (((unsigned)(literal[i])) >> 1);//literal to variable
        }             
        
        CirGate* gate1 = getGate(x[0]);
        CirGate* gate2 = getGate(x[1]);
        CirGate* gate3 = getGate(x[2]);
                     
        if(gate2 == 0)
            gate2 = gatelist[x[1]] = new CirUndefGate(x[1],0);
        if(gate3 == 0)
            gate3 = gatelist[x[2]] = new CirUndefGate(x[2],0);
        
        // input output gate defined
        gate1 ->addInput(gate2,(((unsigned)(literal[1]))&1 ));
        gate1 ->addInput(gate3,(((unsigned)(literal[2]))&1 ));
        gate2 ->addOutput(gate1,(((unsigned)(literal[1]))&1));
        gate3 ->addOutput(gate1,(((unsigned)(literal[2]))&1));
    }

    //construct ouput gate PO
    for(unsigned i = 0 ; i < header[3]; i++)
    {
        unsigned literal = 0;
        unsigned lineNo = i + header[1] + 2;
        err = myStr2Int(literal, all[lineNo -1]);
        CirPoGate* gate = new CirPoGate(i+header[0]+1,lineNo);
        unsigned variable =  ((unsigned)(literal)) >> 1;
        CirGate* _prev = getGate(variable);
        if(_prev == 0)
            _prev = gatelist[variable] = new CirUndefGate(variable,0);
        gate -> addInput(_prev,((unsigned)(literal))&1);
        gatelist[i+header[0]+1] = gate;
        _prev -> addOutput(gate,((unsigned)(literal))&1);
    }

    //define name of gates and tailing comment
    for(unsigned i = header[1]+header[3]+header[4]+1;i<all.size();i++)
    {
        // comment start with char c
        string tmpstr  = all[i];
        if(tmpstr == "c")
        {
            if(i+1 == all.size())
                break;
            string comment = all[i++];
            while(i != all.size() -1)
            {
                comment += '\n';
                comment += all[i++];
            }
            setComment(comment);
            break;                  
        }                     
         
        // symbol
        vector<string> tmp;
        err = lexOptions(tmpstr , tmp , 2);
        string str = tmp[1];
        unsigned id = 0;
        err = myStr2Int(id,tmp[0].substr(1));
        char gateType = tmp [0][0];
        if(gateType == 'i')
        {
            CirGate* gate = getGate( ingate[id] );
            CirPiGate* pigate = (CirPiGate*) gate;
            pigate -> setName( str );
        }
        else if(gateType == 'o')
        {
            CirGate* gate = getGate( id + header[0] + 1 );
            CirPoGate* pogate = (CirPoGate*) gate;
            pogate -> setName( str );
        }
    }
    //for(size_t i = 0 ; i < gatelist.size(); i++)
    //    cout<<gatelist[i]->getId()<<endl;
    firstSim = true;
    return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
     unsigned total = header[1] + header[3] + header[4];
     cout<<'\n';
     cout<<"Circuit Statistics"<<'\n';
     cout<<"=================="<<'\n';
     cout<<setw(2)<<left<<"PI" <<setw(12)<<right<<header[1]<<'\n';
     cout<<setw(2)<<left<<"PO" <<setw(12)<<right<<header[3]<<'\n';
     cout<<setw(2)<<left<<"AIG"<<setw(11)<<right<<header[4]<<'\n';
     cout<<"------------------"<<'\n';
     cout<<setw(2)<<left<<"Total"<<setw(9)<<right<<total<<'\n';
}

void
CirMgr::printNetlist() const
{
    resetMark();
    cout<<'\n';
    bool flag = 1;
    for(unsigned i = header[0] +1; i < header[0]+header[3]+1 ; i++)
    {
        CirGate* gate = getGate(i);
        if(gate != 0)
        {
            if(gate -> getType() == PO_GATE)
            {
                if(flag)
                {
                    gate -> resetGlobalref();
                    flag = 0;
                }
                gate -> printGate();
            }
        }
    }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(unsigned i = 0 ; i < ingate.size() ; i++)
   {
        cout<<' '<<ingate[i];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(unsigned i = 0 ; i < header[3] ; i++)
   {
        CirGate* gate = getGate(i + header[0] + 1);
        if(gate != 0 && gate -> getType() == PO_GATE)
            cout<<' '<<gate -> getId();
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
    vector<unsigned> floating;
    vector<unsigned> unused;
    //getting floating gate ( AIG or PO )
    for(unsigned i = header[1] + 1; i < header[0] + header[3] +1 ; ++i)
    {
        CirGate* gate = getGate(i);
        // id not used
        if(gate == 0)
            continue;
        if(gate -> getType() == PO_GATE)
        {
            if(gate -> getInputGate(0) -> getType() == UNDEF_GATE )
                floating.push_back(gate -> getId());
        }
        else if(gate -> getType() == AIG_GATE)
        {
            if(gate -> getInputGate(0) -> getType() == UNDEF_GATE)
                floating.push_back(gate -> getId());
            else if(gate -> getInputGate(1) -> getType() == UNDEF_GATE)
                floating.push_back(gate -> getId());
        }
    }
    
    //print out floating gate
    if(!floating.empty())
    {
        cout<<"Gates with floating fanin(s):";
        for(unsigned i = 0 ; i < floating.size() ; ++i)
            cout<<" "<<floating[i];
        cout<<'\n';
    }
    
    //getting unused gate
    for(unsigned i = 0; i < header[0] + 1; ++i)
    {
        CirGate* gate = getGate(i);
        
        // id not used
        if(gate == 0)
            continue;
        if(gate -> getType() == PI_GATE)
        {
            if(gate -> fanoutSize() == 0)
                unused.push_back(gate -> getId());
        }
        else if(gate -> getType() == AIG_GATE)
        {
            if(gate -> fanoutSize() == 0)
                unused.push_back(gate -> getId());
        }
    }

    //print out unused gate
    if(!unused.empty())
    {
        cout<<"Gate defined but not used :";
        for(unsigned i = 0 ; i < unused.size() ; ++i)
            cout<<" "<<unused[i];
        cout<<'\n';
    }
}

void
CirMgr::printFECPairs() const
{
    for(size_t i = 0 ; i < _fecGroups.size() ; i++)
    {
        FECGroup* group = _fecGroups[i];
        cout<<"["<<i<<"]";
        for(size_t j = 0 ; j < group -> gatelistSize() ; j++)
        {
            CirGate* gate = group -> getGate(j);
            bool inv = group -> isInv(j) ^ group -> isInv(0);
            cout <<" ";
            if(inv)
                cout<<"!";
            cout<<gate -> getId();
        }
        cout <<endl;
    }
}

void
CirMgr::writeAag(ostream& outfile) const
{

    vector<string> aig;
    vector<unsigned> outgate;
    
    //consturct gate
    resetMark();
    for(unsigned i = header[0]+1 ; i < header[0] + header[3] +1; i++)
    {
        CirGate* gate = getGate(i);

        //jump the blank block
        if(gate == 0)
            continue;
        if(gate -> getType() == PO_GATE)
        {
            Dfs(gate, aig, gate -> faninInv(0));
            unsigned faninlit = ((unsigned)(gate -> getInputGate(0) -> getId()) *2 );
            if(gate -> faninInv(0))
                faninlit ++;
            outgate.push_back(faninlit);
        }
    }
    
    //print out
    outfile << "aag " << header[0] <<" "<< header[1] <<" "<< header[2] <<" "    <<header[3] << " " << header[4] <<endl;
                     
    //pi
    for(unsigned i = 0 ; i < ingate.size(); i++)
        outfile << ((unsigned)(ingate[i]) *2) <<endl;
    
    //po
    for(unsigned i = 0 ; i < outgate.size() ; i++)
        outfile << outgate[i] <<endl;
    
    //aig
    for(unsigned i = 0 ; i < aig.size() ; i++)
    {
        outfile << aig[i]<< endl;
    }
    

    //comment pi 
    for(unsigned i = 0 ; i < ingate.size() ; i++)
    {
        CirPiGate* gate= (CirPiGate*) gatelist[ ingate[i] ];
        if(gate -> getName() != "")
            outfile << "i"<<i<<" "<< gate -> getName() <<endl;
    }
    
    //comment po
    unsigned onum = 0;
    for(unsigned i =  1; i < header[0] + header[3] +1; i++)
    {
        CirGate* gate = getGate(i);
        if(gate -> getType() == PO_GATE)
        {
            CirPoGate* pogate = (CirPoGate*) gate;
            if(pogate -> getName() != "")
                outfile <<"o"<<onum<<" "<< pogate -> getName() << endl;
            onum++;
        }
    }
    
    //comment
    cout<<"c"<<endl;
    cout<<"AAG output by Chung-Yang (Ric) Huang"<<endl;
}


void
CirMgr::Dfs(CirGate* gate,vector<string>& aig,bool invert) const
{
    if(gate -> getMark() != 0)
        return;

    //Dfs
    for(unsigned i = 0 ; i < gate -> faninSize(); ++i)
        Dfs(gate -> getInputGate(i), aig , gate -> faninInv(i));
    if(gate -> getType() == AIG_GATE)
    {
        stringstream str;
        unsigned gatelit = ((unsigned)(gate -> getId()) *2);
        str << gatelit << " ";
        unsigned fanin0lit = ((unsigned)(gate -> getInputGate(0) -> getId())     *2);
        if(gate -> faninInv(0))
            fanin0lit ++;
        str << fanin0lit << " ";
        unsigned fanin1lit = ((unsigned)(gate -> getInputGate(1) -> getId())     *2);
        if(gate -> faninInv(1))
            fanin1lit ++;
        str << fanin1lit;
        aig.push_back(str.str());
    }
    gate -> setMark(1);
}
void
CirMgr::buildWriteDfs(CirGate* gate,vector<CirGate*> wDfsList) const
{
    if(gate -> getMark() != 0)
        return;
    for(unsigned i = 0 ; i < gate -> faninSize() ; i++)
    {
        CirGate* inputGate = gate -> getInputGate(i);
        if(gate != 0)
            buildWriteDfs(inputGate,wDfsList);
    }
    wDfsList.push_back(gate);
    gate -> setMark(1);
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
    vector<unsigned> Input;
    vector<CirGate*> Aig;
    vector<CirGate*> Output;
    vector<CirGate*> wDfsList;
    unsigned max_id = 0;
    //reset environment
    resetMark();
    
    //buildDfsList
    if(g != 0)
        buildWriteDfs(g,wDfsList);
    for(size_t i = 0 ; i < wDfsList.size() ; i++)
    {
        CirGate* gate = wDfsList[i];
        if(gate -> getType() == PI_GATE)
            Input.push_back(gate -> getId());
        else if(gate -> getType() == AIG_GATE)
            Aig.push_back(gate);
        else if(gate -> getType() == PO_GATE)
            Output.push_back(gate);
    }
    //sort Input by magnitude
    std::sort(Input.begin(),Input.end());
    for(size_t i = 0 ; i < wDfsList.size() ; i++)
        if(wDfsList[i] -> getId() > max_id)
            max_id = wDfsList[i] -> getId();

    //print out
    outfile << "aag " << max_id <<" "<< Input.size() <<" 0 " << Output.size() << " " << Aig.size() <<endl;
                     
    //pi
    for(unsigned i = 0 ; i < Input.size(); i++)
        outfile << ((unsigned)(Input[i]) *2) <<endl;
    
    //po
    for(unsigned i = 0 ; i < Output.size() ; i++)
        outfile << ((unsigned)(Output[i] -> getId())*2) <<endl;
    
    //aig
    for(unsigned i = 0 ; i < Aig.size() ; i++)
    {
        outfile << ((unsigned)(Aig[i]->getId())*2)<<" "<<((unsigned)(Aig[i] -> getInputGate(0) -> getId())*2)<<" ";
        outfile << ((unsigned)(Aig[i] -> getInputGate(1) -> getId())*2)<< endl;
    }
    //comment pi 
    for(unsigned i = 0 ; i < Input.size() ; i++)
    {
        CirPiGate* gate= (CirPiGate*) gatelist[ ingate[i] ];
        if(gate -> getName() != "")
            outfile << "i"<<i<<" "<< gate -> getName() <<endl;
    }
    //comment po
    CirPoGate* pogate = (CirPoGate*) Output[0];
    outfile <<"o0 "<< pogate -> getId() << endl;
           
}
