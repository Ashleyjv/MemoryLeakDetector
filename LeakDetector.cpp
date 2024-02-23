
//works
//fin
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/Operator.h"

#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <typeinfo>
#include <vector>

/*
 * When you debug, please use this flag, 
 *  e.g., if (DEBUG) : outs << ... << '\n';
 * Turn this off when you submit
 */ 
bool DEBUG = false;
bool DEBUG2 = false;
using namespace llvm;


std::list<std::list<BasicBlock *>> paths;
// or std::list<std::list<Instruction *>> paths;
// std::list<std::list<int> intPaths;
std::list<unsigned> lineNumbers;
void DiscoverPaths(std::list<BasicBlock *> path, BasicBlock *bb) 
{

    // path.push_back(bb);
    
    // if (path.empty() || path.back() != bb) {
        path.push_back(bb);
    // }
    int bbCount = std::count(path.begin(), path.end(), bb);
    // if (bbCount > 2) {
    //     // outs() << "Reached bbCount check\n";
    //     // loopCount += bbCount - 1;
    //     return;
    // }
    
    // if (loopCount > 2) { //change back to 2
    //     // outs() << "Reached loopCount check\n";
    //     if (bbCount > 1) {
    //         // outs() << "valid\n";
    //         loopCount -= bbCount - 1; // Decrement loopCount for backtracking
    //     }
    //     return;
    // }

    bool hasSuccessors = false;

    for (Instruction& i : (*bb)) {
        BranchInst *BI = dyn_cast<BranchInst>(&i);
        if (BI) {
            hasSuccessors = true;
            
            for (unsigned j = 0; j < BI->getNumSuccessors(); j++) {
                BasicBlock *target = BI->getSuccessor(j);
                int targetCount = std::count(path.begin(), path.end(), target);
                
                if (bbCount == 3 && targetCount == 2) {
                    continue;
                }
                else {
                    DiscoverPaths(path, target);
                }
            }
        }
    }

    if (!hasSuccessors) {
        paths.push_back(path);
    }

    return;
}

std::string valueToString(llvm::Value* val) {
    std::string valueStr;
    llvm::raw_string_ostream rso(valueStr);
    // value->getOperand(0);
    val->print(rso);
    return rso.str(); // This will hold the string representation of the value
}

