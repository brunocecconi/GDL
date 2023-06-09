
#ifndef BC_PLATFORM_H
#define BC_PLATFORM_H

#include "Compiler.h"

// Copied from bx
// https://github.com/bkaradzic/bx/blob/master/include/bx/platform.h

// Architecture
#define BC_ARCH_32BIT 0
#define BC_ARCH_64BIT 0

// Compiler
#define BC_COMPILER_CLANG          0
#define BC_COMPILER_CLANG_ANALYZER 0
#define BC_COMPILER_GCC            0
#define BC_COMPILER_MSVC           0

// Endianness
#define BC_CPU_ENDIAN_BIG    0
#define BC_CPU_ENDIAN_LITTLE 0

// CPU
#define BC_CPU_ARM   0
#define BC_CPU_JIT   0
#define BC_CPU_MIPS  0
#define BC_CPU_PPC   0
#define BC_CPU_RISCV 0
#define BC_CPU_X86   0

// C Runtime
#define BC_CRT_BIONIC 0
#define BC_CRT_BSD    0
#define BC_CRT_GLIBC  0
#define BC_CRT_LIBCXX 0
#define BC_CRT_MINGW  0
#define BC_CRT_MSVC   0
#define BC_CRT_NEWLIB 0

#ifndef BC_CRT_NONE
#	define BC_CRT_NONE 0
#endif // BC_CRT_NONE

// Platform
#define BC_PLATFORM_ANDROID    0
#define BC_PLATFORM_BSD        0
#define BC_PLATFORM_EMSCRIPTEN 0
#define BC_PLATFORM_HAIKU      0
#define BC_PLATFORM_HURD       0
#define BC_PLATFORM_IOS        0
#define BC_PLATFORM_LINUX      0
#define BC_PLATFORM_NX         0
#define BC_PLATFORM_OSX        0
#define BC_PLATFORM_PS4        0
#define BC_PLATFORM_PS5        0
#define BC_PLATFORM_RPI        0
#define BC_PLATFORM_WINDOWS    0
#define BC_PLATFORM_WINRT      0
#define BC_PLATFORM_XBOXONE    0

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(__clang__)
// clang defines __GNUC__ or _MSC_VER
#	undef  BC_COMPILER_CLANG
#	define BC_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#	if defined(__clang_analyzer__)
#		undef  BC_COMPILER_CLANG_ANALYZER
#		define BC_COMPILER_CLANG_ANALYZER 1
#	endif // defined(__clang_analyzer__)
#elif defined(_MSC_VER)
#	undef  BC_COMPILER_MSVC
#	define BC_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#	undef  BC_COMPILER_GCC
#	define BC_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#	error "BC_COMPILER_* is not defined!"
#endif //

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
#if defined(__arm__)     \
 || defined(__aarch64__) \
 || defined(_M_ARM)
#	undef  BC_CPU_ARM
#	define BC_CPU_ARM 1
#	define BC_CACHE_LINE_SIZE 64
#elif defined(__MIPSEL__)     \
 ||   defined(__mips_isa_rev) \
 ||   defined(__mips64)
#	undef  BC_CPU_MIPS
#	define BC_CPU_MIPS 1
#	define BC_CACHE_LINE_SIZE 64
#elif defined(_M_PPC)        \
 ||   defined(__powerpc__)   \
 ||   defined(__powerpc64__)
#	undef  BC_CPU_PPC
#	define BC_CPU_PPC 1
#	define BC_CACHE_LINE_SIZE 128
#elif defined(__riscv)   \
 ||   defined(__riscv__) \
 ||   defined(RISCVEL)
#	undef  BC_CPU_RISCV
#	define BC_CPU_RISCV 1
#	define BC_CACHE_LINE_SIZE 64
#elif defined(_M_IX86)    \
 ||   defined(_M_X64)     \
 ||   defined(__i386__)   \
 ||   defined(__x86_64__)
