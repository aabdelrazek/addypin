#include "CHdfFileAdapter.hpp"
#include "CException.hpp"
#include "NDebugUtils.hpp"

using namespace NDebugUtils;

//! @param rPath Full path of the file used as the storage medium
CHdfFileAdapter::CHdfFileAdapter(const std::string& rPath) :
        mFile(rPath) {
}

CHdfFileAdapter::~CHdfFileAdapter() {
}

//! @copydoc IHdfStorage::Initialize()
void CHdfFileAdapter::Initialize() {
    // Intentionally left blank
}

//! @copydoc IHdfStorage::Write()
void CHdfFileAdapter::Write(const std::string& rData) {
    uint32 lengthB = rData.size();

    mFile.Open(IFile::kWriteTruncate);
    mFile.WriteComplete(reinterpret_cast<const uint8*>(rData.c_str()), lengthB);
    mFile.Close();
}

//! @copydoc IHdfStorage::Read()
void CHdfFileAdapter::Read(std::string& rData) {
    uint32 lengthB = mFile.GetSize();

    if (lengthB) {
        uint8 buffer[lengthB+1];
        mFile.Open(IFile::kReadOnly);
        mFile.ReadComplete(buffer, lengthB);
        buffer[lengthB] = '\0';
        mFile.Close();
        rData = reinterpret_cast<char*>(buffer);
    } else {
        rData.clear();
    }
}

//! @copydoc IHdfStorage::IsStoragePresent()
bool CHdfFileAdapter::IsStoragePresent() const {
    return mFile.IsPresent();
}

//! @copydoc IHdfStorage::GetStorageName()
const char* CHdfFileAdapter::GetStorageName() const {
    return mFile.GetFilename();
}

//! @copydoc IHdfStorage::Clean()
void CHdfFileAdapter::Clean() {
    mFile.Delete();
}

//! @copydoc IHdfStorage::ReadRawData()
void CHdfFileAdapter::ReadRawData(std::string& rOutput) {
    std::string rawData;
    Read(rawData);
    ::HexDump(reinterpret_cast<const uint8*>(rawData.c_str()), rOutput, mFile.GetSize(), 0);
}
