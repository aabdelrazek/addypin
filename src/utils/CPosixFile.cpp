#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef USE_MOCK
    #include <sys/ioctl.h>
#else
    #include <stropts.h>
#endif
#include <sys/mman.h>

#include "CPosixFile.hpp"
#include "CException.hpp"
#include "CInputSelect.hpp"

/*!
 * Posix-file wrapper object. Supply an optional access-mode flag to open the file automatically on construction.
 * @param rPath Full path of the file.
 * @param mode type of file access to associate with file. If write access is specified and the file does not yet exist,
 * the file will then be created.  If this parameter is omitted, this constructor does not open the file automatically,
 * and the caller must make an explicit call to Open()
 */
CPosixFile::CPosixFile(const std::string& rPath, EFileAccessMode mode) :
        mPath(rPath),
        mMode(mode) {
    size_t start = mPath.rfind('/');
    if (start == std::string::npos) {
        mFilename = mPath;
    } else {
        start += 1;
        mFilename = mPath.substr(start, mPath.length() - start);
    }
    mDescriptor = -1;

    // We won't automatically open the file unless an access mode is specified.
    if (mMode != kNoAccess) {
        Open(mMode);
    }
}

//! Closes the file, and unmaps any mappings when destructed
CPosixFile::~CPosixFile() {
    Close();
}

/*!
 * @copydoc IFile::Read(uint8*,uint32,uint32)
 * @throw EXCEPTION if failed to read from the file (variety of reasons - see errno)
 */
uint32 CPosixFile::Read(uint8* pBuffer, uint32 lengthB, uint32 timeoutMs) {
    uint32 bytesRead = 0;
    timeval tv = { timeoutMs / 1000, (timeoutMs % 1000) * 1000 };
    CInputSelect sel;
    sel.Set(mDescriptor);

    try {
        sel.Select(tv);
    } catch (const std::exception& e) {
        throw EXCEPTION("Failed read on %s (%s)", mPath.c_str(), e.what());
    }
    if (sel.IsSet(mDescriptor)) {
        bytesRead = Read(pBuffer, lengthB);
    }

    return bytesRead;
}

/*!
 * @copydoc IFile::Read(uint8*,uint32)
 * @throw EXCEPTIONERRNO if failed to read from the file (variety of reasons - see errno)
 */
uint32 CPosixFile::Read(uint8* pBuffer, uint32 lengthB) {
    ssize_t bytesRead = read(mDescriptor, pBuffer, lengthB);
    if (bytesRead == -1) {
        throw EXCEPTIONERRNO("Failed read on %s", mPath.c_str());
    }

    return static_cast<uint32>(bytesRead);
}

/*!
 * @copydoc IFile::Write()
 * @throw EXCEPTIONERRNO if failed to write to the file (variety of reasons - see errno)
 */
uint32 CPosixFile::Write(const uint8* pBuffer, uint32 lengthB) {
    ssize_t bytesWritten = write(mDescriptor, pBuffer, lengthB);
    if (bytesWritten == -1) {
        throw EXCEPTIONERRNO("Failed write on %s", mPath.c_str());
    }

    return static_cast<uint32>(bytesWritten);
}

/*!
 * @copydoc IFile::Read(uint8*,uint32,uint32)
 * @note This Read routine attempts to ensure the data is fully read, even if not all requested data is available in
 * the first read attempt. (ie. the initial read() call returned less than lengthB).
 * This method loops until lengthB bytes have been read OR the timeout expires, in which case the return value may be
 * less than the requested lengthB.
 * @throw EXCEPTIONERRNO if failed to read from the file (variety of reasons - see errno)
 */
uint32 CPosixFile::ReadComplete(uint8* pBuffer, uint32 lengthB, uint32 timeoutMs) {
    uint32 totalBytesRead = 0;
    timeval tv = { timeoutMs / 1000, (timeoutMs % 1000) * 1000 };
    CInputSelect sel;
    sel.Set(mDescriptor);

    // loop until all requested bytes are read, or the timeout fully expires
    while (totalBytesRead < lengthB) {
        int32 result = 0;
        try {
            result = sel.Select(tv);
        } catch (const std::exception& e) {
            throw EXCEPTION("Failed read on %s (%s)", mPath.c_str(), e.what());
        }
        if (result == 0) {
            break;      // timeout
        }

        if (sel.IsSet(mDescriptor)) {
            uint32 bytesRead = Read(pBuffer, lengthB - totalBytesRead);
            if (bytesRead == 0) {
                break;  // EOF - this is ok. note, this will return < lengthB
            }
            totalBytesRead += bytesRead;
            pBuffer += bytesRead;
        }

        /*
         * If we did not yet read the desired # of bytes, so loop and wait again.  The remaining time left should be
         * in the modified timeout structure.
         * (Note: this behaviour may be limited to Linux systems only - see 'man 2 select').
         */
    }
    return totalBytesRead;
}

