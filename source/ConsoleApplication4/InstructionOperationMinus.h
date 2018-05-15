
  
#ifndef INSTRUCTIONOPERATIONMINUS_H
#define INSTRUCTIONOPERATIONMINUS_H

#include "Config.h"
#include "IInstructionArgument.h"
#include "InstructionArgumentRegister.h"
#include "IInstructionOperation.h"
#include "RegisterCollection.h"

namespace VUWLGP {
  // Represents an addition operation
  template<class T> class InstructionOperationMinus: public IInstructionOperation<T> {
  public:
    virtual ~InstructionOperationMinus() { }

    bool Execute(InstructionArgumentRegister<T>* dest, 
		 const IInstructionArgument<T>* const first,
		 const IInstructionArgument<T>* const second,
		 FitnessEnvironment<T>& fe) const;

    IInstructionOperation<T>* Clone() const;

    std::string ToString() const { return "-"; }
  
    static IInstructionOperation<T>* Generate(Config<T>* conf);
  };
}



///////////////////////////////// INLINE FUNCTION DEFINITIONS //////////////////////////////// 
template <class T> class VUWLGP::IInstructionOperation;

// Adds first to second and writes the result to dest. Returns true to indicate that
// execution should proceed to the next instruction.
template <class T> bool
VUWLGP::InstructionOperationMinus<T>::Execute(InstructionArgumentRegister<T>* dest,
						  const IInstructionArgument<T>* const first,
						  const IInstructionArgument<T>* const second,
						  FitnessEnvironment<T>& fe) const {
  fe.WriteRegister(dest->Index(), first->Value(fe) - second->Value(fe));
  return true;
}



template <class T> VUWLGP::IInstructionOperation<T>* 
VUWLGP::InstructionOperationMinus<T>::Clone() const {
  return static_cast<IInstructionOperation<T>*>(new InstructionOperationMinus());
}



template <class T> VUWLGP::IInstructionOperation<T>* 
VUWLGP::InstructionOperationMinus<T>::Generate(VUWLGP::Config<T>* conf) {
  return new InstructionOperationMinus();
}
#endif
