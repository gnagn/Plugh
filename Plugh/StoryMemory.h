#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

class StoryMemory
{
public:
	typedef std::uint16_t word;
	typedef std::uint8_t byte;
	typedef std::vector<byte> story_memory;
	typedef std::uint16_t story_pointer;

	StoryMemory(std::string filename);
	~StoryMemory();

	story_memory::const_iterator ReadIterator(story_pointer address) const;
	story_memory::const_iterator AbbrevReadIterator(story_pointer address) const;

	byte GetByte(story_pointer address) const { return rawMemory[address]; }
	word GetWord(story_pointer wordAddress) const { return ((word)rawMemory[wordAddress] << 8) + rawMemory[wordAddress + 1]; }
	std::vector<word> GetWordsUntil(story_pointer baseAddress, const std::function<bool(word)>& endCondition) const;
	story_pointer GetAddressAt(story_pointer address) const { return GetWord(address); }
private:

	story_memory rawMemory;

	story_pointer getByteAddress(int value) const { return value; }
	story_pointer getWordAddress(int value) const { return 2 * value; }
	//enum class PackedAddressType
	//{
	//	routine,
	//	string
	//};
	//story_pointer getPackedAddress(int value, PackedAddressType type = PackedAddressType::routine) const;

};

