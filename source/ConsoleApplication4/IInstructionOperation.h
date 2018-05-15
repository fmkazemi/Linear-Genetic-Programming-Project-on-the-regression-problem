
  
#ifndef INSTRUCTIONOPERATION_H
#define INSTRUCTIONOPERATION_H

#include <string>

#include "IInstructionArgument.h"
#include "InstructionArgumentRegister.h"
#include "FitnessEnvironment.h"

namespace VUWLGP {
  template <class T> class Config;

  // An instance of a sub class of this class represents a single register machine operation,
  // e.g. +, -, if<, sqrt
  template<class T> class IInstructionOperation {
  public:
    // Default ctor, cctor (no InstructionOperator stores any values into memory)
    virtual ~IInstructionOperation() { }

    // Performs the operation defined by this class using the registers specified in the
    // first through third arguments of the function call. Returns true if the next
    // instruction should be executed and false if we should skip to the next assignment
    virtual bool Execute(InstructionArgumentRegister<T>* dest,
			 const IInstructionArgument<T>* const first,
			 const IInstructionArgument<T>* const second,
			 FitnessEnvironment<T>& fe) const = 0;

    // Clones a copy of this InstructionOperation. Caller is responsible for deallocating it.
    virtual IInstructionOperation* Clone() const = 0;

    // Returns a new InstructionOperation of whichever class implements this function
    // The Config object currently is used, but who knows if it'll be needed in the future...
    // C++ doesn't allow abstract statics (for good reason, but it would be nice here if 
    // they could be virtual on the type) so every InstructionOperation should implement
    // a method that looks like this.
    // static InstructionOperation<T>* Generate(Config<T>* conf) = 0;

    // Returns a string representation of the operation. If the instruction is a conditional
    // then the caller of this function needs to wrap, e.g. if(...) around the pieces.
    virtual std::string ToString() const = 0;
  };
}

#endif
