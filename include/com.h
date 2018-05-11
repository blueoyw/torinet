/**
  @file com.h
  @date 2015/04/03
  @author Youngwan OH 
  @brief common type define
*/

#ifndef _com_h_
#define _com_h_

#if 0
typedef char				wc8;
typedef unsigned char		wuc8;
typedef short				wi16;
typedef unsigned short		wui16;
typedef int					wi32;
typedef unsigned int		wui32;
typedef long long			wi64;
typedef unsigned long long	wui64;
typedef float				wf32;
typedef double				wf64;

typedef boost::shared_ptr<boost::asio::deadline_timer>	timer_ptr;
typedef boost::chrono::system_clock::time_point time_point;

typedef boost::posix_time::microsec USECS;
typedef boost::posix_time::millisec MSECS;
typedef boost::posix_time::seconds 	SECS;
typedef boost::posix_time::minutes 	MINS;
typedef boost::posix_time::hours 	HOURS;
typedef boost::gregorian::days 		DAYS;

typedef boost::asio::ip::address       address;
typedef boost::asio::ip::tcp::endpoint ep;

#define KILO 1000UL
#define MEGA (KILO * KILO)
#define GIGA (MEGA * KILO)

#define MAX_CPU	32

inline wc8* get_home()
{
	wc8* home = getenv("TEST");
	if (home == NULL) {
		return (wc8*)"/home/ywoh/test";
	}
	return home;
}

//////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////
/*
#define L_TRC   5000
#define L_DEB   10000
#define L_INF   20000
#define L_WAR   30000
#define L_ERR   40000
#define L_FAT   50000

#define LOG_MAXLEN 1024*4

extern log4cxx::LoggerPtr logger;

#define LOG(lev, ...) \
{ \
	log4cxx::LevelPtr level = log4cxx::Level::toLevel(lev); \
	if (logger->isEnabledFor(level)) { \
		wc8 log[LOG_MAXLEN]; \
		snprintf(log, LOG_MAXLEN, __VA_ARGS__); \
		LOG4CXX_LOG(logger, level, log); \
	} \
}\

#define lfunc            __func__
#define SLOG            LOG(L_TRC, "[%s] start", lfunc)
#define ELOG            LOG(L_TRC, "[%s] end", lfunc)

inline void initLog4cxx(wc8* p_exe)
{
    logger = log4cxx::Logger::getLogger(p_exe);

    wc8 file[256];
    sprintf(file, "../config/log4cxx.ini");
    std::string config(file);
    log4cxx::PropertyConfigurator::configure(config);

    ELOG;
}
*/

inline wui64 swap64(wui64 a)
{
	return (((a & 0xff00000000000000LL) >> 56) | \
			((a & 0x00ff000000000000LL) >> 40) | \
			((a & 0x0000ff0000000000LL) >> 24) | \
			((a & 0x000000ff00000000LL) >> 8 ) | \
			((a & 0x00000000ff000000LL) << 8 ) | \
			((a & 0x0000000000ff0000LL) << 24) | \
			((a & 0x000000000000ff00LL) << 40) | \
			((a & 0x00000000000000ffLL) << 56));
}
inline wui64 htonll(wui64 a)
{
	return swap64(a);
}
inline wui64 ntohll(wui64 a)
{
	return swap64(a);
}

#define mask64(a,b)	((wui64)a<<32 | b)
#define upper32(k)	((wui32)(k>>32))
#define lower32(k)	((wui32)(k&0x00000000ffffffff))

#define mask32(a,b)	((wui32)a<<16 | b)
#define upper16(k)	((wui16)(k>>16))
#define lower16(k)	((wui16)(k&0x0000ffff))

inline wc8* ip2str(wui32 ip)
{
	wuc8 p1 = (ip & 0xff000000) >> 24;
	wuc8 p2 = (ip & 0x00ff0000) >> 16;
	wuc8 p3 = (ip & 0x0000ff00) >> 8;
	wuc8 p4 = (ip & 0x000000ff);

	static char sbuff[64];

	sprintf(sbuff, "%d.%d.%d.%d", p1,p2,p3,p4);
	return sbuff;
}

