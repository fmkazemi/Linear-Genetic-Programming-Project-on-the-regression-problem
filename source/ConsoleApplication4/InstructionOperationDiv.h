
  
#ifndef INSTRUCTIONOPERATIONDIV_H
#define INSTRUCTIONOPERATIONDIV_H

#include "Config.h"
#include "IInstructionArgument.h"
#include "InstructionArgumentRegister.h"
#include "IInstructionOperation.h"
#include "RegisterCollection.h"

namespace VUWLGP {
  // Represents an addition operation
  template<class T> class InstructionOperationDiv: public IInstructionOperation<T> {
  public:
    virtual ~InstructionOperationDiv() { }

    bool Execute(InstructionArgumentRegister<T>* dest, 
		 const IInstructionArgument<T>* const first,
		 const IInstructionArgument<T>* const second,
		 FitnessEnvironment<T>& fe) const;

    IInstructionOperation<T>* Clone() const;

    std::string ToString() const { return "/"; }
  
    static IInstructionOperation<T>* Generate(Config<T>* conf);
  };
}



///////////////////////////////// INLINE FUNCTION DEFINITIONS //////////////////////////////// 
template <class T> class VUWLGP::IInstructionOperation;

// Adds first to second and writes the result to dest. Returns true to indicate that
// execution should proceed to the next instruction.
template <class T> bool
VUWLGP::InstructionOperationDiv<T>::Execute(InstructionArgumentRegister<T>* dest,
						  const IInstructionArgument<T>* const first,
						  const IInstructionArgument<T>* const second,
						  FitnessEnvironment<T>& fe) const {
  fe.WriteRegister(dest->Index(), 
		   second->Value(fe) != 0 ? first->Value(fe) / second->Value(fe) : 0);
  return true;
}



template <class T> VUWLGP::IInstructionOperation<T>* 
VUWLGP::InstructionOperationDiv<T>::Clone() const {
  return static_cast<IInstructionOperation<T>*>(new InstructionOperationDiv());
}



template <class T> VUWLGP::IInstructionOperation<T>* 
VUWLGP::InstructionOperationDiv<T>::Generate(VUWLGP::Config<T>* conf) {
  return new InstructionOperationDiv();
}
#endif
