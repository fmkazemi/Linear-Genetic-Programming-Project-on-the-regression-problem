
  
#ifndef MULTICLASSPROGRAM_H
#define MULTICLASSPROGRAM_H

#include "Config.h"
#include "IProgram.h"
#include "Instruction.h"
#include "MultiClassFitnessMeasure.h"

namespace VUWLGP {
  
  template <class T> class MultiClassProgram: public IProgram<T> {
  public:
    MultiClassProgram(unsigned int programSize, Config<T>* conf);
    MultiClassProgram(const MultiClassProgram<T>& rhs);
    ~MultiClassProgram() { }

    static Instruction<T>* InstructionFactory(Config<T>* c) { return new Instruction<T>(c); }

  protected:
    // To make it non-abstract this method from IProgram is instantiated
    void MakeAbstract() const { }
  };
}



////////////////////////////////// INLINE FUNCTION DEFINITIONS /////////////////////////////////
template <class T> 
VUWLGP::MultiClassProgram<T>::MultiClassProgram(unsigned int programSize, 
						VUWLGP::Config<T>* conf):
IProgram<T>(conf, new MultiClassFitnessMeasure<double>(conf)){ 
  // Construct all the instructions, they will be delete'd in the IProgram dtor, as will the
  // FitnessMeasure passed to its ctor.
  instructions.reserve(programSize);

  for(unsigned int i = 0; i < programSize; ++i) {
    instructions.push_back(InstructionFactory(config));
  }
}



template <class T>
VUWLGP::MultiClassProgram<T>::MultiClassProgram(const VUWLGP::MultiClassProgram<T>& rhs): 
IProgram<T>(rhs) {
  // Copy the fitness measure, using its defaultly defined cctor - temp is just a convenience
  // variable which helps take a stupidly long line of code and breaks it up at least a little.
  VUWLGP::MultiClassFitnessMeasure<T>* temp;
  temp = dynamic_cast<VUWLGP::MultiClassFitnessMeasure<T>*>(rhs.fitnessMeasure);
  fitnessMeasure = new VUWLGP::MultiClassFitnessMeasure<T>(*temp);
}
#endif
