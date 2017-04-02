/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{   
    //to decide whether gate should be sweeped(0), not be(1)
    bool sweep[gatelist.size()];
    for(size_t i = 0 ; i < gatelist.size() ;i++)
    {
        sweep[i] = 0;
    }
    vector<CirGate*> delList;

    //construct DFS list
    buildDfsList();
  
    //CONST and PI must not be sweeped
    sweep[0] = 1;
    for(size_t i = 0 ; i < ingate.size() ; i++)
    {
        size_t id = ingate[i];
        sweep[id] = 1;
    }
    //gate in dfsList should not be sweeped
    for(size_t i = 0 ; i < DfsList.size() ; i++)
    { 
        size_t id = DfsList[i]->getId();
        sweep[id] = 1;
    }
     
    //delete gate not connected(sweep = 0)
    for(size_t i = 0 ; i < gatelist.size() ; i++)
    {
    //    cout<<sweep[i]<<" "<<gatelist[i]-> getId() <<endl;
        if(sweep[i])//delete fanin / fanout if it should be sweeped
        {
            for(size_t j = 0 ; j < gatelist[i] -> fanoutSize() ; j++)
            {
                CirGate* fanout = gatelist[i] -> getOutputGate(j);
                if(!sweep[fanout->getId()])//if fanout = 0
                {
                    gatelist[i] -> eraseOutGate(j);
                    --j;
                }
            }
        }
        
        //gate not be reached
         if(!sweep[i] && gatelist[i] && gatelist[i] -> getType() == AIG_GATE)
        {
            header[4] --;
            delList.push_back(gatelist[i]);
            cout<<"Sweeping: ";
            cout<< gatelist[i]->getTypeStr()<<"("<<i<<")";
            cout<<"removed..."<<endl;
            gatelist[i] = 0;
        }
        //gate UNDEF
        else if(!sweep[i] && gatelist[i])
        {
            delList.push_back(gatelist[i]);
            cout<<"Sweeping: ";
            cout<< gatelist[i]->getTypeStr()<<"("<<i<<")";
            cout<<"removed..." <<endl;
            gatelist[i] = 0;
        }
    }
    for(size_t i = 0 ; i < delList.size() ; i++)
        delete delList[i];
}
void
CirMgr::buildDfsList()
{    
    DfsList.clear();
    resetMark();
    for(unsigned i = header[0] ; i < header[0] + header[3] + 1 ; i ++)
    {   
        CirGate* gate = getGate(i);
        if(gate != 0)
            if(gate -> getType() == PO_GATE)
            {
                buildDfs(gate);
            }
    }           
}

