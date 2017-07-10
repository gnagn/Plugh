#include "stdafx.h"
#include "StringDecoder.h"
#include "util.h"

StringDecoder::StringDecoder(byte version)
	: version_(version)
{
	//enum class State
	//{
	//	READ_A0,
	//	READ_A1,
	//	READ_A2,
	//	READ_MULTIBYTE_1,
	//	READ_MULTIBYTE_2,
	//	READ_ABBREV_1,
	//	READ_ABBREV_2,
	//	READ_ABBREV_3,
	//};
	//State currentState_;

	//enum class Input
	//{
	//	ABBREV_1 = 1,
	//	ABBREV_2,
	//	ABBREV_3,
	//	SHIFT_A1,
	//	SHIFT_A2,
	//	MULTI,
	//};
	using namespace std::placeholders;
	stateMap[std::make_pair(State::READ_A0, Input::ABBREV_1)] = std::make_pair(State::READ_ABBREV_1, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A0, Input::ABBREV_2)] = std::make_pair(State::READ_ABBREV_2, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A0, Input::ABBREV_3)] = std::make_pair(State::READ_ABBREV_3, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A0, Input::SHIFT_A1)] = std::make_pair(State::READ_A1, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A0, Input::SHIFT_A2)] = std::make_pair(State::READ_A2, std::bind(&StringDecoder::skip, this, _1));

	stateMap[std::make_pair(State::READ_A1, Input::ABBREV_1)] = std::make_pair(State::READ_ABBREV_1, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A1, Input::ABBREV_2)] = std::make_pair(State::READ_ABBREV_2, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A1, Input::ABBREV_3)] = std::make_pair(State::READ_ABBREV_3, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A1, Input::SHIFT_A1)] = std::make_pair(State::READ_A1, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A1, Input::SHIFT_A2)] = std::make_pair(State::READ_A2, std::bind(&StringDecoder::skip, this, _1));

	stateMap[std::make_pair(State::READ_A2, Input::ABBREV_1)] = std::make_pair(State::READ_ABBREV_1, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A2, Input::ABBREV_2)] = std::make_pair(State::READ_ABBREV_2, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A2, Input::ABBREV_3)] = std::make_pair(State::READ_ABBREV_3, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A1, Input::SHIFT_A1)] = std::make_pair(State::READ_A1, std::bind(&StringDecoder::skip, this, _1));
	stateMap[std::make_pair(State::READ_A1, Input::SHIFT_A2)] = std::make_pair(State::READ_A2, std::bind(&StringDecoder::skip, this, _1));

	stateDefaultsMap[State::READ_ABBREV_1] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::readAbbrev, this, 1, _1));
	stateDefaultsMap[State::READ_ABBREV_2] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::readAbbrev, this, 2, _1));
	stateDefaultsMap[State::READ_ABBREV_3] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::readAbbrev, this, 3, _1));

	stateDefaultsMap[State::READ_A0] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::writeLetter, this, Alphabet::A0, _1));
	stateDefaultsMap[State::READ_A1] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::writeLetter, this, Alphabet::A1, _1));
	stateDefaultsMap[State::READ_A2] = std::make_pair(State::READ_A0, std::bind(&StringDecoder::writeLetter, this, Alphabet::A2, _1));

}


StringDecoder::~StringDecoder()
{
}

const char* StringDecoder::alphabetTable("      abcdefghijklmnopqrstuvwxyz      ABCDEFGHIJKLMNOPQRSTUVWXYZ       \n0123456789.,!?_#'\" / \\ - :()");

std::string StringDecoder::DecodeString(const std::vector<word>& encodedString)
{
	auto encodedChars = extractChars(encodedString);
	currentState_ = State::READ_A0;
	decodedString_ = "";

	for (auto c : encodedChars)
	{
		auto newState = stateMap.find(std::make_pair(currentState_, c));
		if (newState == stateMap.end())
		{
			auto defaultState = stateDefaultsMap.find(currentState_);
			if (defaultState == stateDefaultsMap.end())
			{
				throw std::exception("Invalid State");
			}
			defaultState->second.second(c);
			currentState_ = defaultState->second.first;
		}
		else {
			newState->second.second(c);
			currentState_ = newState->second.first;
		}
	}

	return decodedString_;
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

void StringDecoder::writeLetter(Alphabet alphabet, byte letter)
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
	decodedString_ += alphabetTable[offset + letter];
}

void StringDecoder::readAbbrev(int whichTable, byte whichAbbrev)
{
	return;
}