#ifndef IFILE_HPP
#define IFILE_HPP

#include "types.h"

//! Generic File Interface
class IFile {
public:

    //! List of available access modes for a file
    enum EFileAccessMode {
        kReadOnly,      //!< Read only access
        kReadWrite,     //!< Read/Write access
        kWriteOnly,     //!< Write only access
        kWriteTruncate, //!< Write only access, truncates file before writing (similar to fopen())
        kWriteAppend,   //!< Write only access, append (@see man 2 open, O_APPEND flag for limitations)
        kNoAccess,      //!< No access (ie. don't open the file)
    };

    //! List of available access modes for a mapping of a file
    enum EFileMapAccess {
        kMapShared,     //!< shared mode access to the map (changes affect everything that shares the mapping)
        kMapPrivate,    //!< private mode access to the map (private copy)
        kMapFixed,      //!< fixed mode access to the map
    };


    virtual ~IFile() {}

    /*!
     * Writes data in pBuffer to the file, each successive call increments the file cursor
     * @param[in] pBuffer buffer containing the data
     * @param lengthB length of buffer in bytes
     * @return number of bytes written, may be less than lengthB
     */
    virtual uint32 Write(const uint8* pBuffer, uint32 lengthB) = 0;

    /*!
     * Reads data from the file to pBuffer, each successive call increments the file cursor.  Supports a timeout.
     * @param[out] pBuffer buffer to store data in
     * @param lengthB length of buffer in bytes
     * @param timeoutMs length of time to wait for incoming data in milliseconds
     * @return number of bytes read, may be less than lengthB
     */
    virtual uint32 Read(uint8* pBuffer, uint32 lengthB, uint32 timeoutMs) = 0;

    /*!
     * Reads data from the file to pBuffer, each successive call increments the file cursor
     * @param[out] pBuffer buffer to store data in
     * @param lengthB length of buffer in bytes
     * @return number of bytes read, may be less than lengthB
     */
    virtual uint32 Read(uint8* pBuffer, uint32 lengthB) = 0;

    /*!
     * Maps all or a portion of a file to a region of memory
     * @param offset offset in file to begin mapping
     * @param sizeB size in bytes of mapping
     * @param access type of access to assign to mapping
     * @return virtual address of region of memory that is mapped to file
     */
    virtual uint8* GetMapping(uint32 offset, uint32 sizeB, EFileMapAccess access) = 0;

    //! @return Returns the filename
    virtual const char* GetFilename() const = 0;

    //! @return Returns the size of the file
    virtual uint32 GetSize() const = 0;
};

#endif // IFILE_HPP
