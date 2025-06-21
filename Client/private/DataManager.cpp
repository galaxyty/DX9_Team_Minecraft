#include "DataManager.h"

IMPLEMENT_SINGLETON(CDataManager)

void CDataManager::Initialization()
{
	// 스레드 락 초기화.
	InitializeCriticalSection(&m_CriticalSection);

	try
	{
		// db 연결 시도.
		m_DB = { m_DBName, OPEN_READWRITE };
	}
	catch (const Exception e)
	{
		// 예외 처리 띄움.
		throw MSG_BOX("해당 데이터베이스가 존재하지 않습니다");		
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
		// 예외 처리 띄움.
		throw MSG_BOX("테이블 값 삽입에 실패하였습니다\n해당 테이블 컬럼에 맞는 양식인지 확인해주세요");
	}
}

CTableUser CDataManager::SelectUser(string _column, string _where)
{
	// 반환 데이터.
	CTableUser Data;

	try
	{
		Statement query(m_DB, "SELECT " + _column + " FROM User " + _where);

		while (query.executeStep())
		{
			// 한번에 안 담아져서 지역변수 만들고.
			int num = query.getColumn(0);
			string id = query.getColumn(1);
			string name = query.getColumn(2);
			string password = query.getColumn(3);

			// 여기서 담음.
			Data.num = num;
			Data.id = id;
			Data.name = name;
			Data.password = password;
		}
	}
	catch (const Exception e)
	{
		// 예외 처리 띄움.
		throw MSG_BOX("테이블 선택에 실패하였습니다\n해당 테이블이 존재하는지 확인해주세요");
	}

	return Data;
}

vector<CTableWeapon> CDataManager::SelectWeapon(string _column, string _where)
{
	// 최종 반환 데이터들.
	vector<CTableWeapon> returnDatas;

	// 담을 데이터.
	CTableWeapon Data;

	try
	{
		Statement query(m_DB, "SELECT " + _column + " FROM Weapon " + _where);

		while (query.executeStep())
		{
			// 한번에 안 담아져서 지역변수 만들고.
			int num = query.getColumn(0);
			int user_num = query.getColumn(1);
			string user_name = query.getColumn(2);
			int weapon_num = query.getColumn(3);
			int grade = query.getColumn(4);

			// 여기서 담음.
			Data.num = num;
			Data.user_num = user_num;
			Data.user_name = user_name;
			Data.weapon_num = weapon_num;
			Data.grade = grade;

			// 추가.
			returnDatas.push_back(Data);
		}
	}
	catch (const Exception e)
	{
		// 예외 처리 띄움.
		throw MSG_BOX("테이블 선택에 실패하였습니다\n해당 테이블이 존재하는지 확인해주세요");
	}

	return returnDatas;
}

vector<CTableArmor> CDataManager::SelectArmor(string _column, string _where)
{
	// 최종 반환 데이터들.
	vector<CTableArmor> returnDatas;

	// 담을 데이터.
	CTableArmor Data;

	try
	{
		Statement query(m_DB, "SELECT " + _column + " FROM Armor " + _where);

		while (query.executeStep())
		{
			// 한번에 안 담아져서 지역변수 만들고.
			int num = query.getColumn(0);
			int user_num = query.getColumn(1);
			string user_name = query.getColumn(2);
			int armor_num = query.getColumn(3);
			int grade = query.getColumn(4);

			// 여기서 담음.
			Data.num = num;
			Data.user_num = user_num;
			Data.user_name = user_name;
			Data.armor_num = armor_num;
			Data.grade = grade;

			// 추가.
			returnDatas.push_back(Data);
		}
	}
	catch (const Exception e)
	{
		// 예외 처리 띄움.
		throw MSG_BOX("테이블 선택에 실패하였습니다\n해당 테이블이 존재하는지 확인해주세요");
	}

	return returnDatas;
}

void CDataManager::Save(CUserDataManager* _userData)
{
	// 값이 이상하게 쓰이면 안돼니
	// 여기서부터 다른 스레드가 접근 시도 시 대기시킴.	
	EnterCriticalSection(&m_CriticalSection);

	// 저장 할 데이터.
	vector<CTableWeapon> m_WeaponData = _userData->GetWeaponData();
	vector<CTableArmor> m_ArmorData = _userData->GetArmorData();

	try
	{
		// 트랜잭션.
		Transaction transaction(m_DB);		

		// 무기 데이터 저장.
		for (auto data : m_WeaponData)
		{
			Statement weaponQuery(m_DB, "UPDATE Weapon SET grade = ? WHERE weapon_num == " + to_string(data.weapon_num));
			weaponQuery.bind(1, data.grade);
			weaponQuery.exec();
		}		

		// 갑옷 데이터 저장.
		for (auto data : m_ArmorData)
		{
			Statement armorQuery(m_DB, "UPDATE Armor SET grade = ? WHERE armor_num == " + to_string(data.armor_num));
			armorQuery.bind(1, data.grade);
			armorQuery.exec();
		}

		// 트랜잭션 커밋.
		transaction.commit();
	}
	catch (const Exception e)
	{
		throw MSG_BOX("저장 실패");
	}

	// 이제 다른 스레드 접근해도 되니 스레드 락 끊음.
	LeaveCriticalSection(&m_CriticalSection);	
}

// 저장용 스레드.
_uint APIENTRY ThreadSave(void* _pArg)
{
	// 형변환.
	ThreadSendManager* manager = static_cast<ThreadSendManager*>(_pArg);

	// 저장.
	manager->dataManager->Save(manager->userManager);	

	// 저장 끝나면 메모리 제거.
	Safe_Delete(manager);

	return 1;
}

void CDataManager::AutoSave(CUserDataManager* _userData)
{	
	// 저장 스레드에 보낼 데이터.
	ThreadSendManager* threadManager = new ThreadSendManager();
	threadManager->dataManager = this;
	threadManager->userManager = _userData;

	// 스레드 시작.
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadSave, threadManager, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("저장 스레드 실패");
	}	
}

void CDataManager::Free()
{
	__super::Free();

	DeleteCriticalSection(&m_CriticalSection);
}
