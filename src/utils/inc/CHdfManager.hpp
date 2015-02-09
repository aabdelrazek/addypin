#ifndef CHDFMANAGER_HPP
#define CHDFMANAGER_HPP

#include "types.h"
#include <neo_hdf.h>
#include <string>

class IHdfStorage;

/*!
 * Interface to a hierarchical data storage tree.
 * There are 3 possible states for a CHdfManager object:
 * 1.  Uninitialized (Immediately after construction, or after explicit call to Destroy())
 * 2.  Created (The tree has been allocated, and this object owns the data.  Nodes can be modified, removed, etc.)
 * 3.  Observer ('Looking' at another object's data.  Cannot edit nodes).
 */
class CHdfManager {
public:
    CHdfManager(const CHdfManager& rData);
    CHdfManager(IHdfStorage* pHdfFile);
    ~CHdfManager();

    void Create();
    bool IsCreated() const;
    void Destroy();
    void Reset();

    int32 GetFieldValue(const char* pTag, .../*args*/) const;
    std::string GetFieldText(const char* pTag, .../*args*/) const;
    void SetFieldValue(const int32 value, const char* pTag, .../*args*/);
    void SetFieldText(const std::string& rText, const char* pTag, .../*args*/);

    void RemoveNode(const char* pTag, .../*args*/);
    void CopyNode(const CHdfManager& rMgr, const char* pTag, .../*args*/);
    bool JumpToNext();
    bool JumpToChild();
    void JumpToTop();
    const char* GetNodeName() const;
    const char* GetNodeValue() const;

    void JoinAtNode(const CHdfManager& rMgr, const char* pTag, .../*args*/);
    void JoinAtChild(const CHdfManager& rMgr, const char* pTag, .../*args*/);

    void ImportData();
    void ExportData(bool force = false);
    std::string GetFileName() const;
    bool IsFilePresent() const;

    void Interrogate(std::string& rOutput);

private:
    HDF* mpHdf;                 //!< HDF tree object.  Storage for all data
    IHdfStorage* mpHdfFile;     //!< Pointer to storage medium (ie. file system, EEPROM, NULL, etc.)
    bool mCreated;              //!< Flag to indicate this object has created and owns the HDF tree
    bool mDirtyFile;            //!< Used to reduce unnecessary file-writes. Set when new changes exist within the tree.

    static const uint32 kHdfTagLengthB = 256;               //!< Max size of all HDF tag length in bytes

    // Private operator= does not allow anyone to actually use this notation (compile-time error will occur).
    // Note: cannot use this trick with the copy-constructor - it's needed for stl containers (ie. maps, vectors, etc.)
    CHdfManager& operator= (const CHdfManager& rData);
    //CHdfManager(const CHdfManager& rData);

    void Init();
    void ParseArgs(char* pBuf, const char* pTag, va_list ap) const;
    void ThrowError(NEOERR* pResult, const char* pFunc) const;
};

#endif /* CHDFMANAGER_HPP */
