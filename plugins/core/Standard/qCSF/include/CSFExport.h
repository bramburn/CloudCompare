/**
 * @file CSFExport.h
 *
 * @brief CSF library export macros.
 *
 * @details DLL export/import macros for CSF library.
 */

#pragma once

// CSF_LIB_EXPORTS is set when compiling CSF_lib (the static algorithm lib).
// Consumers (test binaries) don't set it → symbols import automatically.
#ifdef CSF_LIB_EXPORTS
#  define CSF_LIB_API __declspec(dllexport)
#else
#  define CSF_LIB_API
#endif
