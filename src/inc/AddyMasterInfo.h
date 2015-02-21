/*
 * AddyMasterInfo.h
 *
 *  Created on: 2015-02-14
 *      Author: aabdelrazek
 */

#ifndef ADDYMASTERINFO_H_
#define ADDYMASTERINFO_H_
#include <string>
#include <vector>
#include <AddyUserInfo.h>

/*! the master info contains the following:
 * master pin (unique per email)
 * user email
 * list of userinfos (currently pairs of addypins and addresses)
 */
class TiXmlElement;

class AddyMasterInfo {
public:
	AddyMasterInfo(std::string email = "", std::string mpin = "", unsigned int maxEntries = 3);
	virtual ~AddyMasterInfo();

	const std::string& GetMasterPin();
	const std::string& GetEmail();
	const unsigned int GetNumEntries();
	AddyUserInfo* GetEntry(unsigned int id);

	void Deserialize(TiXmlElement* pNode);
	void Serialize(TiXmlElement* pNode);

	bool AddNewEntry(AddyUserInfo* pEntry);
	void DeleteAllEntries();
private:
	std::string mMasterPin;
	std::string mEmail;
	std::vector<AddyUserInfo*> mUserInfoEntries;
	unsigned int mMaxEntries;
};

#endif /* ADDYMASTERINFO_H_ */
