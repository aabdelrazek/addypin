#ifndef CPOSIXFILE_HPP
#define CPOSIXFILE_HPP

#include <string>
#include <map>
#include <sys/stat.h>

#include "IFile.hpp"
#include "types.h"

//! POSIX file wrapper class.  All methods within can deal with POSIX compliant file-system.
class CPosixFile: public IFile {
public:
    CPosixFile(const std::string& rPath, EFileAccessMode mode = kNoAccess);
    ~CPosixFile();

    void Open(EFileAccessMode mode);
    void Close();

    uint32 Write(const uint8* pBuffer, uint32 lengthB);
    uint32 Read(uint8* pBuffer, uint32 lengthB, uint32 timeoutMs);
    uint32 Read(uint8* pBuffer, uint32 lengthB);
    uint32 WriteComplete(const uint8* pBuffer, uint32 lengthB);
    uint32 ReadComplete(uint8* pBuffer, uint32 lengthB, uint32 timeoutMs);
    uint32 ReadComplete(uint8* pBuffer, uint32 lengthB);
    void Flush();
    void Rewind();
    void Truncate(uint32 lengthB);
    void Ioctl(int32 request, int32 arg);
    void Ioctl(int32 request, void* pArg);
    uint8* GetMapping(uint32 offset, uint32 sizeB, EFileMapAccess access);
    void Chmod(mode_t mode);
    void Seek(uint32 offset);
    uint32 GetOffset() const;

    const char* GetFilename() const;
    //! @return the full path of the file
    const std::string& GetPath() const { return mPath; }
    uint32 GetSize() const;
    bool IsPresent() const;
    void Delete();
    void Copy(const std::string& rDestPath);
    void Move(const std::string& rDestPath);

    static bool IsPresent(const std::string& rPath);
    static void Delete(const std::string& rPath);
    static void Copy(const std::string& rSrcPath, const std::string& rDestPath);
    static void Move(const std::string& rSrcPath, const std::string& rDestPath);
    static time_t GetModifiedTime(const std::string& rPath);

protected:
    //! Private struct to manage memory mappings
    struct SMemoryRegion {
        uint8* address;         //!< Base location for the memory mapped region
        uint32 sizeB;           //!< Size of mapped memory region in Bytes
    };

    std::string mPath;          //!< Path to file
    std::string mFilename;      //!< Filename
    int mDescriptor;            //!< file descriptor used with all glibc file related functions
    EFileAccessMode mMode;      //!< access mode to associate with the file when it's opened
    struct stat mInfo;          //!< information about the file when the object is first constructed

    std::map<off_t, SMemoryRegion> mRegions;    //!< list of memory mapped regions
};

#endif // CPOSIXFILE_HPP
