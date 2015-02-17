/*
 * AddyDB.h
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#ifndef ADDYDB_H_
#define ADDYDB_H_

#include "AddyPin.h"
#include "AddyUserInfo.h"
#include "AddyMasterInfo.h"
#include "AddyPinAllocator.h"
#include <map>
#include <list>
#include <string>
#include "../utils/inc/CHdfManager.hpp"
#include "../utils/inc/CHdfFileAdapter.hpp"

using namespace std;

//! the addypin database
class AddyDB {
public:
	typedef enum _OperationResult {
		kSuccess = 0,		//!< operation succeeded
		kSuccessNewUse,		//!< adding new address succeeded and the email is a new user
		kInvalidPin,		//!< given pin is not a valid pin for lookup
		kNotFound,			//!< the pin seams valid, but no address associated with it
		kExceededLimit		//!< addition failed because many address entries for the same email
	} EOperationResult;

	AddyDB();
	virtual ~AddyDB();

	EOperationResult Add(string address, string email, string& rAssignedPin);
	EOperationResult FindByPin(string pin, AddyUserInfo*& pRet);
	EOperationResult GetMasterRecord(string masterPin, string email, list< pair<string, string> >& retPairs);
	EOperationResult SetMasterRecord(string masterPin, string email, list< pair<string, string> >& newPairs);
	AddyDB::EOperationResult GetMasterPin(std::string email, std::string& ret);
	void DumpPinMap();
	bool SaveMap();
	bool LoadMap();

private:
	string AllocateUniquePin(bool master);
	CHdfFileAdapter mHdfFileAdapter;						//!< file on disk
	CHdfManager mHdfManager;								//!< HDF formated file manager
	AddyPinAllocator mAddyPinAllocator;						//!< the only allocator for entries in the DB
	map<string, AddyUserInfo*> mPinAddressMap;				//!< map of pins to user info
	map<string, string> mEmailToMPinMap;					//!< map of email to master pin
	map<string, AddyMasterInfo*> mMPinToInfoListMap;		//!< map of master pin to list of user info
	static const string kDbPath;							//!< path to data base file
	static const string kDbFile;							//!< data base file name
};

#endif /* ADDYDB_H_ */