bool MemoryLeakDetection(std::list<BasicBlock *> path)
{
    llvm::Value* prevLoadInstr = NULL;
    bool mallocFlag = false;
    // std::set<llvm::Value*> allocatedMemories;
    std::set<std::string> allocatedMemories;
    llvm::Value* currMallocAddr = NULL;
    llvm::Value * malloc = NULL;
    llvm::Value * freed = NULL;
    std::set<llvm::Value*> mallocSet;
    std::set<llvm::Value*> freeSet;
    std::map<llvm::Value*, std::vector<llvm::Value*>> assignmentsMap;
    for (auto bb: path) {
        for (Instruction& i : *bb) {

            LoadInst *LI = dyn_cast<LoadInst>(&i);
            if(LI){
                // write your code
                bool flag = false;
                // prevLoadInstr = LI->getOperand(0);
                // outs() << "Loaded : " << LI->getOperand(0) << "\n";
                if(DEBUG){
                    outs() << "[DEBUG][LOAD] "  << i << "\n";
                    outs() << "[DEBUG][LOAD] "  << LI->getOperand(0) << "\n";
                }

                ConstantExpr *CE = dyn_cast<ConstantExpr>(LI->getOperand(0));
                // outs() << "CONST EXPR " << CE->getOperand(0) << "\n";
                if(CE){
                    GetElementPtrInst *GEPI = dyn_cast<GetElementPtrInst>(CE->getAsInstruction(&i));
                    
                    if(GEPI){ 

                        // here comes your analysis code 
                        // outs() << "[LOAD][GEPI] " << GEPI->getOperand(0) << " " << GEPI->getOperand(1) << " " << GEPI->getOperand(2) << "\n";
                        prevLoadInstr = GEPI->getOperand(2);
                        flag = true;

                    }
                }
                if (flag == false) {
                    prevLoadInstr = LI->getOperand(0);

                }
            }
            

            StoreInst *SI = dyn_cast<StoreInst>(&i);
            if(SI){ 
                
                // Write your code 

                // Example of accessing to getOperand()
                if(DEBUG){
                    outs() << "[DEBUG][STORE] "  << i << "\n";
                    outs() << "[DEBUG][STORE] "  << "[INSTR_ADDR]" <<SI->getOperand(0) << " [VALU_ADDR] "<< SI->getOperand(1) << "\n";
                }
                if (malloc && SI->getOperand(0) == malloc) {
                    //malloc
                    bool flag = false;
                    // outs() << "mallocing NOW " << SI->getOperand(1) << "\n";
                    // allocatedMemories.insert(SI->getOperand(1)); //note inserting (1) = addr(x) || (0) = %2
                    // allocatedMemories.insert(SI->getOperand(1).str());
                    ConstantExpr *CE = dyn_cast<ConstantExpr>(SI->getOperand(1));
                    if(CE){
                        GetElementPtrInst *GEPI = dyn_cast<GetElementPtrInst>(CE->getAsInstruction(&i));
                        
                        if(GEPI){ 
                            // here comes your analysis code 
                            // outs() << "[STORE][GEPI] " << GEPI->getOperand(0) << " " << GEPI->getOperand(1) << " " << GEPI->getOperand(2) << "\n";
                            llvm::Value *val = GEPI->getOperand(2); // Get the second operand
                            // outs() << "ADDING TO MALLOCSET " << val << "\n";
                            mallocSet.insert(val); // Insert the operand into the set
                            malloc = NULL;
                            flag = true;
                        }
                    }
                    if (flag == false) {
                        llvm::Value *val = SI->getOperand(1); // Get the second operand
                        mallocSet.insert(val); // Insert the operand into the set
                        malloc = NULL;
                    }
                    
                }

                else {
                    //assignments
                    //assignments can also have gepi
                    bool assigned = false;
                    ConstantExpr *CE = dyn_cast<ConstantExpr>(SI->getOperand(1));
                    if(CE){
                        GetElementPtrInst *GEPI = dyn_cast<GetElementPtrInst>(CE->getAsInstruction(&i));
                        
                        if(GEPI){ 
                            // outs() << "Assigning to GEPI " << GEPI->getOperand(0) << " " << GEPI->getOperand(1) << "\n";
                            llvm::Value *val = GEPI->getOperand(2);
                            assignmentsMap[GEPI->getOperand(2)].push_back(prevLoadInstr);
                            assignmentsMap[prevLoadInstr].push_back(val);
                            assigned = true;
                        }
                    }
                    if (assigned == false) {
                        llvm::Value *val = SI->getOperand(1);
                        // assignmentsMap[SI->getOperand(1)] = prevLoadInstr;
                        // assignmentsMap[prevLoadInstr] = val;
                        // outs() << "NOT GEPI BUT ASSIGNING " << SI->getOperand(1) << " " << prevLoadInstr << "\n";
                        assignmentsMap[SI->getOperand(1)].push_back(prevLoadInstr);
                        assignmentsMap[prevLoadInstr].push_back(val);
                    }
                    

                }

            }
             
            CallInst * CI = dyn_cast<CallInst>(&i);
            if (CI){ 
                if (CI->getCalledFunction()->getName().find("malloc") != std::string::npos) {
                    // Write your code
                    malloc = ::cast<llvm::Value>(CI);
                    // outs() << "[MALLOC] " << i << "\n";
                    // outs() << "[MALLOC] " << malloc << "\n";
                    mallocFlag = true;

                }
                if (CI->getCalledFunction()->getName().find("free") != std::string::npos) {
                    // Write your code
                    freed = ::cast<llvm::Value>(CI);
                    // outs() << "[FREE] " << i << "\n";
                    // outs() << "[FREE] " << prevLoadInstr << "\n";
                    freeSet.insert(prevLoadInstr);
                }           
            }
         
        }
    }
    // outs() << "Malloc Set {";
    for (llvm::Value* val : mallocSet) {
        // Print each value using LLVM's raw_ostream
        // llvm::outs() << val << ", ";
    }
    // outs() << "}\n";

    // outs() << "Free Set {";
    for (llvm::Value* val : freeSet) {
        // Print each value using LLVM's raw_ostream
        // llvm::outs() << val << ", ";
    }
    // outs() << "}\n";


    for (const auto& pair : assignmentsMap) {
        llvm::Value* key = pair.first;  // The key (llvm::Value*)
        const std::vector<llvm::Value*>& values = pair.second;  // The value (vector of llvm::Value*)

        // Print the key
        // llvm::outs() << "Key: ";
        // outs() << key;
        // key->print(llvm::outs());
        // llvm::outs() << " : Values: ";

        // Iterate over the vector and print each llvm::Value*
        for (llvm::Value* value : values) {
            // value->print(llvm::outs());
            // outs() << value;
            // llvm::outs() << ", ";
        }
        // llvm::outs() << "\n";  // New line after each map entry
    }
    bool returnFlag = false;
    //check for leak
    for (llvm::Value* val : mallocSet) {
        // if val in freeSet : continue
        if (freeSet.find(val) != freeSet.end()) {
            // outs() << "Direct free " << val << "\n";
            continue;
        }
        bool found = false;
        auto it = assignmentsMap.find(val);
        // outs() << "Looking for " << val << " in map \n";
        if (it != assignmentsMap.end()) {
            for (llvm::Value* val2 : it->second) {
                // Do something with 'val'
                // For example, print it
                // outs() << val << " " << val2 << "\n";
                if (freeSet.find(val2) != freeSet.end()) {
                    found = true;
                    // outs() << "FOUND\n";
                }
                
            }
            if (found == false) {
                // outs() << "LEAK\n";
                returnFlag = true;
                break;
            }
        }
        else {
            // outs() << "mem is not in MAP\n";
            returnFlag = true;
            break;
        }
    }
    
    /*
     * return True of False 
     */
    return returnFlag;
}



