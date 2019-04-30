#pragma once

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ARCH_64 1
#else
#define ARCH_32 1
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ARCH_64 1
#else
#define ARCH_32 1
#endif
#endif