#include "CHdfManager.hpp"
#include <stdarg.h>
#include <cstring>
#include "IHdfStorage.hpp"
#include "CException.hpp"
#include <tic.h>

//! Basic Constructor: Initializes tree to NULL.
CHdfManager::CHdfManager(IHdfStorage* pHdfFile) :
        mpHdfFile(pHdfFile) {

    Init();
}

/*!
 * Limited copy-constructor.  Does not copy tree contents, instead substitutes an empty tree.
 * Functionally identical to the standard constructor.
 * @param[in] rData Not used.
 * @throw EXCEPTION if rData is already initialized (to prevent memory leak)
 */
CHdfManager::CHdfManager(const CHdfManager& rData) {
    if (rData.IsCreated()) {
        throw EXCEPTION("Failed to copy HDF structure - instance already initialized.");
    }
    Init();
}

/*!
 * Assignment operator.  Does not copy tree contents, instead substitutes an empty tree.
 * @param[in] rData Not used.
 */
CHdfManager& CHdfManager::operator= (const CHdfManager& rData) {
    if (&rData != this) {
        Init();
    }
    return *this;
}

//! Deletes contents of tree, if it was created by this instance.
CHdfManager::~CHdfManager() {
    Destroy();
}

//! Common routine to initialize members.  Used by various constructors and operators.
void CHdfManager::Init() {
    mpHdf       = NULL;
    mCreated    = false;
    mDirtyFile  = false;
}

/*!
 * Creates the tree within the HDF manager.
 * @throw EXCEPTION if tree already created
 * @throw EXCEPTION if unable to initialize
 */
void CHdfManager::Create() {
    if (mpHdf == NULL) {
        NEOERR* pResult = hdf_init(&mpHdf);
        if (pResult != STATUS_OK_INT) {
            ThrowError(pResult, __FUNCTION__);
        } else {
            if (mpHdfFile) {
                mpHdfFile->Initialize();
            }
            mCreated = true;
        }
    } else {
        throw EXCEPTION("Failed to create HDF structure.  Base HDF instance already initialized.");
    }
}

/*!
 * Determine if HDF manager tree has been initialized (by this object)
 * @retval true if the tree has been allocated and is initialized
 * @retval false if the tree is uninitialized, or this object is an observer (ie. used JumpToNode()/JumpToChild())
 */
bool CHdfManager::IsCreated() const {
    return (mCreated == true) && (mpHdf != NULL);
}

//! Deletes contents of tree.
void CHdfManager::Destroy() {
    if ((mCreated == true) && (mpHdf != NULL)) {
        mpHdf = hdf_obj_top(mpHdf);     // make sure we're dealing with the top level node.
        hdf_destroy(&mpHdf);
        Init();                         // clean up flags, set mpHdf to NULL
    }
}

//! Clear out the data manager and start fresh
void CHdfManager::Reset() {
    Destroy();
    Create();

    if (mpHdfFile) {
        mpHdfFile->Clean();
    }
}

/*!
 * Retrieve an integer field from the HDF manager.
 * @param[in] pTag Format string: location of data within this HDF manager.
 * @param ... optional variable arguments, corresponding to 'pTag' format string.
 * @return The retrieved integer field
 * @throw EXCEPTION if unable to find data field (ie. can't find specified node)
 */
int32 CHdfManager::GetFieldValue(const char* pTag, .../*args*/) const {
    static const int32 kDummyValue = -123456789;            // Unofficial indicator of an empty integer-node

    char buf[kHdfTagLengthB];
    int32 value = 0;
    va_list ap;
    va_start(ap, pTag);
    ParseArgs(buf, pTag, ap);
    va_end(ap);
    value = hdf_get_int_value(mpHdf, buf, kDummyValue);

    if (value == kDummyValue) {
        throw EXCEPTION("Cannot find value at node %s", buf);
    }

    return value;
}

/*!
 * Retrieve a string field from the HDF manager.
 * @param[in] pTag Format string: location of data within this HDF manager.
 * @param ... optional variable arguments, corresponding to 'pTag' format string.
 * @return The retrieved text field
 * @throw EXCEPTION if unable to find data field (ie. can't find specified node)
 */
std::string CHdfManager::GetFieldText(const char* pTag, .../*args*/) const {
    static const char kDummyText[] = "~!@#$%^&*";           // Unofficial indicator of an empty text-node

    char buf[kHdfTagLengthB];
    char* retStr = NULL;
    va_list ap;
    va_start(ap, pTag);
    ParseArgs(buf, pTag, ap);
    va_end(ap);
    retStr = hdf_get_value(mpHdf, buf, kDummyText);

    if (strncmp(retStr, kDummyText, strlen(kDummyText)) == 0) {
        throw EXCEPTION("Cannot find text at node %s", buf);
    }

    return retStr;
}