void
CirMgr::buildDfs(CirGate*gate)
{
    if(gate -> getMark() != 0)
        return;
    for(unsigned i = 0 ; i < gate -> faninSize() ; i++)
    {
        CirGate* inputGate = gate -> getInputGate(i);
        if(gate != 0)
            buildDfs(inputGate);
    }
    DfsList.push_back(gate);
    gate -> setMark(1);
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    vector<CirGate*> delList;
    //construct DFSlist
    buildDfsList();
    
    //run through DfsList to delete redundant gate
    for(size_t i = 0 ; i < DfsList.size() ; i++)
    {
        CirGate* gate = DfsList[i];
        CirGate* fanin[2] = { gate -> getInputGate(0), gate -> getInputGate(1) };
        //case of gate is not AIG did not considered in this case 
        if( gate -> getType() != AIG_GATE)
            continue;
        else
        {
            //(a)one of fanin is CONST1,delete it and replace by another fanin
            if(fanin[0]-> getType() == CONST_GATE && gate -> faninInv(0))
            {
                cout << "Simplifying: " << fanin[1]->getId() << " merging " << (gate->faninInv(1) ? "!":"");
                cout << gate -> getId() << "..." << endl;
                replaceGate_same(gate,1);
                delList.push_back(gate);
            }
            else if(fanin[1]->getType() == CONST_GATE && gate -> faninInv(1))
            {
                cout << "Simplifying: " << fanin[0]->getId() << " merging " << (gate->faninInv(0) ? "!":"");
                cout << gate -> getId() << "..." << endl;
                replaceGate_same(gate,0);
                delList.push_back(gate);
            }
            //(b)both fanin are same,delete ir and replace by one fanin
            else if(fanin[0] == fanin[1] && gate -> faninInv(0) == gate -> faninInv(1))
            {
                cout << "Simplifying: " << fanin[0]->getId() << " merging " << (gate->faninInv(0) ? "!":"");
                cout << gate -> getId() << "..." << endl;
                replaceGate_same(gate,0);
                delList.push_back(gate);
            }
            //(c)one of fanin is CONST0,delete it and replace by a CONST0
            else if(fanin[0]->getType() == CONST_GATE && !gate -> faninInv(0) )
            {
                cout << "Simplifying: " << fanin[0]->getId() << " merging ";
                cout << gate -> getId() << "..." << endl;
                replaceGate_0(gate);
                delList.push_back(gate);
            }
            else if(fanin[1]->getType() == CONST_GATE && !gate -> faninInv(1) )
            {
                cout << "Simplifying: " << fanin[1]->getId() << " merging " << "!";
                cout << gate -> getId() << "..." << endl;
                replaceGate_0(gate);
                delList.push_back(gate);
            }
            //(d)fanin are opposite,delete both and add CONST0
            else if(fanin[0] == fanin[1] && gate -> faninInv(0) != gate -> faninInv(1))
            {
                cout << "Simplifying: " << fanin[1]->getId() << " merging " << (gate->faninInv(1) ? "!":"");
                cout << gate -> getId() << "..." << endl;
                replaceGate_0(gate);
                delList.push_back(gate);
            }
            //not in case
            else 
                continue;
        }
    }
    // delete nodes
    for(size_t i = 0 ; i < delList.size() ; i++)
    {
        gatelist[delList[i] -> getId()] = 0;
        header[4] --;
        delete delList[i];
    }

}

void
CirMgr::replaceGate_same(CirGate*gate,bool rep)
{
    //change fanin of gate's fanout to replace gate
    for(size_t i = 0 ; i < gate -> fanoutSize() ; i++)
    {
        CirGate* fanout = gate -> getOutputGate(i);
        if(fanout -> getInputGate(0) == gate)
            fanout->eraseInGate(0);
        else if(fanout -> getInputGate(1) == gate)
            fanout->eraseInGate(1);
        fanout->addInput(gate->getInputGate(rep),gate -> faninInv(rep));
    }
    //change replace gate's fanout
    CirGate* fanin = gate -> getInputGate(rep);
    for(size_t i = 0 ; i < fanin -> fanoutSize() ; i++)//erase gate
        if(fanin -> getOutputGate(i) == gate)
            fanin->eraseOutGate(i);
    for(size_t i = 0 ; i< gate -> fanoutSize() ; i++)
        fanin->addOutput(gate->getOutputGate(i),gate->fanoutInv(i));
    CirGate* another = gate -> getInputGate(!rep);
    for(size_t i = 0 ; i < another -> fanoutSize() ; i++)
        if(another->getOutputGate(i) == gate)
            another->eraseOutGate(i);
}

void
CirMgr::replaceGate_0(CirGate* gate)
{
    CirGate* fanin[2] = {gate->getInputGate(0),gate->getInputGate(1)};
    //change fanin of gate's fanout to CONST0
    for(size_t i = 0 ; i < gate -> fanoutSize() ; i++)
    {
        CirGate* fanout = gate -> getOutputGate(i);
        if(fanout -> getInputGate(0) == gate)
            fanout -> eraseInGate(0);
        else if(fanout -> getInputGate(1) == gate)
            fanout -> eraseInGate(1);
        fanout -> addInput(gatelist[0],0);
    }
    //change fanout of gate's fanin
    for(size_t i = 0 ; i < fanin[0]->fanoutSize(); i++)
        if(fanin[0] -> getOutputGate(i) == gate)
            fanin[0] -> eraseOutGate(i);
    for(size_t i = 0 ; i < fanin[1]->fanoutSize(); i++)
        if(fanin[1] -> getInputGate(i) == gate)
            fanin[1] -> eraseOutGate(i);
    for(size_t i = 0 ; i < gate ->fanoutSize() ;i++)
        gatelist[0] -> addOutput(gate -> getOutputGate(i), 0);
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
