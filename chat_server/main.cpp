#include "Incl.h"

const wc8* g_ver = "CHAT_SERVER_R0.0.1";
const wc8* g_date = __DATE__;
const wc8* g_time = __TIME__;

//world global data
//CharactersMap  	g_characters;
//boost::mutex		g_charactersLock;

//int32_t			g_roomId = 0;
namespace Global
{
    static TcpServer* chatServer;

    //static RoomMap			g_rooms;
    //static boost::mutex	g_roomsLock;
}
//world end

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger(""));

using namespace tori;

void runServer(string address, int port)
{
    const string name = "chatServer";

    //ywoh to do config file
    net::ServerConfig config;
    config._threadCount = 1;

    //ywoh to do regist handler
    Global::chatServer = new TcpServer(name, config);
    Global::chatServer->start(address, port);

    Global::registerSessionOpenedHandler();

	virtual void registerSessionOpenedHandler(const SessionOpenedHandler& handler)
	{
		_openedHandler = handler;
	}

	virtual void registerSessionClosedHandler(const SessionClosedHandler& handler)
	{
		_closedHandler = handler;
	}

	virtual void registerMessageHandler(const MessageHandler& handler)
	{
		_messageHandler = handler;
	}

    //g_rooms.clear();
    //g_characters.clear();
}

int main(int argc, char* argv[])
{
	int 	 numThreads = 8;
	IoServicePoolPtr ioPool;
	try
	{
		boost::program_options::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "display help message")
			("version,v", "display version information")
			("daemon,d", "execute as daemon")
			("threads,t", boost::program_options::value<int>(), "thread count")
			;
		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);

		if( vm.count("help") ) {
			std::cout << desc << std::endl;
			return 0;
		}

		if (vm.count("version")) {
			std::cout << "version : " << g_ver << std::endl;
			std::cout << "date    : " << g_date << std::endl;
			std::cout << "time    : " << g_time << std::endl << std::endl;
			return 0;
		}

		if ( vm.count("threads") ) {
			numThreads = vm["threads"].as<int>();	
		}

		initLog4cxx(argv[0]);

		LOG(L_INF, "Start Server (%s) ------------------>", g_ver);
		LOG(L_INF, "Compiled on %s %s", g_date, g_time);
		LOG(L_INF, "%d threads", numThreads );

		ioPool.reset( new IoServicePool(numThreads)) ;
		boost::asio::io_service& ios = ioPool->getIoService();

		if ( vm.count("daemon") ) {
			ios.notify_fork(boost::asio::io_service::fork_prepare);

			if( pid_t pid = fork() ) {
				if( pid > 0 ) {
					exit(0);
				} else {
					std::cout << "first fork failed" << std::endl;
					return 1;
				}
			}

			chdir("/");
			umask(0);

			setsid();

			close(0);
			close(1);
			close(2);

			ios.notify_fork(boost::asio::io_service::fork_child);
			LOG(L_INF, "daemon started");
		}

        runServer("127.0.0.1", 20000);

		std::cout << "end" << std::endl;

        //getchar();
	} catch (std::exception& e) {
		LOG(L_WAR, "Exception:%s", e.what() );
	}

	return 0;
}

