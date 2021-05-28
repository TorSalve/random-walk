#pragma once

#ifndef M_PI
    #define M_PI 3.141592653589793238462643383279502
#endif

// Backwards compatibility
namespace Ultraleap
{
namespace Haptics
{}
} // namespace Ultraleap

namespace Ultrahaptics
{
using namespace Ultraleap::Haptics;
}

// This is used to control decoration of public functions.
//
// On Windows when building a DLL and .lib functions must be decorated with `__declspec(dllexport)`
// while building the library, and `__declspec(dllimport)` when another program is using the library.
//
// The `__declspec(dllexport)` causes MSVC to actually export the functions and generate the .lib
// file. An alternative to using `__declspec(dllexport)` is to list exported functions manually in
// a .def file. The `__declspec(dllimport)` is an optimisation that removes one `jmp` for each function
// call.
//
// This is explained reasonably well here: https://msdn.microsoft.com/en-us/library/aa271769(v=vs.60).aspx
//
// We use the decoration macros to make symbols visible. By default our build system sets
// all symbols to be invisible. `__attribute__((visibility("default"))` overrides that
// and allows them to be used by programs that link with this library.

// ULH_API_CLASS is applied to classes and global functions.
// ULH_API is applied to free functions and class methods.

// NO_DECORATION is used when generating documentation.
#ifndef NO_DECORATION
    #ifdef _WIN32
        #ifdef ULH_LIBRARY_EXPORT
            #define ULH_API __declspec(dllexport)
            #if RESEARCH_BUILD
                #define ULH_RESEARCH_API __declspec(dllexport)
            #endif
        #else
            #define ULH_API __declspec(dllimport)
            #define ULH_RESEARCH_API __declspec(dllimport)
        #endif
    #else
        #define ULH_API __attribute__((visibility("default")))
        #define ULH_API_CLASS __attribute__((visibility("default")))

        #if RESEARCH_BUILD
            #define ULH_RESEARCH_API __attribute__((visibility("default")))
            #define ULH_RESEARCH_API_CLASS __attribute__((visibility("default")))
        #endif
    #endif
#endif

// Default to undecorated.
#ifndef ULH_API
    #define ULH_API
#endif
#ifndef ULH_API_CLASS
    #define ULH_API_CLASS
#endif
#ifndef ULH_RESEARCH_API
    #define ULH_RESEARCH_API
#endif
#ifndef ULH_RESEARCH_API_CLASS
    #define ULH_RESEARCH_API_CLASS
#endif
