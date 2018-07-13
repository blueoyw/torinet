#include "Room.h"

bool	Room::attach( Ptr<Character>& character) 
{

    return true;
}

bool	Room::dettach( Ptr<Character>& character )
{

    return true;
}

/*
bool	Room::setCharacter( CharacterPtr& ch)
{
	if( m_cnt >= MAX_CHARACTER) {
		LOG(L_WAR, "[%s] MAX[%d]", __func__, m_cnt);
		return false;
	}
	
	LOG(L_DEB, "[%s] cnt[%d]", __func__, m_cnt);
	m_characters[m_cnt++] = ch;
	return true;
}

bool	Room::getCharacter( int32_t& idx, CharacterPtr& ch )
{ 
	if( (idx >= m_cnt) || (idx >= MAX_CHARACTER ) ){
		return false;
	}
	ch = m_characters[idx]; 
	return true;
}

void	Room::leave( CharacterPtr& ch )
{ 
	for( int i=0; i< m_cnt; i++ ) {
		CharacterPtr tmp_ch = m_characters[i]; 
		if( tmp_ch == ch ) {
			tmp_ch.reset();
			m_cnt--;
		}
	}
}
*/
