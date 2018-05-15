
  
#ifndef MULTICLASSFITNESSCASE_H
#define MULTICLASSFITNESSCASE_H

#include <sstream>
#include <string>

#include "IFitnessCase.h"

namespace VUWLGP {
  template <class T> class MultiClassFitnessCase: public IFitnessCase<T> {
  public:
    // Initialise a MultiClassFitnessCase from a pattern file string (see docs for format)
    MultiClassFitnessCase(const std::string& patternFileLine);

    // Return the 0-base class number of the class this case represents.
    unsigned int ClassNumber() const { return classNumber; }

    // Returns a new MultiClassFitnessCase. Caller should manage the memory (which
    // FitnessEnvironment will do if it is just passed to FitnessEnvironment<T>::AddCase).
    static IFitnessCase<T>* Generate(const std::string& patternFileLine);

  protected:
    // To make it non-abstract this method from IFitnessCase is instantiated
    void MakeAbstract() const { }

    unsigned int classNumber;
  };
}



////////////////////////////////// INLINE FUNCTION DEFINITIONS /////////////////////////////////
template <class T> 
VUWLGP::MultiClassFitnessCase<T>::MultiClassFitnessCase(const std::string& line) {
  std::istringstream conv;
  conv.str(line);

  double bufDouble;
  std::string dump;

  conv >> dump; // The original image file for this pattern - don't keep
  conv >> dump; // The x-position in this pattern - don't keep
  conv >> dump; // The y-position in this pattern - don't keep
  conv >> classNumber;
  conv >> dump; // The name of the class in this pattern - don't keep

  while(conv) {
    conv >> bufDouble;
    if(!conv) { break; } // Hacky work around, but it'll do
    features.push_back(bufDouble);
  }
}



template <class T>
VUWLGP::IFitnessCase<T>* VUWLGP::MultiClassFitnessCase<T>::Generate(const std::string& pFL) {
  return dynamic_cast<VUWLGP::IFitnessCase<T>*>(new VUWLGP::MultiClassFitnessCase<T>(pFL));
}
#endif
