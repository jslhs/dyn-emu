
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
#include "llvm/Bitcode/ReaderWriter.h"
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
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();

  LLVMContext Context;
  std::unique_ptr<Module> Owner = make_unique<Module>("main", Context);
  Module *M = Owner.get();
  // Create the add1 function entry and insert this entry into module M.  The
  // function will have a return type of "int" and take an argument of "int".
  // The '0' terminates the list of argument types.
  Function *Add1F =
	  cast<Function>(M->getOrInsertFunction("add1", Type::getInt32Ty(Context),
	  Type::getInt32Ty(Context),
	  nullptr));

  // Add a basic block to the function. As before, it automatically inserts
  // because of the last argument.
  BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", Add1F);

  // Create a basic block builder with default parameters.  The builder will
  // automatically append instructions to the basic block `BB'.
  IRBuilder<> builder(BB);

  // Get pointers to the constant `1'.
  Value *One = builder.getInt32(1);

  // Get pointers to the integer argument of the add1 function...
  assert(Add1F->arg_begin() != Add1F->arg_end()); // Make sure there's an arg
  Argument *ArgX = &*Add1F->arg_begin();          // Get the arg
  ArgX->setName("AnArg");            // Give it a nice symbolic name for fun.

  // Create the add instruction, inserting it into the end of BB.
  Value *Add = builder.CreateAdd(One, ArgX);

  // Create the return instruction and add it to the basic block
  builder.CreateRet(Add);

  // Now, function add1 is ready.

  // Now we're going to create function `foo', which returns an int and takes no
  // arguments.
  
  Function *FooF =
	  cast<Function>(M->getOrInsertFunction("foo", Type::getInt32Ty(Context),
	  nullptr));

  // Add a basic block to the FooF function.
  BB = BasicBlock::Create(Context, "EntryBlock", FooF);

  // Tell the basic block builder to attach itself to the new basic block
  builder.SetInsertPoint(BB);

  // Get pointer to the constant `10'.
  Value *Ten = builder.getInt32(10);

  // Pass Ten to the call to Add1F
  CallInst *Add1CallRes = builder.CreateCall(Add1F, Ten);
  Add1CallRes->setTailCall(true);

  // Create the return instruction and add it to the basic block.
  builder.CreateRet(Add1CallRes);

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

  // Call the `foo' function with no arguments:
  std::vector<GenericValue> noargs;
  GenericValue gv = EE->runFunction(FooF, noargs);

  // Import result of execution:
  outs() << "Result: " << gv.IntVal << "\n";

  M->print(outs(), nullptr);
  outs().flush();
  
  delete EE;
  llvm_shutdown();
  return 0;
}

