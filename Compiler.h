/**
 * MIT License
 *
 * Copyright(c) 2023 Bruno Cecconi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

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
