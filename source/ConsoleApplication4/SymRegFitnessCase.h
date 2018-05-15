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
  
#ifndef SYMREGFITNESSCASE_H
#define SYMREGFITNESSCASE_H

#include <sstream>
#include <string>

#include "IFitnessCase.h"

namespace VUWLGP {
  template <class T> class SymRegFitnessCase: public IFitnessCase<T> {
  public:
    // Initialise a SymRegFitnessCase from a pattern file string (see docs for format)
    SymRegFitnessCase(double xVal, double yVal);

    // Return the 0-base class number of the class this case represents.
    double Y() const { return y; }

    // Returns a new SymRegFitnessCase. Caller should manage the memory (which
    // FitnessEnvironment will do if it is just passed to FitnessEnvironment<T>::AddCase).
    // GENERATE-FROM-STRING IS NOT IMPLEMENTED FOR THIS KIND OF FITNESS CASE
    //static IFitnessCase<T>* Generate(const std::string& patternFileLine);

  protected:
    // To make it non-abstract this method from IFitnessCase is instantiated
    void MakeAbstract() const { }

    // The desired or ideal value for the given x. X is stored as the first/only feature.
    double y;
  };
}



////////////////////////////////// INLINE FUNCTION DEFINITIONS /////////////////////////////////
template <class T> 
VUWLGP::SymRegFitnessCase<T>::SymRegFitnessCase(double xVal, double yVal):
y(yVal) {
  features.push_back(xVal);
}
#endif
