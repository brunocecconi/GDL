
#ifndef BC_COMPILER_H
#define BC_COMPILER_H

// Language standard version
#define BC_LANGUAGE_CPP11 201103L
#define BC_LANGUAGE_CPP14 201402L
#define BC_LANGUAGE_CPP17 201703L
#define BC_LANGUAGE_CPP20 202002L
#define BC_LANGUAGE_CPP23 202207L

#if _MSC_VER
#define BC_INLINE __forceinline
#else
#define BC_INLINE __attribute__((always_inline))
#endif

#define BC_CONSTEXPR constexpr
#define BC_EXPLICIT	 explicit

#endif