namespace {
struct LeakDetector : public ModulePass {
  static char ID;
  LeakDetector() : ModulePass(ID) {}
  bool runOnModule(Module &M) override {
    // outs() << "test\n"; 
    for (Function& fun : M){

        // function in external library 
        if (fun.begin() == fun.end())
            continue;

        
        // Empty path and this will grow in DiscoverPaths()
        std::list<BasicBlock *> path;
        path.clear();

        // paths will be a list of list of BBs (or istructions?)    
        int loopCount = 0;
        DiscoverPaths(path, &(fun.front()));
        int pathCounter = 1;
        for (auto p: paths) 
        { 
            bool leaky = MemoryLeakDetection(p);
            if (leaky) {
                outs() << pathCounter << ": ";
                pathCounter++;
                int blockCounter = 0;
                int prev_lineno = -1;
                for (auto bb: p) {
                    blockCounter++;
                    // outs() <<"-BLOCK(" << blockCounter << ")";
                    
                    for (Instruction& i : *bb) {
                        if ((&i)->getDebugLoc()) {
                            int lineno = (&i)->getDebugLoc().getLine();
                            //added
                            if (prev_lineno != lineno) {
                            outs() << lineno << "->";
                            prev_lineno = lineno;
                            }
                        }
                        // ctr++;
                        // I added this close and open { at 158
                            //ended
                            // ...
                    }
                }
                outs() << "end\n";
            }
            // MemoryLeakDetection(p);
        }
        
    
    }
  return false; 
  } 
};
}


char LeakDetector::ID = 0;
static RegisterPass<LeakDetector> X("ldetector", "LeakDetector Pass", 
                             true,
                             true);


static void registerLeakDetectorPass(const PassManagerBuilder &, legacy::PassManagerBase &PM)
{
        PM.add(new LeakDetector());
}

static RegisterStandardPasses RegisterLeakDetectorPass(
    PassManagerBuilder::EP_ModuleOptimizerEarly, registerLeakDetectorPass);

static RegisterStandardPasses RegisterLeakDetectorPass0(
    PassManagerBuilder::EP_EnabledOnOptLevel0, registerLeakDetectorPass);