#	undef  BC_CPU_X86
#	define BC_CPU_X86 1
#	define BC_CACHE_LINE_SIZE 64
#else // PNaCl doesn't have CPU defined.
#	undef  BC_CPU_JIT
#	define BC_CPU_JIT 1
#	define BC_CACHE_LINE_SIZE 64
#endif //

#if defined(__x86_64__)    \
 || defined(_M_X64)        \
 || defined(__aarch64__)   \
 || defined(__64BIT__)     \
 || defined(__mips64)      \
 || defined(__powerpc64__) \
 || defined(__ppc64__)     \
 || defined(__LP64__)
#	undef  BC_ARCH_64BIT
#	define BC_ARCH_64BIT 64
#else
#	undef  BC_ARCH_32BIT
#	define BC_ARCH_32BIT 32
#endif //

#if BC_CPU_PPC
// __BIG_ENDIAN__ is gcc predefined macro
#	if defined(__BIG_ENDIAN__)
#		undef  BC_CPU_ENDIAN_BIG
#		define BC_CPU_ENDIAN_BIG 1
#	else
#		undef  BC_CPU_ENDIAN_LITTLE
#		define BC_CPU_ENDIAN_LITTLE 1
#	endif
#else
#	undef  BC_CPU_ENDIAN_LITTLE
#	define BC_CPU_ENDIAN_LITTLE 1
#endif // BC_CPU_PPC

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Operating_Systems
#if defined(_DURANGO) || defined(_XBOX_ONE)
#	undef  BC_PLATFORM_XBOXONE
#	define BC_PLATFORM_XBOXONE 1
#elif defined(_WIN32) || defined(_WIN64)
// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif // NOMINMAX
//  If _USING_V110_SDK71_ is defined it means we are using the v110_xp or v120_xp toolset.
#	if defined(_MSC_VER) && (_MSC_VER >= 1700) && !defined(_USING_V110_SDK71_)
#		include <winapifamily.h>
#	endif // defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#	if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#		undef  BC_PLATFORM_WINDOWS
#		if !defined(WINVER) && !defined(_WIN32_WINNT)
#			if BC_ARCH_64BIT
//				When building 64-bit target Win7 and above.
#				define WINVER 0x0601
#				define _WIN32_WINNT 0x0601
#			else
//				Windows Server 2003 with SP1, Windows XP with SP2 and above
#				define WINVER 0x0502
#				define _WIN32_WINNT 0x0502
#			endif // BC_ARCH_64BIT
#		endif // !defined(WINVER) && !defined(_WIN32_WINNT)
#		define BC_PLATFORM_WINDOWS _WIN32_WINNT
#	else
#		undef  BC_PLATFORM_WINRT
#		define BC_PLATFORM_WINRT 1
#	endif
#elif defined(__ANDROID__)
// Android compiler defines __linux__
#	include <sys/cdefs.h> // Defines __BIONIC__ and includes android/api-level.h
#	undef  BC_PLATFORM_ANDROID
#	define BC_PLATFORM_ANDROID __ANDROID_API__
#elif defined(__VCCOREVER__)
// RaspberryPi compiler defines __linux__
#	undef  BC_PLATFORM_RPI
#	define BC_PLATFORM_RPI 1
#elif  defined(__linux__)
#	undef  BC_PLATFORM_LINUX
#	define BC_PLATFORM_LINUX 1
#elif  defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) \
	|| defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
#	undef  BC_PLATFORM_IOS
#	define BC_PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#	undef  BC_PLATFORM_OSX
#	define BC_PLATFORM_OSX __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#elif defined(__EMSCRIPTEN__)
#	undef  BC_PLATFORM_EMSCRIPTEN
#	define BC_PLATFORM_EMSCRIPTEN 1
#elif defined(__ORBIS__)
#	undef  BC_PLATFORM_PS4
#	define BC_PLATFORM_PS4 1
#elif defined(__PROSPERO__)
#	undef  BC_PLATFORM_PS5
#	define BC_PLATFORM_PS5 1
#elif  defined(__FreeBSD__)        \
	|| defined(__FreeBSD_kernel__) \
	|| defined(__NetBSD__)         \
	|| defined(__OpenBSD__)        \
	|| defined(__DragonFly__)
