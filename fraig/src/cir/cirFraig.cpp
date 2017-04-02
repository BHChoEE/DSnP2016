/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
    buildDfsList();
    HashMap<HashKey, CirGate*> hashMap (DfsList.size() * 2);
    for(size_t i = 0 ; i < DfsList.size(); i++)
    {
        CirGate* gate = DfsList[i];
        if(gate -> getType() == AIG_GATE)
        {
            CirGate* fanin[2] = {gate->getInputGate(0),gate->getInputGate(1)};
            size_t id0 = 2*(fanin[0] -> getId());
            size_t id1 = 2*(fanin[1] -> getId());
            if(gate->faninInv(0))
                id0++;
            if(gate->faninInv(1))
                id1++;
            HashKey key (id0,id1);
            CirGate* mergeGate = 0;
            //find if same gate is in hashmap
            if(hashMap.query(key,mergeGate))
            {
                cout << "Strashing: "<< mergeGate -> getId() << " merging ";
                cout << gate -> getId() <<"..." << endl;
                //delete fanout's fanin
                for(size_t j = 0 ; j < gate -> fanoutSize() ; j++)
                {
                    CirGate* fanout = gate -> getOutputGate(j);
                    for(size_t k = 0 ; k < fanout -> faninSize() ; k++)
                        if(fanout -> getInputGate(k) == gate)
                            fanout -> eraseInGate(k);
                    fanout -> addInput(mergeGate);
                }
                //delete fanin/s fanout
                CirGate* fanin[2] = {gate->getInputGate(0),gate->getInputGate(1)};
                for(size_t j = 0 ; j < fanin[0] -> fanoutSize() ; j++)
                    if(fanin[0]->getOutputGate(j) == gate)
                        fanin[0] -> eraseOutGate(j);
                for(size_t j = 0 ; j < fanin[1] -> fanoutSize() ; j++)
                    if(fanin[1] -> getOutputGate(j) == gate)
                        fanin[1] -> eraseOutGate(j);
                //delete gate
                header[4]--;
                gatelist[gate -> getId()] = 0;
                delete gate;
            }
            else
                hashMap.forceInsert(key,gate);
        }
    }
   // buildDfsList();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
