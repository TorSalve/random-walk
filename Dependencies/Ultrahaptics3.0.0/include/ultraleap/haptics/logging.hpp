
#pragma once

#include <cstdint>
#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/errors.hpp"

namespace Ultraleap
{
namespace Haptics
{
/** \cond EXCLUDE_FROM_DOCS */
class LoggingImplementation;
/** \endcond */

/** \brief Class to control logging.
    *
    * An instance of this class can be retrieved using Library::getLogging().
    * It can be used to control the destination of the log and to set the log level.
    */
class ULH_API_CLASS Logging
{
public:
    /** \cond EXCLUDE_FROM_DOCS */
    explicit Logging(LoggingImplementation*);

    /** Destroy the logging access object. */
    ULH_API ~Logging();

    ULH_API Logging(Logging&& other) noexcept;
    ULH_API Logging& operator=(Logging&& other) noexcept;
    ULH_API Logging(const Logging&);
    ULH_API Logging& operator=(const Logging&);
    /** \endcond */

    /** Redirect all logging to the specified path. This can either be "stdout",
        * "stderr", "null" or a filename.
        *
        * @return True if the redirect action succeeded, false if it did not */
    ULH_API bool redirectLogging(const char* value);
    /** Redirect all logging to the standard error stream.
        * This replaces any previous logging destination.
        *
        * @return True if the redirect action succeeded, false if it did not */
    ULH_API bool redirectLoggingToStdErr();

    /** Redirect all logging to the standard output stream.
        * This replaces any previous logging destination.
        *
        * @return True if the redirect action succeeded, false if it did not */
    ULH_API bool redirectLoggingToStdOut();

    /** Redirect all logging to a file.
        * This replaces any previous logging destination.
        *
        * @param filename The path to the log file, which may be non-existent
        *
        * @return True if the redirect action succeeded, false if it did not */
    ULH_API bool redirectLoggingToFile(const char* filename);

    /** Disable all logging output entirely.
        *
        * @return True if the disable action succeeded, false if it did not */
    ULH_API bool redirectLoggingToNull();

    /** Set the log level used by the Ultraleap Haptics SDK
        *
        * Level reference:
        *   - 0: Fatal error
        *   - 1: Error
        *   - 2: Warning
        *   - 3: Verbose
        *   - 4: Extremely verbose
        *
        * @param new_global_log_level The new log level.
        *
        * @return True if the action succeeded, false if it did not */
    ULH_API bool setLogLevel(int new_global_log_level);

    /** Get the current log level used by the Ultraleap Haptics SDK
        *
        * Level reference:
        *   - 0: Fatal error
        *   - 1: Error
        *   - 2: Warning
        *   - 3: Verbose
        *   - 4: Extremely verbose
        *
        * @return The current log level */
    ULH_API int getLogLevel() const;

    /** \cond EXCLUDE_FROM_DOCS */
    ULH_RESEARCH_API void flush();

private:
    /** \cond EXCLUDE_FROM_DOCS */
    LoggingImplementation* impl;
    /** \endcond */
};
} // namespace Haptics

} // namespace Ultraleap
