// Plugh.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Story.h"
#include <iostream>

int main()
{
	try
	{
		Story s("minizork.z3");
		//Story s("ZORK1.DAT");
	}
	catch (std::exception ex)
	{
		std::cout << ex.what() << "\n";
		int foo;
		std::cin >> foo;
	}
	return 0;
}

