#ifndef NOO_ERROR_H
#define NOO_ERROR_H

#include "shim2/main.h"

namespace noo {

namespace util {

class SHIM2_EXPORT Error {
public:
	Error();
	Error(std::string error_message);
	virtual ~Error();
	
	std::string error_message;
};

class SHIM2_EXPORT MemoryError : public Error {
public:
	MemoryError(std::string error_message);
	virtual ~MemoryError();
};

class SHIM2_EXPORT LoadError : public Error {
public:
	LoadError(std::string error_message);
	virtual ~LoadError();
};

class SHIM2_EXPORT FileNotFoundError : public Error {
public:
	FileNotFoundError(std::string error_message);
	virtual ~FileNotFoundError();
};

class SHIM2_EXPORT GLError : public Error {
public:
	GLError(std::string error_message);
	virtual ~GLError();
};

} // End namespace util

} // End namespace noo

#endif // NOO_ERROR_H
