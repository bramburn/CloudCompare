// bridge.h — user-facing header for the CXX bridge.
//
// CXX generates glue that calls `::greet_from_cpp` in the global namespace
// with types from the `rust::cxxbridge1` namespace. The header must
// include the auto-generated cxxbridge header for those types.
//
// Pattern:
//   #include "<crate>/cxxbridge.h"  // types live here
//   <return type> <func name>(<args>);

#pragma once

// cxx-build generates this header at
//   target/.../cxxbridge/include/<crate>/src/lib.rs.h
// It defines ::rust::cxxbridge1::String, ::rust::cxxbridge1::Str, and the
// generated FFI declarations. The include path below is relative to the
// `out/cxxbridge/include` directory that cxx-build adds automatically.
#include "rust_cxx_app_template/src/lib.rs.h"

::rust::cxxbridge1::String greet_from_cpp(::rust::cxxbridge1::Str name);
