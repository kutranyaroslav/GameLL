#pragma once
#include <stdint.h> 
using Bitset = uint32_t;
class Bitmask
{
public:
	Bitmask():bits(0){}
	Bitmask(const Bitset& i_bits):bits(i_bits){}
	Bitset getMask() { return bits; }
	void setMask(const Bitset& i_bits) {bits = i_bits;}
	bool Matches(const Bitmask& i_value, const Bitset& i_relevant = 0) const {
		return(i_relevant ?
			((i_value.getMask() & i_relevant) == (bits & i_relevant))
			: (i_value.getMask() == bits));
	}
	bool getBit(const unsigned int& i_pos) const {
		return((bits & (1 << i_pos)) != 0);
	}
	void turnOnBit(const unsigned int& i_pos) {
		bits |= 1 << i_pos;
	}
	void turnOnBits(const Bitset& i_bits) {
		bits |= i_bits;
	}
	void clearBit(const unsigned int& i_pos){
		bits &= ~(1 << i_pos);
	}
	void toggleBit(const unsigned int& i_pos) {
		bits ^= 1 << i_pos;
	}
	void Clear() { bits = 0; }

private:
	Bitset bits;
};

