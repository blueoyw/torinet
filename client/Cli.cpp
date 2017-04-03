#include "Cli.h"

extern ClientVec g_clientVec;
extern int g_txSize;

bool CmdCreateUser::handle(string& cli, SplitVector& split)
{
	ClientPtr      client;
	LOG(L_DEB,"[%s] vector size[%ld]", __func__, g_clientVec.size() );
	for( size_t i=0; i<g_clientVec.size(); i++) {
		client = g_clientVec[i];
		rq_CrteUser_t cpMsg;
		sprintf(cpMsg.id, client->id().c_str() );
		strcpy(cpMsg.passwd, "test");

		Msg msg;
		msg.encodeMsg(rq_CrteUser, (uint8_t*)&cpMsg, sizeof(cpMsg));
		client->write(msg);
	}
	return true;
}
void CmdCreateUser::help()
{
	std::cout << "create users as many as clients and make DB data in mysql" << std::endl;
}

bool CmdCreateParty::handle(string& cli, SplitVector& split)
{
	if( !g_clientVec[Client::MASTER_CLIENT] ) {
		return false;
	}
	ClientPtr	client =  g_clientVec[Client::MASTER_CLIENT];
	rq_CrteParty_t cpMsg;
	strcpy(cpMsg.id, client->id().c_str() );

	Msg msg;
	msg.encodeMsg(rq_CrteParty, (uint8_t*)&cpMsg, sizeof(cpMsg));
	client->write(msg);
	LOG(L_DEB, "[%s] master id[%s]", __func__, client->id().c_str() );
	return true;
	return 1;
}
void CmdCreateParty::help()
{
	std::cout << "create party" << std::endl;
}

bool CmdRequestJoin::handle(string& cli, SplitVector& split)
{
	if( !g_clientVec[Client::MASTER_CLIENT] ) {
		return false;
	}
	ClientPtr	client =  g_clientVec[Client::MASTER_CLIENT];
	string id = client->id();
	LOG(L_DEB, "[%s] master id[%s] partyId[%d]", __func__, 
			client->id().c_str(), client->m_partyId);
	uint32_t&	partyId = client->m_partyId;
	if( g_clientVec.size() > 1 ) {
		for( size_t i=1; i<g_clientVec.size(); i++) {
			client = g_clientVec[i];
			rq_JoinUser_t cpMsg;
			strcpy(cpMsg.fromId,  id.c_str() );
			strcpy(cpMsg.toId,  client->id().c_str() );
			cpMsg.partyId = partyId;

			LOG(L_DEB, "[%s] from[%s] to[%s] partyId[%d]", __func__, 
					cpMsg.fromId, cpMsg.toId, cpMsg.partyId);
			Msg msg;
			msg.encodeMsg(rq_JoinUser, (uint8_t*)&cpMsg, sizeof(cpMsg));
			client->write(msg);
		}
	}
	return true;
}
void CmdRequestJoin::help()
{
	std::cout << "request join to slave clients"<< std::endl;

}

bool CmdLogin::handle(string& cli, SplitVector& split)
{
	ClientPtr	client;
	for( size_t i=0; i<g_clientVec.size(); i++) {
		client = g_clientVec[i];
		rq_Login_t cpMsg;
		strcpy(cpMsg.id, client->id().c_str() );
		strcpy(cpMsg.passwd, "test");

		Msg msg;
		msg.encodeMsg(rq_Login, (uint8_t*)&cpMsg, sizeof(cpMsg));
		client->write(msg);
	}

	return true;
}
void CmdLogin::help()
{
	std::cout << "login users" << std::endl;
}

bool CmdStartTmr::handle(string& cli, SplitVector& split)
{
	if( !g_clientVec[Client::MASTER_CLIENT] ) {
		return false;
	}
	ClientPtr	client =  g_clientVec[Client::MASTER_CLIENT];
	client->startTmr();
	LOG(L_DEB, "[%s] start timer master id[%s]", __func__, client->id().c_str() );

	return true;
}
void CmdStartTmr::help()
{
	std::cout << "send dummy packet from master client to server per 1 second. server broadcast that packet to slave clients" << std::endl;
}

bool CmdSendPkt::handle(string& cli, SplitVector& split)
{
	if( !g_clientVec[Client::MASTER_CLIENT] ) {
		return false;
	}
	ClientPtr	client =  g_clientVec[Client::MASTER_CLIENT];
	nf_Message_t cpMsg;
	cpMsg.partyId = client->m_partyId;
	strcpy(cpMsg.msg, cli.c_str() );

	Msg msg;
	msg.encodeMsg(nf_Message, (uint8_t*)&cpMsg, sizeof(cpMsg));
	client->write(msg);

	return true;
}
void CmdSendPkt::help()
{
	std::cout << "make dummy packet and send to server" << std::endl;
}

void CmdSendEco::help()
{
	std::cout << "send_eco [packet size]byte. make dummy packet and send to server. server will send eco message to client." << std::endl;
}
bool CmdSendEco::handle(string& cli, SplitVector& split)
{
    g_txSize  = atoi( split[1].c_str() );
	ClientPtr      client;
	LOG(L_DEB,"[%s] cli[%s]", __func__, cli.c_str() );
	for( size_t i=0; i<g_clientVec.size(); i++) 
    {
		client = g_clientVec[i];
		NfEco cpMsg;
        memset( cpMsg.msg, 0xAB, g_txSize);

		Msg msg;
		msg.encodeMsg(ID_nf_Eco, (uint8_t*)&cpMsg, g_txSize );
		client->write(msg);
	}
	return true;
}

void CmdStart::help()
{
	std::cout << "start [bandwidth]Gbps. before this, send_eco first. " << std::endl;
}

bool CmdStart::handle(string& cli, SplitVector& split)
{
    double bandwidth  = atof( split[1].c_str() );
    //int count = bandwidth * 1000 / (g_txSize*8) / g_clientVec.size();
    int count = bandwidth * 1024 * 1024 / ( (g_txSize + Msg::hdrLength)*8) / g_clientVec.size() / 100 ;
	ClientPtr      client;
	for( size_t i=0; i<g_clientVec.size(); i++) 
    {
        LOG(L_FAT, "[%s] %f Gbps timeout %d count per 10 mili-second", lfunc, bandwidth, count);
		client = g_clientVec[i];
        client->startTx( count );
	}
	return true;
}

void CmdStop::help()
{
	std::cout << "stop packet send." << std::endl;
}

bool CmdStop::handle(string& cli, SplitVector& split)
{
	ClientPtr      client;
	for( size_t i=0; i<g_clientVec.size(); i++) 
    {
		client = g_clientVec[i];
        client->stopTx();
	}
	return true;
}

