//===---------------- ExecutorResolutionGenerator.cpp ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/ExecutionEngine/Orc/ExecutorResolutionGenerator.h"

#include "llvm/ExecutionEngine/Orc/AbsoluteSymbols.h"
#include "llvm/ExecutionEngine/Orc/DebugUtils.h"
#include "llvm/Support/Error.h"

#define DEBUG_TYPE "orc"

namespace llvm {
namespace orc {

Expected<std::unique_ptr<ExecutorResolutionGenerator>>
ExecutorResolutionGenerator::getDylibResolver(ExecutionSession &ES,
                                              const char *path) {
  ExecutorProcessControl &EPC = ES.getExecutorProcessControl();
  auto H = EPC.getDylibMgr().loadDylib(path);
  if (H)
    return H.takeError();
  return std::make_unique<ExecutorResolutionGenerator>(EPC, *H);
}

Expected<std::unique_ptr<ExecutorResolutionGenerator>>
ExecutorResolutionGenerator::getProcessSymbolResolver(ExecutionSession &ES) {
  return getDylibResolver(ES, nullptr);
}

Error ExecutorResolutionGenerator::tryToGenerate(
    LookupState &LS, LookupKind K, JITDylib &JD,
    JITDylibLookupFlags JDLookupFlags, const SymbolLookupSet &LookupSet) {
  assert(!LookupSet.empty() && "Request for no Symbol?");

  SymbolLookupSet Symbols;
  for (auto &[N, R] : LookupSet)
    Symbols.add(N, R);

  DylibManager::LookupRequest L(ResolveObj, Symbols);
  EPC.getDylibMgr().lookupSymbolsAsync(L, [this, LS = std::move(LS),
                                           JD = JITDylibSP(&JD),
                                           LookupSet](auto R) mutable {
    if (R)
      return LS.continueLookup(R.takeError());

    const std::vector<ExecutorSymbolDef> &Syms = R->front();
    size_t SymIdx = 0;
    // SymbolNameSet MissingSymbols;
    SymbolMap Result;
    // for (auto &[Name, Flags] : LookupSet) {
    //   auto Sym = Syms[SymIdx++];

    //   if (LLVM_UNLIKELY(!Sym && Flags == SymbolLookupFlags::RequiredSymbol))
    //     MissingSymbols.insert(Name);
    //   else
    //     Result[Name] = Sym ? *Sym : ExecutorSymbolDef();
    // }
    for (auto &[Name, _] : LookupSet) {
      auto Sym = Syms[SymIdx++];
      if (Sym.getAddress())
        Result[Name] = Sym;
    }

    if (Result.empty())
      return LS.continueLookup(Error::success());

    // if (LLVM_UNLIKELY(!MissingSymbols.empty()))
    //   return LS.continueLookup(make_error<SymbolsNotFound>(
    //       this->EPC.getSymbolStringPool(), std::move(MissingSymbols)));

    LS.continueLookup(JD->define(absoluteSymbols(std::move(Result))));
  });

  return Error::success();
}

} // end namespace orc
} // end namespace llvm
