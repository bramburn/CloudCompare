// C++ side of the CXX bridge for `ICPRegistrationTools::Register`.
//
// CXX generates glue that calls `::run_icp_cpp` in the global
// namespace (the default CXX 1.0 namespace). The header must
// include the CXX-generated cxxbridge header for the shared types
// `IcpParamsCpp` and `IcpResultCpp` (also in the global namespace).
//
// Pattern (see experimental/templates/rust_cxx_app/cpp/bridge.h):
//   #include "<crate>/src/<bridge_file>.h"  // CXX-generated types

#pragma once

#include <cstddef>  // for std::size_t

#include "cc_rust/src/ffi.rs.h"   // CXX auto-generated (mirror of src/ffi.rs bridge module)

IcpResultCpp run_icp_cpp(
    const float* model_points,
    std::size_t model_count,
    const float* data_points,
    std::size_t data_count,
    const IcpParamsCpp& params);
