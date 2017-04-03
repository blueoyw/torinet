#pragma once

//#ifndef _ROOM_H_
//#define _ROOM_H_
#include "Incl.h"
#include "../lib/TcpSession.h"

class Character;

typedef boost::shared_ptr< Character >	CharacterPtr;

class Room
:public boost::enable_shared_from_this<Room>
{
public:
	enum { MAX_CHARACTER=2048 };

	Room( CharacterPtr master):m_master(master),m_cnt(0), m_id(0)
	{
	}

	bool	setCharacter( CharacterPtr& ch) ;
	bool	getCharacter( int32_t& idx, CharacterPtr& ch );
	void	leave( CharacterPtr& ch ) ;
	void	setId( int32_t& id){ m_id= id; }
	int32_t	id(){ return m_id; }
	int32_t	cnt(){ return m_cnt; }

private:
	CharacterPtr		m_characters[MAX_CHARACTER];
	CharacterPtr		m_master;
	int					m_cnt;
	int32_t				m_id;
};

typedef boost::shared_ptr<Room> RoomPtr;
typedef boost::weak_ptr<Room> RoomWeakPtr;


//#endif	

