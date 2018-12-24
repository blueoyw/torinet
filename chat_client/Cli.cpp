#include "Cli.h"

extern vector<Ptr<Client>> g_clientVec;
extern int g_txSize;

bool CmdSendMessge::handle(string& cli, SplitVector& split)
{
	LOG(L_DEB,"[%s] vector size[%ld]", __func__, g_clientVec.size() );
	for( size_t i=0; i<g_clientVec.size(); i++) {
	    Ptr<Client> client = g_clientVec[i];
		NfMessage nf;
		strcpy(nf.message, split[1].c_str());

		Msg msg;
		msg.encodeMsg(NF_MESSAGE, (uint8_t*)&nf, sizeof(nf));
		client->send(msg);
	}
	return true;
}
void CmdSendMessge::help()
{
	std::cout << "create users as many as clients and make DB data in mysql" << std::endl;
}

void CmdStop::help()
{
	std::cout << "stop packet send." << std::endl;
}

bool CmdStop::handle(string& cli, SplitVector& split)
{
	for( size_t i=0; i<g_clientVec.size(); i++) 
    {
	    Ptr<Client> client = g_clientVec[i];
        client->stop();
	}
	return true;
}

