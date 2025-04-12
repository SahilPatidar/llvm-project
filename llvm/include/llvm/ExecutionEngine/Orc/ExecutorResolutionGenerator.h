//===----- ExecutorResolver.h - Resolve symbols in executor -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Resolves symbols in the executor.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_EXECUTIONENGINE_ORC_EXECUTORRESOLUTIONGENERATOR_H
#define LLVM_EXECUTIONENGINE_ORC_EXECUTORRESOLUTIONGENERATOR_H

#include "llvm/ExecutionEngine/Orc/Shared/ExecutorSymbolDef.h"
#include "llvm/ExecutionEngine/Orc/Core.h"

namespace llvm::orc {

class ExecutorResolutionGenerator : public DefinitionGenerator {
public:
  using ResolverHandle = ExecutorAddr;

  ExecutorResolutionGenerator(ExecutorProcessControl &EPC,
                               ResolverHandle ResolveObj)
      : EPC(EPC), ResolveObj(ResolveObj) {}
  ExecutorResolutionGenerator(ExecutorProcessControl &EPC) : EPC(EPC) {}

  static Expected<std::unique_ptr<ExecutorResolutionGenerator>>
  getDylibResolver(ExecutionSession &ES, const char *path);

  static Expected<std::unique_ptr<ExecutorResolutionGenerator>>
  getProcessSymbolResolver(ExecutionSession &ES);

  Error tryToGenerate(LookupState &LS, LookupKind K, JITDylib &JD,
                      JITDylibLookupFlags JDLookupFlags,
                      const SymbolLookupSet &LookupSet) override;
private:
    ExecutorProcessControl &EPC;
    // ExecutorAddr ResolveFunc;
    ResolverHandle ResolveObj;
};

}

#endif // LLVM_EXECUTIONENGINE_ORC_EXECUTORRESOLUTIONGENERATOR_H
