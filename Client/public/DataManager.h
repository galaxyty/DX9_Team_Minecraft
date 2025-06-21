#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include <sqlite3.h>
#include <SQLiteCpp.h>
#include "TableUser.h"
#include "TableWeapon.h"
#include "TableArmor.h"
#include "UserDataManager.h"

using namespace SQLite;

BEGIN(Client)

// SQLite3 cpp Database.
// ���̺� ������ �ڵ尡 �ƴ� cmd ��ɾ�� �� ���̶�
// ���̺� ���� �ڵ�� ����.
class CDataManager final : public CBase
{
	DECLARE_SINGLETON(CDataManager);

private:
	CDataManager() = default;
	virtual ~CDataManager() = default;

public:
	// �����ͺ��̽� ����.
	void Initialization();

	// �ش� ���̺� �� ���� (primary�� �����ϰ� �ֱ�)
	void Insert(string _table, string _value);

	// User ���̺� ������ ��ȯ.
	CTableUser SelectUser(string _column, string _where = "");

	// Weapon ���̺� ������ ��ȯ.
	vector<CTableWeapon> SelectWeapon(string _column, string _where = "");

	// Armor ���̺� ������ ��ȯ.
	vector<CTableArmor> SelectArmor(string _column, string _where = "");

	// ���� ������ ���� (Ʈ��������� �Ѳ����� ��� ����).
	void Save(CUserDataManager* _userData);

	// ���� ������ ������ �ڵ� ����.
	void AutoSave(CUserDataManager* _userData);

	virtual void Free() override;

private:
	// �����ͺ��̽� �̸�.
	string m_DBName = { "TOA.db" };

	// �����ͺ��̽� ��ü.
	// �⺻ �����ڰ� ��� �׳� ������ ������ �Լ� ������.
	// �׷��� �׳� �̷��� �־���.
	Database m_DB = { m_DBName, OPEN_READWRITE };

	// �ڵ� ���� ������.
	HANDLE						m_hThread = {};

	// ������ ��.
	CRITICAL_SECTION			m_CriticalSection = {};
};

// ���� �����忡 ���� �Ŵ��� Ŭ����.
class ThreadSendManager
{
public:
	CDataManager* dataManager;
	CUserDataManager* userManager;
};

END
