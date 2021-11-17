环境 ：

```
clang version 10.0.0-4ubuntu1 
Target: x86_64-pc-linux-gnu
Thread model: posix
llvm: llvm-10
ubuntu20.4
```



该想法来源于看雪SDC议题回顾|代码混淆研究新方向

其中提到符号执行工具可以破解很大一部分的传统混淆方式，如基于llvm的Ollvm，其中不透明谓词等混淆操作可以很轻松被识别破解。

因此议题讨论了使angr等符号执行工具快速进入路径爆炸的思路进行代码混淆

本文选择使用llvm实现3x+1猜想在代码混淆中的使用



先看一下结果示例

实验代码

```c
//test.c
#include<stdio.h>
int main(){
    unsigned long long int p;
   scanf("%lld",&p);

        if (p==30)
        {
            printf("right");
        }
        return 0;   
}
```

编译为llvm字节码(作为后续混淆操作的基础)

```bash
clang -c test.c -emit-llvm -o test.bc
```

继续编译成可执行文件

```
llc-10 test.bc 
as test.s -o test.o
gcc test.o -o test1
```

文件正常运行

```
h3@h3-ThinkPad-R480:~/UNIT/llvm-code/llvm-project-release-10.x/hello$ ./test 
30
right
```

混淆之后

```
opt-10  -load ./libhello.so  -hello test.bc -O0 -f >> obf.bc
```

文件正常运行

```
h3@h3-ThinkPad-R480:~/UNIT/llvm-code/llvm-project-release-10.x/hello$ ./obf
30
right
```



angr 测验混淆成果

混淆前

```
(angr) h3@h3-ThinkPad-R480:~/UNIT/AngrCTF_FITM/test$ python3 test2.py 
CRITICAL | 2021-11-16 12:33:26,674 | cle.backends | Deprecation warning: the custom_base_addr parameter has been renamed to base_addr
[+] Success! Solution is: 0000000030
```

混淆后 无法获得结果

```
(angr) h3@h3-ThinkPad-R480:~/UNIT/AngrCTF_FITM/test$ python3 test1.py 
CRITICAL | 2021-11-17 16:47:32,979 | cle.backends | Deprecation warning: the custom_base_addr parameter has been renamed to base_addr
^C
```

混淆原理 ： 3x+1猜想 也称 考拉兹猜想

验证程序：

```
def collatz_conjecture(number):
    while number != 1:
        if number % 2 == 0:
            number /= 2
            print(number)
        elif number % 2 == 1:
            number = number * 3 + 1
            print(number)
```

如上所示 任意数字经过该分段函数后返回值会收敛到1；

针对二进制文件进行符号执行的基本原理可以概括为对所有可能路径的探寻，如果在程序中存在多个分支条件，会导致符号执行进入一种路径爆炸的状态，即探寻是路经过多，导致约束求解时条件过多，求解器无法正常返回结果。

基于该原理，尝试对test程序进行混淆处理



llvm 使用llvm-IR中间语言 ，并提供了很多直接操作IR字节码的函数，常见的OLLVM等著名项目多是基于这些函数，



如何在不影响原本程序的用途前提下，加入实现`3x+1`猜想的代码

首先分析源程序逻辑：

获取输入，判断输入值，根据输入选择下一步操作

这里选择在获取输入值，基于输入值创建混淆使用值x后 创建循环，（也可以使用随机值，总之每次程序运行时x值不能固定，会导致混淆失效，猜想与路径探寻时优化有关）将对混淆的实现与判断条件放在循环中，且让判断条件与分段函数返回值共同决定程序的走向

即只有混淆代码中x收敛为1才会进行脱出循环并进行下一步操作



原理很简单，下面开始代码实现（ps：很少用c++，命名恐惧，见谅，后续慢慢优化）



```c++
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
  struct Hello : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Hello() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) ; //重写runOnFunction
    bool obfuse(Function *f);//实际进行混淆函数
  };
}
bool Hello::runOnFunction(Function &F) {
    Function *f = &F;
    obfuse(f);
    return true;
};
bool Hello::obfuse(Function *f){
 
 
  BasicBlock *loopEntry; 
  BasicBlock *loopEnd; 
  BasicBlock *first; 
  BasicBlock *calt1;
  BasicBlock *cal1; 
  LoadInst *load; 
  AllocaInst *VarBasic; 
  Function::iterator tmp = f->begin(); 
  BasicBlock *insert = &*tmp;
  BranchInst *br = NULL;
  LoadInst *load9;
  Function * fn;
  CallInst *call;
  ICmpInst * ic;
    if(f->getName()=="main"){
        tmp++;
        BasicBlock *insert2 = &*tmp;//获取后续basicblock，为后续插入代码以及改变函数流程做铺垫
        tmp++;
        BasicBlock *insert3 = &*tmp;
        for(BasicBlock::iterator p = insert->begin(); p!=tmp->end();p++)//便利第一个basicblock
        {
          Instruction &inst =*p;
          if (isa<ICmpInst>(inst))//找到判断if语句 并在这里将其与原本块分离
          {
            *(--p);    
          	first = insert->splitBasicBlock(p, "first");
            break;
          }
            
          if (isa<CallInst>(inst))//获取调用输入函数
          {
            Instruction &tmpin= *p;
            call = cast<CallInst>(&tmpin);
            fn =call->getCalledFunction();
          }
          
        }
			//下面声明储存一些以后会用到的变量 用于混淆的Varasic值
          insert->getTerminator()->eraseFromParent();
          VarBasic =  new AllocaInst(Type::getInt64Ty(f->getContext()),0,"VarBasic",insert);
          AllocaInst *const2s =  new AllocaInst(Type::getInt64Ty(f->getContext()),0,"const2",insert);
          LoadInst *loads = new LoadInst(call->getArgOperand(1),"loads",insert);//输入值
          Instruction *add = BinaryOperator::Create(BinaryOperator::Add,loads,ConstantInt::get(Type::getInt64Ty(f->getContext()),2001),"s",insert);//创建x

          new StoreInst(add,VarBasic,insert);
          new StoreInst(loads,const2s,insert);
          loopEntry =  BasicBlock::Create(f->getContext(),"loopEntry",f,first);
          BasicBlock *loopend = BasicBlock::Create(f->getContext(),"loopend",f,insert3);
```

