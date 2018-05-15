
  
// Some code taken and slightly modified from VGP
#include <cmath>

#include "Rand.h"

// Initialise the random number generator with a time-dependent seed.
void VUWLGP::Rand::Init() { srand(time(0)); }



// Initialise the random number generated with a specified seed.
void VUWLGP::Rand::Init(unsigned int seed) { srand(seed); }



// Returns a random integer in the range [0, ceiling). ceiling is at most RAND_MAX (~2*10^9)
// Returns 0 if ceiling == 0
unsigned int VUWLGP::Rand::Int(unsigned int ceiling) {
  return ceiling != 0 ? rand() % ceiling : 0;
}



// Returns a random unsigned integer in the range [0, 2 * RAND_MAX].
// NB: Because rand returns in the range [0, RAND_MAX] , which on this system is the maximum 
// value of a signed integer, rand() needs to be called twice and the two values added to 
// exploit the full range of the unsigned int. This makes 0 and the unsigned integer maximum 
// only half as likely as any one other value in the range.
unsigned int VUWLGP::Rand::Int() { 
  unsigned int r = rand(); 
  return r + rand(); 
}



// Returns a float in the range [0, 1).
float VUWLGP::Rand::Uniform() { 
  return 1.0 * rand() / (1.0 * RAND_MAX + 1); 
}



// Returns a double normally distributed from the mean
double VUWLGP::Rand::Normal(double mean,double sd) {
  double u1,u2,r,theta,v,pi=3.141592654;
  u1 = Uniform();
  u2 = Uniform();
  r = sqrt(-2.0*log(u1));
  theta =2.0*pi*u2;
  v = r*cos(theta);
  return(sd*v+mean);
}



// Example instantiations of GenerateRandomConstant<T>
template <> double VUWLGP::Rand::GenerateRandomConstant<double>() {
  return (2.0 * Rand::Uniform()) - 1;
}

template <> unsigned int VUWLGP::Rand::GenerateRandomConstant<unsigned int>() {
  return Rand::Int();
}
