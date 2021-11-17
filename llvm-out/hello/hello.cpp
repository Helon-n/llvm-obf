#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/InitializePasses.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"

using namespace llvm;

#define DEBUG_TYPE "hello"

STATISTIC(HelloCounter, "Counts number of functions greeted");

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct Hello : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Hello() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) ;
    bool obfuse(Function *f);
  };
}
bool Hello::runOnFunction(Function &F) {
    Function *f = &F;
    obfuse(f);
    return true;
};
bool Hello::obfuse(Function *f){
 
  std::vector<BasicBlock *> origBB; 
  BasicBlock *loopEntry; 
  BasicBlock *loopEnd; 
  BasicBlock *first; 
  BasicBlock *calt1;
  BasicBlock *cal1; 
  LoadInst *load; 
  SwitchInst *switchI; //switch
  AllocaInst *VarBasic; 
  Function::iterator tmp = f->begin(); //++tmp;
  BasicBlock *insert = &*tmp;//insert 
  BranchInst *br = NULL;
  LoadInst *load9;
  Function * fn;
  CallInst *call;
  ICmpInst * ic;
    if(f->getName()=="main"){
        tmp++;
        BasicBlock *insert2 = &*tmp;
        tmp++;
        BasicBlock *insert3 = &*tmp;
        for(BasicBlock::iterator p = insert->begin(); p!=tmp->end();p++)
        {
          Instruction &inst =*p;
          if (isa<ICmpInst>(inst))
          {
           *(--p);
          //  Instruction &tmpin= *p;
          //  load9 = cast<LoadInst>(&tmpin);         
          first = insert->splitBasicBlock(p, "first");
          // Instruction *add = BinaryOperator::Create(BinaryOperator::Add,load9,ConstantInt::get(Type::getInt64Ty(f->getContext()),90),"add",insert);
          
            break;//寻找到icmp 在之前加入赋值语句
          }
          if (isa<CallInst>(inst))
          {
             Instruction &tmpin= *p;
            call = cast<CallInst>(&tmpin);
            fn =call->getCalledFunction();
            

          }
          
        }

          insert->getTerminator()->eraseFromParent();
          VarBasic =  new AllocaInst(Type::getInt64Ty(f->getContext()),0,"VarBasic",insert);
          AllocaInst *consts =  new AllocaInst(Type::getInt64Ty(f->getContext()),0,"const",insert);
          AllocaInst *const2s =  new AllocaInst(Type::getInt64Ty(f->getContext()),0,"const2",insert);
          LoadInst *loads = new LoadInst(call->getArgOperand(1),"loads",insert);
          Instruction *add = BinaryOperator::Create(BinaryOperator::Add,loads,ConstantInt::get(Type::getInt64Ty(f->getContext()),2001),"s",insert);
          // Instruction *addX = BinaryOperator::Create(BinaryOperator::Add,add,VarBasic,"s",insert);

          new StoreInst(add,VarBasic,insert);
          new StoreInst(loads,const2s,insert);
          loopEntry =  BasicBlock::Create(f->getContext(),"loopEntry",f,first);
          BasicBlock *loopend = BasicBlock::Create(f->getContext(),"loopend",f,insert3);


            first->getTerminator()->eraseFromParent();
          for (BasicBlock::iterator p = first->begin(); p!=first->end();p++)
          {
             Instruction &inst =*p;
            if (isa<ICmpInst>(inst))
            {
               Instruction &tmpin= *p;
               ic = cast<ICmpInst>(&tmpin);
              break;
            }
          }


          cal1 = BasicBlock::Create(f->getContext(),"cal1",f,first); //if(x%2==0)
          LoadInst * load1 =   new LoadInst(VarBasic,"tmpvar1",cal1);
          Instruction *frem = BinaryOperator::Create(Instruction::SRem,load1,ConstantInt::get(Type::getInt64Ty(f->getContext()),2),"urem1",cal1);
          ICmpInst *cmp1 =new ICmpInst(*cal1,ICmpInst::ICMP_EQ,frem,ConstantInt::get(Type::getInt64Ty(f->getContext()),0));
          // true x=x/2 
          calt1 = BasicBlock::Create(f->getContext(),"calt1",f,first);
          load1 =   new LoadInst(VarBasic,"tmpvar2",calt1);
          Instruction *div2 = BinaryOperator::Create(BinaryOperator::UDiv,load1,ConstantInt::get(Type::getInt64Ty(f->getContext()),2),"div2",calt1);
          new StoreInst(div2,VarBasic,calt1);

          //false x=3x+1
          BasicBlock *calf1 = BasicBlock::Create(f->getContext(),"calf1",f,first);
          load1 = new LoadInst(VarBasic,"tmpvar3",calf1);
          Instruction *mulp = BinaryOperator::Create(BinaryOperator::Mul,load1,ConstantInt::get(Type::getInt64Ty(f->getContext()),3),"mulp1",calf1);
          Instruction *addx = BinaryOperator::Create(BinaryOperator::Add,mulp,ConstantInt::get(Type::getInt64Ty(f->getContext()),1),"addx",calf1);
          new StoreInst(addx,VarBasic,calf1);
         
        
          
          
          BasicBlock *checks = BasicBlock::Create(f->getContext(),"checks",f,first);
          load1 = new LoadInst(VarBasic,"tmpVar4",checks);
          ICmpInst *cmp2 =new ICmpInst(*checks,ICmpInst::ICMP_NE,load1,ConstantInt::get(Type::getInt64Ty(f->getContext()),1));


          BasicBlock *else1 = BasicBlock::Create(f->getContext(),"else",f,insert2);
          LoadInst *a = new LoadInst(const2s,"a",else1);
          ICmpInst *b = new ICmpInst(*else1,ICmpInst::ICMP_NE,a,ConstantInt::get(Type::getInt64Ty(f->getContext()),30));

     
          


          BranchInst::Create(loopEntry,insert);
          BranchInst::Create(cal1,loopEntry);
          BranchInst::Create(insert2,else1,ic,first);
          BranchInst::Create(calt1,calf1,cmp1,cal1);
          BranchInst::Create(checks,calt1);
          BranchInst::Create(checks,calf1);
          BranchInst::Create(first,loopend,cmp2,checks);
          BranchInst::Create(insert3,loopend,b,else1);
          BranchInst::Create(loopEntry,loopend);
   
    };

    return true;
}
char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass");