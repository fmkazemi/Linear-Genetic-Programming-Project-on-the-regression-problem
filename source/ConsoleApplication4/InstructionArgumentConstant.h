
  
#ifndef INSTRUCTIONARGUMENTCONSTANT_H
#define INSTRUCTIONARGUMENTCONSTANT_H

#include <string>

#include "Config.h"
#include "FitnessEnvironment.h"
#include "IInstructionArgument.h"
#include "Rand.h"

namespace VUWLGP {
  // Represents an argument to an instruction that is a read-write register
  template <class T> 
    class InstructionArgumentConstant: public IInstructionArgument<T> {
  public:
    InstructionArgumentConstant(Config<T>* conf); // default cctor, dtor
    
    // Returns the value of the register in the fitness environment fe this instance specifies
    T Value(const FitnessEnvironment<T>& fe) const { return value; }

    // Copies this InstructionArgument in every respect. Caller is responsible for the memory.
    IInstructionArgument<T>* Clone() const;

    // Caller is responsible for deallocating the memory.
    static IInstructionArgument<T>* Generate(Config<T>*);

    // Stringify's this InstructionArgumentConstant
    std::string ToString() const;

    // Used indirectly in IProgram<T>::MarkIntrons - returns the type of the argument and its
    // index. Types are coded for by the enum defined in IInstructionArgument.h.
    unsigned int ArgumentType() const { return ArgumentTypeConstant; }
    unsigned int ArgumentIndex() const { throw std::string("ArgumentConstant has no index."); }

  private:
    T value;
    
    Config<T>* config;
  };
}



///////////////////////////////// INLINE FUNCTION DEFINITIONS /////////////////////////////////
template <class T> 
VUWLGP::InstructionArgumentConstant<T>::InstructionArgumentConstant(Config<T>* conf):
value(VUWLGP::Rand::GenerateRandomConstant<T>()),
config(conf) {
}



template <class T>
VUWLGP::IInstructionArgument<T>* VUWLGP::InstructionArgumentConstant<T>::Clone() const {
  return static_cast<IInstructionArgument<T>*>(new InstructionArgumentConstant<T>(*this));
}



template <class T> 
VUWLGP::IInstructionArgument<T>* 
VUWLGP::InstructionArgumentConstant<T>::Generate(VUWLGP::Config<T>* conf) {
  return 
    static_cast<IInstructionArgument<T>*>(new InstructionArgumentConstant<T>(conf));
}



template <class T> 
std::string VUWLGP::InstructionArgumentConstant<T>::ToString() const {
  std::ostringstream buffer;
  buffer << value;
  return buffer.str();
}
#endif
