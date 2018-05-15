
  
#ifndef IFITNESSMEASURE_H
#define IFITNESSMEASURE_H

#include "Rand.h"

namespace VUWLGP {
  template <class T> class Config;
  template <class T> class RegisterCollection;
  template <class T> class IFitnessCase;

  // An instance of a sub class of this object stores all fitness information associated with a
  // program. T is the type of the registers. Fitness is assumed to be stored in a double.
  template <class T> class IFitnessMeasure {
  public:
    IFitnessMeasure(Config<T>* conf): config(conf) { }
    // use the default cctor
    virtual ~IFitnessMeasure() { }

    // Calculates the error of a register collection of final register values in a 
    // problem-specific way. Updates the internal measurement of the error to reflect this.
    // fRV are the finalRegisterValues that any change to the fitness measure's error is based 
    // on. fc is the fitness case which lead to the final register values being passed to it.
    virtual void UpdateError(const RegisterCollection<T>& fRV, const IFitnessCase<T>* fc) = 0;

    // Zeroes the fitness - i.e. sets it to perfect, assumes error is going to be added to it.
    virtual void ZeroFitness() { fitness = 0; }

    // Returns some value representing an overall level of fitness. 0 is assumed to be perfect,
    // and higher is assumed to be worse. Fitnesses less than zero probably aren't meaningful.
    virtual double OverallFitness() const { return fitness; }

    // Used mainly in the unit tests. Sets this fitness measure to some value in the range [0,1)
    virtual void RandomiseFitness() { fitness = Rand::Uniform(); }

    // Returns a stringified fitness measure (it should be only one line, but not prefixed with
    // "//" or any other "I am a comment" indication - it should be naked text only.
    virtual std::string ToString() const = 0;

  protected:
    double fitness;
    Config<T>* config;
  };
}
#endif
