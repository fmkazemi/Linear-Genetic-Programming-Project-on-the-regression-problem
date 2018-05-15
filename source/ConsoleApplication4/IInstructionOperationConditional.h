
  
#ifndef INSTRUCTIONOPERATIONCONDITIONAL_H
#define INSTRUCTIONOPERATIONCONDITIONAL_H

namespace VUWLGP {
  template <class T> class IInstructionOperation;

  // An instance of a sub class of this class represents a single cnoditional register machine 
  // operation, e.g. if<, if>
  template <class T> class IInstructionOperationConditional: public IInstructionOperation<T> {
  public:
    virtual ~IInstructionOperationConditional() { }
  };
}

#endif