#	undef  BC_PLATFORM_BSD
#	define BC_PLATFORM_BSD 1
#elif defined(__GNU__)
#	undef  BC_PLATFORM_HURD
#	define BC_PLATFORM_HURD 1
#elif defined(__NX__)
#	undef  BC_PLATFORM_NX
#	define BC_PLATFORM_NX 1
#elif defined(__HAIKU__)
#	undef  BC_PLATFORM_HAIKU
#	define BC_PLATFORM_HAIKU 1
#endif //

#if !BC_CRT_NONE
// https://sourceforge.net/p/predef/wiki/Libraries/
#	if defined(__BIONIC__)
#		undef  BC_CRT_BIONIC
#		define BC_CRT_BIONIC 1
#	elif defined(_MSC_VER)
#		undef  BC_CRT_MSVC
#		define BC_CRT_MSVC 1
#	elif defined(__GLIBC__)
#		undef  BC_CRT_GLIBC
#		define BC_CRT_GLIBC (__GLIBC__ * 10000 + __GLIBC_MINOR__ * 100)
#	elif defined(__MINGW32__) || defined(__MINGW64__)
#		undef  BC_CRT_MINGW
#		define BC_CRT_MINGW 1
#	elif defined(__apple_build_version__) || defined(__ORBIS__) || defined(__EMSCRIPTEN__) || defined(__llvm__) || defined(__HAIKU__)
#		undef  BC_CRT_LIBCXX
#		define BC_CRT_LIBCXX 1
#	elif BC_PLATFORM_BSD
#		undef  BC_CRT_BSD
#		define BC_CRT_BSD 1
#	endif //

#	if !BC_CRT_BIONIC \
	&& !BC_CRT_BSD    \
	&& !BC_CRT_GLIBC  \
	&& !BC_CRT_LIBCXX \
	&& !BC_CRT_MINGW  \
	&& !BC_CRT_MSVC   \
	&& !BC_CRT_NEWLIB
#		undef  BC_CRT_NONE
#		define BC_CRT_NONE 1
#	endif // BC_CRT_*
#endif // !BC_CRT_NONE

///
#define BC_PLATFORM_POSIX (0   \
	||  BC_PLATFORM_ANDROID    \
	||  BC_PLATFORM_BSD        \
	||  BC_PLATFORM_EMSCRIPTEN \
	||  BC_PLATFORM_HAIKU      \
	||  BC_PLATFORM_HURD       \
	||  BC_PLATFORM_IOS        \
	||  BC_PLATFORM_LINUX      \
	||  BC_PLATFORM_NX         \
	||  BC_PLATFORM_OSX        \
	||  BC_PLATFORM_PS4        \
	||  BC_PLATFORM_PS5        \
	||  BC_PLATFORM_RPI        \
	)

///
#define BC_PLATFORM_NONE !(0   \
	||  BC_PLATFORM_ANDROID    \
	||  BC_PLATFORM_BSD        \
	||  BC_PLATFORM_EMSCRIPTEN \
	||  BC_PLATFORM_HAIKU      \
	||  BC_PLATFORM_HURD       \
	||  BC_PLATFORM_IOS        \
	||  BC_PLATFORM_LINUX      \
	||  BC_PLATFORM_NX         \
	||  BC_PLATFORM_OSX        \
	||  BC_PLATFORM_PS4        \
	||  BC_PLATFORM_PS5        \
	||  BC_PLATFORM_RPI        \
	||  BC_PLATFORM_WINDOWS    \
	||  BC_PLATFORM_WINRT      \
	||  BC_PLATFORM_XBOXONE    \
	)

