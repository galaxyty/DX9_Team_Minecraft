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
// 테이블 생성은 코드가 아닌 cmd 명령어로 할 것이라
// 테이블 생성 코드는 없음.
class CDataManager final : public CBase
{
	DECLARE_SINGLETON(CDataManager);

private:
	CDataManager() = default;
	virtual ~CDataManager() = default;

public:
	// 데이터베이스 연결.
	void Initialization();

	// 해당 테이블에 값 삽입 (primary는 생략하고 넣기)
	void Insert(string _table, string _value);

	// User 테이블 데이터 반환.
	CTableUser SelectUser(string _column, string _where = "");

	// Weapon 테이블 데이터 반환.
	vector<CTableWeapon> SelectWeapon(string _column, string _where = "");

	// Armor 테이블 데이터 반환.
	vector<CTableArmor> SelectArmor(string _column, string _where = "");

	// 유저 데이터 저장 (트랜잭션으로 한꺼번에 묶어서 저장).
	void Save(CUserDataManager* _userData);

	// 유저 데이터 스레드 자동 저장.
	void AutoSave(CUserDataManager* _userData);

	virtual void Free() override;

private:
	// 데이터베이스 이름.
	string m_DBName = { "TOA.db" };

	// 데이터베이스 객체.
	// 기본 생성자가 없어서 그냥 넣으면 삭제된 함수 오류뜸.
	// 그래서 그냥 이렇게 넣어줌.
	Database m_DB = { m_DBName, OPEN_READWRITE };

	// 자동 저장 쓰레드.
	HANDLE						m_hThread = {};

	// 스레드 락.
	CRITICAL_SECTION			m_CriticalSection = {};
};

// 저장 스레드에 보낼 매니저 클래스.
class ThreadSendManager
{
public:
	CDataManager* dataManager;
	CUserDataManager* userManager;
};

END
