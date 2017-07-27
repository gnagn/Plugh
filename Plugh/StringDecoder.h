#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "StoryMemory.h"

namespace StringDecoder
{
	typedef std::uint16_t word;
	typedef std::uint8_t byte;

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


	struct StateKey
	{
		State currentState;
		byte inputState;
	};

	inline bool operator<(const StateKey& lh, const StateKey& rh) {
		return lh.currentState < rh.currentState || (lh.currentState == rh.currentState && lh.inputState < rh.inputState);
	}

	struct StateResult
	{
		State newState;
		std::function<std::string(byte)> stateFunction;
	};

	class Decoder
	{
	public:
		Decoder(byte version, const StoryMemory& sm);
		~Decoder();

		std::string DecodeString(int stringAddress) const;
		std::string DecodeString(const std::vector<StringDecoder::word>& rawString) const;
		std::string GetAbbrev(int abbrevTableNum, byte abbrevNum) const;

	private:
		const byte version_;
		const StoryMemory& storyMemory_;

		static const char* alphabetTable;

		static std::vector<byte> extractChars(const std::vector<word>& encodedString);

		std::map<StateKey, StateResult> stateMap;
		std::map<State, StateResult> stateDefaultsMap;

		static std::string writeLetter(Alphabet alphabet, byte letter);
		static std::string skip(byte letter) { return ""; }
		std::string readAbbrev(int whichTable, byte whichAbbrev) const;

		byte firstMulti_;
		std::string firstMulti(byte b);
		std::string secondMulti(byte b);
		static const StoryMemory::story_pointer ADD_ABBREVS_TABLE_LOC = 0x18;
		StoryMemory::story_pointer getAbbrevsTablePointer() const { return storyMemory_.GetAddressAt(ADD_ABBREVS_TABLE_LOC); }
	};

}