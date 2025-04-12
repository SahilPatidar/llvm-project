//===----- ExecutorResolver.h - Symbol resolver -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Symbol resolver.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_EXECUTIONENGINE_ORC_TARGETPROCESS_EXECUTORRESOLVER_H
#define LLVM_EXECUTIONENGINE_ORC_TARGETPROCESS_EXECUTORRESOLVER_H

#include "llvm/ADT/FunctionExtras.h"
#include "llvm/ExecutionEngine/Orc/Shared/ExecutorSymbolDef.h"
#include "llvm/ExecutionEngine/Orc/Shared/SimpleRemoteEPCUtils.h"
#include "llvm/ExecutionEngine/Orc/Shared/TargetProcessControlTypes.h"
#include "llvm/ExecutionEngine/Orc/Shared/WrapperFunctionUtils.h"

namespace llvm::orc {

class ExecutorResolver {
public:
  using ResolveResult = Expected<std::vector<ExecutorSymbolDef>>;
  using YieldResolveResultFn = unique_function<void(ResolveResult)>;
  virtual ~ExecutorResolver() = default;
  virtual void resolveAsync(const RemoteSymbolLookupSet &L,
                            YieldResolveResultFn &&OnResolve) = 0;
};

// class AutoLoadingSymbolResolver : public ExecutorResolver {
// public:
//   void resolveAsync(RemoteSymbolLookupSet &Symbols,
//                     YieldResolveResultFn &&OnResolve) override;
// private:

// };

class DylibSymbolResolver : public ExecutorResolver {
public:
  DylibSymbolResolver(tpctypes::DylibHandle H) : Handle(H) {}
  DylibSymbolResolver() = default;
  void resolveAsync(const RemoteSymbolLookupSet &L,
                    YieldResolveResultFn &&OnResolve) override;

private:
  tpctypes::DylibHandle Handle;
};

} // end namespace llvm::orc

#endif // LLVM_EXECUTIONENGINE_ORC_TARGETPROCESS_EXECUTORRESOLVER_H
