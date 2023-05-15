

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


namespace strUtils {
    std::string llvmTypeAsString(const llvm::Type *T);
    std::string llvmInstructionAsString(llvm::Instruction *I);
}
std::string strUtils::llvmTypeAsString(const llvm::Type *T)
{
    std::string typeName;
    llvm::raw_string_ostream N(typeName);
    T->print(N);
    return N.str();
}

std::string strUtils::llvmInstructionAsString(llvm::Instruction* I)
{
    std::string instString;
    llvm::raw_string_ostream N(instString);
    I->print(N);
    return N.str();
}
namespace dfgutils{
    typedef std::pair<llvm::Value*, std::string> vertex;
    typedef std::pair<vertex, vertex> edge;
    typedef std::list<vertex> vertex_list;
    typedef std::list<edge> edge_list;
    struct maxBBDataFlowGraphMaker : public llvm::ModulePass{
        static char ID;
        maxBBDataFlowGraphMaker();
        bool runOnModule(llvm::Module &M);
        private:
        bool Globals(std::ofstream &);
        bool DataflowEdges(std::ofstream&, llvm::Function &);
        bool dumpvertexs(std::ofstream&, llvm::Function &);
        bool ControlflowEdges(std::ofstream&, llvm::Function &);
        bool FunctionArguments(std::ofstream&, llvm::Function&);
        bool DataflowEdges(std::ofstream&);
        bool FunctionCalls(std::ofstream&);
        bool CompleteDigraph(std::ofstream&);

        std::string remove_special_chars(std::string);
        std::string indent = "";
        std::list<vertex> globals;
        std::map<llvm::Function*, edge_list> func_edges_ctrl;
        std::map<llvm::Function*, vertex_list> func_vertexs_ctrl;
        std::map<llvm::Value*, llvm::Function*> func_calls;
        std::map<llvm::Function*, vertex_list> func_args;
        edge_list data_flow_edges;
        std::list<std::string> defined_clusters;
        std::list<std::string> defined_functions;
    };
    static unsigned int num = 0;
    std::string getvaluestaticname(llvm::Value* val);
}
char dfgutils::maxBBDataFlowGraphMaker::ID = 0;
using namespace  llvm;
static llvm::RegisterPass<dfgutils::maxBBDataFlowGraphMaker> X("max-dot", "Maximum sized basic block and Print data flow graph to a dotfile");
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new dfgutils::maxBBDataFlowGraphMaker()); });

dfgutils::maxBBDataFlowGraphMaker::maxBBDataFlowGraphMaker() : llvm::ModulePass(ID){}

std::string dfgutils::getvaluestaticname(llvm::Value* val)
{
    std::string ret_val = "val";
    if(val->getName().empty()) {ret_val += std::to_string(num);num++;}
    else ret_val = val->getName().str();

    if(llvm::isa<llvm::Instruction>(val))ret_val += ":"+strUtils::llvmInstructionAsString(llvm::dyn_cast<llvm::Instruction>(val));

    return ret_val;
}

bool dfgutils::maxBBDataFlowGraphMaker::runOnModule(llvm::Module &M){
    //Maximum Sized Basic Block
    BasicBlock *maxBB;
    Function *FI;
    size_t maxSize = 0;
    for(auto &F : M)
    {
        for(auto &B : F)
            {
              if(B.size() > maxSize)
              {
                maxBB = &B;
                FI = &F;
                maxSize = B.size();
              }
            }
          }
          errs()<<(*maxBB)<<"\n";
          errs()<<"Size = "<<maxBB->size()<<"\n";
          
          //Data Flow Graph
         for(auto II = maxBB->begin(), IE = maxBB->end(); II != IE; ++II)
            {
                switch(II->getOpcode()){
                    case llvm::Instruction::Call:
                        {
                            llvm::CallInst * callinst = llvm::dyn_cast<llvm::CallInst>(II);
                            llvm::Function * func = callinst->getCalledFunction();
                            func_calls[&*II]= func;
                            for(auto arg_idx = func->arg_begin(), arg_end = func->arg_end();arg_idx != arg_end; ++arg_idx)
                            {
                                func_args[func].push_back(vertex(&*arg_idx, dfgutils::getvaluestaticname(&*arg_idx)));
                                data_flow_edges.push_back(edge(vertex(&*II, dfgutils::getvaluestaticname(&*II)), vertex(&*arg_idx, dfgutils::getvaluestaticname(&*arg_idx))));
                            }
                        }
                        break;
                    case llvm::Instruction::Store:
                        {
                            llvm::StoreInst* storeinst = llvm::dyn_cast<llvm::StoreInst>(II);
                            llvm::Value* storevalptr = storeinst->getPointerOperand();
                            llvm::Value* storeval    = storeinst->getValueOperand();
                            data_flow_edges.push_back(edge(vertex(&*II, dfgutils::getvaluestaticname(&*II)), vertex(storevalptr, dfgutils::getvaluestaticname(storevalptr))));
                            data_flow_edges.push_back(edge(vertex(storeval, dfgutils::getvaluestaticname(storeval)), vertex(&*II, dfgutils::getvaluestaticname(&*II))));
                        }
                        break;
                    case llvm::Instruction::Load:
                        {
                            llvm::LoadInst* loadinst = llvm::dyn_cast<llvm::LoadInst>(II);
                            llvm::Value* loadvalptr = loadinst->getPointerOperand();
                            data_flow_edges.push_back(edge(vertex(loadvalptr, dfgutils::getvaluestaticname(loadvalptr)), vertex(&*II, dfgutils::getvaluestaticname(loadvalptr))));
                        }break;
                    default :
                        {
                            for(llvm::Instruction::op_iterator op = II->op_begin(), ope = II->op_end(); op != ope; ++op)
                            {
                                if(llvm::dyn_cast<llvm::Instruction>(*op)||llvm::dyn_cast<llvm::Argument>(*op))
                                {
                                    data_flow_edges.push_back(edge(vertex(op->get(), dfgutils::getvaluestaticname(op->get())), vertex(&*II, dfgutils::getvaluestaticname(&*II))));
                                }
                            }
                        }break;
                }


                llvm::BasicBlock::iterator previous = II;
                ++previous;
                func_vertexs_ctrl[&*FI].push_back(vertex(&*II, dfgutils::getvaluestaticname(&*II)));
                if(previous != IE)func_edges_ctrl[&*FI].push_back(edge(vertex(&*II, dfgutils::getvaluestaticname(&*II)), vertex(&*previous, dfgutils::getvaluestaticname(&*previous))));

            }

            llvm::Instruction* TI = maxBB->getTerminator();
            for(unsigned int succ_idx  = 0, succ_num = TI->getNumSuccessors(); succ_idx != succ_num; ++succ_idx)
            {
                llvm::BasicBlock * Succ = TI->getSuccessor(succ_idx);
                llvm::Value* succ_inst = &*(Succ->begin());
                func_edges_ctrl[&*FI].push_back(edge(vertex(TI, dfgutils::getvaluestaticname(TI)), vertex(succ_inst, dfgutils::getvaluestaticname(succ_inst))));
            }
    std::ofstream outgraphfile("dfg.dot");
    CompleteDigraph(outgraphfile);
    return false;
}

