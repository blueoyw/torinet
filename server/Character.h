#pragma once

//#ifndef _CHARACTER_H_
//#define _CHARACTER_H_
#include "Incl.h"
#include "../lib/TcpSession.h"
#include "Room.h"

typedef enum {
	E_HEAD = 0,
	E_END
} EQUEP_t;

class Room;
typedef boost::shared_ptr<Room> RoomPtr;
typedef boost::weak_ptr<Room> RoomWeakPtr;

class Character
:public boost::enable_shared_from_this<Character>
{
public:
	Character(const string& id, const string& name, TcpSessionPtr sess )
		:m_id(id), m_name(name), m_sess(sess)
	{
	}

	bool setRoom( RoomPtr& room);
	void leaveRoom();

	//getter
	TcpSessionPtr 	session() { return m_sess; }
	string& id() { return m_id; }

	RoomWeakPtr			m_room;
private:
	string				m_id;
	string				m_name;
	uint64_t				m_exp;
	uint32_t				m_equip[E_END];
	TcpSessionPtr		m_sess;

};

typedef boost::shared_ptr<Character> CharacterPtr;
typedef boost::weak_ptr<Character> CharacterWeakPtr;


//#endif	

