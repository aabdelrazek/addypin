#include "CException.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <cstring>


/*!
 * Exception class constructor that also records the function, file & line where the exception has occurred.
 * @note ALWAYS use the EXCEPTION macro instead of this constructor.  This hides the dirty details of __FILE__, etc.
 * @note Use the EXCEPTIONERRNO macro to capture the system errno at the time of creating the exception object
 * @param pFunc use __FUNCTION__
 * @param pFile use __FILE__
 * @param line use __LINE__
 * @param includeErrno Set true to include the errno information automatically in the exception string
 * @param pFormat Format string, including any desired conversion specifications (ie. %d, %s).
 * Optional variable arguments follow (...)
 */
CException::CException(const char* pFunc, const char* pFile, int32 line, bool includeErrno, const char* pFormat, ...) {
    // strip out the full path if present, and use the short file name instead
    const char* pNewFile = strrchr(pFile, '/');
    if (pNewFile) {
        pNewFile += 1;
    } else {
        pNewFile = pFile;
    }
    mPreMsgLenB = snprintf(mBuffer, sizeof(mBuffer), "Exception: %s(), %s:%d. ", pFunc, pNewFile, line);

    va_list argList;
    va_start(argList, pFormat);
    vsnprintf(mBuffer + mPreMsgLenB, sizeof(mBuffer) - mPreMsgLenB, pFormat, argList);
    va_end(argList);

    // lastly, tack on the errno & string description to the what() message, if necessary
    if (includeErrno) {
        mErrno = errno;
        snprintf(mBuffer + strlen(mBuffer), sizeof(mBuffer) - strlen(mBuffer), " '%s' (%d)", strerror(mErrno), mErrno);
    } else {
        mErrno = -1;    // invalid errno value, to indicate the exception did not capture the real errno
    }
}

/*!
 * @param rMsg the exception message to be logged at the 'catch' locations
 */
CTimeoutException::CTimeoutException(const std::string& rMsg) : std::runtime_error(rMsg) {}

//! @param rMsg the exception message to be logged at the 'catch' locations
CThreadInterruptException::CThreadInterruptException(const std::string& rMsg) : std::runtime_error(rMsg) {}