bool dfgutils::maxBBDataFlowGraphMaker::CompleteDigraph(std::ofstream& Out){
    Out << indent << "digraph \"data_flow\"{\n";
    indent = "\t";
    Out << indent << "compound=true;\n";
    Out << indent << "vertexsep=1.0;\n";
    Out << indent << "rankdir=TB;\n";
    Out << indent << "subgraph cluster_globals{\n";
    Out << indent << "color=green;\n";
    if(Globals(Out))return true;
    Out << indent << "}\n";
    for(auto func_vertex_pair : func_vertexs_ctrl)
    {
        indent = "\t";
        if(FunctionArguments(Out, *func_vertex_pair.first)) return true;
        Out << indent << "subgraph cluster_" << func_vertex_pair.first->getName().str() << "{\n";
        indent = "\t\t";
        Out << indent << "label=\""<< func_vertex_pair.first->getName().str() << "\";\n";
        Out << indent << "color=blue;\n";
        if(dumpvertexs(Out, *func_vertex_pair.first)) return true;
        if(ControlflowEdges(Out, *func_vertex_pair.first)) return true;
        indent = "\t";
        Out << indent << "}\n\n";
    }
    DataflowEdges(Out);
    FunctionCalls(Out);
    Out << indent << "label=data_flow_graph;\n";
    indent = "";
    Out << indent << "}\n";
    return false;
}

bool dfgutils::maxBBDataFlowGraphMaker::dumpvertexs(std::ofstream& Out, llvm::Function &F)
{
    for(auto vertex_l: func_vertexs_ctrl[&F])
        Out << indent << "\tvertex" << vertex_l.first << "[shape=record, label=\"" << vertex_l.second << "\"];\n";
    return false;
}

bool dfgutils::maxBBDataFlowGraphMaker::ControlflowEdges(std::ofstream& Out, llvm::Function &F)
{
    for(auto edge_l : func_edges_ctrl[&F])
        Out << indent << "\tvertex" << edge_l.first.first << " -> vertex" << edge_l.second.first << ";\n";
    return false;
}

bool dfgutils::maxBBDataFlowGraphMaker::Globals(std::ofstream& Out)
{
    for(auto globalval : globals)
        Out << indent << "\tvertex" << globalval.first << "[shape=record, label=\"" << globalval.second << "\"];\n";
    return false;
}

bool dfgutils::maxBBDataFlowGraphMaker::DataflowEdges(std::ofstream& Out)
{
    for(auto edge_l : data_flow_edges)
        Out << indent << "\tvertex" << edge_l.first.first << " -> vertex" << edge_l.second.first << "[color=red];\n";
    return false;
}

bool dfgutils::maxBBDataFlowGraphMaker::FunctionCalls(std::ofstream& Out)
{
    for(auto call_l : func_calls)
        Out << indent << "\tvertex" << &*(call_l.second->front().begin()) << " -> vertex"<< call_l.first <<"[ltail = cluster_"<< remove_special_chars(call_l.second->getName().str())<<", color=red, label=return];\n";
    return false;
}

bool dfgutils::maxBBDataFlowGraphMaker::FunctionArguments(std::ofstream& Out, llvm::Function &F)
{
    for(auto arg_l : func_args[&F])
        Out << indent << "\tvertex" << arg_l.first<<"[label="<<arg_l.second<<", shape=doublecircle, style=filled, color=blue , fillcolor=red];\n";
    return false;
}

std::string dfgutils::maxBBDataFlowGraphMaker::remove_special_chars(std::string in_str)
{
    std::string ret_val = in_str;
    size_t pos;
    while((pos = ret_val.find('.')) != std::string::npos)
    {
        ret_val[pos] = '_';
    }

    return ret_val;
}

