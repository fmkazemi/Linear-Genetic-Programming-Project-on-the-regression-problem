
  
#ifndef IINSTRUCTIONARGUMENT_H
#define IINSTRUCTIONARGUMENT_H

#include <string>

namespace VUWLGP {
  enum ArgumentType { ArgumentTypeConstant, ArgumentTypeFeature, ArgumentTypeRegister };

  template <class T> class FitnessEnvironment;

  // An instance of this class is one argument (expression) in an instruction, e.g. r[2], f1.
  template <class T> class IInstructionArgument {
  public:
    // Returns the value of this register in the fitness environment fe
    virtual T Value(const FitnessEnvironment<T>& fe) const = 0;

    // Copies this InstructionArgument in every respect
    virtual IInstructionArgument<T>* Clone() const = 0;

    // Stringify's this InstructionArgument
    virtual std::string ToString() const = 0;

    // Used when doing IProgram<T>::MarkIntrons - returns the type of the argument  and its
    // index. Types are coded for by the enum defined above.
    virtual unsigned int ArgumentType() const = 0;
    virtual unsigned int ArgumentIndex() const = 0;

    // Returns a new InstructionArgument of whichever class implements this function
    // The Config object currently is used, but who knows if it'll be needed in the future...
    // C++ doesn't allow abstract statics (for good reason, but it would be nice here if 
    // they could be virtual on the type) so every InstructionArgument should implement
    // a method that looks like this.
    // static IInstructionArgument<T>* Generate(Config<T>* conf) = 0;
  };
}

#endif
