
  
#ifndef IPROGRAM_H
#define IPROGRAM_H

#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Config.h"
#include "FitnessEnvironment.h"
#include "IFitnessMeasure.h"
#include "Instruction.h"
#include "RegisterCollection.h"

namespace VUWLGP {
  // The interface/abstract definition of a program - this can be specialised, e.g. for 
  // hill-climbing programs. T is the type of the features and registers. Sub classes 
  // must onlygive the constructors, although they can override other methods too if 
  // need be. They should also assign an IFitnessMeasure of some type (the right type 
  // whatever that is) to the member "fitnessMeasure" in their constructors.
  template <class T> class IProgram {
  public:
    IProgram(Config<T>* conf, IFitnessMeasure<T>* fm);
    IProgram(const IProgram& rhs);
    virtual ~IProgram();

    // This method is used to create all Instructions which are used with this subclass 
    // of IProgram. This method should really be pure virtual, but it just throws an 
    // error in itsimplementation in this class. Sub classes should (but don't have to) 
    // reimplement it.
    // Stupid C++ - it's old and broken.
    static Instruction<T>* InstructionFactory();
    
    // Executes the environment on the registers and current fitness case in fe. 
    // Assumes the registers start with the correct values (i.e. 0 or whatever is 
    // relevant). Caches the final register values into the member 
    // IProgram::finalRegisterValues.
    virtual void Execute(FitnessEnvironment<T>& fe);

    // Updates a program's fitness according to the fitness cases in the 
    // FitnessEnvironment passed to it.
    void UpdateFitness(FitnessEnvironment<T>& fe);

    // Sets the IsIntron flag to true for each instruction in the program if that 
    // instruction is a structural intron, false otherwise. Marking structural introns 
    // can massively speed up execution on long programs.
    virtual void MarkIntrons();

    // If the program is longer than the maximum length allowed by the configuration,
    // instructions are randomly selected and removed until it is at the maximum length.
    virtual void RandomlyCullToSize(unsigned int cullToThisSize);

    // Helper Methods, e.g. used in the unit tests and by evolutionary operators, and 
    // randomly cull to size:
    void RemoveRandomInstruction();

    // Returns the number of instructions (both introns and exons) in the program.
    virtual unsigned int Size() const { return instructions.size(); }

    // Returns a program's overall fitness level according to its fitness measure
    double Fitness() const { return fitnessMeasure->OverallFitness(); }

    // Just a wrapper around the IFitnessMeasure method
    void ZeroFitness() { fitnessStatus = false; fitnessMeasure->ZeroFitness(); }

    // Randomises this programs fitness to a value in the range [0,1). Mostly useful in 
    // the unit tests
    void RandomiseFitness() {fitnessStatus = false; fitnessMeasure->RandomiseFitness();}

    // NOTE: THIS METHOD VIOLATES THE WHOLE OBJECT ENCAPSULATION SCHEMATA. It is 
    // necessary though, else all of the EvolutionaryOperationFunc methods would have to
    // be declared as friends of this class or be members and that is just annoying for 
    // any user/implementor/extender of VUWLGP.
    std::vector<Instruction<T>*>& RawAccessToTheInternalInstructions();

    // NOTE: THIS METHOD VIOLATES THE WHOLE OBJECT ENCAPSULATION SCHEMATA. It works as 
    // RawAccessToTheInternalInstructions does, but returns const and works with const 
    // instances
    const std::vector<Instruction<T>*>& ConstRawAccessToTheInternalInstructions() const;

    // Returns a reference to the correctness-status of this programs FitnessMeasure - 
    // true if it is accurate, false if it is not or is unknown (the latter is more 
    // frequent/likely).
    bool& FitnessStatusReference() { return fitnessStatus; }

    // Returns a C++-style function as a string representation of this program.
    virtual std::string 
      ToString(bool printFitness = true, bool commentIntrons = false) const;
  protected:
    std::vector<Instruction<T>*> instructions;
    RegisterCollection<T> finalRegisterValues;
    IFitnessMeasure<T>* fitnessMeasure;
    bool fitnessStatus; // true if this program's fitness is correct, false otherwise
    
    Config<T>* config;

    // This method exists purely to make this class abstract:
    virtual void MakeAbstract() const = 0;
  };
}



