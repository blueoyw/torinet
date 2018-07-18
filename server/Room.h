#pragma once

#include "Incl.h"
#include <set>

using namespace tori;
using namespace tori::net;

class Character;

class Room
:public boost::enable_shared_from_this<Room>
{
public:
	enum { MAX_CHARACTER=8 };

	Room( Ptr<Character> master, int id): m_master(master),m_cnt(1), m_id(id)
	{
	}

	bool	attach( Ptr<Character>& character) ;
	bool	dettach( Ptr<Character>& character );

	int32_t	getId(){ return m_id; }
	int32_t	getCount(){ return m_cnt; }

private:
    set< Ptr<Character> > m_characters;
	Ptr<Character>		m_master;

	int					m_cnt;
	int32_t				m_id;
};
