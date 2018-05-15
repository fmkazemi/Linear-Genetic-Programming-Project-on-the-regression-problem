
  
#ifndef INSTRUCTIONOPERATIONIFLT_H
#define INSTRUCTIONOPERATIONIFLT_H

#include <string>

#include "IInstructionOperationConditional.h"
#include "IInstructionArgument.h"
#include "InstructionArgumentRegister.h"
#include "RegisterCollection.h"

namespace VUWLGP {
  // Represents an if< operation
  template<class T> class InstructionOperationIflt: public IInstructionOperationConditional<T> {
  public:
    virtual ~InstructionOperationIflt() { }

    bool Execute(InstructionArgumentRegister<T>* dest,
		 const IInstructionArgument<T>* const first,
		 const IInstructionArgument<T>* const second,
		 FitnessEnvironment<T>& fe) const;

    IInstructionOperation<T>* Clone() const;

    std::string ToString() const { return "<"; }
  
    static IInstructionOperation<T>* Generate(VUWLGP::Config<T>* conf);
  };
}



///////////////////////////////// INLINE FUNCTION DEFINITIONS //////////////////////////////// 
template <class T> class VUWLGP::IInstructionOperation;

// Returns true if the first argument is less than the second
template <class T> bool
VUWLGP::InstructionOperationIflt<T>::Execute(InstructionArgumentRegister<T>* dest,
						  const IInstructionArgument<T>* const first,
						  const IInstructionArgument<T>* const second,
						  FitnessEnvironment<T>& fe) const {
  return first->Value(fe) < second->Value(fe);
}



template <class T> VUWLGP::IInstructionOperation<T>* 
VUWLGP::InstructionOperationIflt<T>::Clone() const {
  return static_cast<IInstructionOperation<T>*>(new InstructionOperationIflt());
}



template <class T> VUWLGP::IInstructionOperation<T>* 
VUWLGP::InstructionOperationIflt<T>::Generate(Config<T>* conf) {
  return new InstructionOperationIflt();
}
#endif
