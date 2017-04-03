#pragma once

#include "Incl.h"

using namespace boost;

typedef struct {
	string 		command;
	string 		help;
} command_t;

typedef vector< string > SplitVector;

class BaseCommand {
public:
	virtual bool handle(string& cli, SplitVector& split) = 0;
	virtual void help() = 0;
};

template<typename T>
BaseCommand* createCommand() {
	return new T;
}

typedef	BaseCommand* (*CommandFuncPtr)();
typedef	std::map<std::string, CommandFuncPtr>::iterator CommandMapItor;

class CommandFactory {
public:
	typedef	std::map<std::string, CommandFuncPtr> CommandMap;

	template<typename T>
	bool				registerCommand(const std::string& cmd)
	{
		CommandMapItor	itor;
		itor = m_table.find(cmd);
		if( itor != m_table.end() ) {
			return false;
		}
		CommandFuncPtr function = &createCommand<T>;
		m_table.insert( std::make_pair(cmd, function) );
		return true;
	}

	void	help()
	{
		std::cout << "     command     :     comment" << std::endl;
		std::cout << "=====================================" << std::endl;
		for( CommandMapItor itor=m_table.begin(); itor!=m_table.end(); itor++) {
			std::cout << itor->first << " :     ";
			itor->second()->help();
		}
	}

	CommandMap m_table;
};

typedef boost::shared_ptr<CommandFactory> CommandFactoryPtr;

class CmdCreateUser :public BaseCommand {
public:
	CmdCreateUser(){};
	virtual ~CmdCreateUser(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

class CmdCreateParty :public BaseCommand {
public:
	CmdCreateParty(){};
	virtual ~CmdCreateParty(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

class CmdRequestJoin :public BaseCommand {
public:
	CmdRequestJoin(){};
	virtual ~CmdRequestJoin(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

class CmdLogin :public BaseCommand {
public:
	CmdLogin(){};
	virtual ~CmdLogin(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

class CmdStartTmr :public BaseCommand {
public:
	CmdStartTmr(){};
	virtual ~CmdStartTmr(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

class CmdSendPkt :public BaseCommand {
public:
	CmdSendPkt(){};
	virtual ~CmdSendPkt(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

class CmdSendEco :public BaseCommand {
public:
	CmdSendEco(){};
	virtual ~CmdSendEco(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

class CmdStart :public BaseCommand {
public:
	CmdStart(){};
	virtual ~CmdStart(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

class CmdStop :public BaseCommand {
public:
	CmdStop(){};
	virtual ~CmdStop(){};

	virtual bool handle(string& cli, SplitVector& split);
	virtual void help();
};

