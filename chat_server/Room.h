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

	Room( Ptr<Character> master, int id): master_(master),cnt_(1), id_(id)
	{
	}

	bool	attach( Ptr<Character>& character) ;
	bool	dettach( Ptr<Character>& character );

	int32_t	getId(){ return id_; }
	int32_t	getCount(){ return cnt_; }

private:
    set< Ptr<Character> > characters_;
	Ptr<Character>		master_;

	int					cnt_;
	int32_t				id_;
};
