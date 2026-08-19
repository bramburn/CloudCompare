// C++ side of the CXX bridge.

#include "cpp/bridge.h"
#include <string>
#include "rust_cxx_app_template/src/lib.rs.h"

::rust::cxxbridge1::String greet_from_cpp(::rust::cxxbridge1::Str name) {
    std::string nameStr(name.data(), name.size());
    return ::rust::cxxbridge1::String("Hello, " + nameStr + ", from C++!");
}
