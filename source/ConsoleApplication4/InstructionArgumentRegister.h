
  
#ifndef INSTRUCTIONARGUMENTREGISTER_H
#define INSTRUCTIONARGUMENTREGISTER_H
#include <sstream>

#include "IInstructionArgument.h"
#include "FitnessEnvironment.h"
#include "Rand.h"

namespace VUWLGP {
  template <class T> class Config;

  // Represents an argument to an instruction that is a read-write register
  template <class T> class InstructionArgumentRegister: public IInstructionArgument<T> {
  public:
    InstructionArgumentRegister(Config<T>* conf); // default cctor, dtor

    // Returns the value of the register in the fitness environment fe this instance specifies
    T Value(const FitnessEnvironment<T>& fe) const { return fe.ReadRegister(registerIndex); }
    unsigned int Index() const { return registerIndex; }

    // Copies this InstructionArgument in every respect. Caller is responsible for the memory.
    IInstructionArgument<T>* Clone() const;

    // Caller is responsible for deallocating the memory.
    static IInstructionArgument<T>* Generate(Config<T>*);

    // Stringify's this InstructionArgumentConstant
    std::string ToString() const;

    // Used indirectly in IProgram<T>::MarkIntrons - returns the type of the argument and its
    // index. Types are coded for by the enum defined in IInstructionArgument.h.
    unsigned int ArgumentType() const { return ArgumentTypeRegister; }
    unsigned int ArgumentIndex() const { return registerIndex; }

  private:
    unsigned int registerIndex;

    Config<T>* config;
  };
}



//////////////////////////////// INLINE FUNCTION DEFINITIONS //////////////////////////////////
template <class T>
VUWLGP::InstructionArgumentRegister<T>::InstructionArgumentRegister(Config<T>* conf):
registerIndex(Rand::Int(conf->numRegisters)),
config(conf) {
}



template <class T>
VUWLGP::IInstructionArgument<T>* VUWLGP::InstructionArgumentRegister<T>::Clone() const {
  return static_cast<IInstructionArgument<T>*>(new InstructionArgumentRegister<T>(*this));
}



template <class T> VUWLGP::IInstructionArgument<T>* 
VUWLGP::InstructionArgumentRegister<T>::Generate(VUWLGP::Config<T>* conf) {
  return static_cast<IInstructionArgument<T>*>(new InstructionArgumentRegister<T>(conf));
}



template <class T> std::string VUWLGP::InstructionArgumentRegister<T>::ToString() const {
  std::ostringstream buffer;
  buffer << "r[" << registerIndex << "]";
  return buffer.str();
}
#endif