///
#define BC_PLATFORM_OS_CONSOLE  (0 \
	||  BC_PLATFORM_NX             \
	||  BC_PLATFORM_PS4            \
	||  BC_PLATFORM_PS5            \
	||  BC_PLATFORM_WINRT          \
	||  BC_PLATFORM_XBOXONE        \
	)

///
#define BC_PLATFORM_OS_DESKTOP  (0 \
	||  BC_PLATFORM_BSD            \
	||  BC_PLATFORM_HAIKU          \
	||  BC_PLATFORM_HURD           \
	||  BC_PLATFORM_LINUX          \
	||  BC_PLATFORM_OSX            \
	||  BC_PLATFORM_WINDOWS        \
	)

///
#define BC_PLATFORM_OS_EMBEDDED (0 \
	||  BC_PLATFORM_RPI            \
	)

///
#define BC_PLATFORM_OS_MOBILE   (0 \
	||  BC_PLATFORM_ANDROID        \
	||  BC_PLATFORM_IOS            \
	)

///
#define BC_PLATFORM_OS_WEB      (0 \
	||  BC_PLATFORM_EMSCRIPTEN     \
	)

///
#if BC_COMPILER_GCC
#	define BC_COMPILER_NAME "GCC "       \
		BC_STRINGIZE(__GNUC__) "."       \
		BC_STRINGIZE(__GNUC_MINOR__) "." \
		BC_STRINGIZE(__GNUC_PATCHLEVEL__)
#elif BC_COMPILER_CLANG
#	define BC_COMPILER_NAME "Clang "      \
		BC_STRINGIZE(__clang_major__) "." \
		BC_STRINGIZE(__clang_minor__) "." \
		BC_STRINGIZE(__clang_patchlevel__)
#elif BC_COMPILER_MSVC
#	if BC_COMPILER_MSVC >= 1930 // Visual Studio 2022
#		define BC_COMPILER_NAME "MSVC 17.0"
#	elif BC_COMPILER_MSVC >= 1920 // Visual Studio 2019
#		define BC_COMPILER_NAME "MSVC 16.0"
#	elif BC_COMPILER_MSVC >= 1910 // Visual Studio 2017
#		define BC_COMPILER_NAME "MSVC 15.0"
#	elif BC_COMPILER_MSVC >= 1900 // Visual Studio 2015
#		define BC_COMPILER_NAME "MSVC 14.0"
#	elif BC_COMPILER_MSVC >= 1800 // Visual Studio 2013
#		define BC_COMPILER_NAME "MSVC 12.0"
#	elif BC_COMPILER_MSVC >= 1700 // Visual Studio 2012
#		define BC_COMPILER_NAME "MSVC 11.0"
#	elif BC_COMPILER_MSVC >= 1600 // Visual Studio 2010
#		define BC_COMPILER_NAME "MSVC 10.0"
#	elif BC_COMPILER_MSVC >= 1500 // Visual Studio 2008
#		define BC_COMPILER_NAME "MSVC 9.0"
#	else
#		define BC_COMPILER_NAME "MSVC"
#	endif //
#endif // BC_COMPILER_

#if BC_PLATFORM_ANDROID
#	define BC_PLATFORM_NAME "Android " \
				BC_STRINGIZE(BC_PLATFORM_ANDROID)
#elif BC_PLATFORM_BSD
#	define BC_PLATFORM_NAME "BSD"
#elif BC_PLATFORM_EMSCRIPTEN
#	define BC_PLATFORM_NAME "asm.js "          \
		BC_STRINGIZE(__EMSCRIPTEN_major__) "." \
		BC_STRINGIZE(__EMSCRIPTEN_minor__) "." \
		BC_STRINGIZE(__EMSCRIPTEN_tiny__)
