//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

#include <assert.h>

#define runtime_assert(x) if (!(x)) { printf("rip\n"); printf(#x); assert(#x); abort(); }