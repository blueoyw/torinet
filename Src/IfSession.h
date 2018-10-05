#ifndef _IF_SESSION_H_
#define _IF_SESSION_H_

class SessionHandler
{
public:
	virtual bool	read( uint32_t& msgId, int16_t& length, VoidPtr sess, VoidWeakPtr owner ) = 0;
	virtual bool	error( const boost::system::error_code& error, VoidPtr sess, VoidWeakPtr owner ) = 0;
	virtual bool	connect( VoidPtr sess, VoidWeakPtr owner ) = 0;
};

typedef boost::shared_ptr<SessionHandler>	SessionHandlerPtr;

#endif	

