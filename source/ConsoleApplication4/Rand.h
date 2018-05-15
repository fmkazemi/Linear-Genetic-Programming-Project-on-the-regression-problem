
  
#ifndef RAND_H
#define RAND_H

#include <cstdlib>
#include <ctime>

namespace VUWLGP {
  namespace Rand {
    // Initialise the random number generator with a time-dependent seed.
    void Init();
    
    // Initialise the random number generated with a specified seed.
    void Init(unsigned int seed);
    
    // Returns a random integer in the range [0, ceiling).
    unsigned int Int(unsigned int ceiling);
    
    // Returns a random integer in the range [0, RAND_MAX].
    unsigned int Int();
    
    // Returns a float in the range [0, 1).
    float Uniform();
    
    // Returns a float normally distributed from the mean
    double Normal(double mean,double sd);

    // Generates a random value of type T (e.g. for an ephemeral random constant)
    template <class T> T GenerateRandomConstant();
  }
}
#endif
