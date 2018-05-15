
  
#ifndef GENERATIONALTOURNAMENTPOPULATION_H
#define GENERATIONALTOURNAMENTPOPULATION_H

#include "IPopulation.h"
#include "IProgram.h"

namespace VUWLGP {
  template <class IProgramSubclass, class T> 
  class GenerationalTournamentPopulation: public IPopulation<IProgramSubclass, T> {
  public:
    // Uses the default cctor, dtor
    GenerationalTournamentPopulation(Config<T>* conf): IPopulation<IProgramSubclass,T>(conf) { }

    // Builds the next generation from the current one, based on program's assigned fitness.
    void IteratePopulation();

    // The proportion of the population which is replicated through elitism to the next
    // generation. Should be a value between 0 and 1 else the behaviour will be indeterminate.
    static double proportionElitism;

    // Carries out a size GenerationalTournamentPopulation::tournamentSize tournament:
    //const IProgram<T>* SelectProgByFitness() const;
    const IProgramSubclass* SelectProgByFitness() const;
    static unsigned int tournamentSize;

  protected:
    void AddElite(unsigned int number, std::vector<IProgramSubclass*>& addTo);
  };
}



////////////////////////////////// Inline Function Definitions /////////////////////////////////
template <class IProgramSubclass, class T> void 
VUWLGP::GenerationalTournamentPopulation<IProgramSubclass, T>::IteratePopulation() {
  // Store the next population of programs into another vector temporarily:
  std::vector<IProgramSubclass*> nextGen;
  nextGen.reserve(config->populationSize);

  // Carry out the elitism, rounding down the number of individuals we create according 
  // to the proportion.
  AddElite(static_cast<unsigned int>(proportionElitism * Size()), nextGen);

  // Carry out the other evolutionary operations, weightings are percentages since 
  // they sum to 100:
  VUWLGP::WeightedCollection<IProgramSubclass* 
    (IPopulation<IProgramSubclass, T>::*)() const> o;
  o.AddElement(&IPopulation<IProgramSubclass, T>::MacroMutation, 30);
  o.AddElement(&IPopulation<IProgramSubclass, T>::MicroMutation, 30);
  o.AddElement(&IPopulation<IProgramSubclass, T>::CrossoverFree, 30);

  while(nextGen.size() < Size()) {
    nextGen.push_back((this->*o.GetRandomElement())());
  }
  
  // Delete the existing generation and replace it with the one just built.
  for(unsigned int i = 0; i < Size(); ++i) {
    delete programs[i];
  }

  programs.clear();
  programs.reserve(nextGen.size());
  for(unsigned int i = 0; i < nextGen.size(); ++i) {
    nextGen[i]->RandomlyCullToSize(config->maxLength);
    programs.push_back(nextGen[i]);
  }
}



template <class IProgramSubclass, class T> const IProgramSubclass*
VUWLGP::GenerationalTournamentPopulation<IProgramSubclass, T>::SelectProgByFitness() const {
  if(Size() == 1) {
    return dynamic_cast<IProgramSubclass*>(programs[0]);
  }

  unsigned int bestProgIndex = Rand::Int(programs.size());
  unsigned int candidateProgIndex;

  for(unsigned int i = 1; i < tournamentSize && i < Size(); ++i) {
    candidateProgIndex = Rand::Int(Size());
 
    // If the best program has a higher (worse) fitness than the candidate then the candidate is
    // the new best:
    if(bestProgIndex == candidateProgIndex) {
      --i; // Select another, different program
    }
    else if(programs[bestProgIndex]->Fitness() > programs[candidateProgIndex]->Fitness()) {
	bestProgIndex = candidateProgIndex;
    }
  }
  
  return dynamic_cast<IProgramSubclass*>(programs[bestProgIndex]);
}



template <class IProgramSubclass, class T> 
void VUWLGP::GenerationalTournamentPopulation<IProgramSubclass, T>::AddElite
(unsigned int number, std::vector<IProgramSubclass*>& addTo) {
  SortFittestFirst();
  for(unsigned int i = 0; i < number; ++i) {
    IProgramSubclass* temp = dynamic_cast<IProgramSubclass*>(programs[i]);
    addTo.push_back(new IProgramSubclass(*temp));
  }
}
#endif
