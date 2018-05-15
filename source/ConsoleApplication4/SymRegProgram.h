/*
Copyright (C) Christopher Fogelberg, Mengjie Zhang 2005

This file is part of VUWLGP.

VUWLGP is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

VUWLGP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with VUWLGP; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
  
#ifndef SYMREGPROGRAM_H
#define SYMREGPROGRAM_H

#include "Config.h"
#include "IProgram.h"
#include "Instruction.h"
#include "SymRegFitnessMeasure.h"

namespace VUWLGP {
  // A basic multi-class LGP program. Note that this is done just by having its constructor
  // use a multi-class fitness measure. This can be trivially changed so a program for another
  // kind of problem can be easily created.
  template <class T> class SymRegProgram: public IProgram<T> {
  public:
    SymRegProgram(unsigned int programSize, Config<T>* conf);
    SymRegProgram(const SymRegProgram<T>& rhs);
    ~SymRegProgram() { }

    static Instruction<T>* InstructionFactory(Config<T>* c) { return new Instruction<T>(c); }

  protected:
    // To make it non-abstract this method from IProgram is instantiated
    void MakeAbstract() const { }
  };
}



////////////////////////////////// INLINE FUNCTION DEFINITIONS /////////////////////////////////
template <class T> 
VUWLGP::SymRegProgram<T>::SymRegProgram(unsigned int programSize, VUWLGP::Config<T>* conf):
IProgram<T>(conf, new SymRegFitnessMeasure<double>(conf)){ 
  // Construct all the instructions, they will be delete'd in the IProgram dtor, as will the
  // FitnessMeasure passed to its ctor.
  instructions.reserve(programSize);

  for(unsigned int i = 0; i < programSize; ++i) {
     instructions.push_back(InstructionFactory(config));
  }
}



template <class T>
VUWLGP::SymRegProgram<T>::SymRegProgram(const VUWLGP::SymRegProgram<T>& rhs): 
IProgram<T>(rhs) {
  // Copy the fitness measure, using its defaultly defined cctor
  VUWLGP::SymRegFitnessMeasure<T>* temp = 
    dynamic_cast<VUWLGP::SymRegFitnessMeasure<T>*>(rhs.fitnessMeasure);
  fitnessMeasure = new VUWLGP::SymRegFitnessMeasure<T>(*temp);
}
#endif

