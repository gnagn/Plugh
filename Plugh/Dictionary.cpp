#include "stdafx.h"
#include "Dictionary.h"
#include "StringDecoder.h"

Dictionary::Dictionary(StoryMemory::byte version, const StoryMemory& storyMemory)
	:version_(version)
{
	parseDictionary(storyMemory);
}


Dictionary::~Dictionary()
{
}

void Dictionary::parseDictionary(const StoryMemory& storyMemory)
{
	StringDecoder::Decoder stringDecoder(version_, storyMemory);
	auto dictionaryPointer = storyMemory.GetAddressAt(ADD_DICTIONARY_LOC);

	StoryCursor cursor(storyMemory, dictionaryPointer);

	auto numSeparators = cursor.NextByte();
	std::vector<StoryMemory::byte> wordSeparators(numSeparators);
	for (auto i = 0; i < numSeparators; i++)
	{
		wordSeparators[i] = cursor.NextByte();
	}

	auto entryLength = cursor.NextByte();
	auto numEntries = cursor.NextWord();

	dictionaryEntries.resize(numEntries);
	std::vector<StoryMemory::word> curEntry(2);
	for (auto i = 0; i < numEntries; ++i)
	{
		curEntry[0] = cursor.NextWord();
		curEntry[1] = cursor.NextWord();
		for (auto j = 4; j < entryLength; ++j)
		{
			cursor.NextByte();
		}
		auto foo = stringDecoder.DecodeString(curEntry);
		dictionaryEntries[i] = stringDecoder.DecodeString(curEntry);

	}
}