
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

using namespace llvm;

int main() {
  InitializeNativeTarget();
  LLVMInitializeX86TargetInfo();
  LLVMInitializeX86Target();
  LLVMInitializeX86TargetMC();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();

  LLVMContext Context;
  std::unique_ptr<Module> Owner = make_unique<Module>("main", Context);
  Module *M = Owner.get();

  std::string err;
  EngineBuilder eb(std::move(Owner));
  eb.setErrorStr(&err);
  eb.setUseOrcMCJITReplacement(false);
  eb.setEngineKind(EngineKind::JIT);
  ExecutionEngine* EE = eb.create();

  if (!EE)
  {
	  outs() << "create error." << err;
	  return -1;
  }
  
  delete EE;
  llvm_shutdown();
  return 0;
}

