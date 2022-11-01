#pragma once

#include "stdint.h"


class IDisplay
{
public:
	IDisplay()
	{
	}

	virtual ~IDisplay()
	{
	}

	virtual void print(const char *str) = 0;

};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
