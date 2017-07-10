#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "StoryMemory.h"
#include "StringDecoder.h"

class Story
{
public:
	Story(std::string filename);
	~Story();

private:
	StoryMemory storyMemory;

	static const StoryMemory::story_pointer ADD_VERSION = 0x0;
	static const StoryMemory::story_pointer ADD_FLAGS1 = 0x1;
	static const StoryMemory::story_pointer ADD_HIMEM_BASE = 0x4;
	static const StoryMemory::story_pointer ADD_PROGRAM_INIT = 0x6;
	static const StoryMemory::story_pointer ADD_DICTIONARY_LOC = 0x8;
	static const StoryMemory::story_pointer ADD_OBJECT_TABLE_LOC = 0xA;
	static const StoryMemory::story_pointer ADD_GLOBAL_TABLE_LOC = 0xC;
	static const StoryMemory::story_pointer ADD_STATIC_MEM_BASE = 0xE;
	static const StoryMemory::story_pointer ADD_FLAGS2 = 0x10;
	static const StoryMemory::story_pointer ADD_ABBREVS_TABLE_LOC = 0x18;
	static const StoryMemory::story_pointer ADD_FILE_LENGTH = 0x1A;
	static const StoryMemory::story_pointer ADD_FILE_CHECKSUM = 0x1C;
	static const StoryMemory::story_pointer ADD_INTERPRETER_NUMBER = 0x1E;
	static const StoryMemory::story_pointer ADD_INTERPRETER_VERSION = 0x1F;
	static const StoryMemory::story_pointer ADD_SCREEN_HEIGHT_LINES = 0x20;
	static const StoryMemory::story_pointer ADD_SCREEN_WIDTH_CHARS = 0x21;
	static const StoryMemory::story_pointer ADD_SCREEN_HEIGHT_UNITS = 0x22;
	static const StoryMemory::story_pointer ADD_SCREEN_WIDTH_UNITS = 0x24;
	static const StoryMemory::story_pointer ADD_FONT_WIDTH_UNITS_V5 = 0x26;
	static const StoryMemory::story_pointer ADD_FONT_HEIGHT_UNITS_V6 = 0x26;
	static const StoryMemory::story_pointer ADD_FONT_HEIGHT_UNITS_V5 = 0x27;
	static const StoryMemory::story_pointer ADD_FONT_WIDTH_UNITS_V6 = 0x27;
	static const StoryMemory::story_pointer ADD_ROUTINES_OFFSET = 0x28;
	static const StoryMemory::story_pointer ADD_STATIC_STRINGS_OFFSET = 0x2A;
	static const StoryMemory::story_pointer ADD_BACKGROUND_COLOR = 0x2C;
	static const StoryMemory::story_pointer ADD_FOREGROUND_COLOR = 0x2D;
	static const StoryMemory::story_pointer ADD_TERMINATING_CHARS_TABLE_LOC = 0x2E;
	static const StoryMemory::story_pointer ADD_STREAM_3_PIXELS = 0x30;
	static const StoryMemory::story_pointer ADD_STANDARD_REVISION = 0x32;
	static const StoryMemory::story_pointer ADD_ALPHABET_TABLE_LOC = 0x34;
	static const StoryMemory::story_pointer ADD_HEADER_EXTENSION_TABLE_LOC = 0x36;

	StoryMemory::byte getStoryVersion() const { return storyMemory.GetByte(ADD_VERSION); }
	StoryMemory::story_pointer getAbbrevsTablePointer() const { return storyMemory.GetAddressAt(ADD_ABBREVS_TABLE_LOC); }
	StringDecoder stringDecoder;

	std::string getAbbrev(int abbrevTableNum, int abbrevNum) const;

};

