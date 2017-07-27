#include "stdafx.h"
#include "StoryMemory.h"
#include <fstream>
#include <filesystem>

namespace fs = std::experimental::filesystem;

StoryMemory::StoryMemory(std::string filename)
{
	auto p = fs::current_path() / filename;
	auto size = fs::file_size(p);

	std::ifstream file(filename, std::ios::binary);

	rawMemory.resize(static_cast<story_memory::size_type>(size));
	rawMemory.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

}


StoryMemory::~StoryMemory()
{
}

StoryMemory::story_memory::const_iterator StoryMemory::ReadIterator(StoryMemory::story_pointer address) const
{
	return std::begin(rawMemory) + address;
}

// Abbreviations are stored with word addresses
StoryMemory::story_memory::const_iterator StoryMemory::AbbrevReadIterator(StoryMemory::story_pointer address) const
{
	return std::begin(rawMemory) + (2 * address);
}

std::vector<StoryMemory::word> StoryMemory::GetWordsUntil(story_pointer baseAddress, const std::function<bool(word)>& endCondition) const
{
	std::vector<word> outWords;
	auto curAddress = baseAddress;
	outWords.push_back(GetWord(curAddress));
	while (!endCondition(outWords.back()))
	{
		curAddress += 2;
		outWords.push_back(GetWord(curAddress));
	}
	return outWords;
}


StoryMemory::byte StoryCursor::NextByte() 
{ 
	auto val = storyMemory_.GetByte(pos_);
	pos_++;
	return val;
}

StoryMemory::word StoryCursor::NextWord()
{
	auto val = storyMemory_.GetWord(pos_);
	pos_ += 2;
	return val;
}
