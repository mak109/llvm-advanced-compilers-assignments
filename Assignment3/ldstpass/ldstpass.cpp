

#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <list>
#include <iostream>
#include <fstream>

#include <list>


using namespace llvm;

namespace {
    
    struct ldstpass : public ModulePass {
        static char ID;
        ldstpass() : ModulePass(ID) {}
        virtual bool  runOnModule(Module &M) override {
            Instruction *IP;
            //To store the reference of function printLL() and printSS()
            Function *FuncL = NULL,*FuncS = NULL ;
            //Get the type reference of unsigned int
            Type *Typ = Type::getInt32Ty(M.getContext());
            FuncL = M.getFunction("printLL"); //printLL() reference
            FuncS = M.getFunction("printSS"); //printSS() reference
            CallInst* callL ,*callS; //call inst for ld/st        
          for (Module::iterator F = M.begin(), E = M.end(); F!= E; ++F) 
          {
            //Skipping printSS() or printLL()
            if(!F->getName().equals("printLL") && !F->getName().equals("printSS"))
            {
            
            for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
            {
                for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI)
                {
                    //Code to insert call instruction after every load/store
                    IP =  llvm::dyn_cast<llvm::Instruction>(BI);
                    if(BI->getOpcode() == Instruction::Load)
                    {
                         Value *v = IP->getOperand(0);
                         //Inserting a cast instruction to pass by value address as unsigned int
                         PtrToIntInst *ptoi = new PtrToIntInst(v,Typ,"",IP);
                         IP->moveBefore(ptoi);
                         ArrayRef<Value *> args(ptoi);
                         callL = CallInst::Create(FuncL,args,"",ptoi);
                         ptoi->moveBefore(callL);
                        
                    }
                    else if(BI->getOpcode() == Instruction::Store)
                    {
                        
                         Value *v = IP->getOperand(1);
                          //Inserting a cast instruction to pass by value address as unsigned int
                         PtrToIntInst *ptoi = new PtrToIntInst(v,Typ,"",IP);
                         IP->moveBefore(ptoi);
                         ArrayRef<Value *> args(ptoi);
                         callS = CallInst::Create(FuncS,args,"",ptoi);
                         ptoi->moveBefore(callS);
                        
                        
            
                    }
                }
            }
            }
           
            
          }
           //Printing the modified module to console
        for(auto &F : M)
            {
                errs()<<F.getName()<<"\n";
                for(auto &B : F)
                    errs()<<B<<"\n";
            }
          //Creatig new .ll file containing modified ll file
          std:: string str;
          llvm::raw_string_ostream(str) << M;
          std::ofstream outfile("testcases/modified.ll");
          outfile<<str;
    return true;
  }
}; // end of struct new Pass
}  // end of anonymous namespace
char ldstpass::ID = 0;
static RegisterPass<ldstpass> X("ldst", "LD ST PASS",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);


static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new ldstpass()); });
