#ifndef IHDFSTORAGE_HPP
#define IHDFSTORAGE_HPP

#include <string>

class IHdfStorage {
public:
    virtual ~IHdfStorage() {};

    //! Performs any necessary initialization of the storage medium
    virtual void Initialize() = 0;

    //! Erases the contents at the storage medium
    virtual void Clean() = 0;

    /*!
     * Writes string data to the storage medium.
     * @param[in] rData buffer to read from
     */
    virtual void Write(const std::string& rData) = 0;

    /*!
     * Reads data from the storage medium and stores in a buffer
     * @param[out] rData buffer to store data in
     */
    virtual void Read(std::string& rData) = 0;

    //! @return true if the 'file' exists already, false if not
    virtual bool IsStoragePresent() const = 0;

    //! @return The filename or equivalent name
    virtual const char* GetStorageName() const = 0;

    /*!
     * Retrieves raw, unformatted data from the storage medium
     * @param[out] rOutput Raw output of data on the storage medium (ie. as a hex table)
     */
    virtual void ReadRawData(std::string& rOutput) = 0;
};

#endif /* IHDFSTORAGE_HPP */
