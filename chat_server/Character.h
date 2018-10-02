#pragma once
#include "Incl.h"
#include "GameObject.h"
#include "Room.h"

using namespace tori;
using namespace tori::net;


class Room;

class Character
:public GameObject, public boost::enable_shared_from_this<Character>
{
public:
    enum Equipment {
        E_HEAD = 0,
        E_BODY ,
        E_END
    };

	Character(const string& id, const string& name, Ptr<TcpSession> sess )
		:m_id(id), m_name(name), m_sess(sess)
	{
	}

    virtual void update();

	bool setRoom( Ptr<Room>& room);
	void leaveRoom();

	Ptr<TcpSession> session() { return m_sess; }
	string id() { return m_id; }

private:
	string				m_id;
	string				m_name;
	uint64_t	    	m_exp;
	Equipment			m_equip[Equipment::E_END];
	Ptr<TcpSession>		m_sess;
	WeakPtr<Room>		m_room;
};

