#include "UserDataManager.h"

IMPLEMENT_SINGLETON(CUserDataManager)

void CUserDataManager::SetUserVectorData(const CTableUser _data)
{
	m_TableUser = _data;
}

void CUserDataManager::SetWeaponVectorData(const vector<CTableWeapon> _data)
{
	m_TableWeapon = _data;
}

void CUserDataManager::SetArmorVectorData(const vector<CTableArmor> _data)
{
	m_TableArmor = _data;
}

void CUserDataManager::SetWeaponGradeData(int _index, int _grade)
{
	// 범위 체크.
	if (m_TableWeapon.size() <= _index)
	{
		return;
	}

	m_TableWeapon[_index].grade = _grade;
}

const CTableUser CUserDataManager::GetUserData() const
{
	return m_TableUser;
}

const vector<CTableWeapon> CUserDataManager::GetWeaponData() const
{
	return m_TableWeapon;
}

const vector<CTableArmor> CUserDataManager::GetArmorData() const
{
	return m_TableArmor;
}
