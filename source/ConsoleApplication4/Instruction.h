
  
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <sstream>

#include "InstructionArgumentRegister.h"

#include "IInstructionOperation.h"
#include "IInstructionOperationConditional.h"

#include "Rand.h"
#include "RegisterCollection.h"

namespace VUWLGP {
  template <class T> class Config;
  template <class T> class FitnessEnvironment;
  template <class T> class IInstructionArgument;

  // An instance of this class represents a single register machine instruction.
  template<class T> class Instruction {
  public:
    Instruction(Config<T>* conf);
    Instruction(const Instruction& rhs);
    virtual ~Instruction();

    // Executes this instruction (via op). Writes its result, if any, to r and returns
    // true unless it was a conditional that was false.
    virtual bool Execute(FitnessEnvironment<T>& fe) const;
    
    // Copys the Instruction, caller must manage the memory returned.
    virtual Instruction<T>* Clone() const { return new Instruction<T>(*this); }
    
    // Performs a Micromutation on this instruction
    virtual void Mutate();

    // Returns true if this instruction's InstructionOperation is a sub-class of 
    // ConditionalInstructionOperation
    bool IsConditional() const;

    virtual std::string ToString(bool commentIntrons = false) const;
    
    // Marked true if this instruction is a structural intron and does not need to be
    // executed.
    bool IsIntron;

    // Used in the IProgram<T>::MarkIntrons method - they return only the register index and
    // register type used in each register referred to:
    unsigned int DestinationIndex() const { return destination->ArgumentIndex(); }
    unsigned int FirstArgumentType() const { return firstArgument->ArgumentType(); }
    unsigned int FirstArgumentIndex() const { return firstArgument->ArgumentIndex(); }
    unsigned int SecondArgumentType() const { return secondArgument->ArgumentType(); }
    unsigned int SecondArgumentIndex() const { return secondArgument->ArgumentIndex(); }
    
  private:
    IInstructionOperation<T>* op;
    InstructionArgumentRegister<T>* destination;
    IInstructionArgument<T>* firstArgument;
    IInstructionArgument<T>* secondArgument;

    Config<T>* config;
  };
}



////////////////////////////////// Inline Function Definitions ////////////////////////////////
template <class T> VUWLGP::Instruction<T>::Instruction(Config<T>* conf) {
  config = conf;
  op = conf->instructionOperations->GetRandomElement()(config);
  destination = 
    static_cast<InstructionArgumentRegister<T>*>(
    InstructionArgumentRegister<T>::Generate(config));
  firstArgument = conf->argumentGenerators->GetRandomElement()(config);
  secondArgument = conf->argumentGenerators->GetRandomElement()(config);

  IsIntron = false;
}



template <class T> VUWLGP::Instruction<T>::Instruction(const Instruction<T>& rhs) {
  op = rhs.op->Clone();
  // It should always be an IAR, but dynamic_cast it, rather than static_cast, just for 
  // safety:
  destination = 
    dynamic_cast<InstructionArgumentRegister<double>*>(rhs.destination->Clone());
  firstArgument = rhs.firstArgument->Clone();
  secondArgument = rhs.secondArgument->Clone();
  config = rhs.config;
  IsIntron = rhs.IsIntron;
}



template <class T> VUWLGP::Instruction<T>::~Instruction() {
  // We do not deallocate config - everything else is owned by this object:
  delete op;
  delete destination;
  delete firstArgument;
  delete secondArgument;
}



template <class T> 
bool VUWLGP::Instruction<T>::Execute(FitnessEnvironment<T>& fe) const {
  return op->Execute(destination, firstArgument, secondArgument, fe);
}



template <class T> void VUWLGP::Instruction<T>::Mutate() {
  // Select one of the four parts:
  unsigned int part = Rand::Int(4);
  std::string currentString;

  // Then convert it ToString, delete it and regenerate it until its string rep changes:
  switch(part) {
  case 0: // Change the operation
    currentString = op->ToString();
    do {
      delete op;
      op = config->instructionOperations->GetRandomElement()(config);
    } while(currentString == op->ToString());
    break;
  case 1: // Change the destination register
    currentString = destination->ToString();
    do {
      delete destination;
      destination = static_cast<InstructionArgumentRegister<T>*>
	(InstructionArgumentRegister<T>::Generate(config));
    } while(currentString == destination->ToString());
    break;
  case 2: // Change the first argument
    currentString = firstArgument->ToString();
    do {
      delete firstArgument;
      firstArgument = config->argumentGenerators->GetRandomElement()(config);
    } while(currentString == firstArgument->ToString());
    break;
  case 3: // Change the second argument
    currentString = secondArgument->ToString();
    do {
      delete secondArgument;
      secondArgument = config->argumentGenerators->GetRandomElement()(config);    
    } while(currentString == secondArgument->ToString());
    break;
  }
}



template <class T> bool VUWLGP::Instruction<T>::IsConditional() const {
  return (dynamic_cast<IInstructionOperationConditional<T>*>(op) != 0);
}



template <class T> std::string VUWLGP::Instruction<T>::ToString(bool commentIntrons) const {
  std::ostringstream buffer;

  if(commentIntrons && IsIntron) {
    buffer << "//";
  }
  
  if(IsConditional()) {
    buffer << "if(" << firstArgument->ToString() << " " << op->ToString() << " " 
	   << secondArgument->ToString() << ")";
  }
  else {
    buffer << destination->ToString() << " = " << firstArgument->ToString() << " "
	   << op->ToString() << " " << secondArgument->ToString() << ";";
  }
  
  return buffer.str();
}
#endif
