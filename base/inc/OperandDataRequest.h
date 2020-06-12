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
#ifndef Force_OperandDataRequest_H
#define Force_OperandDataRequest_H

#include <Defines.h>
#include <Object.h>

namespace Force {

  class Data;

  /*!
    \class OperandDataRequest
    \breif A data request to the test generator thread to generate an instruction.
  */
  class OperandDataRequest : public Object {
  public:
    OperandDataRequest(const std::string& name, const std::string& valueStr); //!< Constructor with name and value string given.
    ~OperandDataRequest(); //!< Destructor.
    Object* Clone() const override;  //!< Return a cloned OperandDataRequest object of the same type and content.
    const std::string ToString() const override; //!< Return a string describing the current state of the OperandRequest object.
    const char* Type() const override { return "OperandDataRequest"; } //!< Return type of the OperandDataRequest object.
    const std::string& Name() const { return mName; } //!< Return operand name.
    void SetDataRequest(const std::string& valueStr); //!< Set value request of the operand in string format.
    void SetApplied() const {mApplied = true;} //!< the request is applied
    bool IsApplied() const {return mApplied; }//!< return applied status
    const std::string& GetDataString() const { return mstrData; } //!< Return data request string.
  protected:
    OperandDataRequest(const OperandDataRequest& rOther); //!< Copy constructor.
  protected:
    std::string mName; //!< Operand name.
    std::string mstrData; //!< data request string.
    mutable bool mApplied; //!< whether the request is applied or not
  };

}

#endif