////////////////////////////// INLINE FUNCTION DEFINITIONS /////////////////////////////
template <class T>
VUWLGP::IProgram<T>::IProgram(VUWLGP::Config<T>* conf, VUWLGP::IFitnessMeasure<T>* fm):
finalRegisterValues(conf->numRegisters),
fitnessMeasure(fm),
fitnessStatus(false),
config(conf) {
}



template <class T>
VUWLGP::IProgram<T>::IProgram(const VUWLGP::IProgram<T>& rhs):
finalRegisterValues(rhs.finalRegisterValues),
fitnessStatus(rhs.fitnessStatus),
config(rhs.config) {
  // The fitnessMeasure should be copied in the sub-class which uses IProgram; 
  // fitnessStatus, finalRegisterValues and config are copied in the init list.
  // Copy the instructions:
  instructions.reserve(rhs.Size());

  for(unsigned int i = 0; i < rhs.Size(); ++i) {
    instructions.push_back(rhs.instructions[i]->Clone());
  }
}



template <class T>
VUWLGP::IProgram<T>::~IProgram() {
  for(unsigned int i = 0; i < Size(); ++i) {
    delete instructions[i];
  }

  delete fitnessMeasure;
}



template <class T> 
VUWLGP::Instruction<T>* VUWLGP::IProgram<T>::InstructionFactory() { 
  throw std::string("I should be a pure virtual static function, but C++ is broken");
}



template <class T>
void VUWLGP::IProgram<T>::Execute(VUWLGP::FitnessEnvironment<T>& fe) {
  bool executeNextAssignment = true;
  typename std::vector<Instruction<T>*>::iterator i;

  for(i = instructions.begin(); i != instructions.end(); ++i) {
    if(executeNextAssignment) { 
      // Then we are not in the middle of nor have just finished a conjunction of 
      // conditionals w/ a false in them, so exec it  (unless is a structural intron, 
      // when we just carry on)
      if(!(*i)->IsIntron) {
	executeNextAssignment = (*i)->Execute(fe);
      }
      // else executeNextAssignment instruction stays true - structural introns can't 
      // effect code flow.
    }
    else {
      // If this is an assignment it is the one we should skip, so reset the execute 
      // flag (even if its a structural intron, although if it is all the conditionals 
      // before it should have been as well). Otherwise, if it's a conditional just skip
      // it - same reasoning about structural introns applys here as it does for 
      // assignment instructions.
      if(!(*i)->IsConditional()) {
	executeNextAssignment = true;
      }
      // else leave it false as it should be (as described in previous comment block)
    }
  }

  // Now cache finalRegisterValues
  for(unsigned int i = 0; i < config->numRegisters; ++i) {
    finalRegisterValues.Write(i,fe.ReadRegister(i));
  }
}



template <class T>
void VUWLGP::IProgram<T>::UpdateFitness(VUWLGP::FitnessEnvironment<T>& fe) {
  // Zero the fitness to a null state, mark the introns to optimise execution time:
  ZeroFitness();
  MarkIntrons();

  if(!fe.LoadFirstCase()) { 
    throw std::string("No fitness cases in fe to evaluate against"); 
  }
  
  do {
    fe.ZeroRegisters();
    Execute(fe);
    fitnessMeasure->UpdateError(finalRegisterValues, fe.CurrentCase());
  } while(fe.LoadNextCase());

  // This program's fitness measure now has the correct fitness values:
  fitnessStatus = true;
}



