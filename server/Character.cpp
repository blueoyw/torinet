#include "Character.h"
#include "Room.h"

bool	Character::setRoom( RoomPtr& room)
{
	RoomPtr tmp = m_room.lock();
	if( tmp ) {
		LOG(L_WAR, "[%s] already in room[%p]", __func__, tmp.get());
		return false;
	}
	m_room = room;
	return true;
}

void	Character::leaveRoom()
{
	m_room.reset();
}

