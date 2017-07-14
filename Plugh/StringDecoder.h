#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "StoryMemory.h"

class StringDecoder
{
	typedef std::uint16_t word;
	typedef std::uint8_t byte;

public:
	StringDecoder(byte version, const StoryMemory& sm);
	~StringDecoder();

	std::string DecodeString(int stringAddress) const;
	std::string GetAbbrev(int abbrevTableNum, byte abbrevNum) const;

private:
	const byte version_;
	const StoryMemory& storyMemory_;

	static const char* alphabetTable;

	static std::vector<byte> extractChars(const std::vector<word>& encodedString);

	enum State
	{
		READ_A0,
		READ_A1,
		READ_A2,
		READ_MULTIBYTE_1,
		READ_MULTIBYTE_2,
		READ_ABBREV_1,
		READ_ABBREV_2,
		READ_ABBREV_3,
	};

	enum Input
	{
		ABBREV_1 = 1,
		ABBREV_2,
		ABBREV_3,
		SHIFT_A1,
		SHIFT_A2,
		MULTI,
	};

	enum Alphabet
	{
		A0,
		A1,
		A2
	};

	std::map<std::pair<State, byte>, std::pair<State, std::function<std::string(byte)>>> stateMap;
	std::map<State, std::pair<State, std::function<std::string(byte)>>> stateDefaultsMap;

	static std::string writeLetter(Alphabet alphabet, byte letter);
	static std::string skip(byte letter) { return ""; }
	std::string readAbbrev(int whichTable, byte whichAbbrev) const;
	static const StoryMemory::story_pointer ADD_ABBREVS_TABLE_LOC = 0x18;
	StoryMemory::story_pointer getAbbrevsTablePointer() const { return storyMemory_.GetAddressAt(ADD_ABBREVS_TABLE_LOC); }
};