/*!
 * Save an integer field to the HDF manager.
 * @param value The integer field to be saved.
 * @param[in] pTag Format string: location used to save the field within this HDF manager.
 * @param ... optional variable arguments, corresponding to 'pTag' format string.
 * @throw EXCEPTION if tree is uninitialized or is not owned by this object
 * @throw EXCEPTION if unable to set data field (ie. error in tree)
 */
void CHdfManager::SetFieldValue(const int32 value, const char* pTag, .../*args*/) {
    if (mCreated == true) {
        char buf[kHdfTagLengthB];
        va_list ap;
        va_start(ap, pTag);
        ParseArgs(buf, pTag, ap);
        va_end(ap);
        NEOERR* pResult = hdf_set_int_value(mpHdf, buf, value);
        if (pResult != STATUS_OK_INT) {
            ThrowError(pResult, __FUNCTION__);
        } else {
            mDirtyFile = true;
        }
    } else {
        throw EXCEPTION("Failed to access uninitialized/read-only HDF structure");
    }
}

/*!
 * Save a string field to the HDF manager.
 * @param[in] rText The string field to be saved.
 * @param[in] pTag Format string: location used to save the field within this HDF manager.
 * @param ... optional variable arguments, corresponding to 'pTag' format string.
 * @throw EXCEPTION if tree is uninitialized or this object is an observer
 * @throw EXCEPTION if unable to set data field (ie. error in tree)
 */
void CHdfManager::SetFieldText(const std::string& rText, const char* pTag, .../*args*/) {
    if (mCreated == true) {
        char buf[kHdfTagLengthB];
        va_list ap;
        va_start(ap, pTag);
        ParseArgs(buf, pTag, ap);
        va_end(ap);
        NEOERR* pResult = hdf_set_value(mpHdf, buf, rText.c_str());
        if (pResult != STATUS_OK_INT) {
            ThrowError(pResult, __FUNCTION__);
        } else {
            mDirtyFile = true;
        }
    } else {
        throw EXCEPTION("Failed to access uninitialized/read-only HDF structure");
    }
}

/*!
 * Delete a node from this HDF manager
 * @param[in] pTag Format string: location within this HDF manager to delete.
 * @param ... optional variable arguments, corresponding to 'pTag' format string.
 * @throw EXCEPTION if tree is uninitialized or this object is an observer
 * @throw EXCEPTION if unable to delete (ie. can't find specified node)
 */
void CHdfManager::RemoveNode(const char* pTag, .../*args*/) {
    if (mCreated == true) {
        char buf[kHdfTagLengthB];
        va_list ap;
        va_start(ap, pTag);
        ParseArgs(buf, pTag, ap);
        va_end(ap);
        NEOERR* pResult = hdf_remove_tree(mpHdf, buf);
        mDirtyFile = true;
        if (pResult != STATUS_OK_INT) {
            ThrowError(pResult, __FUNCTION__);
        }
    } else {
        throw EXCEPTION("Failed to access uninitialized/read-only HDF structure");
    }
}

/*!
 * Copy a separate HDF manager into this HDF manager
 * @param[in] rMgr HDF manager 'source' object.  The 'destination' object is *this.
 * @param[in] pTag Format string: location within this HDF manager to copy to.
 *        Use "" to copy to the root of this HDF manager.  Use "a" to copy the contents of rMgr to a new node in
 *        the destination called "a".
 * @param ... optional variable arguments, corresponding to 'pTag' format string.
 * @throw EXCEPTION if tree is uninitialized or this object is an observer
 * @throw EXCEPTION if unable to copy
 */
void CHdfManager::CopyNode(const CHdfManager& rMgr, const char* pTag, .../*args*/) {
    if ((mCreated == true) && (rMgr.mpHdf != NULL)) {
        char buf[kHdfTagLengthB];
        va_list ap;
        va_start(ap, pTag);
        ParseArgs(buf, pTag, ap);
        va_end(ap);
        NEOERR* pResult = hdf_copy(mpHdf, buf, rMgr.mpHdf);
        if (pResult != STATUS_OK_INT) {
            ThrowError(pResult, __FUNCTION__);
        } else {
            mDirtyFile = true;
        }
    } else {
        throw EXCEPTION("Failed to access uninitialized/read-only HDF structure");
    }
}

/*!
 * Re-point the HDF manager to the next neighboring node (from its current position within the tree).
 * @retval true if successful in jumping to the next node
 * @retval false if there is no 'next' node to jump to
 */
