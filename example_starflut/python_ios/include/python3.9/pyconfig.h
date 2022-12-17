/* pyconfig.h.  Generated from pyconfig.h.in by configure.  */
/* pyconfig.h.in.  Generated from configure.ac by autoheader.  */


#ifndef Py_PYCONFIG_ARCH_H
#define Py_PYCONFIG_ARCH_H

#if defined(__LP64__) && __LP64__

#if defined(__arm__) || defined(__arm64__)
#include "pyconfig-arm64.h"
#endif

#if defined(__x86_64__)
#include "pyconfig-x86_64.h"
#endif

#else

#if defined(__arm__)
#include "pyconfig-armv7.h"
#endif

#if defined(__i386__)
#include "pyconfig-i386.h"
#endif

#endif

#endif /*Py_PYCONFIG_H*/

/*
 Mac OS X (64-bit)
 
 __X86_64__
 __SSE__
 __SSE2__
 __LP64__
 OSX
 MACOS
 
 iOS (32-bit)
 
 __ARM__
 IOS
 
 iOS (64-bit)
 
 __ARM__
 __ARM64__
 __LP64__
 IOS
 
 iOS Simulator (32-bit)
 
 __I386__
 __SSE__
 __SSE2__
 IOS
 IOSSIMULATOR
 SIMULATOR
 
 iOS Simulator (64-bit)
 
 __X86_64__
 __SSE__
 __SSE2__
 __LP64__
 IOS
 IOSSIMULATOR
 SIMULATOR
 
 watchOS (32-bit)
 
 __ARM__
 WATCHOS
 
 watchOS Simulator (32-bit)
 
 __I386__
 __SSE__
 __SSE2__
 WATCHOS
 WATCHOSSIMULATOR
 SIMULATOR
 
 tvOS (64-bit)
 
 __ARM__
 __ARM64__
 __LP64__
 TVOS
 
 tvOS Simulator (64-bit)
 
 __X86_64__
 __SSE__
 __SSE2__
 __LP64__
 TVOS
 TVOSSIMULATOR
 SIMULATOR

*/