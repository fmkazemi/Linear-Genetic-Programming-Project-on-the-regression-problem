
  
#ifndef IPOPULATION_H
#define IPOPULATION_H

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Config.h"
#include "FitnessEnvironment.h"
#include "Instruction.h"

namespace VUWLGP {
  // Forward declarations:
  template <class T> class IProgram;

  // Represents a population of programs. The template parameter IP should be some subclass of
  // IProgram, otherwise compilation errors will result. Program classes should have a 
  // constructor which takes an unsigned int (the program size/length) and a Config<T> object, 
  // in that order.
  template <class IProgramSubclass, class T> class IPopulation {
  public:
    IPopulation(Config<T>* conf);
    virtual ~IPopulation();

    // Evolves the population for up to the maximum number of generations against the fitness
    // cases in the FitnessEnvironment passed to it as a parameter. Returns true if a perfect
    // solution (fitness < epsilon) is found, otherwise false.
    virtual unsigned int Evolve(FitnessEnvironment<T>& fe);

    // Updates the fitness of all programs which currently have false fitness-is-correct status
    // flags. After this method is called all programs in this population will have their
    // fitness values set correctly.
    virtual void EvaluateFlaggedPrograms(FitnessEnvironment<T>& fe);

    // Returns true if a program with fitness <= epsilon exists.
    virtual bool SolutionExists() const;

    // This function builds the next generation from the current one (e.g. steady-state,
    // generational + hill climbing, whatever the function implements) according to the current
    // fitness of each program.
    virtual void IteratePopulation() = 0;

    // This function selects a program according to their fitnesses and the selection method
    // this function implements. The population object retains ownership of the program pointed
    // to by the pointer returned though and it probably should not be changed by the calling
    // method which gets the pointer.
    //virtual const IProgram<T>* SelectProgByFitness() const = 0;
    virtual const IProgramSubclass* SelectProgByFitness() const = 0;

    // Returns the number of programs in this population:
    unsigned int Size() const { return programs.size(); }

    // Sorts the population in ascending order of fitness, so that the fittest programs are 
    // first in the internal member (meaning they will be printed first).
    void SortFittestFirst();

    // Sorts the population in descending order of fitness, so that the least fit programs are 
    // first in the internal member (meaning they will be printed first, also useful in 
    // steady-state population iterations). Method is considered const even though it changes
    // the order of the instructions as this is not a substantial change.
    void SortFittestLast();

    // Randomises the fitness of each program in this population to be in the range [0,1).
    // Mostly useful in the unit tests. Method is considered const even though it changes
    // the order of the instructions as this is not a substantial change.
    void RandomiseFitness();

    // Returns a non-constant (so modifiable if necessary) pointer to the fittest program in the
    // population. Also sorts the programs into fitness ascending (fittest first) order. 
    IProgramSubclass* GetFittestProgram();

    // This method calls the statistics logging and full population logging functions. The 
    // method assumes the fitnesses are all correct and that the introns are correctly marked.
    // Not const because LogStatistics isn't.
    virtual void Log(std::string identifier);

    // Logs the statistics and sometimes the population, depending on the value of gen. The 
    // method assumes the fitnesses are all correct and that the introns are correctly marked.
    // Not const because LogStatistics isn't.
    virtual void Log(unsigned int gen);

    // Evolutionary Operators:
    IProgramSubclass* MacroMutation() const;
    IProgramSubclass* MicroMutation() const;
    IProgramSubclass* CrossoverFree() const; // described above the impl and in the docs
    IProgramSubclass* CrossoverGA() const; // described above the impl and in the docs
    IProgramSubclass* CrossoverHomologous() const; // described above the impl and in the docs
    IProgramSubclass* AddRandomInstruction() const;
    IProgramSubclass* RemoveRandomInstruction() const;
    
    // Returns a string representation of each program in the population. The fitness of each
    // program can optionally be printed, and the instructions which are currently marked as 
    // introns in each program will be commented out if the second parameter is true.
    virtual std::string ToString(bool printFitness = true, bool commentIntrons = false) const;

  protected:
    // Appends statistics regarding the population to the file filePath. identifier is the 
    // generation number or something similar identifying why the logging is happening. The
    // method assumes the fitneses for each program are correct. This method is not const
    // because it calls GetFittestProgram, which is not const because it sorts the internal
    // vector of programs.
    virtual void LogStatistics(std::string filePath, std::string identifier);

    // Logs the string representation of the entire population to the file filePath, overwriting
    // whatever is already at filePath. The method assumes the fitnesses are all correct and
    // that the introns are correctly marked.
    virtual void LogPopulation(std::string filePath) const;
    

    // The bool in each pair indicates whether or not the associated program has been changed
    // and therefore needs to have its fitness updated - false means its fitness is correct.
    std::vector<IProgram<T>*> programs;

    Config<T>* config;
  };



