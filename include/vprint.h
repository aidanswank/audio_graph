// This is a standardized way of variadic printing that is made for console applications. This will not work in JUCE GUI applications as they don't have a console window. For a version that works with JUCE GUI applications check VariadicPrintJUCE.h

#ifndef VARIADIC_PRINT_H
#define VARIADIC_PRINT_H

#include <iostream>

inline void print() { std::cout << std::endl; }

template <typename T, typename... RestTs>
void print(T&& toPrint, RestTs&&... rest)
{
	std::cout << toPrint << " ";
	print(rest...);
}

#endif