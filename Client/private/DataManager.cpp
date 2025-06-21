#include "DataManager.h"

IMPLEMENT_SINGLETON(CDataManager)

void CDataManager::Initialization()
{
	// ������ �� �ʱ�ȭ.
	InitializeCriticalSection(&m_CriticalSection);

	try
	{
		// db ���� �õ�.
		m_DB = { m_DBName, OPEN_READWRITE };
	}
	catch (const Exception e)
	{
		// ���� ó�� ���.
		throw MSG_BOX("�ش� �����ͺ��̽��� �������� �ʽ��ϴ�");		
	}
}

void CDataManager::Insert(string _table, string _value)
{
	try
	{
		Statement query(m_DB, "INSERT INTO " + _table + " VALUES(NULL, " + _value + ")");
		query.exec();
	}
	catch (const Exception e)
	{
		// ���� ó�� ���.
		throw MSG_BOX("���̺� �� ���Կ� �����Ͽ����ϴ�\n�ش� ���̺� �÷��� �´� ������� Ȯ�����ּ���");
	}
}

CTableUser CDataManager::SelectUser(string _column, string _where)
{
	// ��ȯ ������.
	CTableUser Data;

	try
	{
		Statement query(m_DB, "SELECT " + _column + " FROM User " + _where);

		while (query.executeStep())
		{
			// �ѹ��� �� ������� �������� �����.
			int num = query.getColumn(0);
			string id = query.getColumn(1);
			string name = query.getColumn(2);
			string password = query.getColumn(3);

			// ���⼭ ����.
			Data.num = num;
			Data.id = id;
			Data.name = name;
			Data.password = password;
		}
	}
	catch (const Exception e)
	{
		// ���� ó�� ���.
		throw MSG_BOX("���̺� ���ÿ� �����Ͽ����ϴ�\n�ش� ���̺��� �����ϴ��� Ȯ�����ּ���");
	}

	return Data;
}

vector<CTableWeapon> CDataManager::SelectWeapon(string _column, string _where)
{
	// ���� ��ȯ �����͵�.
	vector<CTableWeapon> returnDatas;

	// ���� ������.
	CTableWeapon Data;

	try
	{
		Statement query(m_DB, "SELECT " + _column + " FROM Weapon " + _where);

		while (query.executeStep())
		{
			// �ѹ��� �� ������� �������� �����.
			int num = query.getColumn(0);
			int user_num = query.getColumn(1);
			string user_name = query.getColumn(2);
			int weapon_num = query.getColumn(3);
			int grade = query.getColumn(4);

			// ���⼭ ����.
			Data.num = num;
			Data.user_num = user_num;
			Data.user_name = user_name;
			Data.weapon_num = weapon_num;
			Data.grade = grade;

			// �߰�.
			returnDatas.push_back(Data);
		}
	}
	catch (const Exception e)
	{
		// ���� ó�� ���.
		throw MSG_BOX("���̺� ���ÿ� �����Ͽ����ϴ�\n�ش� ���̺��� �����ϴ��� Ȯ�����ּ���");
	}

	return returnDatas;
}

vector<CTableArmor> CDataManager::SelectArmor(string _column, string _where)
{
	// ���� ��ȯ �����͵�.
	vector<CTableArmor> returnDatas;

	// ���� ������.
	CTableArmor Data;

	try
	{
		Statement query(m_DB, "SELECT " + _column + " FROM Armor " + _where);

		while (query.executeStep())
		{
			// �ѹ��� �� ������� �������� �����.
			int num = query.getColumn(0);
			int user_num = query.getColumn(1);
			string user_name = query.getColumn(2);
			int armor_num = query.getColumn(3);
			int grade = query.getColumn(4);

			// ���⼭ ����.
			Data.num = num;
			Data.user_num = user_num;
			Data.user_name = user_name;
			Data.armor_num = armor_num;
			Data.grade = grade;

			// �߰�.
			returnDatas.push_back(Data);
		}
	}
	catch (const Exception e)
	{
		// ���� ó�� ���.
		throw MSG_BOX("���̺� ���ÿ� �����Ͽ����ϴ�\n�ش� ���̺��� �����ϴ��� Ȯ�����ּ���");
	}

	return returnDatas;
}

void CDataManager::Save(CUserDataManager* _userData)
{
	// ���� �̻��ϰ� ���̸� �ȵŴ�
	// ���⼭���� �ٸ� �����尡 ���� �õ� �� ����Ŵ.	
	EnterCriticalSection(&m_CriticalSection);

	// ���� �� ������.
	vector<CTableWeapon> m_WeaponData = _userData->GetWeaponData();
	vector<CTableArmor> m_ArmorData = _userData->GetArmorData();

	try
	{
		// Ʈ�����.
		Transaction transaction(m_DB);		

		// ���� ������ ����.
		for (auto data : m_WeaponData)
		{
			Statement weaponQuery(m_DB, "UPDATE Weapon SET grade = ? WHERE weapon_num == " + to_string(data.weapon_num));
			weaponQuery.bind(1, data.grade);
			weaponQuery.exec();
		}		

		// ���� ������ ����.
		for (auto data : m_ArmorData)
		{
			Statement armorQuery(m_DB, "UPDATE Armor SET grade = ? WHERE armor_num == " + to_string(data.armor_num));
			armorQuery.bind(1, data.grade);
			armorQuery.exec();
		}

		// Ʈ����� Ŀ��.
		transaction.commit();
	}
	catch (const Exception e)
	{
		throw MSG_BOX("���� ����");
	}

	// ���� �ٸ� ������ �����ص� �Ǵ� ������ �� ����.
	LeaveCriticalSection(&m_CriticalSection);	
}

// ����� ������.
_uint APIENTRY ThreadSave(void* _pArg)
{
	// ����ȯ.
	ThreadSendManager* manager = static_cast<ThreadSendManager*>(_pArg);

	// ����.
	manager->dataManager->Save(manager->userManager);	

	// ���� ������ �޸� ����.
	Safe_Delete(manager);

	return 1;
}

void CDataManager::AutoSave(CUserDataManager* _userData)
{	
	// ���� �����忡 ���� ������.
	ThreadSendManager* threadManager = new ThreadSendManager();
	threadManager->dataManager = this;
	threadManager->userManager = _userData;

	// ������ ����.
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadSave, threadManager, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("���� ������ ����");
	}	
}

void CDataManager::Free()
{
	__super::Free();

	DeleteCriticalSection(&m_CriticalSection);
}
