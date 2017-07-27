#pragma once
#include "StoryMemory.h"
#include <vector>

class Dictionary
{
public:
	Dictionary(StoryMemory::byte version, const StoryMemory& storyMemory);
	~Dictionary();

	std::string GetEntry(int entryNum) { return dictionaryEntries[entryNum]; }
private:
	StoryMemory::byte version_;

	std::vector<std::string> dictionaryEntries;
	static const StoryMemory::story_pointer ADD_DICTIONARY_LOC = 0x8;

	void parseDictionary(const StoryMemory& storyMemory);
};