/*!
 * @copydoc IFile::Read(uint8*,uint32)
 * @note This Read routine ensures the data is fully read, even if interrupted (ie. the initial read() call
 * returned less than lengthB).  This method loops until lengthB bytes are guaranteed read (blocks if data unavailable)
 * @throw EXCEPTIONERRNO if failed to read from the file (variety of reasons - see errno)
 */
uint32 CPosixFile::ReadComplete(uint8* pBuffer, uint32 lengthB) {
    uint32 totalBytesRead = 0;
    while (totalBytesRead < lengthB) {
        uint32 bytesRead = Read(pBuffer, lengthB - totalBytesRead);
        if (bytesRead == 0) {
            break;  // EOF - this is ok. note, this will return < lengthB
        }
        totalBytesRead += bytesRead;
        pBuffer += bytesRead;
    }
    return totalBytesRead;
}

/*!
 * @copydoc IFile::Write()
 * @note This Write routine ensures the data is fully written, even if interrupted (ie. the initial write() call
 * returns less than lengthB).  It loops until lengthB bytes are guaranteed written.
 * @throw EXCEPTION if underlying Write() call returned an unexpected 0 bytes written for a given chunk
 */
uint32 CPosixFile::WriteComplete(const uint8* pBuffer, uint32 lengthB) {
    uint32 totalBytesWritten = 0;
    while (totalBytesWritten < lengthB) {
        uint32 bytesWritten = Write(pBuffer, lengthB - totalBytesWritten);
        if (bytesWritten == 0) {
            throw EXCEPTION("Failed write on %s (write() return 0. %d of %d bytes written)",
                    mPath.c_str(), totalBytesWritten, lengthB);
        }
        totalBytesWritten += bytesWritten;
        pBuffer += bytesWritten;
    }
    return totalBytesWritten;
}

/*!
 * Forces a write flush (sync) on the file.
 * @throw EXCEPTIONERRNO if failed to sync output to the file (variety of reasons - see errno)
 */
void CPosixFile::Flush() {
    int result = fsync(mDescriptor);
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed sync on %s", mPath.c_str());
    }
}

/*!
 * Returns the cursor to the beginning of the file.  Affects reads/writes.
 * @throw EXCEPTIONERRNO if failed to rewind/seek the file (variety of reasons - see errno)
 */
void CPosixFile::Rewind() {
    off_t result = lseek(mDescriptor, 0, SEEK_SET);
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed seek on %s", mPath.c_str());
    }
}

/*!
 * Affects length of file.  If specified length is greater that size of file, the file will be extended,
 * shortened otherwise.
 * @param lengthB new length of the file in bytes
 * @throw EXCEPTIONERRNO if failed to truncate the file (variety of reasons - see errno)
 */
void CPosixFile::Truncate(uint32 lengthB) {
    int result = ftruncate(mDescriptor, lengthB);
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed truncate on %s", mPath.c_str());
    }
}

/*!
 * Wrapper for the ioctl function call. It takes an int32 as parameter.
 * @param request this argument is passed as is to the ioctl function
 * @param arg this argument is passed as is to the ioctl function
 * @throw EXCEPTIONERRNO if failed to send the IOCTL to the file (variety of reasons - see errno)
 */
void CPosixFile::Ioctl(int32 request, int32 arg) {
    int result = ioctl(mDescriptor, request, arg);
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed ioctl %d on %s", request, mPath.c_str());
    }
}

/*!
 * Wrapper for the ioctl function call. It takes a pointer as parameter.
 * @param request this argument is passed as is to the ioctl function
 * @param[in] pArg this argument is passed as is to the ioctl function
 * @throw EXCEPTIONERRNO if failed to send the IOCTL to the file (variety of reasons - see errno)
 */
void CPosixFile::Ioctl(int32 request, void* pArg) {
    int result = ioctl(mDescriptor, request, pArg);
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed ioctl %d on %s", request, mPath.c_str());
    }
}

//! @copydoc IFile::GetFilename()
const char* CPosixFile::GetFilename() const {
    return mFilename.c_str();
}

/*!
 * @copydoc IFile::GetSize()
 * @throw EXCEPTIONERRNO if unable to stat the file (variety of reasons - see errno)
 */
uint32 CPosixFile::GetSize() const {
    uint32 sizeB = 0;

    if (mMode != kNoAccess) {
        // file already opened and we have cached statistics
        sizeB = mInfo.st_size;
    } else {
        // in the case the file is not yet open, mInfo is not yet filled in.  Do a fresh stat instead.
        struct stat st;
        int result = stat(mPath.c_str(), &st);
        if (result == -1) {
            throw EXCEPTIONERRNO("Failed to retrieve file stats on %s", mPath.c_str());
        }
        sizeB = st.st_size;
    }
    return sizeB;
}