template <class T>
void VUWLGP::IProgram<T>::MarkIntrons() {
  /*
    Source: Brameier, M 2004  On Linear Genetic Programming (thesis)

    Algorithm 3.1 (detection of structural introns)
    1. Let set R_eff always contain all registers that are effective at the current
       program position. R_eff := { r | r is output register }.
       Start at the last program instruction and move backwards.
    2. Mark the next preceding operation in program with:
        destination register r_dest element-of R_eff.
       If such an instruction is not found then go to 5.
    3. If the operation directly follows a branch or a sequence of branches then mark
       these instructions too. Otherwise remove r_dest from R_eff .
    4. Insert each source (operand) register r_op of newly marked instructions in R_eff
       if not already contained. Go to 2.
    5. Stop. All unmarked instructions are introns.
  */

  typename std::vector<Instruction<T>*>::reverse_iterator ri;

  // Assume all instructions are introns and mark as such. We do it backwards because we
  // need a reverse iterator later on and hell, why allocate two iterators - clutters 
  // the code and the memory.
  //int count = 0;
  for(ri = instructions.rbegin(); ri != instructions.rend(); ++ri) {
    (*ri)->IsIntron = true;
  }

  // Assume all registers are involved in the output (base class can override this if 
  // need be)
  std::set<unsigned int> usedRegisters;

  for(unsigned int i = 0; i < config->numRegisters; ++i) {
    usedRegisters.insert(i);
  }

  // Now go through steps 2-4 of the algorithm:
  for(ri = instructions.rbegin(); ri != instructions.rend(); ++ri) {
    if((*ri)->IsConditional()) {
      // Then: If the next instruction is not an intron and if this instruction is not 
      // the last one in the program then this instruction is also not an intron, 
      // otherwise it is:
      if(ri != instructions.rbegin() && !(*(ri-1))->IsIntron) {
	(*ri)->IsIntron = false;

	// Add its arguments to the array of registers which are important, because its
	// not an intron:
	if((*ri)->FirstArgumentType() == ArgumentTypeRegister) {
	  usedRegisters.insert((*ri)->FirstArgumentIndex());
	}
	
	if((*ri)->SecondArgumentType() == ArgumentTypeRegister) {
	  usedRegisters.insert((*ri)->SecondArgumentIndex());
	}
      }
    }
    else { // Instruction ri is an assignment instruction
      if(usedRegisters.find((*ri)->DestinationIndex()) != usedRegisters.end()) {
	(*ri)->IsIntron = false;
	// If the instruction immediately before this one is a conditional then don't
	// remove the register this instruction writes to, as this instruction could be
	// a semantic intron. If this is the first instruction then we don't need to
	// worry what happens with the usedRegisters set since it isn't used anymore:
	if( (ri + 1) != instructions.rend() && !(*(ri + 1))->IsConditional() ) {
	  usedRegisters.erase((*ri)->DestinationIndex());
	}

	// Add its arguments to the array of registers which are important, because its
	// not an intron:
	if((*ri)->FirstArgumentType() == ArgumentTypeRegister) {
	  usedRegisters.insert((*ri)->FirstArgumentIndex());
	}
	
	if((*ri)->SecondArgumentType() == ArgumentTypeRegister) {
	  usedRegisters.insert((*ri)->SecondArgumentIndex());
	}
      }
    }
  }
}



template <class T>
void VUWLGP::IProgram<T>::RandomlyCullToSize(unsigned int cullToThisSize) {
  while(Size() > cullToThisSize) {
    RemoveRandomInstruction();
  }
}



template <class T> 
void VUWLGP::IProgram<T>::RemoveRandomInstruction() {
  typename std::vector<Instruction<T>*>::iterator it = instructions.begin() + Rand::Int(Size());
  instructions.erase(it);
  fitnessStatus = false;
}


template <class T>
std::vector<VUWLGP::Instruction<T>*>&
VUWLGP::IProgram<T>::RawAccessToTheInternalInstructions() { 
  return instructions; 
}



template <class T>
const std::vector<VUWLGP::Instruction<T>*>&
VUWLGP::IProgram<T>::ConstRawAccessToTheInternalInstructions() const {
  return instructions;
}



template <class T> std::string
VUWLGP::IProgram<T>::ToString(bool printFitness, bool commentIntrons) const {
  std::ostringstream buffer;

  if(printFitness) {
    buffer << "// Fitness Information: " << fitnessMeasure->ToString() << "\n";
  }
 
  buffer << "template <class T>\n";
  buffer << 
    "void VUWLGP::Program::Execute(std::vector<T>& r, const std::vector<T>& cf) {\n";

  for(unsigned int i = 0; i < instructions.size(); ++i) {
    buffer << "\t" << instructions[i]->ToString(commentIntrons) << "\n";
  }

  buffer << "}";

  return buffer.str();
}
#endif