  ///////////////////////////////// Population Sorting Functors  ///////////////////////////////
  // Ascending IProgram sorting function - for std::sort. This will put the fittest programs at 
  // the beginning of the container.
  template <class T> struct IProgramAscendingSort {
    bool operator()(IProgram<T>* const & left, IProgram<T>* const & right) {
      return left->Fitness() < right->Fitness();
    }
  };
  
  // Descending IProgram sorting function - for std::sort. This will put the fittest programs 
  // at the beginning of the container.
  template <class T> struct IProgramDescendingSort {
    bool operator()(IProgram<T>* const & left, IProgram<T>* const & right) {
      return left->Fitness() > right->Fitness();
    }
  };
}



////////////////////////////////// Inline Function Definitions ////////////////////////////////
template <class IProgramSubclass, class T> 
VUWLGP::IPopulation<IProgramSubclass, T>::IPopulation(VUWLGP::Config<T>* conf):
config(conf) {
  programs.reserve(config->populationSize);

  // Now construct the programs, uniformly distributed in size over the range of sizes specified
  // in the config object:
  unsigned int size = config->initialMinLength;
  const int numEachSize = config->populationSize / (config->initialMaxLength - size + 1);
  for(unsigned int i = 0; i < config->populationSize; ++i) {
    programs.push_back(new IProgramSubclass(size, config));

    if(i != 0 && i % numEachSize == 0) {
      ++size;
    }
  }
}



template <class IProgramSubclass, class T>
VUWLGP::IPopulation<IProgramSubclass, T>::~IPopulation() {
  for(unsigned int i = 0; i < programs.size(); ++i) {
    delete programs[i];
  }
}



template <class IProgramSubclass, class T> unsigned int 
VUWLGP::IPopulation<IProgramSubclass, T>::Evolve(VUWLGP::FitnessEnvironment<T>& fe) {
  EvaluateFlaggedPrograms(fe);
  Log("initial");
 
  if(SolutionExists()) { return 0; }

  for(unsigned int generation = 1; generation <= config->maxGenerations; ++generation) {
     IteratePopulation();
     EvaluateFlaggedPrograms(fe);
     Log(generation);
     if(SolutionExists()) { return generation; }
  }

  return config->maxGenerations + 1;
}



template <class IProgSub, class T> void
VUWLGP::IPopulation<IProgSub, T>::EvaluateFlaggedPrograms(VUWLGP::FitnessEnvironment<T>& fe) {
  for(unsigned int i = 0; i < Size(); ++i) {
    if(!programs[i]->FitnessStatusReference()) {
      programs[i]->UpdateFitness(fe);
    }
  }
}



template <class IProgramSubclass, class T>
bool VUWLGP::IPopulation<IProgramSubclass, T>::SolutionExists() const {
  for(unsigned int i = 0; i < Size(); ++i) {
    if(programs[i]->Fitness() <= config->epsilon) {
      return true;
    }
  }

  return false;
}



template <class IProgramSubclass, class T>
void VUWLGP::IPopulation<IProgramSubclass, T>::SortFittestFirst() {
  std::sort(programs.begin(), programs.end(), VUWLGP::IProgramAscendingSort<T>());
}



template <class IProgramSubclass, class T>
void VUWLGP::IPopulation<IProgramSubclass, T>::SortFittestLast() {
  std::sort(programs.begin(), programs.end(), VUWLGP::IProgramDescendingSort<T>());
}



template <class IProgramSubclass, class T>
void VUWLGP::IPopulation<IProgramSubclass, T>::RandomiseFitness() {
  for(unsigned int i = 0; i < Size(); ++i) {
    programs[i]->RandomiseFitness();
  }
}



template <class IProgramSubclass, class T>
IProgramSubclass* VUWLGP::IPopulation<IProgramSubclass, T>::GetFittestProgram() {
  SortFittestFirst();
  return dynamic_cast<IProgramSubclass*>(programs[0]);
}



template <class IProgramSubclass, class T>
void VUWLGP::IPopulation<IProgramSubclass, T>::Log(std::string identifier) {
  this->LogStatistics(config->statsLogFilePath + ".txt", identifier);
  this->LogPopulation(config->popLogFilePath + "." + identifier + ".txt");
}



