#include "stdafx.h"
#include "Story.h"
#include "StringDecoder.h"
#include <iostream>


Story::Story(std::string filename)
	:storyMemory(filename),
	stringDecoder(getStoryVersion())
{
	std::cout << static_cast<int>(getStoryVersion()) << "\n";
	for (auto i = 1; i < 4; i++)
	{
		for (auto j = 0; j < 32; j++)
		{
			std::cout << getAbbrev(i, j) << "\n";
		}
	}
	int bar;
	std::cin >> bar;
}


Story::~Story()
{
}

std::string Story::getAbbrev(int abbrevTableNum, int abbrevNum) const
{
	auto abbrevTable = getAbbrevsTablePointer();
	auto abbrevOffest = (32 * (abbrevTableNum - 1) + abbrevNum) * 2;
	auto abbrevStringPointer = storyMemory.GetAddressAt(abbrevTable + abbrevOffest) * 2;
	auto encodedString = storyMemory.GetWordsUntil(abbrevStringPointer, ([](StoryMemory::word w) { return (w & 0x8000) == 0 ? false : true; }));

	StringDecoder sd(getStoryVersion());
	return sd.DecodeString(encodedString);
}

//Story::story_pointer Story::getPackedAddress(int value, Story::PackedAddressType type) const
//{
//	switch (getStoryVersion())
//	{
//	case 1:
//	case 2:
//	case 3:
//		return 2 * value;
//		break;
//	case 4:
//	case 5:
//		return 4 * value;
//		break;
//	case 6:
//	case 7:
//		switch (type)
//		{
//		case Story::PackedAddressType::routine:
//			return 4 * value + getRoutinesOffset();
//			break;
//		case Story::PackedAddressType::string:
//			return 4 * value + getStringsOffset();
//			break;
//		default:
//			throw std::exception("foo");
//			break;
//		}
//		break;
//	case 8:
//		return 8 * value;
//		break;
//	default:
//		throw std::exception("foo");
//		break;
//	}
//}
