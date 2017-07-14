#include "stdafx.h"
#include "StringDecoder.h"
#include "util.h"

StringDecoder::StringDecoder(byte version, const StoryMemory& sm)
	: version_(version)
	, storyMemory_(sm)
{
	using namespace std::placeholders;

	stateMap[std::make_pair(State::READ_A0, Input::ABBREV_1)] = std::make_pair(State::READ_ABBREV_1, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A0, Input::ABBREV_2)] = std::make_pair(State::READ_ABBREV_2, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A0, Input::ABBREV_3)] = std::make_pair(State::READ_ABBREV_3, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A0, Input::SHIFT_A1)] = std::make_pair(State::READ_A1, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A0, Input::SHIFT_A2)] = std::make_pair(State::READ_A2, &StringDecoder::skip);

	stateMap[std::make_pair(State::READ_A1, Input::ABBREV_1)] = std::make_pair(State::READ_ABBREV_1, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A1, Input::ABBREV_2)] = std::make_pair(State::READ_ABBREV_2, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A1, Input::ABBREV_3)] = std::make_pair(State::READ_ABBREV_3, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A1, Input::SHIFT_A1)] = std::make_pair(State::READ_A1, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A1, Input::SHIFT_A2)] = std::make_pair(State::READ_A2, &StringDecoder::skip);

	stateMap[std::make_pair(State::READ_A2, Input::ABBREV_1)] = std::make_pair(State::READ_ABBREV_1, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A2, Input::ABBREV_2)] = std::make_pair(State::READ_ABBREV_2, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A2, Input::ABBREV_3)] = std::make_pair(State::READ_ABBREV_3, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A2, Input::SHIFT_A1)] = std::make_pair(State::READ_A1, &StringDecoder::skip);
	stateMap[std::make_pair(State::READ_A2, Input::SHIFT_A2)] = std::make_pair(State::READ_A2, &StringDecoder::skip);

	stateDefaultsMap[State::READ_ABBREV_1] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::readAbbrev, this, 1, _1));
	stateDefaultsMap[State::READ_ABBREV_2] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::readAbbrev, this, 2, _1));
	stateDefaultsMap[State::READ_ABBREV_3] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::readAbbrev, this, 3, _1));

	stateDefaultsMap[State::READ_A0] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::writeLetter, Alphabet::A0, _1));
	stateDefaultsMap[State::READ_A1] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::writeLetter, Alphabet::A1, _1));
	stateDefaultsMap[State::READ_A2] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::writeLetter, Alphabet::A2, _1));

}


StringDecoder::~StringDecoder()
{
}

const char* StringDecoder::alphabetTable("      abcdefghijklmnopqrstuvwxyz      ABCDEFGHIJKLMNOPQRSTUVWXYZ       \n0123456789.,!?_#'\" / \\ - :()");

std::string StringDecoder::DecodeString(int stringAddress) const
{
	auto encodedString = storyMemory_.GetWordsUntil(stringAddress, ([](StoryMemory::word w) { return (w & 0x8000) == 0 ? false : true; }));

	auto encodedChars = extractChars(encodedString);
	auto currentState = State::READ_A0;
	std::string decodedString = "";

	for (auto c : encodedChars)
	{
		auto newState = stateMap.find(std::make_pair(currentState, c));
		if (newState == stateMap.end())
		{
			auto defaultState = stateDefaultsMap.find(currentState);
			if (defaultState == stateDefaultsMap.end())
			{
				throw std::exception("Invalid State");
			}
			decodedString += defaultState->second.second(c);
			currentState = defaultState->second.first;
		}
		else {
			decodedString += newState->second.second(c);
			currentState = newState->second.first;
		}
	}

	return decodedString;
}

std::string StringDecoder::GetAbbrev(int abbrevTableNum, byte abbrevNum) const
{
	auto abbrevTable = getAbbrevsTablePointer();
	auto abbrevOffest = (32 * (abbrevTableNum - 1) + abbrevNum) * 2;
	auto abbrevStringPointer = storyMemory_.GetAddressAt(abbrevTable + abbrevOffest) * 2;
	auto encodedString = storyMemory_.GetWordsUntil(abbrevStringPointer, ([](StoryMemory::word w) { return (w & 0x8000) == 0 ? false : true; }));

	return DecodeString(abbrevStringPointer);
}

std::vector<StringDecoder::byte> StringDecoder::extractChars(const std::vector<word>& encodedString)
{
	std::vector<byte> outChars;
	for (auto curWord : encodedString)
	{
		for (auto i = 0; i < 3; ++i) {
			auto curLetter = util::getBits(curWord, 14 - i * 5, 5);
			outChars.push_back(static_cast<byte>(curLetter));
		}
	}
	return outChars;
}

std::string StringDecoder::writeLetter(Alphabet alphabet, byte letter) 
{
	int offset = 0;
	switch (alphabet)
	{
	case Alphabet::A1:
		offset = 32;
		break;
	case Alphabet::A2:
		offset = 64;
		break;
	default:
		offset = 0;
	}
	return std::string(alphabetTable + offset + letter, 1);
}

std::string StringDecoder::readAbbrev(int whichTable, byte whichAbbrev) const
{
	return GetAbbrev(whichTable, whichAbbrev);
}