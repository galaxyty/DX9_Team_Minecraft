#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "TableUser.h"
#include "TableWeapon.h"
#include "TableArmor.h"

// ���� ������.
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
	// ���� ������ ���� ������.
	CTableUser m_TableUser;

	// ���� ������.
	vector<CTableWeapon> m_TableWeapon;

	// ��� ������.
	vector<CTableArmor> m_TableArmor;
};

