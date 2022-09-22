#ifndef NOO_TOKENIZER_H
#define NOO_TOKENIZER_H

#include "shim2/main.h"

namespace noo {

namespace util {

class SHIM2_EXPORT Tokenizer {
public:

	Tokenizer(std::string s, char delimiter, bool skip_bunches = false);
	std::string next();

private:
	std::string s;
	char delimiter;
	size_t offset;
	bool skip_bunches;
};

} // End namespace util

} // End namespace noo

#endif // NOO_TOKENIZER_H
