#include "CInputSelect.hpp"
#include "CException.hpp"
#include <algorithm>

//! Construct the input select() wrapper class
CInputSelect::CInputSelect() {
    Zero();
}

//! FD_ZERO wrapper: clears all descriptor bits in the set
void CInputSelect::Zero() {
    FD_ZERO(&mSet);
    mMaxFd = 0;
}

/*!
 * select() wrapper to handle input I/O
 * @param[in,out] rTimeout Timeout
 * @return The total number of bits that are set in the input descriptor set, or 0 if the timeout occurs.
 * @throw EXCEPTIONERRNO if select error occurs
 */
int32 CInputSelect::Select(timeval& rTimeout) {
    int32 n = select(mMaxFd + 1, &mSet, NULL, NULL, &rTimeout);
    if (n == -1) {
        throw EXCEPTIONERRNO("Select error");
    }
    return n;
}

/*!
 * select() wrapper to handle input I/O
 * @param[in,out] rTimeout Timeout
 * @return The total number of bits that are set in the input descriptor set, or 0 if the timeout occurs.
 * @throw EXCEPTIONERRNO if select error occurs
 */
int32 CInputSelect::SelectWrite(timeval& rTimeout) {
    int32 n = select(mMaxFd + 1, NULL, &mSet, NULL, &rTimeout);
    if (n == -1) {
        throw EXCEPTIONERRNO("Select error");
    }
    return n;
}

/*!
 * Helper routine to keep track of the maximum file descriptor number (needed for select() call)
 * @param fd File descriptor being added
 */
void CInputSelect::UpdateMax(uint32 fd) {
    mMaxFd = std::max(mMaxFd, fd);
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
/*!
 * FD_SET wrapper: add a descriptor to the set
 * @param fd File descriptor to add to the set
 */
void CInputSelect::Set(uint32 fd) {
    FD_SET(fd, &mSet);
    UpdateMax(fd);
}

/*!
 * FD_CLR wrapper: remove a descriptor from the set
 * @param fd File descriptor to remove from the set
 */
void CInputSelect::Clear(uint32 fd) {
    FD_CLR(fd, &mSet);
    // Note: without keeping a list of all descriptors, we can't effectively remove this fd from the mMax, so we just
    // leave it alone. It's OK for mMaxFd to be too big here - albeit *slightly* less efficient for the select() call.
}

/*!
 * FD_ISSET wrapper
 * @param fd File descriptor to test
 * @retval True if the descriptor was set, indicating data is available
 * @retval False if the descriptor was not set, indicating NO data is available
 */
bool CInputSelect::IsSet(uint32 fd) {
    return FD_ISSET(fd, &mSet);
}

