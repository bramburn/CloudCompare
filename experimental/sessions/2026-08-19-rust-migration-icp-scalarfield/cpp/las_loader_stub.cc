// Stub: CCCoreLib not configured yet.
// See sandbox/CONFIGURE_CCCORELIB.md
// Replace this with the real las_loader.cc when CCCoreLib is built.

// Include the CXX-generated header so the generated wrapper finds our stubs.
// CXX generates this header before C++ compilation starts.
#include "cc_sandbox/lib.h"

::std::string g_last_error;

::rust::Box<CloudLoadResult> sandbox_load_las(::rust::Str path) {
    (void)path;
    g_last_error = "CCCoreLib not configured — see sandbox/CONFIGURE_CCCORELIB.md";
    return nullptr; // null Box = failure
}

::rust::String sandbox_last_error() {
    return ::rust::String::From(g_last_error);
}
