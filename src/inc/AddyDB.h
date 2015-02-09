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
#include "AddyPinAllocator.h"
#include <map>
#include <list>
#include <string>
#include "../utils/inc/CHdfManager.hpp"
#include "../utils/inc/CHdfFileAdapter.hpp"

//! the addypin database
class AddyDB {
public:
	typedef enum _LookupResult{
		kSuccess = 0,
		kInvalidPin,
		kNotFound
	} ELookupResult;

	AddyDB();
	virtual ~AddyDB();

	AddyPin Add(AddyUserInfo& user);
	ELookupResult FindByPin(std::string pin, AddyUserInfo& rRet);
	AddyUserInfo& CreateUserInfo(std::string addr, std::string name);
	void DumpPinMap();
	bool SaveMap();
	bool LoadMap();
private:
	CHdfFileAdapter mHdfFileAdapter;					//!< file on disk
	CHdfManager mHdfManager;							//!< HDF formated file manager
	std::map<std::string, AddyUserInfo&> mUserInfoMap;	//!< map of pins to user info
	std::map<std::string, AddyPin> mPinMap;				//!< map of all available pins so far
	AddyPinAllocator mAddyPinAllocator;					//!< the only allocator for entries in the DB
	static const std::string kDbPath;
	static const std::string kDbFile;
};

#endif /* ADDYDB_H_ */
