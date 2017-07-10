#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <functional>

class StringDecoder
{
	typedef std::uint16_t word;
	typedef std::uint8_t byte;

public:
	StringDecoder(byte version);
	~StringDecoder();

	std::string DecodeString(const std::vector<word>& encodedString);

private:
	const byte version_;
	static const char* alphabetTable;

	std::string decodedString_;
	
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
	State currentState_;

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

	std::map<std::pair<State, byte>, std::pair<State, std::function<void(byte)>>> stateMap;
	std::map<State, std::pair<State, std::function<void(byte)>>> stateDefaultsMap;

	void writeLetter(Alphabet alphabet, byte letter);
	void skip(byte letter) { return; }
	void readAbbrev(int whichTable, byte whichAbbrev);
};