inline wui64 get_unix_time(wui64 erfts)
{
	unsigned long long lts;
	struct timeval tv;

	lts = erfts;
	tv.tv_sec = lts >> 32;
	lts = ((lts & 0xffffffffULL) * 1000 * 1000);
	lts += (lts & 0x80000000ULL) << 1;  /* rounding */
	tv.tv_usec = lts >> 32;
	if (tv.tv_usec >= 1000000) {
		tv.tv_usec -= 1000000;
		tv.tv_sec += 1;
	}

	wui64 ts = tv.tv_sec*1000000LL + tv.tv_usec;
	return ts;
}

inline wui64 get_unix_time()
{
	timeval tv;
	gettimeofday(&tv, NULL);

	wui64 ts = tv.tv_sec*1000000LL + tv.tv_usec;
	return ts;
}

#define chk_shm_size(_size_) 	((_size_ >= 1) && (_size_ <= 32))
#define chk_erf_size(_size_) 	((_size_ >= 100) && (_size_ <= 1000))
#define chk_http_tmout(_msec_) 	((_msec_ >= 100) && (_msec_ <= 60000) && (_msec_%100==0))
#define chk_svc_elaps(_sec_) 	((_sec_ >= 10) && (_sec_ <= 3600))
#define chk_adr_dir(_dir_) 		((!_dir_.empty()) && (_dir_.length() <= 255))
#define chk_erf_dir(_dir_) 		((!_dir_.empty()) && (_dir_.length() <= 255))
#define chk_svc_dur(_dur_) 		((_dur_ != 0) && (_dur_%10 == 0) && (_dur_) <= 100)
#define chk_svc_vol(_vol_) 		((_vol_ >= 1) && (_vol_ <= 4294967295))

#define MAX_FILTER	7

//#define FE_REQ        "(REQUEST_EVENT)(=|!=)(GET|POST|HEAD|PUT|DELETE|CONNECT|OPTION|TRACE)"
//#define FE_REQ        "(REQUEST_EVENT)(=|!=)(GET)"
#define FE_REQ		"(REQUEST_EVENT)(=|!=)(.+)"
#define FE_HOST		"(HOST)(=|!=)(.+)"
#define FE_CONT		"(CONTENT_TYPE)(=|!=)(.+)"
#define FE_PROC		"(PROCESS_TIME)(>|<)(\\d+)"
#define FE_PROCRX	"(PROCESS_RX_TIME)(>|<)(\\d+)"
#define FE_SETUP	"(SETUP_TIME)(>|<)(\\d+)"
#define FE_BYTES	"(RX_BYTES)(>|<)(\\d+)"

#define IP_V4        4
#define IP_V6        6

typedef enum {
   NONE_APP = 0,
   APP_HTTP = 1,
   APP_STREAMING = 2,
   APP_AFREECA = 3
} app_type;

typedef enum {
   NONE_BOUND = 0,
   IN_BOUND = 1,
   OUT_BOUND = 2
} bound_type;

struct ipc_ip {
   wc8  ip[48];
   wui16 port;
   wui16 ver;

   ipc_ip()
   {
      memset(ip, 0x00, sizeof(ip));
      port = 0;
      ver = 0;
   }
};

inline boost::asio::ip::address get_addr(wui32 ip)
{
   boost::asio::ip::address_v4::bytes_type bt;

   bt[0] = (ip & 0xff000000) >> 24;;
   bt[1] = (ip & 0x00ff0000) >> 16;;
   bt[2] = (ip & 0x0000ff00) >> 8;;
   bt[3] = (ip & 0x000000ff);

   boost::asio::ip::address_v4 a4(bt);
   boost::asio::ip::address addr(a4);
   return addr;
}
inline boost::asio::ip::address get_addr(wuc8* ip)
{
   boost::asio::ip::address_v6::bytes_type bt;
   for (size_t i=0; i<bt.size(); ++i) bt[i] = ip[i];

   boost::asio::ip::address_v6 a6(bt);
   boost::asio::ip::address addr(a6);
   return addr;
}
inline boost::asio::ip::tcp::endpoint get_ep(wui32 ip, wui16 port)
{
   boost::asio::ip::tcp::endpoint ep(get_addr(ip), port);
   return ep;
}
inline boost::asio::ip::tcp::endpoint get_ep(wuc8* ip, wui16 port)
{
   boost::asio::ip::tcp::endpoint ep(get_addr(ip), port);
   return ep;
}


