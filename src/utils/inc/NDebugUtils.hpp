#ifndef NDEBUGUTILS_HPP
#define NDEBUGUTILS_HPP

#include <string>
#include <stdio.h>
#include "types.h"

//!< Contains various helper functions for debugging
namespace NDebugUtils {
    void HexDump(volatile const uint8* pData, std::string& rOutput, uint32 sizeB, uint32 offsetB = 0);
    std::string MiniHexDump(volatile const uint8* pData, uint32 sizeB);
};

#endif /* NDEBUGUTILS_HPP */
