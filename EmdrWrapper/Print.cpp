#include "Print.h"
#include "EmdrWrapper.h"

size_t Print::print(const char text[])
{
    return EmdrWrapper::EmdrSketch::lcd::OnPrint(text); 
}

size_t Print::print(char c)
{
    char text[2] = { c, 0 };
    return EmdrWrapper::EmdrSketch::lcd::OnPrint(text);
}