typedef struct ip_key {

   boost::asio::ip::tcp::endpoint   src;
   boost::asio::ip::tcp::endpoint   dst;

   ip_key() {}
   ip_key(const ip_key& k) { src = k.src; dst = k.dst; }
   ip_key(boost::asio::ip::tcp::endpoint& s, boost::asio::ip::tcp::endpoint& d) {
      src = s; dst = d;
   }
   ip_key(wui32 s, wui32 d, wui16 sp, wui16 dp) { set(s,d,sp,dp); }
   ip_key(wuc8* s, wuc8* d, wui16 sp, wui16 dp) { set(s,d,sp,dp); }
   ip_key(address& s, address& d, wui16 sp, wui16 dp) { set(s,d,sp,dp); }
   void set(wui32 s, wui32 d, wui16 sp, wui16 dp) {
      src = get_ep(s, sp); 
      dst = get_ep(d,dp);
   }
   void set(wuc8* s, wuc8* d, wui16 sp, wui16 dp) {
      src = get_ep(s,sp); dst = get_ep(d,dp);
   }
   void set(address& s, address& d, wui16 sp, wui16 dp) {
      boost::asio::ip::tcp::endpoint se(s, sp);
      src = se;
      boost::asio::ip::tcp::endpoint de(d, dp);
      dst = de;
   }
   void set(ep& s, ep& d) {
      src = s;
      dst = d;
   }
   bool operator<(const ip_key& k) const {
      if (src != k.src) return (src < k.src);
      return (dst < k.dst);
   }
   bool operator==(const ip_key& k) const {
      return ((src==k.src) && (dst==k.dst));
   }
   bool operator!=(const ip_key& k) const {
      return ((src!=k.src) || (dst!=k.dst));
   }

   std::string to_string() {
      wc8   s[256];
      sprintf(s, "src=%s(%d),dst=%s(%d)",
            src.address().to_string().c_str(), src.port(),
            dst.address().to_string().c_str(), dst.port());
      return std::string(s);
   }
} ip_key_t;

