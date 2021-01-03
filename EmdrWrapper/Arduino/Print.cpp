#include <stdio.h>
#include "Print.h"
#include "EmdrWrapper.h"

size_t Print::print(const char text[])
{
    return EmdrWrapper::EmdrSketch::lcd::OnPrint(text); 
}

size_t Print::print(char c)
{
    char text[2] = { c, 0 };
    return print(text);
}

size_t Print::print(unsigned char c, int format) 
{
    char text[256] = "";
    if (format == HEX)
        snprintf(text, sizeof(text), "0x%x", c);
    else
        snprintf(text, sizeof(text), "%u", c);

    return print(text);
}


size_t Print::print(const Printable& printable)
{
    return printable.printTo(*this);
}
