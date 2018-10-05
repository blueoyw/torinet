#include "Character.h"

bool Character::setRoom( Ptr<Room>& room)
{
    /*
	Ptr<Room> tmp = m_room.lock();
	if( tmp ) {
		LOG(L_WAR, "[%s] already in room[%p]", __func__, tmp.get());
		return false;
	}
	m_room = room;
	return true;
    */
    return true;
}

void Character::leaveRoom()
{
}

void Character::update()
{
}
