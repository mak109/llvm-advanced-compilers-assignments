#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

// bin/opt -load lib/LLVMnewPass.so -newPass < lib/hello.ll > /dev/null 

using namespace llvm;

namespace {
    struct newPass : public ModulePass {
        static char ID;
        newPass() : ModulePass(ID) {}
        virtual bool runOnModule(Module &M)  {
          long numFun  = 0; //counting number of functions
          //Iterating through each function in Module
          for(auto &F : M){
          	//Checking for only defined functions
            if(!F.isDeclaration())
            {
              SymbolTableList<BasicBlock>  &BasicBlocks = F.getBasicBlockList();  //Getting the Basic blocks of the function
             //Printing name arity and number of basic blocks of each function
              errs()<< "function name : " << F.getName() <<"\t"<< "Arity : "<<F.arg_size() << "\tNumber of basic blocks : "<<BasicBlocks.size() << '\n';
              numFun++;
            }
          }
    errs() << "Number of Functions : "<<numFun << '\n';
    return false;
  }
}; // end of struct new Pass
}  // end of anonymous namespace
char newPass::ID = 0;
static RegisterPass<newPass> X("newp", "new Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);


static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new newPass()); });
