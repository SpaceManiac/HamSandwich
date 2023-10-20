#include "string_extras.h"
#include <string.h>

char* StringDestination::prepare(size_t *len)
{
	if (!ptr || !sz)
	{
		*len = 0;
		return nullptr;
	}
	else if (sz == SZ_STD_STRING)
	{
		std::string* s = static_cast<std::string*>(ptr);
		s->resize(*len);
		return s->data();
	}
	else
	{
		char* buf = (char*) ptr;
		if (*len > sz - 1)
			*len = sz - 1;
		buf[*len] = '\0';
		return buf;
	}
}

void StringDestination::assign(std::string_view s)
{
	size_t len = s.length();
	char* ptr = prepare(&len);
	memcpy(ptr, s.data(), len);
}