#elif BC_PLATFORM_HAIKU
#	define BC_PLATFORM_NAME "Haiku"
#elif BC_PLATFORM_HURD
#	define BC_PLATFORM_NAME "Hurd"
#elif BC_PLATFORM_IOS
#	define BC_PLATFORM_NAME "iOS"
#elif BC_PLATFORM_LINUX
#	define BC_PLATFORM_NAME "Linux"
#elif BC_PLATFORM_NONE
#	define BC_PLATFORM_NAME "None"
#elif BC_PLATFORM_NX
#	define BC_PLATFORM_NAME "NX"
#elif BC_PLATFORM_OSX
#	define BC_PLATFORM_NAME "OSX"
#elif BC_PLATFORM_PS4
#	define BC_PLATFORM_NAME "PlayStation 4"
#elif BC_PLATFORM_PS5
#	define BC_PLATFORM_NAME "PlayStation 5"
#elif BC_PLATFORM_RPI
#	define BC_PLATFORM_NAME "RaspberryPi"
#elif BC_PLATFORM_WINDOWS
#	define BC_PLATFORM_NAME "Windows"
#elif BC_PLATFORM_WINRT
#	define BC_PLATFORM_NAME "WinRT"
#elif BC_PLATFORM_XBOXONE
#	define BC_PLATFORM_NAME "Xbox One"
#else
#	error "Unknown BC_PLATFORM!"
#endif // BC_PLATFORM_

#if BC_CPU_ARM
#	define BC_CPU_NAME "ARM"
#elif BC_CPU_JIT
#	define BC_CPU_NAME "JIT-VM"
#elif BC_CPU_MIPS
#	define BC_CPU_NAME "MIPS"
#elif BC_CPU_PPC
#	define BC_CPU_NAME "PowerPC"
#elif BC_CPU_RISCV
#	define BC_CPU_NAME "RISC-V"
#elif BC_CPU_X86
#	define BC_CPU_NAME "x86"
#endif // BC_CPU_

#if BC_CRT_BIONIC
#	define BC_CRT_NAME "Bionic libc"
#elif BC_CRT_BSD
#	define BC_CRT_NAME "BSD libc"
#elif BC_CRT_GLIBC
#	define BC_CRT_NAME "GNU C Library"
#elif BC_CRT_MSVC
#	define BC_CRT_NAME "MSVC C Runtime"
#elif BC_CRT_MINGW
#	define BC_CRT_NAME "MinGW C Runtime"
#elif BC_CRT_LIBCXX
#	define BC_CRT_NAME "Clang C Library"
#elif BC_CRT_NEWLIB
#	define BC_CRT_NAME "Newlib"
#elif BC_CRT_NONE
#	define BC_CRT_NAME "None"
#else
#	error "Unknown BC_CRT!"
#endif // BC_CRT_

#if BC_ARCH_32BIT
#	define BC_ARCH_NAME "32-bit"
#elif BC_ARCH_64BIT
#	define BC_ARCH_NAME "64-bit"
#endif // BC_ARCH_

#if defined(__cplusplus)
#	if   __cplusplus < BC_LANGUAGE_CPP14
#		error "C++14 standard support is required to build."
#	elif __cplusplus < BC_LANGUAGE_CPP17
#		define BC_CPP_NAME "C++14"
#	elif __cplusplus < BC_LANGUAGE_CPP20
#		define BC_CPP_NAME "C++17"
#	elif __cplusplus < BC_LANGUAGE_CPP23
#		define BC_CPP_NAME "C++20"
#	else
// See: https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b#orthodox-c
#		define BC_CPP_NAME "C++WayTooModern"
#	endif // BC_CPP_NAME
#else
#	define BC_CPP_NAME "C++Unknown"
#endif // defined(__cplusplus)

#if BC_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if BC_PLATFORM_OS_DESKTOP 
#define BC_PLATFORM_ALIGNMENT (8)
#endif

#endif