template <class IProgramSubclass, class T>
void VUWLGP::IPopulation<IProgramSubclass, T>::Log(unsigned int gen) {
  std::ostringstream conv;
  conv << gen;
  std::string genString = conv.str();

  this->LogStatistics((config->statsLogFilePath + ".txt").c_str(), genString);

  if(gen % config->popLogInterval == 0) {
    this->LogPopulation(config->popLogFilePath + "." + genString + ".txt");
  }
}



template <class IProgramSubclass, class T>
void VUWLGP::IPopulation<IProgramSubclass, T>::LogStatistics(std::string filePath, 
							     std::string identifier) {
  std::ofstream fout(filePath.c_str(), std::ios::out | std::ios::app);
  fout << "############################## " << identifier << " ##############################"
       << std::endl;

  // Calculate the statistics:
  double fitnessSum = 0;
  double sizeSum = 0;
  std::map<std::string, unsigned int> programCount;

  for(unsigned int i = 0; i < Size(); ++i) {
    fitnessSum += programs[i]->Fitness();
    sizeSum += programs[i]->Size();
    ++programCount[programs[i]->ToString()];
  }

  fout << std::left << std::setw(17) << "Diversity:" << programCount.size() << " / " 
       << Size() << std::endl;
  fout << std::left << std::setw(17) << "Average Fitness:" << fitnessSum / Size() 
       << std::endl;
  fout << std::left << std::setw(17) << "Average Size:" << sizeSum / Size() << "\n" 
       << std::endl;

  fout << "Current Best Program:\n" << GetFittestProgram()->ToString(true, true) 
       << std::endl;
  fout << "\n\n\n\n";
  fout.close();
}



template <class IProgramSubclass, class T> void
VUWLGP::IPopulation<IProgramSubclass, T>::LogPopulation(std::string filePath) const {
  std::ofstream fout(filePath.c_str(), std::ios::out);
  fout << ToString(true, true) << std::endl;
  fout.close();
}



template <class IProgramSubclass, class T> std::string 
VUWLGP::IPopulation<IProgramSubclass, T>::ToString(bool printFitness, 
						   bool commentIntrons) const {
  std::ostringstream buffer;

  for(unsigned int i = 0; i < Size(); ++i) {
    buffer << "Program " << i << ":\n" 
	   << programs[i]->ToString(printFitness, commentIntrons) << "\n" << std::endl;
  }

  return buffer.str();
}


    



//////////////////////////////// Evolutionary Operators ////////////////////////////////
template <class IProgramSubclass, class T> IProgramSubclass*
VUWLGP::IPopulation<IProgramSubclass, T>::MacroMutation() const { 
  IProgramSubclass* result = 
    new IProgramSubclass(*SelectProgByFitness());

  unsigned int mutand = Rand::Int(result->Size());
  delete result->RawAccessToTheInternalInstructions()[mutand];
  // Assume a population shares its config with the instructions, so just pass the its 
  // one in:
  result->RawAccessToTheInternalInstructions()[mutand] = 
    IProgramSubclass::InstructionFactory(config);
  result->FitnessStatusReference() = false;

  return result;
}



template <class IProgramSubclass, class T> IProgramSubclass*
VUWLGP::IPopulation<IProgramSubclass, T>::MicroMutation() const { 
  IProgramSubclass* result = 
    new IProgramSubclass(*SelectProgByFitness());
  result->RawAccessToTheInternalInstructions()[Rand::Int(result->Size())]->Mutate();
  result->FitnessStatusReference() = false;

  return result;
}



// Replaces a random sequence of instructions in the first parent with a random 
// sequence of instructions from the second. The sequences may be of different length. 
// Does *not* randomly cull to size if it exceeds the maximum length criterion.
template <class IProgramSubclass, class T> IProgramSubclass*
VUWLGP::IPopulation<IProgramSubclass, T>::CrossoverFree() const { 
  IProgramSubclass* first = 
    new IProgramSubclass(*SelectProgByFitness());

  const IProgramSubclass* second = SelectProgByFitness();

  // Subsequence to be removed:
  unsigned int firstStart = Rand::Int(first->Size() - 1);
  unsigned int firstEnd = firstStart + 1 + Rand::Int(first->Size() - firstStart);
  
  // Subsequence to be added:
  unsigned int secondStart = Rand::Int(second->Size() - 1);
  unsigned int secondEnd = secondStart + 1 + Rand::Int(second->Size() - secondStart);

  // Do the transfer...
  std::vector<VUWLGP::Instruction<T>*>& instrsFirst = 
    first->RawAccessToTheInternalInstructions();
  const std::vector<VUWLGP::Instruction<T>*>& instrsSecond = 
    second->ConstRawAccessToTheInternalInstructions();

  // ... first remove the subsequence from first.
  typename std::vector< VUWLGP::Instruction<T>* >::iterator insertAt =
    instrsFirst.erase(instrsFirst.begin() + firstStart, instrsFirst.begin() + firstEnd);

  // Then add the one from second in its place - can't insert a range directly since 
  // we're using pointers and they will only be shallow copied:
  for(unsigned int i = secondStart; i < secondEnd; ++i) {
    insertAt = instrsFirst.insert(insertAt, instrsSecond[i]->Clone());
  }

  first->RandomlyCullToSize(config->maxLength);
  first->FitnessStatusReference() = false;

  return first;
}



