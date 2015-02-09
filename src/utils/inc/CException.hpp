#ifndef CEXCEPTION_HPP
#define CEXCEPTION_HPP

#include "types.h"
#include <stdexcept>
#include <errno.h>

/*!
 * Macro to simplify calling an exception with the function name, file:line info
 * @see http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */
#define EXCEPTION(a, ...) CException(__FUNCTION__, __FILE__, __LINE__, false, a, ##__VA_ARGS__)
#define EXCEPTIONERRNO(a, ...) CException(__FUNCTION__, __FILE__, __LINE__, true, a, ##__VA_ARGS__)

//! Derived from std::exception, this class allows the use of variable arguments when constructing the what() message.
class CException : public std::exception {
public:
    CException(const char* pFunc, const char* pFile, int32 line, bool includeErrno, const char* pFormat, ...);
    virtual ~CException() throw() {}

    //! @return overridden call returns the processed VA_arg buffer
    virtual const char* what() const throw() { return mBuffer; }

    //! @return the small version of the processed VA_arg buffer - ie. without the message pre-amble
    const char* whatMini() const throw() { return mBuffer + mPreMsgLenB; }

    //! @return the errno value from when this exception was thrown, if applicable.  If not captured, will return -1
    int32 GetErrno() const { return mErrno; }

private:
    static const uint32 kMaxExceptionMsgLengthB = 512;      //!< max length of (processed) exception string
    char mBuffer[kMaxExceptionMsgLengthB];                  //!< buffer to store the processed exception string
    int32 mPreMsgLenB;                                      //!< Length of the message pre-amble
    int32 mErrno;                                           //!< the current errno value is optionally stored here
};

//! A timeout exception
class CTimeoutException : public std::runtime_error {
public:
    CTimeoutException(const std::string& rMsg);
};

//! A thread interrupt exception
class CThreadInterruptException : public std::runtime_error {
public:
    CThreadInterruptException(const std::string& rMsg);
};

#endif // CEXCEPTION_HPP