bool CHdfManager::JumpToNext() {
    bool ret = true;
    HDF* pNext = hdf_obj_next(mpHdf);
    if (!pNext) {
        ret = false;
    } else {
        mpHdf = pNext;
    }
    return ret;
}

/*!
 * Re-point the HDF manager to the (first) child node (from its current position within the tree).
 * @retval true if successful in jumping to the first child node
 * @retval false if there is no child node to jump to
 */
bool CHdfManager::JumpToChild() {
    bool ret = true;
    HDF* pChild = hdf_obj_child(mpHdf);
    if (!pChild) {
        ret = false;
    } else {
        mpHdf = pChild;
    }
    return ret;
}

/*!
 * Re-point the HDF manager to the top node
 * @throw EXCEPTION if tree is uninitialized
 */
void CHdfManager::JumpToTop() {
    HDF* pTop = hdf_obj_top(mpHdf);
    if (!pTop) {
        throw EXCEPTION("Failed to access uninitialized HDF structure");
    } else {
        mpHdf = pTop;
    }
}

/*!
 * Read the name of the node currently pointed to.
 * @return c-string of current node's name.
 */
const char* CHdfManager::GetNodeName() const {
    return hdf_obj_name(mpHdf);
}

/*!
 * Read the data at the node currently pointed to.  Equivalent to GetFieldText("").
 * @return c-string of current node's data string (may be the string representation of an integer field).
 */
const char* CHdfManager::GetNodeValue() const {
    return hdf_obj_value(mpHdf);
}

/*!
 * Join/observe a HDF manager at a particular node of another HDF manager.
 * @param[in] rMgr HDF manager object that you are 'observing'.  This can be used to point a manager to itself at a child
 * node, but be aware that all pTag strings will refer to child node as opposed to the 'top' node.
 * @param[in] pTag Format string: location within the observed HDF manager to position this HDF manager.
 *         Use "" to position at the root of the observed HDF manager.
 * @param ... optional variable arguments, corresponding to 'pTag' format string.
 * @throw EXCEPTION if both rMgr and *this are different, and both already initialized - cannot abandon *this's data
 * @throw EXCEPTION if unable to locate the specified node
 */
void CHdfManager::JoinAtNode(const CHdfManager& rMgr, const char* pTag, .../*args*/) {
    char buf[kHdfTagLengthB];

    if (&rMgr != this && this->IsCreated()) {
        throw EXCEPTION("Unable to join a different HDF structure. Destroy this structure before joining another.");
    }

    va_list ap;
    va_start(ap, pTag);
    ParseArgs(buf, pTag, ap);
    va_end(ap);
    HDF* node = hdf_get_obj(rMgr.mpHdf, buf);
    if (node == NULL) {
        throw EXCEPTION("Failed to join HDF structure.  Node not found at %s.", buf);
    } else {
        mpHdf = node;
    }
}

/*!
 * Join/observe a HDF manager at _the first child of_ a particular node of another HDF manager.
 * @param[in] rMgr HDF manager object that you are 'observing'.  This can be used to point a manager to itself at a child
 * node, but be aware that all pTag strings will refer to child node as opposed to the 'top' node.
 * @param[in] pTag Format string: location within the observed HDF manager to position this HDF manager (at its first child)
 *         Use "" to position at the first child of the root of the observed HDF manager.
 * @param ... optional variable arguments, corresponding to 'pTag' format string.
 * @throw EXCEPTION if both rMgr and *this are different, and both already initialized - cannot abandon *this's data
 * @throw EXCEPTION if unable to locate the specified node
 */
void CHdfManager::JoinAtChild(const CHdfManager& rMgr, const char* pTag, .../*args*/) {
    char buf[kHdfTagLengthB];

    if (&rMgr != this && this->IsCreated()) {
        throw EXCEPTION("Unable to join a different HDF structure. Destroy this structure before joining another.");
    }

    va_list ap;
    va_start(ap, pTag);
    ParseArgs(buf, pTag, ap);
    va_end(ap);
    HDF* pChild = hdf_get_child(rMgr.mpHdf, buf);
    if (pChild == NULL) {
        throw EXCEPTION("Failed to join HDF structure.  Child not found at %s.", buf);
    } else {
        mpHdf = pChild;
    }
}

/*!
 * Read an HDF file into memory.  File is automatically 'cleaned' to strip any ## comments.
 * @throw EXCEPTION if file not found
 * @throw EXCEPTION if read error occurs
 * @throw EXCEPTION if destination tree is uninitialized or this object is an observer
 */
