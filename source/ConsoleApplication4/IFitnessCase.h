
  
#ifndef IFITNESSCASE_H
#define IFITNESSCASE_H

#include <vector>

namespace VUWLGP {
  // Represents a single fitness case (e.g. case to be classified, point in a symreg) with
  // features of type T.
  template <class T> class IFitnessCase {
  public:
    virtual ~IFitnessCase() { }

    // Returns the value of the i'th feature.
    T F(unsigned int i) const { return features[i]; }

    // Returns a new IFitnessCase of whichever class implements this function
    // The Config object currently is used, but who knows if it'll be needed in the future...
    // C++ doesn't allow abstract statics (for good reason, but it would be nice here if 
    // they could be virtual on the type) so every IFitnessCase should implement
    // a method that looks like this. The caller of this function is responsible for the memory.
    // static IFitnessCase<T>* Generate(const std::string& patternFileLine) = 0;

  protected:
    std::vector<T> features;

    // This method exists just to make this class an abstract class.
    virtual void MakeAbstract() const = 0;
  };
}
#endif
