
  
#ifndef INSTRUCTIONARGUMENTFEATURE_H
#define INSTRUCTIONARGUMENTFEATURE_H

#include <iostream>
#include <sstream>

#include "Config.h"
#include "FitnessEnvironment.h"
#include "IInstructionArgument.h"

namespace VUWLGP {
  // Represents an argument for an instruction that is a feature
  template <class T> class InstructionArgumentFeature: public IInstructionArgument<T> {
  public:
    InstructionArgumentFeature(Config<T>* conf); // default cctor, dtor

    // Returns the value of the register in the fitness environment fe this instance specifies
    T Value(const FitnessEnvironment<T>& fe) const { return fe.ReadFeature(featureIndex); }

    // Copies this InstructionArgument in every respect. Caller is responsible for the memory.
    IInstructionArgument<T>* Clone() const;

    // Caller is responsible for deallocating the memory.
    static IInstructionArgument<T>* Generate(Config<T>*);

    // Stringify's this InstructionArgumentConstant
    std::string ToString() const;

    // Used indirectly in IProgram<T>::MarkIntrons - returns the type of the argument and its
    // index. Types are coded for by the enum defined in IInstructionArgument.h.
    unsigned int ArgumentType() const { return ArgumentTypeFeature; }
    unsigned int ArgumentIndex() const { return featureIndex; }

  private:
    unsigned int featureIndex;

    Config<T>* config;
  };
}



///////////////////////////////// INLINE FUNCTION DEFINITIONS //////////////////////////////// 
template <class T> 
VUWLGP::InstructionArgumentFeature<T>::InstructionArgumentFeature(Config<T>* conf):
featureIndex(Rand::Int(conf->numFeatures)),
config(conf) {
}



template <class T>
VUWLGP::IInstructionArgument<T>* VUWLGP::InstructionArgumentFeature<T>::Clone() const {
  return static_cast<IInstructionArgument<T>*>(new InstructionArgumentFeature<T>(*this));
}


#include <iostream>
template <class T> VUWLGP::IInstructionArgument<T>* 
VUWLGP::InstructionArgumentFeature<T>::Generate(VUWLGP::Config<T>* conf) {
  return static_cast<IInstructionArgument<T>*>(new InstructionArgumentFeature<T>(conf));
}



template <class T> std::string VUWLGP::InstructionArgumentFeature<T>::ToString() const {
  std::ostringstream buffer;
  buffer << "cf[" << featureIndex << "]";
  return buffer.str();
}
#endif