void CHdfManager::ImportData() {
    if (mCreated == true && mpHdfFile) {
        std::string data;
        mpHdfFile->Read(data);

        /*
         * the hdf_read_string() method strips out ## comments.  Actually, it considers a single # to indicate a
         * comment.  Long ago we decided to use double ##, and we used to manually strip them out.
         * Upon discovering hdf_read_string() does this anyway, the unnecessary pre-processing was removed.
         */
        NEOERR* pResult = hdf_read_string(mpHdf, data.c_str());
        if (pResult != STATUS_OK_INT) {
            ThrowError(pResult, __FUNCTION__);
        }

    } else {
        throw EXCEPTION("Failed to read HDF file into uninitialized or read-only structure");
    }
}

/*!
 * Write an HDF formatted string from the tree structure in memory.
 * @note this routine will honor the 'mDirtyFile' flag, and clear it after successful write
 * @param force Optionally set True to force a write to file, ignoring the 'mDirtyFile' flag.
 * @throw EXCEPTION if write error occurs
 */
void CHdfManager::ExportData(bool force) {
    if (mDirtyFile || force) {
        char* pTemp = NULL;
        NEOERR* pResult = hdf_write_string(mpHdf, &pTemp);
        std::string data = pTemp;
        free(pTemp);     // hdf_write_string() will malloc the char* buffer, and we need to free it asap

        if (pResult != STATUS_OK_INT) {
            ThrowError(pResult, __FUNCTION__);
        } else {
            if (mpHdfFile) {
                mpHdfFile->Write(data);
                mDirtyFile = false;
            }
        }
    }
}

//! @return The file name used by the storage medium
std::string CHdfManager::GetFileName() const {
    std::string name = "";
    if (mpHdfFile) {
        name = mpHdfFile->GetStorageName();
    }
    return name;
}

/*!
 * Simple test to determine if a 'file' exists within the storage-system.  Does not validate the stored data.
 * @retval true if file exists in the file system
 * @retval false if no file exists
 */
bool CHdfManager::IsFilePresent() const {
    int32 retVal  = false;
    if (mpHdfFile) {
        retVal = mpHdfFile->IsStoragePresent();
    }
    return retVal;
}

/*!
 * Helper routine to handle variable argument input.
 * @param[out] pBuf processed output will be stored here
 * @param[in] pTag input formatting string
 * @param ap input variable argument list.  This list is managed outside of this routine via va_start/va_end macros.
 * @throw EXCEPTION if unable to process the formatting string and/or its variable argument list
 */
void CHdfManager::ParseArgs(char* pBuf, const char* pTag, va_list ap) const {
    int32 size = 0;

    size = vsnprintf(pBuf, kHdfTagLengthB - 1, pTag, ap);
    pBuf[kHdfTagLengthB-1] = '\0';

    if (size < 0 || size >= static_cast<int32>(kHdfTagLengthB)) {
        throw EXCEPTION("Failed to parse argument list.  vsnprintf returned %d", size);
    }
}

/*!
 * Helper routine to throw an exception due to hdf-tree problems
 * @param[in] pResult Error structure generated by an hdf call.
 * @param[in] pFunc The function name calling this helper (use __FUNCTION__)
 * @throw EXCEPTION The resulting HDF error
 */
void CHdfManager::ThrowError(NEOERR* pResult, const char* pFunc) const {
    if (pResult != STATUS_OK_INT) {
    	NEOSTRING err;
        string_init(&err);
        nerr_error_traceback(pResult, &err);
        throw EXCEPTION("HDF Error in %s: (%s)", pFunc, err.buf);
        //TODO: skip the "Traceback (innermost last):\n  " preamble
    }
}

/*!
 * This function dumps the RAM data from this client and the raw data from the EEPROM to a string
 * @param[out] rOutput String to save the interrogation data to
 */
void CHdfManager::Interrogate(std::string& rOutput) {
    std::string fileData = "";
    std::string fileDataHexStr = "";

    char* temp = NULL;
    NEOERR* pResult = hdf_write_string(mpHdf, &temp);
    rOutput = "Ram Contents:\n------------\n";
    rOutput += temp;
    free(temp);     // hdf_write_string() will malloc the char* buffer, and we need to free it asap

    if (pResult != STATUS_OK_INT) {
        ThrowError(pResult, __FUNCTION__);
    } else {
        if (mpHdfFile) {
            // Read the contents of the storage medium.  This should normally be the same as the RAM data above.
            mpHdfFile->Read(fileData);
            mpHdfFile->ReadRawData(fileDataHexStr);
        }
    }

    // return as 3 parts:           // local ram cached copy, HDF formatted (see above)
    rOutput += "\nFile Contents:\n-------------\n";
    rOutput += fileData;            // file copy, HDF formatted
    rOutput += "\nRaw File Contents:\n-----------------\n";
    rOutput += fileDataHexStr;      // Hex table format of the file data
}
