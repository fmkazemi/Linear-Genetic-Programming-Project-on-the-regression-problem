
  
#ifndef REGISTERCOLLECTION_H
#define REGISTERCOLLECTION_H

#include <sstream>
#include <string>
#include <vector>

namespace VUWLGP {
  // This class represents some collection of registers. If they are read-only just make
  // the collection const.
  template<class T> class RegisterCollection {
  public:
    // Creates a RegisterCollection of size size and type T.
    RegisterCollection(unsigned int size): registers(size) { };

    // Returns the size (number of registers) of this RegisterCollection
    unsigned int Size() const { return registers.size(); }

    // Returns the value of the i'th register. Does not bounds check.
    T Read(unsigned int i) const { return registers[i]; }

    // Writes the value T to the i'th register. Does not bounds check.
    void Write(unsigned int i, T val) { registers[i] = val; }

    // Zeroes all of the registers
    void ZeroRegisters();

    // Helper Methods:
    // Returns the register index of the register with the highest value (used in, e.g. winner
    // takes all). If two registers are equal it returns the one with the lower index.
    unsigned int LargestRegisterIndex() const;

    std::string ToString() const;

  private:
    std::vector<T> registers;
  };
}



//////////////////////////////////// INLINE FUNCTION METHODS ///////////////////////////////////
template <class T>
void VUWLGP::RegisterCollection<T>::ZeroRegisters() {
  for(unsigned int i = 0; i < Size(); ++i) {
    Write(i, 0);
  }
}



template <class T> 
unsigned int VUWLGP::RegisterCollection<T>::LargestRegisterIndex() const {
  unsigned int max = 0;
  for(unsigned int i = 1; i < Size(); ++i) {
    if(registers[i] > registers[max]) {
      max = i;
    }
  }
  
  return max;
}



template <class T>
std::string VUWLGP::RegisterCollection<T>::ToString() const {
  std::ostringstream buffer;
  buffer << registers[0];
  for(unsigned int i = 1; i < Size(); ++i) {
    buffer << " " << registers[i];
  }
  return buffer.str();
}
#endif
