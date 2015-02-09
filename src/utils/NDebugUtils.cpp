#include "NDebugUtils.hpp"

namespace NDebugUtils {

//! A look up table to convert a hexadecimal digit to a char
static const char HexToCharTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

/*!
 * Dumps a buffer / register map to a multi-line hex table (similar to hexdump Linux util)
 * @param[in] pData Collection of data to dump (can be a pointer to registers or a buffer of some sort)
 * @param[in,out] rOutput Formatted data will be written here.
 * @param sizeB Number of bytes to dump
 * @param offsetB Optional parameter - if specified, adds a byte offset to the printed hex address
 * @note the output data is concatenated to the rOutput string. ie. is not cleared before writing the hex data
 */
void HexDump(volatile const uint8* pData, std::string& rOutput, uint32 sizeB, uint32 offsetB) {
    static const uint32 kNumRegistersPerLine = 16;
    uint32 numRegPerLine = 0;
    char temp[20] = "";
    rOutput.reserve(rOutput.size() + sizeB * 5);   // slight performance optimization: pre-size output string by an factor of 5 (approx)

    uint32 numLines = sizeB / kNumRegistersPerLine;
    for (uint32 lines = 0, address = offsetB; lines <= numLines; ++lines, address += kNumRegistersPerLine) {
        if (lines == numLines) {
            numRegPerLine = sizeB % kNumRegistersPerLine;
        } else {
            numRegPerLine = kNumRegistersPerLine;
        }
        if (numRegPerLine) {
            snprintf(temp, sizeof(temp), "%08x  ", address);
            rOutput += temp;
            uint32 i = 0;
            for (; i < numRegPerLine; ++i) {
                uint8 data = *pData++;
                rOutput += HexToCharTable[(data >> 4) & 0xf];
                rOutput += HexToCharTable[data & 0xf];
                rOutput += " ";
                if (i == 7) {
                    rOutput += " ";     // an extra space every 8 bytes
                }
            }
            // if not a full line, fill with spaces
            for (; i < kNumRegistersPerLine; ++i) {
                rOutput += "   ";
                if (i == 7) {
                    rOutput += " ";     // an extra space every 8 bytes
                }
            }

            pData -= numRegPerLine;     // rewind the pointer to print the same data in ascii format
            rOutput += " |";
            for (i = 0; i < numRegPerLine; ++i) {
                uint8 c = *pData++;
                rOutput += ((c < 0x20 || c >= 0x7f) ? '.' : c);      // show a dot '.' for any non-readable character
            }
            rOutput += "|\n";
        }
    }

    snprintf(temp, sizeof(temp), "%08x\n", offsetB + sizeB);
    rOutput += temp;
}

/*!
 * Write contents of a buffer as a hex-formatted string, in the form of: "de ad be ef 00 11"
 * @param[in] pData input byte data
 * @param sizeB number of hex bytes to print
 * @return trimmed, hex-formatted string.  Returns an empty string if sizeB = 0.
 */
std::string MiniHexDump(volatile const uint8* pData, uint32 sizeB) {
    char temp[8];
    std::string str;
    for (uint32 i = 0; i < sizeB; ++i) {
        snprintf(temp, sizeof(temp), "%02x ", *pData++);
        str += temp;
    }
    if (str.length()) {
        str.erase(str.end() - 1, str.end());   // trim the extra space
    }
    return str;
}

} // NDebugUtils
