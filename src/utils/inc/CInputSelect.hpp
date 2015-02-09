#ifndef CINPUTSELECT_HPP
#define CINPUTSELECT_HPP

#include "types.h"
#include <sys/select.h>

//! simple class to wrap input-only select() calls, and hide any unwanted old-style-cast warnings
class CInputSelect {
public:
    CInputSelect();
    void Zero();
    int32 Select(timeval& rTimeout);
    int32 SelectWrite(timeval& rTimeout);
    void Set(uint32 fd);
    void Clear(uint32 fd);
    bool IsSet(uint32 fd);

private:
    fd_set mSet;        //!< Descriptor set
    uint32 mMaxFd;      //!< Highest descriptor in the set

    void UpdateMax(uint32 fd);
};

#endif /* CINPUTSELECT_HPP */
