#ifndef CHDFFILEADAPTER_HPP
#define CHDFFILEADAPTER_HPP

#include "IHdfStorage.hpp"
#include "CPosixFile.hpp"

class CHdfFileAdapter : public IHdfStorage {
public:
    CHdfFileAdapter(const std::string& rPath);
    ~CHdfFileAdapter();

    virtual void Initialize();
    virtual void Clean();
    virtual void Write(const std::string& rData);
    virtual void Read(std::string& rData);
    virtual bool IsStoragePresent() const;
    virtual const char* GetStorageName() const;
    virtual void ReadRawData(std::string& rOutput);

private:
    CPosixFile mFile;           //!< underlying file object
};

#endif /* CHDFFILEADAPTER_HPP */
