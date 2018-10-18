#pragma once
#include "Incl.h"

using namespace std;

namespace tori {
namespace net {
enum CloseReason {
	ActiveClose = 0,
	Disconnected,
	Timeout
};
class Session
{
public:
	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;

	virtual ~Session() {};

	virtual int getID() const = 0;
	virtual bool getRemoteEndpoint( string& ip, uint16_t& port ) const = 0;
	virtual void start() = 0 ;
	virtual void send( const uint8_t* data, size_t size ) = 0;
	virtual void send ( const Msg& msg ) = 0;
	virtual void read( size_t size ) = 0;
	virtual void send( Msg& msg ) = 0;
	virtual void close() = 0;
	virtual bool isOpen() const = 0;
	virtual Msg& getMsg() = 0;
	//virtual asio::strand& getStrand() const = 0;

protected:
	Session() {};
};

// Handler to be notified on session creation.
using SessionOpenedHandler = function<void(const Ptr<Session>&)>;

// Handler to be notified on session close. This one ignores the close reason.
using SessionClosedHandler = function<void(const Ptr<Session >&, const CloseReason&)>;

// Receive handler type.
//using MessageHandler = function<void(const Ptr<Session>&, const uint8_t*, size_t)>;
using MessageHandler = function<void(const Ptr<Session>&)>;


}
}