//4 tuple hash, src ip, port dst ip, port
struct ip_4_hash {
	std::size_t operator()(const ip_key& k) const {
		std::size_t seed = 0;

		if (k.src.address().is_v4()) {
			boost::asio::ip::address_v4::bytes_type bt = k.src.address().to_v4().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else if (k.src.address().is_v6()) {
			boost::asio::ip::address_v6::bytes_type bt = k.src.address().to_v6().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else {
			boost::hash_combine(seed, k.src.address().to_string());
		}
		boost::hash_combine(seed, k.src.port());

		if (k.dst.address().is_v4()) {
			boost::asio::ip::address_v4::bytes_type bt = k.dst.address().to_v4().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else if (k.dst.address().is_v6()) {
			boost::asio::ip::address_v6::bytes_type bt = k.dst.address().to_v6().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else {
			boost::hash_combine(seed, k.dst.address().to_string());
		}
		boost::hash_combine(seed, k.dst.port());

		return seed;
	}
};

//2 tuple hash, ip, port
struct ip_2_hash {
	std::size_t operator()(const ep& k) const {
		std::size_t seed = 0;

		if (k.address().is_v4()) {
			boost::asio::ip::address_v4::bytes_type bt = k.address().to_v4().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else if (k.address().is_v6()) {
			boost::asio::ip::address_v6::bytes_type bt = k.address().to_v6().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else {
			boost::hash_combine(seed, k.address().to_string());
		}
		boost::hash_combine(seed, k.port());

		return seed;
	}
};

//1 tuple hash src ip hash
struct ip_1_hash {
	std::size_t operator()(const address& k) const {
		std::size_t seed = 0;

		if (k.is_v4()) {
			boost::asio::ip::address_v4::bytes_type bt = k.to_v4().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else if (k.is_v6()) {
			boost::asio::ip::address_v6::bytes_type bt = k.to_v6().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else {
			boost::hash_combine(seed, k.to_string());
		}

		return seed;
	}
};


//ywoh diameter
//fragment key and hash
struct sctp_frag_key {
   boost::asio::ip::tcp::endpoint   src;
   boost::asio::ip::tcp::endpoint   dst;
   wui64 id;

   sctp_frag_key() { id = 0; }
   sctp_frag_key(const sctp_frag_key& k) { src = k.src; dst = k.dst; id = k.id; }
   sctp_frag_key(boost::asio::ip::tcp::endpoint& s, boost::asio::ip::tcp::endpoint& d, wui16 i) {
      src = s; dst = d; id = (wui64)i;
   }
   sctp_frag_key(boost::asio::ip::tcp::endpoint& s, boost::asio::ip::tcp::endpoint& d, wui32 i) {
      src = s; dst = d; id = (wui64)i;
   }
   sctp_frag_key(boost::asio::ip::tcp::endpoint& s, boost::asio::ip::tcp::endpoint& d, wui64 i) {
      src = s; dst = d; id = i;
   }
   sctp_frag_key(wui32 s, wui32 d, wui16 sp, wui16 dp, wui16 i) { set(s,d,sp,dp,i); }
   sctp_frag_key(wuc8* s, wuc8* d, wui16 sp, wui16 dp, wui32 i) { set(s,d,sp,dp,i); }

   void set(wui32 s, wui32 d, wui16 sp, wui16 dp, wui16 i) {
      src = get_ep(s, sp); 
      dst = get_ep(d,dp);
      id = (wui64)i;
   }
   void set(wuc8* s, wuc8* d, wui16 sp, wui16 dp, wui32 i) {
      src = get_ep(s,sp); dst = get_ep(d,dp); id = (wui64)i;
   }
   void set(wuc8* s, wuc8* d, wui16 sp, wui16 dp, wui64 i) {
      src = get_ep(s,sp); dst = get_ep(d,dp); id = i;
   }
   void set(address& s, address& d, wui16 sp, wui16 dp, wui32 i ) {
      boost::asio::ip::tcp::endpoint se(s, sp);
      src = se;
      boost::asio::ip::tcp::endpoint de(d, dp);
      dst = de;
      id = i;
   }

   void set(ep& s, ep& d, wui16 i ) {
      src = s;
      dst = d;
      id = (wui64)i;
   }
   void set(ep& s, ep& d, wui32 i ) {
      src = s;
      dst = d;
      id = (wui64)i;
   }
   void set(ep& s, ep& d, wui64 i ) {
      src = s;
      dst = d;
      id = i;
   }

   bool operator<(const sctp_frag_key& k) const {
      if (src != k.src) return (src < k.src);
      if (dst != k.dst) return (dst < k.dst);
      return (id < k.id);
   }
   bool operator==(const sctp_frag_key& k) const {
      return ((src==k.src) && (dst==k.dst) && (id==k.id));
   }
   bool operator!=(const sctp_frag_key& k) const {
      return ((src!=k.src) || (dst!=k.dst) || (id!=k.id));
   }

   std::string to_string() {
      wc8   s[256];
      sprintf(s, "src=%s(%d),dst=%s(%d),id(%llu)",
            src.address().to_string().c_str(), src.port(),
            dst.address().to_string().c_str(), dst.port(), id );
      return std::string(s);
   }
};

struct sctp_frag_hash {
   std::size_t operator()(const sctp_frag_key& k) const {
      std::size_t seed = 0;

		if (k.src.address().is_v4()) {
			boost::asio::ip::address_v4::bytes_type bt = k.src.address().to_v4().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else if (k.src.address().is_v6()) {
			boost::asio::ip::address_v6::bytes_type bt = k.src.address().to_v6().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else {
			boost::hash_combine(seed, k.src.address().to_string());
		}
		boost::hash_combine(seed, k.src.port());

		if (k.dst.address().is_v4()) {
			boost::asio::ip::address_v4::bytes_type bt = k.dst.address().to_v4().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else if (k.dst.address().is_v6()) {
			boost::asio::ip::address_v6::bytes_type bt = k.dst.address().to_v6().to_bytes();
			for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
		}
		else {
			boost::hash_combine(seed, k.dst.address().to_string());
		}
		boost::hash_combine(seed, k.dst.port());

      boost::hash_combine(seed, k.id);
      return seed;
   }
};

struct ip_frag_key {
   boost::asio::ip::address   src;
   boost::asio::ip::address   dst;
   
   wui32 id;

   ip_frag_key() { id = 0; }
   ip_frag_key(const ip_frag_key& k) { src = k.src; dst = k.dst; id = k.id; }
   ip_frag_key(boost::asio::ip::address& s, boost::asio::ip::address& d, wui16 i) {
      src = s; dst = d; id = (wui32)i;
   }
   ip_frag_key(boost::asio::ip::address& s, boost::asio::ip::address& d, wui32 i) {
      src = s; dst = d; id = i;
   }
   ip_frag_key(wui32 s, wui32 d, wui16 i) { set(s,d,i); }
   ip_frag_key(wuc8* s, wuc8* d, wui32 i) { set(s,d,i); }
   void set(wui32 s, wui32 d, wui16 i) {
      src = get_addr(s); dst = get_addr(d); id = (wui32)i;
   }
   void set(wuc8* s, wuc8* d, wui32 i) {
      src = get_addr(s); dst = get_addr(d); id = i;
   }
   void set(address& s, address& d, wui32 i) {
      src = s;
      dst = d;
      id = i;
   }
   bool operator<(const ip_frag_key& k) const {
      if (src != k.src) return (src < k.src);
      if (dst != k.dst) return (dst < k.dst);
      return (id < k.id);
   }
   bool operator==(const ip_frag_key& k) const {
      return ((src==k.src) && (dst==k.dst) && (id==k.id));
   }
   std::string to_string() {
      wc8   s[256];
      sprintf(s, "src=%s,dst=%s,id=0x%08x", src.to_string().c_str(), dst.to_string().c_str(), id);
      return std::string(s);
   }
};

struct ip_frag_hash {
   std::size_t operator()(const ip_frag_key& k) const {
      std::size_t seed = 0;

      if (k.src.is_v4()) {
         boost::asio::ip::address_v4::bytes_type bt = k.src.to_v4().to_bytes();
         for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
      }
      else {
         boost::asio::ip::address_v6::bytes_type bt = k.src.to_v6().to_bytes();
         for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
      }

      if (k.dst.is_v4()) {
         boost::asio::ip::address_v4::bytes_type bt = k.dst.to_v4().to_bytes();
         for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
      }
      else {
         boost::asio::ip::address_v6::bytes_type bt = k.dst.to_v6().to_bytes();
         for (size_t i=0; i<bt.size(); ++i) boost::hash_combine(seed, bt[i]);
      }

      boost::hash_combine(seed, k.id);
      return seed;
   }
};

//unsigned long long hash
struct wui64_key {
   wui64 imsi;

   wui64_key() { imsi = 0; }

   std::string to_string() {
      wc8 tmp[1024];
      memset(tmp, 0x00, sizeof(tmp));
      sprintf( tmp, "imsi[%llu] ", imsi ) ;
      std::string rtn(tmp);
      return rtn;
   }

   bool operator<(const wui64_key& k) const {
      return (imsi < k.imsi);
   }
   bool operator==(const wui64_key& k) const {
      return ( imsi == k.imsi );
   }
};

struct wui64_hash {
	std::size_t operator()(const wui64_key& k) const {
		std::size_t seed = 0;

		boost::hash_combine(seed, k.imsi);
		return seed;
	}
};
#endif



#endif	// _com_h_
