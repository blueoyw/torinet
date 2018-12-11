#include "Incl.h"

using namespace std;
using namespace boost;
using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

///////////////////////Global
const wc8* g_ver = "CLIENT_R0.1.1";
const wc8* g_date = __DATE__;
const wc8* g_time = __TIME__;


log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger(""));

vector<Ptr<Client>> g_clientVec;

CommandFactoryPtr g_cmdFactory;
bool g_bTerm;
int g_txSize;

static void sigHandler(int sig)
{
    g_bTerm = true;
    signal(SIGINT,0);
    signal(SIGTERM,0);
    signal(SIGSEGV,0);

	LOG(L_FAT, "[%s] signal[%d] end", __func__, sig);
	for( size_t i=0; i<g_clientVec.size(); i++) 
    {
		Ptr<Client> client = g_clientVec[i];
		client->stop();
	}
    g_clientVec.clear();

    switch(sig) 
    {
        case SIGSEGV:
            break;
        default:
            exit(0);
    }
}

void startRx(Ptr<Client> client)
{
	client->start();
}

//0: master
//others: slaves
void readLine() 
{
	string str;

	while( !g_bTerm ) 
    {
		cout << "> ";
		getline( cin, str );
		if ( str.length() > 0 ) 
        {
            LOG(L_FAT, "[%s] cli[%s]", lfunc, str.c_str() );
			if ( !str.compare("quit") ) 
            {
                g_bTerm = true;
				return;
			} 
            else if ( !str.compare("?") || !str.compare("help") ) 
            {
				g_cmdFactory->help();
				continue;
			}

			SplitVector splitVec;
			split( splitVec, str, is_any_of(" "), token_compress_on );

			CommandMapItor itor = g_cmdFactory->m_table.find(splitVec[0]);
			if( itor !=  g_cmdFactory->m_table.end() )
				itor->second()->handle(str, splitVec);
		}
	}
}

int main(int argc, char* argv[])
{
	try
	{
		std::string host;
		int numThread = 1;

		boost::program_options::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "display help message")
			("version,v", "display version information")
			("remote,r", boost::program_options::value<string>(), "remote ip")
			("threads,t", boost::program_options::value<int>(), "client thread number")
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
			numThread = vm["threads"].as<int>();
		}

		if ( vm.count("remote") ) {
			host = vm["remote"].as<string>();
		} else {
			host = "127.0.0.1";
		}

		initLog4cxx(argv[0]);

		signal(SIGINT, sigHandler);
		signal(SIGTERM, sigHandler);
        signal(SIGSEGV, sigHandler);
        signal(SIGPIPE, SIG_IGN);

		//register command
		g_cmdFactory.reset(new CommandFactory());
		g_cmdFactory->registerCommand<CmdCreateUser>("create_user");

		//boost::asio::io_service ios;
		std::vector< boost::shared_ptr<boost::thread> > threads;
		LOG(L_FAT, "[%s] create io_service[%d]",__func__, numThread);

		char tmp[32];
		std::string id;
		for( int16_t i=0; i<numThread ; i++ ) {
			memset(tmp,0x00, sizeof(tmp));
			sprintf(tmp, "%s_%d", argv[2], i );
			LOG(L_DEB, "[%s] id[%s]",__func__, tmp);
			id = tmp;
            
            const string name = "localhost";
            const int port = 20000;

            //ywoh to do
            tcp::endpoint ep;
            Ptr<Client> client( new Client(name, port));

			boost::shared_ptr<boost::thread> rx_thread(new boost::thread(startRx, client));
			threads.push_back(rx_thread);
			g_clientVec.push_back(client);
			LOG(L_DEB, "[%s] id[%s] connect",__func__, tmp);
		}

        g_bTerm = false;
		readLine();

		for ( size_t i=0; i< threads.size() ; i++ ) {
			threads[i]->join();
		}

	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

