#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "TableUser.h"
#include "TableWeapon.h"
#include "TableArmor.h"

// 유저 데이터.
class CUserDataManager final : public CBase
{
	DECLARE_SINGLETON(CUserDataManager);
private:
	CUserDataManager() = default;
	virtual ~CUserDataManager() = default;

public:
	void SetUserVectorData(const CTableUser _data);
	void SetWeaponVectorData(const vector<CTableWeapon> _data);
	void SetArmorVectorData(const vector<CTableArmor> _data);

	void SetWeaponGradeData(int _index, int _grade);

	const CTableUser GetUserData() const;
	const vector<CTableWeapon> GetWeaponData() const;
	const vector<CTableArmor> GetArmorData() const;

private:
	// 유저 정보를 담은 데이터.
	CTableUser m_TableUser;

	// 무기 데이터.
	vector<CTableWeapon> m_TableWeapon;

	// 장비 데이터.
	vector<CTableArmor> m_TableArmor;
};

