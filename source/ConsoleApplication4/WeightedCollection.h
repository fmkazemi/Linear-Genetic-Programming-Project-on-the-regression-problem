
  
#ifndef WEIGHTEDCOLLECTION_H
#define WEIGHTEDCOLLECTION_H

#include <utility>
#include <vector>

#include "Rand.h"

namespace VUWLGP {
  // Represents a collection of type T and provides methods which allow elements to be added 
  // or randomly selected according to its weight. This class does not manage the memory for 
  // these pointers and sometimes segfaults on pointers. It's clearly imperfect but it is
  // sufficient.
  template <class T> class WeightedCollection {
  public:
    // Uses default ctor, cctor, dtor
    
    // Adds elem with weight "weight" to this WeightedCollection.
    void AddElement(T elem, double weight = 1);
    
    // Gets an element randomly according to the weights. An element's probability of being
    // selected is equal to the proportion of the sum of all weights. Zero weights are ok but
    // elements with this weight will never be selected. Negative weights are not allowed and
    // will throw an exception.
    T GetRandomElement();

    // Misc helper/inspection methods:
    unsigned int NumberOfElements() const { return elements.size(); }
    double WeightSum() const { return weightSum; }

  private:
    std::vector< std::pair<double, T> > elements;
    double weightSum;
  };
}
 


///////////////////////////////////// Function Definitions ////////////////////////////////////
template <class T> void VUWLGP::WeightedCollection<T>::AddElement(T elem, double weight) {
  if(weight < 0) {
    throw std::string("VUWLGP::WeightedCollection::AddElement - negative weight not allowed");
  }

  elements.push_back(std::pair<double, T>(weight, elem));
  weightSum += weight;
}


template <class T> T VUWLGP::WeightedCollection<T>::GetRandomElement() {
  double entrySum = Rand::Uniform() * weightSum;
  double sumSoFar = 0;
  typename std::vector< std::pair<double, T> >::iterator i;

  int cnt = 0;
  for(i = elements.begin(); i != elements.end(); ++i) {
    sumSoFar += i->first;

    if(sumSoFar > entrySum) {
      return i->second;
    }

    ++cnt;
  }

  // We got to here, and we shouldn't have done, so just return the final element:
  return elements[elements.size() - 1].second;
}

#endif
