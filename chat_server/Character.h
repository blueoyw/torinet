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
	Character(const string& id, const string& name, Ptr<TcpSession> sess )
		:id_(id), name_(name), sess_(sess)
	{
	}

    virtual void update();

	bool setRoom( Ptr<Room>& room);
	void leaveRoom();

	Ptr<TcpSession> session() { return sess_; }
	string id() { return id_; }

private:
	string				id_;
	string				name_;
	Ptr<TcpSession>		sess_;
	WeakPtr<Room>		room_;
};

