#ifndef IOEXT_H
#define IOEXT_H

#include <iostream>
#include <memory>

class FilePtrStream : public std::iostream
{
	std::unique_ptr<std::streambuf> sb;
public:
	explicit FilePtrStream(FILE *f);
};

#endif  // IOEXT_H
