#pragma once

// redifined platform macros for conditional compilation
#if defined __linux__
#define WINPLAT_LINUX
#define WIN_USE_OPENGL
#include <iostream>

#elif defined _WIN32
#define WINPLAT_WINDOWS
#define WIN_USE_OPENGL
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#else
#error "unsupported platform"
#endif

#include <string>

#define WIN_NO_COPY_MOVE(classname) \
	classname(const classname&) = delete; \
	classname(classname&&) = delete; \
	void operator=(const classname&) = delete; \
	void operator=(classname&&) = delete

#define WIN_NO_COPY(classname) \
	classname(const classname&) = delete; \
	void operator=(const classname&) = delete

namespace win
{

[[noreturn]] inline void bug(const std::string &msg)
{
#ifdef WINPLAT_WINDOWS
	MessageBox(NULL, ("IMPLEMENTATION BUG:\n" + msg).c_str(), "BUG", MB_ICONEXCLAMATION);
#else
	std::cerr << "IMPLEMENTATION BUG:\n=================\n" << msg << "\n=================" << std::endl;
#endif
	std::abort();
}

}

#include <chrono>

#define WIN_APPEND2(a, b) a##b
#define WIN_APPEND(a, b) WIN_APPEND2(a, b)

#define bench(name, x, thresholdms) \
	const auto WIN_APPEND(benchmark, __LINE__) = std::chrono::high_resolution_clock::now();\
	x;\
	const float WIN_APPEND(benchmarkms, __LINE__) = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - WIN_APPEND(benchmark, __LINE__)).count(); \
	if (WIN_APPEND(benchmarkms, __LINE__) >= thresholdms) fprintf(stderr, "%s: %.4fms\n", name, WIN_APPEND(benchmarkms, __LINE__))
