
  
#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>

#include "WeightedCollection.h"

namespace VUWLGP {
  template <class T> class Config; // needed for the typedefs/use of Config* in Config's defn
  template <class T> class IInstructionArgument;
  template <class T> class IInstructionOperation;
  template <class T> class IInstructionOperationConditional;
  template <class T> class IProgram;
  template <class P, class T> class IPopulation;

  // An instance of this class stores all of the configuration details of a single VUWLGP run.
  // T should be the type of the features and registers, P should be whichever subclass of
  // IProgram is being used.
  template <class T> class Config {
  private:
    // An operation generator function generates an IInstructionOperation for use as, 
    // e.g., the operation of an instruction.
    typedef IInstructionOperation<T>* (*OperationGeneratorFunc)(Config<T>*);

    // An argument generator function generates an IInstructionArgument* for use in, 
    // e.g., an an Instruction
    typedef IInstructionArgument<T>* (*ArgumentGeneratorFunc)(Config<T>*);

  public:
    // Should not really be copy-constructed, so the cctor is private.
    // All data members are public as they're just properties.
    Config();
    Config(int argc, char** argv, bool printAsParses); // parses the command line
    virtual ~Config();

    // Initialises the config object based on what are assumed to be the command line 
    // arguments passed to it:
    void Init(int argc, char** argv, bool printAsParses);

    // Program/Instruction configuration parameters:
    unsigned int numRegisters;
    unsigned int numFeatures;
    double epsilon; // If a program's fitness is <= this it is considered a perfect solution

    // Stores pointers to the generators of instruction operations (e.g. +, -, etc.)
    WeightedCollection<ArgumentGeneratorFunc>* argumentGenerators; // can't spec on CL
    WeightedCollection<OperationGeneratorFunc>* instructionOperations; // can't spec on CL

    // Population configuration parameters
    unsigned int initialMinLength;
    unsigned int initialMaxLength;
    unsigned int maxLength;
    unsigned int populationSize;

    // Evolutionary configuration - the maximum number of times the population will be 
    // changed and then re-evaluated, excluding the initial generation and evaluation of the 
    // full population. The 1'th is the result of 1 selection and iteration, the 2'th the result
    // of 2, the maxGeneration'th the result of maxGenerations. The 0'th is logged as 'initial'
    unsigned int maxGenerations;

    // Logging parameters - statistics log file, how often should the full pop be logged, etc.
    // .txt and generation information will be postpended to the paths given for the pop and
    // stats logs, but not to the run log file path, which should be a .csv file.
    unsigned int popLogInterval;
    std::string popLogFilePath;
    std::string statsLogFilePath;
    std::string runLogFilePath;

    // Random number generation parameters - the seed value and whether or not it was specified
    unsigned int randSeed;
    bool seedSpecified;

  private:
    Config(const Config& rhs) { }
  };
}



////////////////////////////// INLINE FUNCTION DEFINITIONS /////////////////////////////
template <class T>
VUWLGP::Config<T>::Config() {
  instructionOperations = new WeightedCollection<OperationGeneratorFunc>();
  argumentGenerators = new WeightedCollection<ArgumentGeneratorFunc>();
}



template <class T>
VUWLGP::Config<T>::Config(int argc, char** argv, bool printAsParses) {
  instructionOperations = new WeightedCollection<OperationGeneratorFunc>();
  argumentGenerators = new WeightedCollection<ArgumentGeneratorFunc>();

  Init(argc, argv, printAsParses);
}



template <class T>
void VUWLGP::Config<T>::Init(int argc, char** argv, bool printAsParses) {
  for(int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    unsigned int colonPos = arg.find(':');

    if(colonPos == std::string::npos) { // then this arg isn't validly structured, so ignore
      std::cout << "Invalidly structured argument: '" << arg << "'" << std::endl;
      continue;
    }
    else if(arg.substr(0, colonPos) == "numRegisters") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> numRegisters;
      if(printAsParses) { 
	std::cout << "Set Config::numRegisters to " << numRegisters << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "numFeatures") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> numFeatures;
      if(printAsParses) { 
	std::cout << "Set Config::numFeatures to " << numFeatures << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "epsilon") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> epsilon;
      if(printAsParses) { 
	std::cout << "Set Config::epsilon to " << epsilon << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "initialMinLength") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> initialMinLength;
      if(printAsParses) { 
	std::cout << "Set Config::initialMinLength to " << initialMinLength << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "initialMaxLength") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> initialMaxLength;
      if(printAsParses) { 
	std::cout << "Set Config::initialMaxLength to " << initialMaxLength << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "maxLength") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> maxLength;
      if(printAsParses) { 
	std::cout << "Set Config::maxLength to " << maxLength << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "populationSize") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> populationSize;
      if(printAsParses) { 
	std::cout << "Set Config::populationSize to " << populationSize << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "maxGenerations") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> maxGenerations;
      if(printAsParses) { 
	std::cout << "Set Config::maxGenerations to " << maxGenerations << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "popLogInterval") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> popLogInterval;
      if(printAsParses) { 
	std::cout << "Set Config::popLogInterval to " << popLogInterval << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "popLogFilePath") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> popLogFilePath;
      if(printAsParses) { 
	std::cout << "Set Config::popLogFilePath to " << popLogFilePath << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "statsLogFilePath") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> statsLogFilePath;
      if(printAsParses) { 
	std::cout << "Set Config::statsLogFilePath to " << statsLogFilePath << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "runLogFilePath") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> runLogFilePath;
      if(printAsParses) {
	std::cout << "Set Config::runLogFilePath to " << runLogFilePath << std::endl;
      }
    }
    else if(arg.substr(0, colonPos) == "randSeed") {
      std::istringstream conv(arg.substr(colonPos + 1));
      conv >> randSeed;
      seedSpecified = true;
      if(printAsParses) { 
	std::cout << "Set Config::randSeed to " << randSeed << std::endl;
      }
    }
    else if(colonPos != std::string::npos) {
      std::cout << "Argument '" << arg << "' unrecognised." << std::endl;
    }
  }
}



template <class T> VUWLGP::Config<T>::~Config() {
  delete instructionOperations;
  delete argumentGenerators;
}
#endif
