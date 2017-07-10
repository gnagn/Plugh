#pragma once
#include <cstdint>
#include <cassert>

namespace util
{
	std::uint16_t getBits(std::uint16_t word, unsigned int firstBit, unsigned int length)
	{
		assert(firstBit < 16);
		assert(length <= 16);

		std::uint16_t bits = word >> (firstBit - length + 1);
		std::uint16_t mask = 0xFFFF >> (16 - length);
		return bits & mask;
	}
}