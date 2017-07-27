#include "stdafx.h"
#include "StringDecoder.h"
#include "util.h"
#include <boost/format.hpp>

using namespace StringDecoder;

Decoder::Decoder(byte version, const StoryMemory& sm)
	: version_(version)
	, storyMemory_(sm)
{
	using namespace std::placeholders;

	stateMap[StateKey{ State::READ_A0, Input::ABBREV_1 }] = StateResult{ State::READ_ABBREV_1, &Decoder::skip };
	stateMap[StateKey{ State::READ_A0, Input::ABBREV_2 }] = StateResult{ State::READ_ABBREV_2, &Decoder::skip };
	stateMap[StateKey{ State::READ_A0, Input::ABBREV_3 }] = StateResult{ State::READ_ABBREV_3, &Decoder::skip };
	stateMap[StateKey{ State::READ_A0, Input::SHIFT_A1 }] = StateResult{ State::READ_A1, &Decoder::skip };
	stateMap[StateKey{ State::READ_A0, Input::SHIFT_A2 }] = StateResult{ State::READ_A2, &Decoder::skip };

	stateMap[StateKey{ State::READ_A1, Input::ABBREV_1 }] = StateResult{ State::READ_ABBREV_1, &Decoder::skip };
	stateMap[StateKey{ State::READ_A1, Input::ABBREV_2 }] = StateResult{ State::READ_ABBREV_2, &Decoder::skip };
	stateMap[StateKey{ State::READ_A1, Input::ABBREV_3 }] = StateResult{ State::READ_ABBREV_3, &Decoder::skip };
	stateMap[StateKey{ State::READ_A1, Input::SHIFT_A1 }] = StateResult{ State::READ_A1, &Decoder::skip };
	stateMap[StateKey{ State::READ_A1, Input::SHIFT_A2 }] = StateResult{ State::READ_A2, &Decoder::skip };

	stateMap[StateKey{ State::READ_A2, Input::ABBREV_1 }] = StateResult{ State::READ_ABBREV_1, &Decoder::skip };
	stateMap[StateKey{ State::READ_A2, Input::ABBREV_2 }] = StateResult{ State::READ_ABBREV_2, &Decoder::skip };
	stateMap[StateKey{ State::READ_A2, Input::ABBREV_3 }] = StateResult{ State::READ_ABBREV_3, &Decoder::skip };
	stateMap[StateKey{ State::READ_A2, Input::SHIFT_A1 }] = StateResult{ State::READ_A1, &Decoder::skip };
	stateMap[StateKey{ State::READ_A2, Input::SHIFT_A2 }] = StateResult{ State::READ_A2, &Decoder::skip };
	stateMap[StateKey{ State::READ_A2, Input::MULTI }] = StateResult{ State::READ_MULTIBYTE_1, &Decoder::skip };

	stateDefaultsMap[State::READ_ABBREV_1] = StateResult{ State::READ_A0, std::bind(&Decoder::readAbbrev, this, 1, _1) };
	stateDefaultsMap[State::READ_ABBREV_2] = StateResult{ State::READ_A0, std::bind(&Decoder::readAbbrev, this, 2, _1) };
	stateDefaultsMap[State::READ_ABBREV_3] = StateResult{ State::READ_A0, std::bind(&Decoder::readAbbrev, this, 3, _1) };

	stateDefaultsMap[State::READ_A0] = StateResult{ State::READ_A0, std::bind(&Decoder::writeLetter, Alphabet::A0, _1) };
	stateDefaultsMap[State::READ_A1] = StateResult{ State::READ_A0, std::bind(&Decoder::writeLetter, Alphabet::A1, _1) };
	stateDefaultsMap[State::READ_A2] = StateResult{ State::READ_A0, std::bind(&Decoder::writeLetter, Alphabet::A2, _1) };

	stateDefaultsMap[State::READ_MULTIBYTE_1] = StateResult{ State::READ_MULTIBYTE_2, std::bind(&Decoder::firstMulti, this, _1) };
	stateDefaultsMap[State::READ_MULTIBYTE_2] = StateResult{ State::READ_A0, std::bind(&Decoder::secondMulti, this, _1) };
}


Decoder::~Decoder()
{
}

const char* Decoder::alphabetTable("      abcdefghijklmnopqrstuvwxyz      ABCDEFGHIJKLMNOPQRSTUVWXYZ       \n0123456789.,!?_#'\"/\\-:()");

std::string Decoder::DecodeString(int stringAddress) const
{
	auto encodedString = storyMemory_.GetWordsUntil(stringAddress, ([](StoryMemory::word w) { return (w & 0x8000) == 0 ? false : true; }));

	return DecodeString(encodedString);
}

std::string Decoder::DecodeString(const std::vector<StringDecoder::word>& encodedString) const
{
	auto encodedChars = extractChars(encodedString);
	auto currentState = State::READ_A0;
	std::string decodedString = "";

	for (auto c : encodedChars)
	{
		const auto newState = stateMap.find(StateKey{ currentState, c });
		if (newState == stateMap.end())
		{
			auto defaultState = stateDefaultsMap.find(currentState);
			if (defaultState == stateDefaultsMap.end())
			{
				throw std::runtime_error((boost::format("%1%(%2%): Invalid state provided. Current state: %3% -- New state: %4%") % __FILE__ % __LINE__ % currentState % (int)c).str());
			}
			decodedString += defaultState->second.stateFunction(c);
			currentState = defaultState->second.newState;
		}
		else {
			decodedString += newState->second.stateFunction(c);
			currentState = newState->second.newState;
		}
	}

	return decodedString;
}

std::string Decoder::GetAbbrev(int abbrevTableNum, byte abbrevNum) const
{
	auto abbrevTable = getAbbrevsTablePointer();
	auto abbrevOffest = (32 * (abbrevTableNum - 1) + abbrevNum) * 2;
	auto abbrevStringPointer = storyMemory_.GetAddressAt(abbrevTable + abbrevOffest) * 2;
	auto encodedString = storyMemory_.GetWordsUntil(abbrevStringPointer, ([](StoryMemory::word w) { return (w & 0x8000) == 0 ? false : true; }));

	return DecodeString(abbrevStringPointer);
}

std::vector<byte> Decoder::extractChars(const std::vector<word>& encodedString)
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

std::string Decoder::writeLetter(Alphabet alphabet, byte letter) 
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

std::string Decoder::readAbbrev(int whichTable, byte whichAbbrev) const
{
	return GetAbbrev(whichTable, whichAbbrev);
}

std::string Decoder::firstMulti(byte b)
{
	firstMulti_ = b;
	return "";
}

std::string Decoder::secondMulti(byte b)
{
	byte output = firstMulti_ << 5;
	output += b;
	return std::string(1, output);
}