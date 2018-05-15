
  
#ifndef SYMREGFITNESSMEASURE_H
#define SYMREGFITNESSMEASURE_H

#include <sstream>
#include <string>

#include "SymRegFitnessCase.h"
#include "IFitnessMeasure.h"

namespace VUWLGP {
  template <class T> class IFitnessCase;
  template <class T> class Config;

  // Stores a fitness measurement for a multi class classification problem. T is the type of the
  // registers. Fitness is assumed to be stored in a double. Class numbers are assumed to be 
  // zero-based, i.e. the first class is class 0. In addition, the 0'th class is always
  // represented by the 0'th register using a winner-takes-all algorithm.
  template <class T> class SymRegFitnessMeasure: public IFitnessMeasure<T> {
  public:
    // default cctor, dtor - no members
    SymRegFitnessMeasure(Config<T>* conf): IFitnessMeasure<T>(conf) { }

    // fRV are the finalRegisterValues that any change to the fitness measure's error is based 
    // on. fc is the fitness case which lead to the final register values being passed to it.
    void UpdateError(const RegisterCollection<T>& fRV, const IFitnessCase<T>* fc);

    // Inherited from IFitnessMeasure
    std::string ToString() const;
  };
}



template <class T> void 
VUWLGP::SymRegFitnessMeasure<T>::UpdateError(const VUWLGP::RegisterCollection<T>& fRV,
					     const VUWLGP::IFitnessCase<T>* fc) {
  const SymRegFitnessCase<T>* mcfc;
  if( (mcfc = dynamic_cast<const SymRegFitnessCase<T>*>(fc)) != 0) {
    // then its a valid FC for sym reg
    fitness += (fRV.Read(0) - mcfc->Y()) * (fRV.Read(0) - mcfc->Y());
  }
  else { // wrong type of fitness measure for sym reg
    throw std::string("Error: tried to pass a non-sym reg fitness case to UpdateError");
  }
}



template <class T>
std::string VUWLGP::SymRegFitnessMeasure<T>::ToString() const {
  std::ostringstream buffer;
  buffer << OverallFitness();
  return buffer.str();
}
#endif
