//
// Copyright (C) [2020] Futurewei Technologies, Inc.
//
// FORCE-RISCV is licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
// FIT FOR A PARTICULAR PURPOSE.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include <InitialSetupRISCV.h>
#include <Generator.h>
#include <Config.h>
#include <Choices.h>
#include <ChoicesModerator.h>
#include <Log.h>
#include <Register.h>
#include <VmInfo.h>
#include <Memory.h>

#include <memory>
#include <algorithm>
#include <sstream>

using namespace std;

namespace Force {

  void SystemOption::Process(const Config& rConfig)
  {
    mValue = rConfig.GetOptionValue(ESystemOptionType_to_string(mType), mValid);
  }

  InitialSetupRISCV::InitialSetupRISCV(Generator* pGen)
    : mpConfig(nullptr), mpGenerator(pGen), mpFlatMapOption(nullptr), mpPrivilegeLevelOption(nullptr), mpDisablePagingOption(nullptr)
  {
    mpConfig = Config::Instance();
    mpFlatMapOption = new SystemOption(ESystemOptionType::FlatMap);
    mpPrivilegeLevelOption = new SystemOption(ESystemOptionType::PrivilegeLevel);
    mpDisablePagingOption = new SystemOption(ESystemOptionType::DisablePaging);
  }

  InitialSetupRISCV::~InitialSetupRISCV()
  {
    delete mpFlatMapOption;
    delete mpPrivilegeLevelOption;
    delete mpDisablePagingOption;
  }

  void InitialSetupRISCV::Process()
  {
    mpFlatMapOption->Process(*mpConfig);
    mpPrivilegeLevelOption->Process(*mpConfig);
    mpDisablePagingOption->Process(*mpConfig);

    if (mpFlatMapOption->mValid)
    {
      auto choices_mod = mpGenerator->GetChoicesModerator(EChoicesType::PagingChoices);
      std::map<std::string, uint32> modifier;
      if (mpFlatMapOption->mValue)
      {
        modifier["RandomFreeAlloc"] = 0;
        modifier["FlatMapAlloc"] = 100;
      }
      else
      {
        modifier["RandomFreeAlloc"] = 100;
        modifier["FlatMapAlloc"] = 0;
      }
      choices_mod->DoChoicesModification("Page Allocation Scheme", modifier);
    }

    std::map<string, uint64> field_map;

    //misa init
    //TODO potentially enable N - User level interrupts, and V - Vector extension (based on .7 draft of riscv-v ext document)
    field_map["MXL"] = 0x2; //encodes MXLEN val of 64 - current register file based on XLEN=64 assumption

    field_map["EXTENSIONS"] = 0;
    for (char ext : "MAFDC") {
      field_map["EXTENSIONS"] |= 1L << (ext - 'A');
    }

    mpGenerator->InitializeRegisterFields("misa", field_map);

    //mstatus init
    field_map.clear();
    field_map["SXL"] = 0x2;
    field_map["UXL"] = 0x2;
    field_map["TSR"] = 0x0;
    field_map["TW"] = 0x0;
    field_map["TVM"] = 0x0;
    field_map["MXR"] = 0x0;
    field_map["SUM"] = 0x0;
    field_map["MPRV"] = 0x0;
    field_map["MPP"] = 0x0;
    field_map["SPP"] = 0x1;
    field_map["MPIE"] = 0x1;
    field_map["SPIE"] = 0x0;
    field_map["FS"] = 0x3;
    field_map["MIE"] = 0x1;
    field_map["SIE"] = 0x0;
    mpGenerator->InitializeRegisterFields("mstatus", field_map);
    mpGenerator->RandomInitializeRegister("mstatus", "");

    //scause and mcause init
    field_map.clear();
    field_map["INTERRUPT"] = 0x0; 
    field_map["EXCEPTION CODE_VAR"] = 0x18; //This code is reserved for custom use, used here to deliberately not match any exception class.
    mpGenerator->InitializeRegisterFields("scause", field_map);
    mpGenerator->InitializeRegisterFields("mcause", field_map);

    //fcsr init
    field_map.clear();
    field_map["FRM"] = 0x0; //TODO determine preferred default value for rounding mode
    field_map["NZ"] = 0x0;
    field_map["DZ"] = 0x0;
    field_map["OF"] = 0x0;
    field_map["UF"] = 0x0;
    field_map["NX"] = 0x0;
    mpGenerator->InitializeRegisterFields("fcsr", field_map);
    mpGenerator->RandomInitializeRegister("fcsr", "");

    //satp init
    field_map.clear();
    field_map["MODE"] = 0x9;
    if (mpDisablePagingOption->mValid && (mpDisablePagingOption->mValue != 0))
    {
      field_map["MODE"] = 0x0;
    }
    mpGenerator->InitializeRegisterFields("satp", field_map);

    //internal privilege register init
    field_map.clear();
    field_map["MODE"] = 0x3;
    if (mpPrivilegeLevelOption->mValid)
    {
      field_map["MODE"] = mpPrivilegeLevelOption->mValue;
    }
    mpGenerator->InitializeRegisterFields("privilege", field_map);

    Memory::InitializeFillPattern();
  }
}