以上实现对判断的分离以及一些重要值的创建

```c++

          cal1 = BasicBlock::Create(f->getContext(),"cal1",f,first); //if(x%2==0)
          LoadInst * load1 =   new LoadInst(VarBasic,"tmpvar1",cal1);
          Instruction *frem = BinaryOperator::Create(Instruction::SRem,load1,ConstantInt::get(Type::getInt64Ty(f->getContext()),2),"urem1",cal1);
          ICmpInst *cmp1 =new ICmpInst(*cal1,ICmpInst::ICMP_EQ,frem,ConstantInt::get(Type::getInt64Ty(f->getContext()),0));


		

          calt1 = BasicBlock::Create(f->getContext(),"calt1",f,first);// true x=x/2 
          load1 =   new LoadInst(VarBasic,"tmpvar2",calt1);
          Instruction *div2 = BinaryOperator::Create(BinaryOperator::UDiv,load1,ConstantInt::get(Type::getInt64Ty(f->getContext()),2),"div2",calt1);
          new StoreInst(div2,VarBasic,calt1);

          
          BasicBlock *calf1 = BasicBlock::Create(f->getContext(),"calf1",f,first);//false x=3x+1
          load1 = new LoadInst(VarBasic,"tmpvar3",calf1);
          Instruction *mulp = BinaryOperator::Create(BinaryOperator::Mul,load1,ConstantInt::get(Type::getInt64Ty(f->getContext()),3),"mulp1",calf1);
          Instruction *addx = BinaryOperator::Create(BinaryOperator::Add,mulp,ConstantInt::get(Type::getInt64Ty(f->getContext()),1),"addx",calf1);
          new StoreInst(addx,VarBasic,calf1);
          
```

以上是对于分段函数的实现

```c++

		//检查x是否收敛到1 
          BasicBlock *checks = BasicBlock::Create(f->getContext(),"checks",f,first);
          load1 = new LoadInst(VarBasic,"tmpVar4",checks);
          ICmpInst *cmp2 =new ICmpInst(*checks,ICmpInst::ICMP_NE,load1,ConstantInt::get(Type::getInt64Ty(f->getContext()),1));
          
		//处理原本判断
		  BasicBlock *else1 = BasicBlock::Create(f->getContext(),"else",f,insert2);
          LoadInst *a = new LoadInst(const2s,"a",else1);
          ICmpInst *b = new ICmpInst(*else1,ICmpInst::ICMP_NE,a,ConstantInt::get(Type::getInt64Ty(f->getContext()),30));
		//为创建的块之间建立代码逻辑 ，处理会错误流程出现的情况
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
```

混淆前



```
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %3 = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i64 0, i64 0), i64* %2)
  %4 = load i64, i64* %2, align 8
  %5 = icmp eq i64 %4, 30
  br i1 %5, label %6, label %8

6:                                                ; preds = %0
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.1, i64 0, i64 0))
  br label %8

8:                                                ; preds = %6, %0
  ret i32 0
}

```

混淆后

```
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %3 = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i64 0, i64 0), i64* %2)
  %VarBasic = alloca i64
  %const = alloca i64
  %const2 = alloca i64
  %loads = load i64, i64* %2
  %s = add i64 %loads, 2001
  store i64 %s, i64* %VarBasic
  store i64 %loads, i64* %const2
  br label %loopEntry

loopEntry:                                        ; preds = %loopend, %0
  br label %cal1

cal1:                                             ; preds = %loopEntry
  %tmpvar1 = load i64, i64* %VarBasic
  %urem1 = srem i64 %tmpvar1, 2
  %4 = icmp eq i64 %urem1, 0
  br i1 %4, label %calt1, label %calf1

calt1:                                            ; preds = %cal1
  %tmpvar2 = load i64, i64* %VarBasic
  %div2 = udiv i64 %tmpvar2, 2
  store i64 %div2, i64* %VarBasic
  br label %checks

calf1:                                            ; preds = %cal1
  %tmpvar3 = load i64, i64* %VarBasic
  %mulp1 = mul i64 %tmpvar3, 3
  %addx = add i64 %mulp1, 1
  store i64 %addx, i64* %VarBasic
  br label %checks

checks:                                           ; preds = %calf1, %calt1
  %tmpVar4 = load i64, i64* %VarBasic
  %5 = icmp ne i64 %tmpVar4, 1
  br i1 %5, label %first, label %loopend

first:                                            ; preds = %checks
  %6 = load i64, i64* %2, align 8
  %7 = icmp eq i64 %6, 30
  br i1 %7, label %9, label %loopend

else:                                             ; No predecessors!
  %a = load i64, i64* %const2
  %8 = icmp ne i64 %a, 30
  br i1 %8, label %11, label %loopend

9:                                                ; preds = %first
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.1, i64 0, i64 0))
  br label %11

loopend:                                          ; preds = %else, %checks, %first
  br label %loopEntry

11:                                               ; preds = %else, %9
  ret i32 0
}

```