/*!
 * Opens up the file with the requested access mode
 * @param mode type of file access to associate with file
 * @throw EXCEPTION if an invalid file access 'mode' is supplied
 * @throw EXCEPTIONERRNO if unable to open or stat the file (variety of reasons - see errno)
 */
void CPosixFile::Open(EFileAccessMode mode) {
    int access = 0;
    switch (mode) {
        case kReadOnly:
            access = O_RDONLY;
            break;
        case kReadWrite:
            access = O_RDWR | O_CREAT;
            break;
        case kWriteOnly:
            access = O_WRONLY | O_CREAT;
            break;
        case kWriteTruncate:
            access = O_WRONLY | O_CREAT | O_TRUNC;
            break;
        case kWriteAppend:
            access = O_WRONLY | O_CREAT | O_APPEND;
            break;
        case kNoAccess:
        default:
            throw EXCEPTION("Invalid argument for mode");
            break;
    }

    // test to see if we already have an open handle on this file.  If so, close the first handle.
    if (mDescriptor != -1) {
        Close();
    }

    mMode = mode;
    if (access & O_CREAT) {
        mDescriptor = open(mPath.c_str(), access, S_IRWXU);
    } else {
        mDescriptor = open(mPath.c_str(), access);
    }
    if (mDescriptor == -1) {
        throw EXCEPTIONERRNO("Failed open on %s", mPath.c_str());
    }

    int result = fstat(mDescriptor, &mInfo);
    if (result == -1) {
        close(mDescriptor);
        throw EXCEPTIONERRNO("Failed stat on %s", mPath.c_str());
    }
}

//! Closes the file, unmaps any memory mappings
void CPosixFile::Close() {
    std::map<off_t, SMemoryRegion>::const_iterator memIter;
    for (memIter = mRegions.begin(); memIter != mRegions.end(); ++memIter) {
        munmap(memIter->second.address, memIter->second.sizeB);
    }
    mRegions.clear();

    if (mDescriptor != -1) {
        close(mDescriptor);
    }
    mDescriptor = -1;
    mMode = kNoAccess;
}

/*!
 * Changes the access mode of the file
 * @param mode is desired mode.  See "man 2 chmod" for more information about this argument
 * @throw EXCEPTIONERRNO if unable to change the access mode of the file (variety of reasons - see errno)
 */
void CPosixFile::Chmod(mode_t mode) {
    int result = chmod(mPath.c_str(), mode);
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed chmod on %s", mPath.c_str());
    }
}

/*!
 * Moves the file cursor to the desired offset
 * @param offset location within file to place cursor
 * @throw EXCEPTIONERRNO if unable to seek within the file (variety of reasons - see errno)
 */
void CPosixFile::Seek(uint32 offset) {
    int result = lseek(mDescriptor, offset, SEEK_SET);
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed seek on %s", mPath.c_str());
    }
}

/*!
 * @return Returns the current offset in the file
 * @throw EXCEPTIONERRNO if unable to seek within the file (variety of reasons - see errno)
 */
uint32 CPosixFile::GetOffset() const {
    off_t filepos;
    filepos = lseek(mDescriptor, 0, SEEK_CUR);
    if (filepos == -1) {
        throw EXCEPTIONERRNO("Failed seek on %s", mPath.c_str());
    }
    return filepos;
}

//! @return True if this object's file exists, False if it doesn't exist.
bool CPosixFile::IsPresent() const {
    bool isPresent = false;

    if (mMode != kNoAccess) {
        // file already opened, therefore it exists
        isPresent = true;
    } else {
        isPresent = CPosixFile::IsPresent(mPath);
    }
    return isPresent;
}

//! Delete this object's file.  Closes all handles and mappings first.
void CPosixFile::Delete() {
    Close();                            // close the original file if necessary, we're about to delete it
    CPosixFile::Delete(mPath);
}

/*!
 * Make a copy of this object's file to another location
 * @param rDestPath Full path to the new file location
 */
void CPosixFile::Copy(const std::string& rDestPath) {
    CPosixFile::Copy(mPath, rDestPath);
}

/*!
 * Move this object's file to another location.  Closes all handles and mappings first, and re-opens the new file
 * within this same object (does not maintain any previous mappings)
 * @param rDestPath Full path to the new file location
 */
void CPosixFile::Move(const std::string& rDestPath) {
    EFileAccessMode currentMode = mMode;
    Close();                                        // close the original file if necessary, we're about to move it
    CPosixFile::Move(mPath, rDestPath);
    // re-open this object at the new location (this does not maintain old mappings or cursor position)
    mPath = rDestPath;
    Open(currentMode);
}