// Replaces a random sequence of instructions in the first parent with a random sequence
// of instructions from the second. The sequences will always be the same length but 
// may come from different positions in the second parent. If the programs are different
//  size just select again and do a free crossover.
template <class IProgramSubclass, class T> IProgramSubclass*
VUWLGP::IPopulation<IProgramSubclass, T>::CrossoverGA() const { 
  IProgramSubclass* first = 
    new IProgramSubclass(*SelectProgByFitness());

  const IProgramSubclass* second = SelectProgByFitness();

  if(first->Size() != second->Size()) {
    return CrossoverFree();
  }
  else {
    // Put copies of second.instructions [start,end) over [start,end) in first:
    unsigned int firstStart = Rand::Int(first->Size());
    unsigned int firstEnd = firstStart + Rand::Int(first->Size() - firstStart + 1);

    unsigned int secondStart = Rand::Int(first->Size() - (firstEnd - firstStart) + 1);
    unsigned int secondEnd = secondStart + (firstEnd - firstStart);

    unsigned int iFirst = firstStart; // Move these out of the loop declaration just to
    unsigned int iSecond = secondStart; // make shorter, so it fits on just one line.
    for(; iFirst < firstEnd && iSecond < secondEnd; ++iFirst, ++iSecond) {
      delete first->RawAccessToTheInternalInstructions()[iFirst];
      first->RawAccessToTheInternalInstructions()[iFirst] = 
	second->ConstRawAccessToTheInternalInstructions()[iSecond]->Clone();
    }
    
    first->FitnessStatusReference() = false;
    
    return first;
  }
}



// Replaces a random sequence of instructions in the first parent with a random sequence
// of instructions from the second. The sequences will be the same length and start at 
// the same position in both of the parents. If the programs are different size just 
// select again and do a free crossover.
template <class IProgramSubclass, class T> IProgramSubclass*
VUWLGP::IPopulation<IProgramSubclass, T>::CrossoverHomologous() const { 
  IProgramSubclass* first = 
    new IProgramSubclass(*SelectProgByFitness());

  const IProgramSubclass* second = SelectProgByFitness();

  if(first->Size() != second->Size()) {
    return CrossoverFree();
  }
  else {
    // Put copies of second.instructions [start,end) over [start,end) in first:
    unsigned int start = Rand::Int(first->Size());
    unsigned int end = start + Rand::Int(first->Size() - start + 1);

    for(unsigned int i = start; i < end; ++i) {
      delete first->RawAccessToTheInternalInstructions()[i];
      first->RawAccessToTheInternalInstructions()[i] = 
	second->ConstRawAccessToTheInternalInstructions()[i]->Clone();
    }

    first->FitnessStatusReference() = false;

    return first;			   
  }
}



template <class IProgramSubclass, class T> IProgramSubclass*
VUWLGP::IPopulation<IProgramSubclass, T>::AddRandomInstruction() const { 
  IProgramSubclass* result = 
    new IProgramSubclass(*SelectProgByFitness());

  typename std::vector<VUWLGP::Instruction<T>*>::iterator it = 
    result->RawAccessToTheInternalInstructions().begin() + Rand::Int(result->Size() + 1);
  result->RawAccessToTheInternalInstructions(). // continued on next line...
    insert(it, IProgramSubclass::InstructionFactory(config));
  
  result->RandomlyCullToSize(config->maxLength);
  result->FitnessStatusReference() = false;

  return result;
}



template <class IProgramSubclass, class T> IProgramSubclass*
VUWLGP::IPopulation<IProgramSubclass, T>::RemoveRandomInstruction() const { 
  IProgramSubclass* result = 
    new IProgramSubclass(*SelectProgByFitness());

  if(result->Size() == 1) {
    // Regress to macro-mutation in the case of a remove on a size 1 program:
    return MacroMutation();
  }

  result->RemoveRandomInstruction();
  result->FitnessStatusReference() = false;

  return result;
}
#endif
