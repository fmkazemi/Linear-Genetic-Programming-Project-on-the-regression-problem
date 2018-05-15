
  
#ifndef FITNESSENVIRONMENT_H
#define FITNESSENVIRONMENT_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "IFitnessCase.h"
#include "RegisterCollection.h"

namespace VUWLGP {
  template <class T> class Config;

  // This class stores the memory environment, fitness cases and have methods for evaluating 
  // programs. T is the type of the features and registers for this problem. This class manages 
  // the memory of the fitness cases added to it.
  template <class T> class FitnessEnvironment {
  public:
    FitnessEnvironment(Config<T>* c);
    virtual ~FitnessEnvironment();

    // Returns the value of the i'th feature
    T ReadFeature(unsigned int i) const { return cases[currentCase]->F(i); }

    // Returns the value of register i
    T ReadRegister(unsigned int i) const { return registers.Read(i); }
    
    // Write the value val to the register i
    void WriteRegister(unsigned int i, T val) { registers.Write(i, val); }

    // Sets all read/write registers to have a value of 0
    void ZeroRegisters() { registers.ZeroRegisters(); }

    // Returns true if the feature vector for the first case could be loaded, false otherwise
    bool LoadFirstCase() { currentCase = 0; return NumberOfCases() > 0; }

    // Returns true if there was a "next case" to load
    bool LoadNextCase() { return ++currentCase < NumberOfCases(); }

    // Returns a pointer to the current case
    const IFitnessCase<T>* CurrentCase() { return cases[currentCase]; }

    // Returns the number of cases in this fitness environment:
    unsigned int NumberOfCases() const { return cases.size(); }

    // Adds the specified case to the FitnessEnvironment - these get passed to fitness measures
    // when a program's error is being updated, so that is where they should probably be checked
    // as being the correct type. An instance of this class will manage the memory of the 
    // pointers to the fitness cases.
    void AddCase(IFitnessCase<T>* fc) { cases.push_back(fc); }


    // Assumes the file is a series of fitness cases of whatever type the function pointer
    // expects, one per line.
    void AddCasesFromFile(std::string filePath, 
			  IFitnessCase<T>* (*generator)(const std::string&));

    // Returns a string representation of this FitnessEnvironment, i.e. the value of the
    // features for the current fitness case and the current values of the registers. Does
    // not place a trailing new line after the string representation, even though its multiline.
    std::string ToString() const;
    
  private:
    RegisterCollection<T> registers; // the read-write registers
    Config<T>* config; // the configuration variables

    std::vector<IFitnessCase<T>*> cases;
    unsigned int currentCase;
  };
}



////////////////////////////////// INLINE FUNCTION DEFINITIONS /////////////////////////////////
template <class T> 
VUWLGP::FitnessEnvironment<T>::FitnessEnvironment(VUWLGP::Config<T>* conf):
registers(conf->numRegisters),
config(conf),
currentCase(0) {
}



template <class T> 
VUWLGP::FitnessEnvironment<T>::~FitnessEnvironment() {
  for(unsigned int i = 0; i < cases.size(); ++i) {
    delete cases[i];
  }
}



template <class T> void
VUWLGP::FitnessEnvironment<T>::AddCasesFromFile(std::string filePath, 
					    IFitnessCase<T>* (*generator)(const std::string&)) {
  std::ifstream fin(filePath.c_str(), std::ios::in);
  std::string buffer;

  while(getline(fin, buffer)) {
    AddCase(generator(buffer));
  }
}



template <class T> 
std::string VUWLGP::FitnessEnvironment<T>::ToString() const {
  std::ostringstream buffer;

  buffer << "R: ";
  for(unsigned int i = 0; i < config->numRegisters; ++i) {
    buffer << ReadRegister(i) << " ";
  }
  buffer << std::endl;

  buffer << "F: ";
  for(unsigned int i = 0; i < config->numFeatures; ++i) {
    buffer << ReadFeature(i) << " ";
  }
  buffer << std::endl;

  buffer << "Number of cases loaded: " << cases.size();

  return buffer.str();
}
#endif