/*!
 * Static call to get the time when file was modified
 * @param rPath Full path to the file in question.
 * @return time_t Time structure
 */
time_t CPosixFile::GetModifiedTime(const std::string& rPath) {
    struct stat st;
    if (stat(rPath.c_str(), &st) == -1) {
        throw EXCEPTIONERRNO("Failed to get %s modified time", rPath.c_str());
    }
    return st.st_mtime;
}

/*!
 * Static call to determine if a file exists or not.
 * @param rPath Full path to the file in question.
 * @return True if file exists, False if it doesn't exist.
 */
bool CPosixFile::IsPresent(const std::string& rPath) {
    struct stat st;
    int result = stat(rPath.c_str(), &st);
    return (result == 0);
}

/*!
 * Static call to delete a file within the file system.  If the file doesn't exist, no action is taken.
 * @param rPath Full path to the file being removed.
 * @throw EXCEPTIONERRNO if unable to delete the file (variety of reasons - see errno).
 */
void CPosixFile::Delete(const std::string& rPath) {
    int result = remove(rPath.c_str());
    if (result == -1) {
        if (errno != ENOENT) {         // don't throw if the file doesn't exist in the first place.  throw other errors
            throw EXCEPTIONERRNO("Failed to remove %s : %s", rPath.c_str());
        }
    }
}

/*!
 * Static call to copy one file to another location
 * @param rSrcPath Full path to the source file
 * @param rDestPath Full path to the destination file
 * @throw EXCEPTIONERRNO if unable to copy the file (variety of reasons - see errno)
 */
void CPosixFile::Copy(const std::string& rSrcPath, const std::string& rDestPath) {
    char str[1024];
    snprintf(str, sizeof(str), "cp -f -p %s %s", rSrcPath.c_str(), rDestPath.c_str());

    int result = system(str);
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed to copy %s to %s", rSrcPath.c_str(), rDestPath.c_str());
    }
}

/*!
 * Static call to move one file to another location
 * @param rSrcPath Full path to the source file
 * @param rDestPath Full path to the destination file
 * @throw EXCEPTIONERRNO if unable to move the file (variety of reasons - see errno)
 */
void CPosixFile::Move(const std::string& rSrcPath, const std::string& rDestPath) {
    int result = rename(rSrcPath.c_str(), rDestPath.c_str());
    if (result == -1) {
        throw EXCEPTIONERRNO("Failed to move/rename %s to %s", rSrcPath.c_str(), rDestPath.c_str());
    }
}

// Note: this pragma eliminates the old-style-cast warning for the MAP_FAILED macro
// GetMapping() is purposely located at the end of the file, since any code following would also ignore the same warning
// (this pragma is file-scoped)
// This would not be a concern if GCC 4.6+'s '#pragma GCC diagnostic push/pop' methods were available.
#pragma GCC diagnostic ignored "-Wold-style-cast"

/*!
 * @copydoc IFile::GetMapping()
 * @throw EXCEPTION if the file is not yet open
 * @throw EXCEPTION if an invalid map 'access' parameter was supplied
 * @throw EXCEPTIONERRNO if failed to get a mapping to the file (variety of reasons - see errno)
 */
uint8* CPosixFile::GetMapping(uint32 offset, uint32 sizeB, EFileMapAccess access) {
    uint8* pVirtualAddress = reinterpret_cast<uint8*>(MAP_FAILED);

    if (mRegions.find(offset) != mRegions.end()) {
        pVirtualAddress = mRegions[offset].address;
    } else if (mMode != kNoAccess) {
        int prot = 0;
        int flags = 0;
        switch (mMode) {
            case kReadOnly:
                prot = PROT_READ;
                break;
            case kReadWrite:
                prot = PROT_READ | PROT_WRITE;
                break;
            case kWriteOnly:
                prot = PROT_WRITE;
                break;
            default:
                throw EXCEPTION("Invalid access level for map access");
                break;
        }

        switch (access) {
            case kMapShared:
                flags = MAP_SHARED;
                break;
            case kMapPrivate:
                flags = MAP_PRIVATE;
                break;
            case kMapFixed:
                flags = MAP_FIXED;
                break;
            default:
                throw EXCEPTION("Invalid argument for map access");
                break;
        }

        pVirtualAddress = reinterpret_cast<uint8*>(mmap(0, sizeB, prot, flags, mDescriptor, offset));
        if (pVirtualAddress == MAP_FAILED) {
            throw EXCEPTIONERRNO("Failed mmap on %s at (0x%08x) of size %d", mPath.c_str(),
                    static_cast<uint32>(offset), sizeB);
        }

        mRegions[offset].address = pVirtualAddress;
        mRegions[offset].sizeB = sizeB;
    }

    return pVirtualAddress;
}

