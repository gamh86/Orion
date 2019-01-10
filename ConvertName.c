ConvertNumberToE164.c                                                                               0000600 0001750 0001750 00000001523 13361263617 013337  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
ConvertNumberToE164(uc *target, uc *number, size_t *targetlen)
{
	if (target == NULL || number == NULL || targetlen == NULL)
	  {
		errno = EINVAL;
		fprintf(stderr, "ConvertNumberToE164: %s\r\n", strerror(errno));
		errno = EINVAL;
		return(-1);
	  }

	static uc *tmp = NULL, *p = NULL, *t = NULL;
	static size_t numlen;
	static int k;

	if (!(tmp = (uc *)calloc_e(tmp, 256, sizeof(char))))
		goto __err;

	numlen = strlen(number);
	p = (number + (numlen - 1));
	t = tmp;

	while (p >= number)
	  {
		*t++ = *p--;
		*t++ = 0x2e;
	  }

	sprintf(t, "%s", "e164.arpa");
	t += strlen("e164.arpa");
	*t = 0;

	if (strncpy(target, tmp, strlen(tmp)) == NULL)
		{ perror("ConvertNumberToE164"); goto __err; }

	*targetlen = strlen(tmp);

	if (tmp != NULL) free(tmp);
	return(0);

	__err:
	if (tmp != NULL) free(tmp);
	return(-1);
}
                                                                                                                                                                             ConvertToPtr6.c                                                                                     0000600 0001750 0001750 00000003124 13361263617 012401  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
ConvertToPtr6(uc *out, uc *in, size_t *strptr_len)
{
	if (in == NULL || out == NULL || strptr_len == NULL)
	  {
		errno = EINVAL;
		return(-1);
	  }

	static uc *tmp = NULL, *p1 = NULL, *p2 = NULL, *t = NULL;
	static int k;
	static int err;
	static size_t len;
	if (!(tmp = (uc *)calloc_e(tmp, 73, sizeof(uc))))
		goto __err;
	len = strlen((uc *)in);
	k = 0;
	/*2a03:2880:f12a:183:face:b00c:0:25de*/
	/* 		 ^  ^
	*/
	/* add in zeros where needed */
	p1 = in; p2 = in; t = tmp;

	for(;;)
	  {
		k = 0;
		while (*p1 != ':' && p1 < (in + len))
			++p1;
		if (*p1 != ':' && p1 != (in + len))
		  {
			errno = EPROTO;
			goto __err;
		  }
		if (p1 == (in + len) && p2 == (in + len))
			break;
		if ((p1 - p2) < 4)
		  {
			while (k < (4 - (p1 - p2)))
				{ *t++ = 0x30; ++k; }
			while (p2 != p1)
				*t++ = *p2++;
			if (p1 != (in + len))
				{ *t++ = *p1++; ++p2; }
		  }
		else
		  {
			while (p2 != p1)
				*t++ = *p2++;
			if (p1 != (in + len))
				{ *t++ = *p1++; ++p2; }
		  }
	  }
	
	*t = 0; /* null terminate */
	*strptr_len = strlen((uc *)tmp);
	t = (tmp + (strlen((uc *)tmp)-1));
	p1 = out;
	while (t >= tmp)
	  {
		if (*t == ':')
			--t;
		*p1++ = *t--;
		*p1++ = '.';
	  }
	t = tmp;
	if (strncpy((uc *)p1, "ip6-arpa.", 9) == NULL)
		{ perror("ConvertToPtr: strncpy "); goto __err; }
	p1 = (p1 + 9);
	*p1 = 0;
	memset(tmp, 0, 73);
	memcpy(tmp, out, strlen((uc *)out));
	memset(out, 0, strlen((uc *)out));
	if (ConvertName(out, tmp, strptr_len) == -1)
		goto __err;
	if (tmp != NULL) free(tmp);
	return(0);

	__err:
	err = errno;
	if (tmp != NULL) free(tmp);
	errno = err;
	return(-1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                            ConvertToPtr.c                                                                                      0000600 0001750 0001750 00000003103 13361263617 012310  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
ConvertToPtr(uc *name, uc *host, size_t *qnamelen)
{
	if (name == NULL || host == NULL)
	  {
		errno == EINVAL;
		return(-1);
	  }

	static int err = 0, i = 0, hostmax = 0;
	uc _1[4], _2[4], _3[4], _4[4];
	uc *ptr = NULL, *tmp = NULL;
	size_t hlen = strlen((uc *)host);

	if ((hostmax = sysconf(_SC_HOST_NAME_MAX)) == 0)
		hostmax = 256;
	if (hlen >= hostmax)
	  { errno = ENAMETOOLONG; goto __err; }
	if ((tmp = (uc *)calloc_e((uc *)tmp, hostmax, sizeof(uc))) == NULL)
		goto __err;

	host[hlen++] = '.';
	ptr = host;
	while (*ptr != '.' && ptr < (host+hlen))
		_1[i++] = *ptr++;
	_1[i] = 0;
	i = 0;
	++ptr;
	while (*ptr != '.' && ptr < (host+hlen))
		_2[i++] = *ptr++;
	_2[i] = 0;
	i = 0;
	++ptr;
	while (*ptr != '.' && ptr < (host+hlen))
		_3[i++] = *ptr++;
	_3[i] = 0;
	i = 0;
	++ptr;
	while (*ptr != '.' && ptr < (host+hlen+1))
		_4[i++] = *ptr++;
	_4[i] = 0;
	i = 0;
	snprintf((uc *)tmp, hostmax, "%s.%s.%s.%s.in-addr.arpa", _4, _3, _2, _1);
	size_t tlen = strlen((uc *)_1)+1+strlen((uc *)_2)+1+strlen((uc *)_3)+1+strlen((uc *)_4)+1+strlen("in-addr.arpa");
	tmp[tlen++] = '.';
	tmp[tlen] = 0;
	int cnt = 0, start = 0, nidx = 0, k = 0;
	size_t len = strlen((uc *)tmp);
/* 31.29.182.80.in-addr.arpa
 *
 */
	for (i = 0; i < len; ++i)
	  {
		if (tmp[i] == '.')
		  {
			start = (i-cnt);
			name[nidx++] = cnt;
			for (k = 0; k < cnt; ++k)
				name[nidx++] = tmp[start++];
			cnt = 0;
			continue;
		  }
		++cnt;
	  }
	name[nidx] = 0;
	*qnamelen = nidx;
	if (tmp != NULL) free(tmp);
	return(0);

	__err:
	err = errno;
	if (tmp != NULL) free(tmp);
	errno = err;
	return(-1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                             DHCP_GetNS.c                                                                                        0000600 0001750 0001750 00000006611 13361367121 011457  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

static sigjmp_buf __ALARM_ENV__;

static void
catch_sigalarm(int signo)
{
	if (signo != SIGALRM)
		return;
	fprintf(stderr, "\e[3;31mrequest timed out\e[m\r\n");
	siglongjmp(__ALARM_ENV__, 1);
}

ssize_t
DHCP_GetNS(uc *ns)
{
	static DHCP_MSG *dhcp = NULL;
	static uc *buf = NULL, *p = NULL;
	static int s, opt;
	static struct ifreq ifr;
	static struct ether_addr eaddr;
	static struct sockaddr_in s4;
	static struct in_addr *in4;
	static uc *ether_str = NULL, ipv4_str[INET_ADDRSTRLEN];
	static size_t tosend;
	static struct sigaction n_act;

	if (ns == NULL)
	  {
		errno = EINVAL;
		return(-1);
	  }

	memset(&n_act, 0, sizeof(n_act));
	sigemptyset(&n_act.sa_mask);
	sigaddset(&n_act.sa_mask, SIGINT);
	sigaddset(&n_act.sa_mask, SIGQUIT);
	n_act.sa_flags = 0;
	n_act.sa_handler = catch_sigalarm;
	if (sigaction(SIGALRM, &n_act, NULL) < 0)
		{ perror("DHCP_GetNS: failed to set signal handler for SIGALRM"); goto __err; }
	
	if (!(buf = (uc *)calloc_e(buf, BUFSIZ, sizeof(uc))))
		goto __err;

	printf("\e[3;02mUsing DHCP to obtain primary/secondary DNS servers\e[m\r\n");
	memset(&s4, 0, sizeof(s4));
	dhcp = (DHCP_MSG *)buf;
	dhcp->op = htons(DHCP_REQUEST);
	dhcp->htype = htons(DHCP_HTYPE_ETH);
	dhcp->hlen = htons(ETH_ALEN);
	dhcp->hops = 0;
	dhcp->transaction_id = htons(getpid());
	dhcp->seconds = 10;
	dhcp->flags &= ~dhcp->flags;
	dhcp->your_ipv4 &= ~dhcp->your_ipv4;
	dhcp->server_ipv4 &= ~dhcp->server_ipv4;
	dhcp->router_ipv4 &= ~dhcp->router_ipv4;
	memset(dhcp->server_hname, 0, 64);
	memset(dhcp->boot_filename, 0, 128);
	p = &buf[sizeof(DHCP_MSG)];
	*p++ = 0x6; /* DNS server option */
	*p++ = 0xff; /* End of DHCP options marker */
	*p = 0; /* NULL terminate */
	s4.sin_family = AF_INET;
	s4.sin_port = htons(67);
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, "wlp2s0", 6);

	printf("\e[3;00mSending DHCP request for local DNS server\e[m\r\n");
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		{ perror("DHCP_GetNS: socket"); goto __err; }

	/* get hardware address */
	if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0)
		{ perror("DHCP_GetNS: ioctl"); goto __err; }
	memcpy(dhcp->client_hwd, ifr.ifr_ifru.ifru_hwaddr.sa_data, sizeof(eaddr));
	
	/* get ip address */
	if (ioctl(s, SIOCGIFADDR, &ifr) < 0)
		{ perror("DHCP_GetNS: ioctl"); goto __err; }
	memcpy(&s4, &ifr.ifr_ifru.ifru_addr, sizeof(s4)); 
	memcpy(&dhcp->client_ipv4, &s4.sin_addr.s_addr, sizeof(s4.sin_addr.s_addr));

	/* get broadcast address */
	if (ioctl(s, SIOCGIFBRDADDR, &ifr) < 0)
		{ perror("DHCP_GetNS: ioctl"); goto __err; }
	if (memcpy(&s4, &ifr.ifr_ifru.ifru_broadaddr, sizeof(s4)) == NULL)
		{ perror("DHCP_GetNS: memcpy"); goto __err; }

	opt = 1;
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &opt, (socklen_t)sizeof(opt)) < 0)
		{ perror("DHCP_GetNS: socksetopt"); goto __err; }
	tosend = (sizeof(DHCP_MSG)+2);
	if (sendto_a(s, buf, tosend, 0, (struct sockaddr *)&s4, (socklen_t)sizeof(s4)) == -1)
		goto __err;
	if (sigsetjmp(__ALARM_ENV__, 1) != 0)
		goto __err;
	alarm(15);
	if (recv(s, buf, BUFSIZ, 0) == -1)
		goto __err;
	alarm(0);
	p = &buf[sizeof(DHCP_MSG)];
	++p;
	in4 = (struct in_addr *)p;
	in4->s_addr = ntohl(in4->s_addr);
	if (inet_ntop(AF_INET, &in4->s_addr, ipv4_str, INET_ADDRSTRLEN) == NULL)
		{ perror("inet_ntop"); goto __err; }
	printf("\e[3;02mfound DNS server @%s\e[m\r\n", ipv4_str);
	memcpy(ns, ipv4_str, INET_ADDRSTRLEN);
	goto __err; /* for testing */

	__err:
	if (buf != NULL) free(buf);
	return(-1);
	
}
                                                                                                                       dns.h                                                                                               0000600 0001750 0001750 00000012624 13362122143 010465  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#undef BUFSIZ
#define BUFSIZ 65536

#define SECS_PER_WEEK (60*60*24*7)
#define TTL_OK(x) ((x) < SECS_PER_WEEK)

#define LOCAL_DNS "127.0.1.1"
#define LOCAL_DNS2 "127.0.0.1"
#define GOOGLE_DNS "8.8.8.8"
#define GOOGLE_DNS2 "8.8.4.4"

typedef unsigned char uc;
typedef unsigned int ui;
typedef unsigned short us;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef volatile sig_atomic_t _atomic_;

int STALE_OK;

enum QTYPE
{
	QTYPE_A = 1,
	QTYPE_NS = 2,
	QTYPE_CNAME = 5,
	QTYPE_SOA = 6,
	QTYPE_PTR = 12,
	QTYPE_MX = 15,
	QTYPE_TXT = 16,
	QTYPE_RP = 17,
	QTYPE_AFSDB = 18,
	QTYPE_SIG = 24,
	QTYPE_KEY = 25,
	QTYPE_AAAA = 28,
	QTYPE_LOC = 29,
	QTYPE_SRV = 33,
	QTYPE_NAPTR = 35,
	QTYPE_KX = 36,
	QTYPE_CERT = 37,
	QTYPE_DNAME = 39,
	QTYPE_OPT = 41,
	QTYPE_APL = 42,
	QTYPE_DS = 43,
	QTYPE_SSHFP = 44,
	QTYPE_IPSECKEY = 45,
	QTYPE_RRSIG = 46,
	QTYPE_NSEC = 47,
	QTYPE_DNSKEY = 48,
	QTYPE_DHCID = 49,
	QTYPE_NSEC3 = 50,
	QTYPE_NSEC3PARAM = 51,
	QTYPE_TLSA = 52,
	QTYPE_HIP = 55,
	QTYPE_CDS = 59,
	QTYPE_CDNSKEY = 60,
	QTYPE_OPENPGPGKEY = 61,
	QTYPE_TKEY = 249,
	QTYPE_TSIG = 250, /* ttl = 0; class = any; */
	QTYPE_IXFR = 251,
	QTYPE_AXFR = 252,
	QTYPE_ANY = 255,
	QTYPE_URI = 256,
	QTYPE_TA = 32768,
	QTYPE_DLV = 32769
};

typedef enum QTYPE DNS_QTYPE;

enum QCLASS
{
	QCLASS_INET = 1,
	QCLASS_CHAOS = 3,
	QCLASS_HESIOD = 4,
	QCLASS_NONE = 254,
	QCLASS_ALL = 255
};

typedef enum QCLASS DNS_QCLASS;

struct HEADER
{
	us ident;
#if __BYTE_ORDER == __LITTLE_ENDIAN
	us rd:1; /* recursion desired */
	us tc:1; /* response truncated */
	us aa:1; /* authoritative answers */
	us opcode:4;
	us qr:1; /* query / response */
	us rcode:4;
	us cd:1;  /* checking disabled */
	us ad:1; /* authentic data */
	us z:1; /* reserved; zero */
	us ra:1; /* recursion available */
#elif __BYTE_ORDER == __BIG_ENDIAN
	us qr:1;
	us opcode:4;
	us aa:1;
	us tc:1;
	us rd:1;
	us ra:1;
	us z:1;
	us ad:1;
	us cd:1;
	us rcode:4;
#else
# error "please adjust <bits/endian.h>"
#endif
	us qdcnt;
	us ancnt;
	us nscnt;
	us arcnt;
};

typedef struct HEADER DNS_HEADER;

struct QUESTION
{
	us qtype;
	us qclass;
};

typedef struct QUESTION DNS_QUESTION;

struct QUERY
{
	uc *name;
	struct QUESTION *q;
};

typedef struct QUERY DNS_QUERY;

struct RDATA
{
	us type;
	us _class;
	u32 ttl;
	us len;
} __attribute__ ((__packed__));

typedef struct RDATA DNS_RDATA;

struct RRECORD
{
	uc *name;
	struct RDATA *resource;
	uc *rdata;
};

typedef struct RRECORD DNS_RRECORD;

struct NAPTR_DATA
{
	us order;
	us pref;
	uc *flags;
	uc *services;
	uc *regex;
	uc *replace;
} __attribute__ ((__packed__));

typedef struct NAPTR_DATA NAPTR_DATA;



/* __START_FUNC_DECS__ */
ssize_t ConvertName(uc *, uc *, size_t *) __THROW __nonnull ((1,2,3)) __wur;
ssize_t ConvertToPtr(uc *, uc *, size_t *) __THROW __nonnull ((1,2,3)) __wur;
ssize_t ConvertToPtr6(uc *, uc *, size_t *) __THROW __nonnull ((1,2,3)) __wur;
ssize_t ConvertNumberToE164(uc *, uc *, size_t *) __THROW __nonnull ((1,2,3)) __wur;
ssize_t GetName(uc *, uc *, uc *, size_t *) __THROW __nonnull ((1,2,3,4)) __wur;
ssize_t GetAnswers(DNS_RRECORD[], u16, uc *, _atomic_) __THROW __nonnull ((1,3)) __wur;
ssize_t GetRecords(DNS_RRECORD[], u16, uc *, uc *, size_t, size_t *)
	__THROW __nonnull ((1,3,4,6)) __wur;
uc *GetQClass(us) __THROW __wur;
uc *GetQType(us) __THROW __wur;
uc *GetOpCode(us) __THROW __wur;
uc *GetRCode(us) __THROW __wur;
ssize_t DoQuery(uc *, uc *, _atomic_, _atomic_) __THROW __nonnull ((1,2)) __wur;
void Usage(void) __attribute__ ((__noreturn__));
ssize_t ResolveQType(const uc *, _atomic_ *) __THROW __nonnull ((1,2)) __wur;
ssize_t PrintInfoDNS(uc *, int, u16, uc *) __THROW __nonnull ((1,4)) __wur;
ssize_t DoTCP(uc *, size_t, uc *) __THROW __nonnull ((1,3)) __wur;
ssize_t DoUDP(uc *, size_t, uc *) __THROW __nonnull ((1,3)) __wur;
ssize_t HandleNAPTRrecord(DNS_RRECORD *) __THROW __nonnull ((1)) __wur;
/* __END_FUNC_DECS__ */

/*wrappers*/
void *calloc_e(void *, size_t, size_t) __attribute__ ((alloc_size(2,3)));
void *malloc_e(void *, size_t) __attribute__ ((alloc_size(2)));
int socket_e(int, int, int, int);
/*a == 'all' */
ssize_t send_a(int, uc *, size_t, int) __THROW __nonnull ((2)) __wur;
ssize_t recv_a(int, uc *, size_t, int) __THROW __nonnull ((2)) __wur;
ssize_t sendto_a(int, uc *, size_t, int, struct sockaddr *, socklen_t) __THROW __nonnull ((2)) __wur;
ssize_t recvfrom_a(int, uc *, size_t, int, struct sockaddr *, socklen_t *) __THROW __nonnull ((2,5,6)) __wur;


/* DHCP stuff to get the primary and secondary DNS servers */

struct DHCP_MSG
{
	u8 op;
	u8 htype;
	u8 hlen;
	u8 hops;
	u32 transaction_id;
	u16 seconds;
	u16 flags;
	u32 client_ipv4;
	u32 your_ipv4;
	u32 server_ipv4;
	u32 router_ipv4;
	u8 client_hwd[16];
	u8 server_hname[64];
	u8 boot_filename[128];
// options
};

#define DHCP_REQUEST 1
#define DHCP_REPLY 2
#define DHCP_HTYPE_ETH 1
#define DHCP_HLEN_ETH 6

#define DHCP_MAGIC_COOKIE 0x63825363
#define DHCP_OPTION_SUBNET 1
#define DHCP_OPTION_ROUTER 3
#define DHCP_OPTION_DNS_SERVER 6
#define DHCP_OPTION_DNS 15

typedef struct DHCP_MSG DHCP_MSG;

ssize_t DHCP_GetNS(uc *) __THROW __nonnull ((1)) __wur;
                                                                                                            DoQuery.c                                                                                           0000600 0001750 0001750 00000010101 13361364776 011273  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
DoQuery(uc *host, uc *ns, _atomic_ q_type, _atomic_ q_class)
{
	static u16 transaction_id;
	static uc *buf = NULL, *qname = NULL, *p = NULL, *e164 = NULL;
	static int i, err, z, __USED_TCP = 0, hostmax = 0;
	static DNS_RRECORD answers[20], auth[20], addit[20];
	static DNS_HEADER *dns = NULL;
	static DNS_QUESTION *q = NULL;
	static struct timespec t1, t2;
	static size_t qnamelen, tosend, trcvd, delta;
	static double diff;
	static int V6_PTR = 0;

	if ((hostmax = sysconf(_SC_HOST_NAME_MAX)) == 0)
		hostmax = 256;
	if (!(buf = calloc_e(buf, BUFSIZ, sizeof(uc))))
		goto __err;
	for (z = 0; z < 20; ++z)
	  {
		answers[z].name = NULL;
		answers[z].rdata = NULL;
		auth[z].name = NULL;
		auth[z].rdata = NULL;
		addit[z].name = NULL;
		addit[z].rdata = NULL;
	  }

	dns = (DNS_HEADER *)buf;
	qname = &buf[sizeof(DNS_HEADER)];
	dns->ident = htons(getpid());
	transaction_id = ntohs(dns->ident);
	dns->qr = 0;
	dns->opcode = 0;
	dns->aa = 0;
	dns->tc = 0;
	dns->rd = 1;
	dns->ra = 0;
	dns->z = 0;
	dns->ad = 0;
	dns->cd = 1;
	dns->rcode = 0;
	dns->qdcnt = htons(1);
	dns->ancnt = 0;
	dns->nscnt = 0;
	dns->arcnt = 0;
	if (q_type == 12) /* PTR record */
	  {
		for (i = 0; i < strlen(host); ++i)
		  {
			if (isalpha(host[i]))
				{ V6_PTR = 1; break; }
		  }
		if (!V6_PTR)
		  {
			if (ConvertToPtr(qname, host, &qnamelen) == -1)
		  	  {
				goto __err;
		  	  }
		
		  }
		else
		  {
			if (ConvertToPtr6(qname, host, &qnamelen) == -1)
			  {
				goto __err;
			  }
		  }
	  }
	else if (q_type == 35) /* NAPTR record */
	  {
		if (!(e164 = (uc *)calloc_e(e164, hostmax, sizeof(char))))
			goto __err;
		if (ConvertNumberToE164(e164, host, &qnamelen) == -1)
			{ perror("DoQuery: ConvertNumberToE164"); goto __err; }
		if (ConvertName(qname, e164, &qnamelen) == -1)
			{ perror("DoQuery: ConvertName"); goto __err; }
	  }
	else
	  {
		if (ConvertName(qname, host, &qnamelen) == -1)
		  {
			goto __err;
		  }
	  }

	q = (DNS_QUESTION *)&buf[sizeof(DNS_HEADER)+qnamelen+1];
	q->qtype = htons(q_type);
	q->qclass = htons(q_class);

	tosend = (sizeof(DNS_HEADER)+qnamelen+1+sizeof(DNS_QUESTION));
	trcvd = 0;
	memset(&t1, 0, sizeof(t1));
	memset(&t2, 0, sizeof(t2));
	if (clock_gettime(CLOCK_REALTIME, &t1) < 0)
		{ perror("doquery: clock_gettime"); goto __err; }
	if (q_type == 252) /* axfr */
	  {
		if ((trcvd = DoTCP(buf, tosend, ns)) == -1)
			goto __err;
		__USED_TCP = 1;
	  }
	else
	  {
		if ((trcvd = DoUDP(buf, tosend, ns)) == -1)
			goto __err;
	  }
	if (clock_gettime(CLOCK_REALTIME, &t2) < 0)
		{ perror("doquery: clock_gettime"); goto __err; }
	diff = ((double)(((double)t2.tv_nsec/1000000) - ((double)t1.tv_nsec/1000000)));

	printf("\e[3;02mDNS Response (%3.2lf ms; Protocol %s)\e[m\r\n",
			diff,
			(__USED_TCP==1?"TCP":"UDP"));

	dns = (DNS_HEADER *)buf;
	p = buf;
	if (PrintInfoDNS(p, 1, transaction_id, ns) == -1)
		{ perror("PrintInfoDNS"); goto __err; }


			/* Extract the DNS Records */

	p = &buf[sizeof(DNS_HEADER)+qnamelen+1+sizeof(DNS_QUESTION)];
	if (ntohs(dns->ancnt) > 0)
	  {
		if (GetRecords(answers, ntohs(dns->ancnt), p, buf, trcvd, &delta) == -1)
			{ perror("GetRecords"); goto __err; }
		p += delta;
	  }
	if (ntohs(dns->nscnt) > 0)
	  {
		if (GetRecords(auth, ntohs(dns->nscnt), p, buf, trcvd, &delta) == -1)
			goto __err;
		p += delta;
	  }
	if (ntohs(dns->arcnt) > 0)
	  {
		if (GetRecords(addit, ntohs(dns->arcnt), p, buf, trcvd, &delta) == -1)
			goto __err;
		p += delta;
	  }

			/* Print the DNS Resource Records */

	if (ntohs(dns->ancnt) > 0)
	  {
		printf("\e[1;02m\e[3;32m\tANSWER RESOURCE RECORDS\e[m\r\n");
		if (GetAnswers(answers, ntohs(dns->ancnt), buf, q_type) == -1)
			goto __err;
	  }
	if (ntohs(dns->nscnt) > 0)
	  {
		printf("\e[1;02m\e[3;32m\tAUTHORITATIVE RESOURCE RECORDS\e[m\r\n");
		if (GetAnswers(auth, ntohs(dns->nscnt), buf, q_type) == -1)
			goto __err;
	  }
	if (ntohs(dns->arcnt) > 0)
	  {
		printf("\e[1;02m\e[3;32m\tADDITIONAL RESOURCE RECORDS\e[m\r\n");
		if (GetAnswers(addit, ntohs(dns->arcnt), buf, q_type) == -1)
			goto __err;
	  }

	if (buf != NULL) free(buf);
	return(0);

	__err:
	err = errno;
	if (buf != NULL) free(buf);
	errno = err;
	return(-1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                               DoTCP.c                                                                                             0000600 0001750 0001750 00000003573 13361365014 010615  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

static sigjmp_buf __ALARM_ENV__;

static void
catch_sigalarm(int signo)
{
	if (signo != SIGALRM)
		return;
	fprintf(stderr, "\e[3;31mrequest timed out\e[m\r\n");
	siglongjmp(__ALARM_ENV__, 1);
}

ssize_t
DoTCP(uc *buf, size_t size, uc *ns)
{
	if (buf == NULL || size <= 0 || ns == NULL)
	  {
		errno = EINVAL;
		return(-1);
	  }

	static struct sockaddr_in s4;
	static struct sigaction n_act;
	static ssize_t ret = 0;
	static int s;
	static char tstring[50];
	static time_t seed;
	static struct tm *_time;

	memset(&n_act, 0, sizeof(n_act));
	n_act.sa_handler = catch_sigalarm;
	sigemptyset(&n_act.sa_mask);
	sigaddset(&n_act.sa_mask, SIGINT);
	sigaddset(&n_act.sa_mask, SIGQUIT);
	n_act.sa_flags = 0;
	if (sigaction(SIGALRM, &n_act, NULL) < 0)
		{ perror("do_tcp: failed to set signal handler for SIGALRM"); goto __err; }
	memset(&s4, 0, sizeof(s4));
	s4.sin_family = AF_INET;
	s4.sin_port = htons(53);
	if (inet_pton(AF_INET, ns, &s4.sin_addr.s_addr) < 0)
		{ perror("do_tcp: inet_pton"); goto __err; }
	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		{ perror("do_tcp: socket"); goto __err; }
	if (sigsetjmp(__ALARM_ENV__, 0) != 0)
		goto __err;
	alarm(10);
	if (connect(s, (struct sockaddr *)&s4, (socklen_t)sizeof(s4)) != 0)
		{ perror("do_tcp: connect"); goto __err; }
	alarm(0);
	seed = time(NULL);
	if ((_time = localtime(&seed)) == NULL)
		goto __err;
	if (strftime(tstring, 30, "%a %d %b %Y %H:%M:%S", _time) < 0)
		goto __err;
	printf("\r\n\e[3;02mDNS Query (sent %s [TZ %s])\e[m\r\n", tstring, _time->tm_zone);
	if (PrintInfoDNS(buf, 0, getpid(), ns) == -1)
		goto __err;
	alarm(10);
	if ((ret = send_a(s, buf, size, 0)) == -1)
		{ perror("do_tcp: send_a"); goto __err; }
	alarm(0);
	alarm(10);
	if ((ret = recv_a(s, buf, BUFSIZ, 0)) == -1)
		{ perror("do_tcp: recv_a"); goto __err; }
	alarm(0);
	shutdown(s, SHUT_RDWR);
	buf[ret] = 0;
	return(ret);

	__err:
	return(-1);
}
                                                                                                                                     DoUDP.c                                                                                             0000600 0001750 0001750 00000003361 13361364746 010625  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

static sigjmp_buf __ALARM_ENV__;

static void
catch_sigalarm(int signo)
{
	if (signo != SIGALRM)
		return;
	fprintf(stderr, "\e[3;31mrequest timed out\e[m\r\n");
	siglongjmp(__ALARM_ENV__, 1);
}

ssize_t
DoUDP(uc *buf, size_t size, uc *ns)
{
	if (buf == NULL || size <= 0 || size > 512 || ns == NULL)
	  {
		errno = EINVAL;
		return(-1);
	  }

	static struct sockaddr_in s4;
	static struct sigaction n_act;
	static ssize_t ret = 0;
	static int s;
	static time_t seed;
	static struct tm *_time;
	static char tstring[50];

	memset(&n_act, 0, sizeof(n_act));
	n_act.sa_handler = catch_sigalarm;
	n_act.sa_flags = 0;
	sigemptyset(&n_act.sa_mask);
	sigaddset(&n_act.sa_mask, SIGINT);
	sigaddset(&n_act.sa_mask, SIGQUIT);
	if (sigaction(SIGALRM, &n_act, NULL) < 0)
		{ perror("do_udp: failed to set signal handler for SIGALRM"); goto __err; }
	memset(&s4, 0, sizeof(s4));
	s4.sin_family = AF_INET;
	s4.sin_port = htons(53);
	if (inet_pton(AF_INET, ns, &s4.sin_addr.s_addr) < 0)
		{ perror("do_udp: inet_ntop"); goto __err; }
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		{ perror("do_udp: socket"); goto __err; }
	seed = time(NULL);
	if ((_time = localtime(&seed)) == NULL)
		goto __err;
	if (strftime(tstring, 30, "%a %d %b %Y %H:%M:%S", _time) < 0)
		goto __err;
	printf("\r\n\e[3;02mDNS Query (sent %s [TZ %s])\e[m\r\n", tstring, _time->tm_zone);
	if (PrintInfoDNS(buf, 0, getpid(), ns) == -1)
		goto __err;
	if (sendto_a(s, buf, size, 0, (struct sockaddr *)&s4, (socklen_t)sizeof(s4)) == -1)
		{ perror("do_udp: sendto_a"); goto __err; }
	if (sigsetjmp(__ALARM_ENV__, 1) != 0)
		goto __err;
	alarm(10);
	if ((ret = recv(s, buf, 512, 0)) < 0)
		{ perror("do_udp: recv"); goto __err; }
	alarm(0);
	buf[ret] = 0;
	return(ret);

	__err:
	return(-1);
}
                                                                                                                                                                                                                                                                               GetAnswers.c                                                                                        0000600 0001750 0001750 00000015602 13362122232 011754  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
GetAnswers(DNS_RRECORD ans_array[], u16 acnt, uc *buf, _atomic_ q_type)
{
	if (ans_array == NULL || acnt <= 0 || buf == NULL)
	  {
		errno = EINVAL;
		return(-1);
	  }

	static int k, hostmax = 0, err;
	static uc *string = NULL, *domain = NULL, *mailn = NULL;
	static char a128_str[INET6_ADDRSTRLEN], *a32_str = NULL;
	static struct in_addr *a32 = NULL;
	static struct in6_addr *a128 = NULL;
	static u16 *pref = NULL, *a16 = NULL;
	static u32 *serial = NULL, *refresh = NULL, *retry = NULL, *expire = NULL, *min = NULL;
	static uc *p = NULL;
	static size_t delta;

	if ((hostmax = sysconf(_SC_HOST_NAME_MAX)) == 0)
		hostmax = 256;
	if (!(string = (uc *)calloc_e(string, hostmax, sizeof(uc))))
		goto __err;
	if (!(domain = (uc *)calloc_e(domain, hostmax, sizeof(uc))))
		goto __err;
	if (!(mailn = (uc *)calloc_e(mailn, hostmax, sizeof(uc))))
		goto __err;

	for (k = 0; k < acnt; ++k)
	  {
		if (! TTL_OK(ntohl(ans_array[k].resource->ttl)))
		  {
			if (STALE_OK)
				goto __stale_ok;
			printf("\e[3;31mResource record is stale (ttl:\e[m %u\e[3;02m)\e[m\r\n",
					ntohl(ans_array[k].resource->ttl));
			free(ans_array[k].name); free(ans_array[k].rdata);
			ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			continue;
		  }
		__stale_ok:
		switch(ntohs(ans_array[k].resource->type))
		  {
			case(1): /* IP address */
			if (ntohs(ans_array[k].resource->_class) == 3) /* CHAOS */
			  {
				p = ans_array[k].rdata;
				if (GetName(p, buf, string, &delta) == -1)
					goto __err;
				p += delta;
				a16 = (u16 *)p;
				printf("\e[3;02m%18s\e[m %s [%u]\r\n"
					"%18s %0o\r\n",
					"Chaos Name",
					string,
					ntohl(ans_array[k].resource->ttl),
					"Chaos Address",
					*a16);
				p += sizeof(u16);
				free(ans_array[k].name); free(ans_array[k].rdata);
				ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			  }
			else
			  {
				p = ans_array[k].rdata;
				a32 = (struct in_addr *)p;
				if ((a32_str = inet_ntoa(*a32)) == NULL)
					{ perror("GetAnswers: inet_ntoa"); goto __err; }
				printf("\e[3;02m%18s\e[m %s [%u]\r\n",
					"IPv4 Address",
					a32_str,
					ntohl(ans_array[k].resource->ttl));
				free(ans_array[k].name); free(ans_array[k].rdata);
				ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			  }
			break;
			case(2):
			p = ans_array[k].rdata;
			if (GetName(p, buf, string, &delta) == -1)
				goto __err;
			p += delta;
			printf("\e[3;02m%18s\e[m %s [%u]\r\n",
				"Name Server",
				string,
				ntohl(ans_array[k].resource->ttl));
			free(ans_array[k].name); free(ans_array[k].rdata);
			ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			break;
			case(5): /* Canonical name */
			p = ans_array[k].rdata;
			if (GetName(p, buf, string, &delta) == -1)
				goto __err;
			p += delta;
			printf("\e[3;02m%18s\e[m %s [%u]\r\n",
				"Canonical Name",
				string,
				ntohl(ans_array[k].resource->ttl));
			free(ans_array[k].name); free(ans_array[k].rdata);
			ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			break;
			case(6): /* SOA - start of authority */
			p = ans_array[k].rdata;
			if (GetName(p, buf, domain, &delta) == -1)
				goto __err;
			p += delta;
			if (GetName(p, buf, mailn, &delta) == -1)
				goto __err;
			p += delta;
			serial = (u32 *)p;
			*serial = ntohl(*serial);
			p += sizeof(u32);
			refresh = (u32 *)p;
			*refresh = ntohl(*refresh);
			p += sizeof(u32);
			retry = (u32 *)p;
			*retry = ntohl(*retry);
			p += sizeof(u32);
			expire = (u32 *)p;
			*expire = ntohl(*expire);
			p += sizeof(u32);
			min = (u32 *)p;
			*min = ntohl(*min);
			p += sizeof(u32);
			printf("\e[3;02m%18s\e[m %s\r\n"
				"\e[3;02m%18s\e[m %s\r\n"
				"\e[3;02m%18s \e[m%u\r\n"
				"\e[3;02m%18s \e[m%d\r\n"
				"\e[3;02m%18s \e[m%d\r\n"
				"\e[3;02m%18s \e[m%d\r\n"
				"\e[3;02m%18s \e[m%d\r\n"
				"\e[3;02m%18s\e[m %u\r\n",
				"domain", domain, "mail", mailn,
				"S#", *serial,
				"REFR", (signed)*refresh,
				"RETR", (signed)*retry,
				"EXP", (signed)*expire,
				"MIN", (signed)*min,
				"TTL", ntohl(ans_array[k].resource->ttl));

			free(ans_array[k].name); free(ans_array[k].rdata);
			ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			break;
			case(12): /* PTR record */
			p = ans_array[k].rdata;
			if (GetName(p, buf, string, &delta) == -1)
				goto __err;
			p += delta;
			printf("\e[3;02m%18s\e[m %s [%u]\r\n",
				"Pointer",
				string,
				ntohl(ans_array[k].resource->ttl));
			free(ans_array[k].name); free(ans_array[k].rdata);
			ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			break;
			case(15): /* MX record */
			p = ans_array[k].rdata;
			pref = (u16 *)p;
			*pref = ntohs(*pref);
			p += sizeof(u16);
			if (GetName(p, buf, string, &delta) == -1)
				goto __err;
			p += delta;
			printf("\e[3;02m%18s\e[m %s (\e[3;02mpref\e[m %hu) [%u]\r\n",
				"Mail Exchange",
				string,
				*pref,
				ntohl(ans_array[k].resource->ttl));
			free(ans_array[k].name); free(ans_array[k].rdata);
			ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			break;
			case(16): /* txt record */
			printf("\e[3;02m%s\e[m\r\n", "Txt Record");
			p = ans_array[k].rdata;
			while (p < (ans_array[k].rdata + ntohs(ans_array[k].resource->len)))
			  {
				if (iscntrl(*p) && *p != '\r' && *p != '\n')
					++p;
				putchar(*p++);
			  } 
			putchar('\n');
			free(ans_array[k].name); free(ans_array[k].rdata);
			ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			break;
			case(28): /* AAAA record (ipv6) */
			p = ans_array[k].rdata;
			a128 = (struct in6_addr *)p;
			if (! inet_ntop(AF_INET6, &a128->s6_addr, a128_str, INET6_ADDRSTRLEN))
				{ perror("GetAnswers: inet_ntop"); goto __err; }
			p += ntohs(ans_array[k].resource->len);
			printf("\e[3;02m%18s\e[m %s [%u]\r\n",
				"IPv6 Address",
				a128_str,
				ntohl(ans_array[k].resource->ttl));
			break;
			case(35): /* NAPTR record */
			if (HandleNAPTRrecord(&ans_array[k]) == -1)
				{ perror("GetAnswers: HandleNAPTRrecord"); goto __err; }
			break;
			case(252): /* AXFR record */
			/*TODO*/
			break;
			case(256): /* URI record */
			p = ans_array[k].rdata;
			if (GetName(p, buf, string, &delta) == -1)
				goto __err;
			p += delta;
			printf("\e[3;02m%18s\e[m %s [%u]\r\n", 
				"URI",
				string,
				ntohl(ans_array[k].resource->ttl));
			break;
			default:
			if (ans_array[k].name != NULL) free(ans_array[k].name);
			if (ans_array[k].rdata != NULL) free(ans_array[k].rdata);
			ans_array[k].name = NULL; ans_array[k].rdata = NULL;
			goto __err;
		  }
	  }

	if (string != NULL) free(string);
	if (domain != NULL) free(domain);
	if (mailn != NULL) free(mailn);
	for (k = 0; k < acnt; ++k)
	  {
		if (ans_array[k].name != NULL) free(ans_array[k].name);
		if (ans_array[k].rdata != NULL) free(ans_array[k].rdata);
	  }
	return(0);

	__err:
	err = errno;
	if (string != NULL) free(string);
	if (domain != NULL) free(domain);
	if (mailn != NULL) free(mailn);
	for (k = 0; k < acnt; ++k)
	  {
		if (ans_array[k].name != NULL) free(ans_array[k].name);
		if (ans_array[k].rdata != NULL) free(ans_array[k].rdata);
	  }
	errno = err;
	return(-1);
}
                                                                                                                              GetName.c                                                                                           0000600 0001750 0001750 00000002235 13361313605 011215  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
GetName(uc *rcvd, uc *buf, uc *target, size_t *delta)
{
	static int i, offset;
	static size_t len, delt;
	static uc *start = NULL, *p = NULL;
	static _atomic_ jmp_fl;

/* blah3com0blahblahblahblahblahblahblah3www6google[192][4] */
	if (rcvd == NULL || buf == NULL || target == NULL || delta == NULL)
	  {
		errno = EINVAL;
		return(-1);
	  }

	start = (uc *)rcvd; jmp_fl = 0; offset = 0; len = 0; delt = 0;

	for (p = (uc *)rcvd; *p != 0; ++p)
	  {
		if (*p >= 0xc0) // >= 192
		  {
			offset = ((*p) * 256) + *(p+1) - (192*256);
			p = (uc *)(buf + offset);
			jmp_fl = 1;
			offset = 0;
		  }
		target[len++] = *p;
		if (!jmp_fl)
			++delt;
	  }
	if (jmp_fl == 1)
		{ p = (start + delt); ++p; }
	++p;
	*delta = (p - start);

	/* convert to normal format */
	for (i = 0; i < len; ++i)
	  {
		target[i] = target[i+1];
		if (!isalpha(target[i]) && !isdigit(target[i]) && target[i] != '-')
			target[i] = '.';
	  }
	i = 0;
	/*while (target[0] == '.')
	  {
		for (i = 0; i < (len-1); ++i)
			target[i] = target[i+1];
		--len;
		target[len] = 0;
	  }
	while (target[len-1] == '.')
	  {
		target[--len] = 0;
	  }*/
	target[len] = 0;
	jmp_fl = 0;
	return(0);
}
                                                                                                                                                                                                                                                                                                                                                                   GetOpCode.c                                                                                         0000600 0001750 0001750 00000000501 13361263617 011507  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

uc *
GetOpCode(unsigned short opcode)
{
	switch(opcode)
	  {
		case(0):
		return("standard");
		break;
		case(1):
		return("inverse");
		break;
		case(2):
		return("status");
		break;
		case(4):
		return("notify");
		break;
		case(5):
		return("update");
		break;
		default:
		return("unknown");
	  }
}
                                                                                                                                                                                               GetQClass.c                                                                                         0000600 0001750 0001750 00000000544 13361263617 011533  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

uc *
GetQClass(unsigned short qclass)
{
	switch(qclass)
	  {
		case(1):
		return("internet");
		break;
		case(QCLASS_CHAOS):
		return("chaos");
		break;
		case(QCLASS_HESIOD):
		return("hesiod");
		break;
		case(QCLASS_NONE):
		return("none");
		break;
		case(QCLASS_ALL):
		return("all");
		break;
		default:
		return("unknown");
	  }
}
                                                                                                                                                            GetQType.c                                                                                          0000600 0001750 0001750 00000003665 13361263617 011416  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

uc *
GetQType(unsigned short qtype)
{
	switch(qtype)
	  {
		case(1):
		return("A");
		break;
		case(2):
		return("NS");
		break;
		case(5):
		return("CNAME");
		case(6):
		return("SOA");
		break;
		case(12):
		return("PTR");
		break;
		case(13):
		return("HINFO (obsolete)");
		break;
		case(15):
		return("MX");
		break;
		case(16):
		return("TXT");
		break;
		case(17):
		return("RP (obsolete)");
		break;
		case(18):
		return("AFSDB");
		break;
		case(19):
		return("X25 (obsolete)");
		break;
		case(24):
		return("SIG (obsolete)");
		break;
		case(25):
		return("KEY (obsolete)");
		break;
		case(28):
		return("AAAA");
		break;
		case(29):
		return("LOC");
		break;
		case(33):
		return("SRV");
		break;
		case(35):
		return("NAPTR");
		break;
		case(36):
		return("KX");
		break;
		case(37):
		return("CERT");
		break;
		case(39):
		return("DNAME");
		break;
		case(41):
		return("OPT");
		break;
		case(42):
		return("APL");
		break;
		case(43):
		return("DS");
		break;
		case(44):
		return("SSHFP");
		break;
		case(45):
		return("IPSECKEY");
		break;
		case(46):
		return("RRSIG");
		break;
		case(47):
		return("NSEC");
		break;
		case(48):
		return("DNSKEY");
		break;
		case(49):
		return("DHCID");
		break;
		case(50):
		return("NSEC3");
		break;
		case(51):
		return("NSEC3PARAM");
		break;
		case(52):
		return("TLSA");
		break;
		case(55):
		return("HIP");
		break;
		case(59):
		return("CDS");
		break;
		case(60):
		return("CDNSKEY");
		break;
		case(61):
		return("OPENPGPKEY");
		break;
		case(99):
		return("SPF (obsolete)");
		break;
		case(249):
		return("TKEY");
		break;
		case(250):
		return("TSIG");
		break;
		case(251):
		return("IXFR");
		break;
		case(252):
		return("AXFR");
		break;
		case(255):
		return("ANY");
		break;
		case(256):
		return("URI");
		break;
		case(257):
		return("CAA");
		break;
		case(32768):
		return("TA");
		break;
		case(32769):
		return("DLV");
		break;
		default:
		return("unknown");
	  }
}
                                                                           GetRCode.c                                                                                          0000600 0001750 0001750 00000002002 13361361220 011315  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

uc *
GetRCode(unsigned short rcode)
{
	switch(rcode)
	  {
		case(0):
		return("\e[1;02m\e[1;32mno error\e[m");
		break;
		case(1):
		return("\e[3;31mformat error\e[m");
		break;
		case(2):
		return("\e[3;31mserver failure\e[m");
		break;
		case(3):
		return("\e[3;31mnon-existent domain\e[m");
		break;
		case(4):
		return("\e[3;31mnot implemented\e[m");
		break;
		case(5):
		return("\e[3;31mquery refused\e[m");
		break;
		case(6):
		return("\e[3;31mname exists but should not\e[m");
		break;
		case(7):
		return("\e[3;31mRRSet exists but should not\e[m");
		break;
		case(8):
		return("\e[3;31mRRSet does not exist but should\e[m");
		break;
		case(9):
		return("\e[3;31mserver not authorised for zone\e[m");
		break;
		case(10):
		return("\e[3;31mname not contained in zone\e[m");
		break;
		case(16):
		return("\e[3;31mbad SIG\e[m");
		break;
		case(17):
		return("\e[3;31mbad key\e[m");
		break;
		case(18):
		return("\e[3;31mbad time\e[m");
		break;
		default:
		return("\e[3;31munknown\e[m");
	  }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              GetRecords.c                                                                                        0000600 0001750 0001750 00000002622 13361316017 011736  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
GetRecords(DNS_RRECORD array[], u16 cnt, uc *ptr, uc *buf, size_t size, size_t *delta)
{
	static int k, hostmax = 0;
	static uc *p = NULL;

	if (array == NULL || cnt <= 0 || buf == NULL || ptr == NULL || size <= 0 || delta == NULL)
	  {
		fprintf(stderr, "\e[1;02m\e[3;31m%s%s\e[m\r\n",
	(array==NULL?"array":cnt<=0?"cnt":buf==NULL?"buf":ptr==NULL?"ptr":size<=0?"size":"delta"),
(array==NULL?"==NULL":cnt<=0?"<=0":buf==NULL?"==NULL":ptr==NULL?"==NULL":size<=0?"<=0":"NULL"));
		errno = EINVAL;
		return(-1);
	  }
	if ((hostmax = sysconf(_SC_HOST_NAME_MAX)) == 0)
		hostmax = 256;
	*delta &= ~(*delta); p = ptr;

	for (k = 0; k < cnt; ++k)
	  {
		if (!(array[k].name = (uc *)calloc_e(array[k].name, hostmax, sizeof(uc))))
			goto __err;
		if (GetName(p, buf, array[k].name, delta) == -1)
			goto __err;
		p += *delta;
		if (!(array[k].resource = (DNS_RDATA *)malloc_e(array[k].resource, sizeof(DNS_RDATA))))
			goto __err;
		if (memcpy(array[k].resource, p, sizeof(DNS_RDATA)) == NULL)
			{ perror("GetRecords: memcpy"); goto __err; }
		p += sizeof(DNS_RDATA);
		if (!(array[k].rdata = (uc *)calloc_e(array[k].rdata, ntohs(array[k].resource->len)+1, sizeof(uc))))
			goto __err;
		if (memcpy(array[k].rdata, p, ntohs(array[k].resource->len)) == NULL)
			{ perror("GetRecords: memcpy"); goto __err; }
		p += ntohs(array[k].resource->len);
	  }

	*delta = (p - ptr);
	return(0);

	__err:
	return(-1);
}
                                                                                                              HandleNAPTRrecord.c                                                                                 0000600 0001750 0001750 00000002630 13361263617 013102  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
HandleNAPTRrecord(DNS_RRECORD *record)
{
	if (record == NULL)
	  {
		fprintf(stderr, "\e[3;31mHandleNAPTRrecord: record is NULL\e[m\r\n");
		errno = EINVAL;
		return(-1);
	  }

	static uc *p = NULL, *name = NULL;
	static size_t len, delta;
	static int hostmax;
	static NAPTR_DATA *naptr = NULL;

	naptr = (NAPTR_DATA *)record->rdata;
	len = ntohs(record->resource->len);
	printf("\e[3;02mNAPTR record\e[m\r\n\r\n");
	p = record->name;
	printf("\e[3;02mnumber\e[m ");
	while (*p != 0 && (void *)p < (void *)(record + len))
		putchar(*p++);
	printf("\r\n");
	printf("\e[3;02morder\e[m %hu\r\n", ntohs(naptr->order));
	printf("\e[3;02mpreference\e[m %hu\r\n", ntohs(naptr->pref));
	printf("\e[3;02mservices\e[m ");
	p = naptr->services;
	while (*p != 0 && (void *)p < (void *)(record + len))
		putchar(*p++);
	printf("\r\n");
	printf("\e[3;02mregex\e[m ");
	p = naptr->regex;
	while (*p != 0 && (void *)p < (void *)(record + len))
		putchar(*p++);
	printf("\r\n");
	p = naptr->replace;
	if ((hostmax = sysconf(_SC_HOST_NAME_MAX)) == 0)
		hostmax = 256;
	if (!(name = (uc *)calloc_e((uc *)name, hostmax, sizeof(uc))))
		goto __err;
	delta = 0;
	if (GetName(p, record->rdata, name, &delta) == -1)
		{ perror("HandleNAPTRrecord: GetName"); goto __err; }
	p += delta;
	printf("\e[3;02mreplacement\e[m %s", name);


	if (name != NULL) free(name);
	return(0);

	__err:
	if (name != NULL) free(name);
	return(-1);
}
                                                                                                        lib.c                                                                                               0000600 0001750 0001750 00000002672 13362125603 010450  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

void
DieSys(const char const *EMsg)
{
	fprintf(stderr, "\e[1;02m\e[3;31m%s - %s\e[m\r\n", EMsg, strerror(errno));
	exit(1);
}

void
Usage(void)
{
	fprintf(stderr, "usage: orion <hostname> [OPTIONS]\r\n"
			"  -D	send DHCP request to find local DNS server(s)\r\n"
			"  -S	specify DNS server [default is 127.0.1.1]\r\n"
			"  -t	specify the type of query:\r\n"
			"		A	IPv4 address\r\n"
			"		AAAA	IPv6 address\r\n"
			"		PTR	convert IPv4/IPv6 address to name\r\n"
			"		NAPTR	find SIP URI for telephone number\r\n"
			"		MX	mail exchange information\r\n"
			"		TXT	server text file information\r\n"
			"		SOA	zone authority information\r\n"
			"		NS	name server information\r\n"
			"		AXFR	request transfer of zone information\r\n"
			"  -x	accept resource records with stale time-to-live\r\n"
			"\r\n"
			"\r\n"
			"Examples:\r\n"
			"\r\n"
			"orion example.com -t AAAA -S 8.8.8.8\r\n"
			"	finds IPv6 address for \"example.com\"\r\n"
			"	using Google's DNS server at 8.8.8.8#53\r\n"
			"\r\n"
			"orion +447123456789 -t NAPTR\r\n"
			"	finds SIP URI for +447123456789\r\n"
			"	using default DNS resolver at 127.0.1.1#53\r\n"
			"\r\n"
			"orion example.com -t AAAA -D\r\n"
			"	finds IPv6 address for \"example.com\"\r\n"
			"	using DNS server found via DHCP request\r\n"
			"\r\n"
			"orion example.com -t NS -x\r\n"
			"	finds name server for \"example.com\" and\r\n"
			"	accepts records with stale time-to-live values\r\n");
	exit(1);
}

                                                                      main.c                                                                                              0000600 0001750 0001750 00000007017 13362122156 010624  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

int
main(int argc, char *argv[])
{
	static char c, typeStr[20], classStr[20], ns[INET_ADDRSTRLEN];
	static uc *telno = NULL, *p = NULL, *host = NULL;
	static int DNS_SRV_FL = 0;
	static int hostmax = 0, k;
	static _atomic_ q_type = 0, q_class = 0;

	if (argc == 1)
		Usage();
	host = argv[1];
	if ((hostmax = sysconf(_SC_HOST_NAME_MAX)) == 0)
		hostmax = 256;
	if (strlen(host) >= hostmax)
	  {
		fprintf(stderr, "\e[3;31mmain: hostname exceeds maximum number of chars allowed\e[m\r\n");
		errno = ENAMETOOLONG;
		goto __err;
	  }
	while ((c = getopt(argc, argv, "hDS:t:c:x")) != -1)
	  {
		switch(c)
		  {
			case(0x78):
			STALE_OK = 1;
			break;
			case(0x68):
			Usage();
			break;
			case(0x44):
			if (DHCP_GetNS(ns) == -1)
				{ perror("DHCP_GetNS"); goto __err; }
			DNS_SRV_FL = 1;
			case(0x53):
			if (strncpy(ns, optarg, INET_ADDRSTRLEN) == NULL)
				{ perror("strncpy"); goto __err; }
			DNS_SRV_FL = 1;
			break;
			case(0x63): /* specify class */
			if (strncpy(classStr, optarg, strlen(optarg)) == NULL)
				{ perror("strncpy"); goto __err; }
			for (int k = 0; k < strlen(optarg); ++k)
				classStr[k] = tolower(classStr[k]);
			if (strncmp(classStr, "in", 2) == 0)
				q_class = 1;
			else if (strncmp(classStr, "chaos", 5) == 0)
				q_class = 3;
			else
				q_class = 1;
			break;
			case(0x74):
			strncpy(typeStr, optarg, strlen(optarg));
			for (int k = 0; k < (strlen(typeStr)); ++k)
				typeStr[k] = tolower(typeStr[k]);
			if (strncmp(typeStr, "a", 1) == 0 &&
			    strncmp(typeStr, "aaaa", 4) != 0 &&
			    strncmp(typeStr, "axfr", 4) != 0)
			  {
				q_type = 1;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "ns", 2) == 0)
			  {
				q_type = 2;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "cname", 5) == 0)
			  {
				q_type = 5;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "soa", 3) == 0)
			  {
				q_type = 6;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "ptr", 3) == 0)
			  {
				q_type = 12;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "mx", 2) == 0)
			  {
				q_type = 15;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "txt", 3) == 0)
			  {
				q_type = 16;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "aaaa", 4) == 0)
			  {
				q_type = 28;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "naptr", 5) == 0)
			  {
				q_type = 35;
				p = host; k = 0;
				for (k = 0; k < strlen(argv[1]); ++k)
					if (isalpha(argv[1][k]))
						{ errno = EPROTO; perror("invalid argument for NAPTR"); goto __err; }
				k = 0;
				if (!(telno = (uc *)calloc_e(telno, hostmax, sizeof(char))))
					goto __err;
				while (p < (host + strlen(host)))
				  {
					if (*p == 0x2d || *p == 0x2b || *p == 0x20)
						++p;
					else
						telno[k++] = *p++;
				  }
				telno[k] = 0;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "axfr", 4) == 0)
			  {
				q_type = 252;
				goto __got_type;
			  }
			else if (strncmp(typeStr, "any", 3) == 0)
			  {
				q_type = 255;
				goto __got_type;
			  }
			else
			  {
				q_type = 255;
				goto __got_type;
			  }
			__got_type:
			break;
			case(0x3f):
			Usage();
			default:
			Usage();
		  }
	  }
	if (!DNS_SRV_FL)
	  {
		strncpy(ns, "127.0.1.1", INET_ADDRSTRLEN);
		ns[strlen("127.0.1.1")] = 0;
	  }
	if (q_class == 0)
		q_class = 1;

	if (telno == NULL)
	  {
		if (DoQuery(host, ns, q_type, q_class) == -1)
			goto __err;
	  }
	else
	  {
		if (DoQuery(telno, ns, q_type, q_class) == -1)
			goto __err;
	  }

	if (telno != NULL) free(telno);
	exit(0);

	__err:
	if (telno != NULL) free(telno);
	exit(-1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 Makefile                                                                                            0000600 0001750 0001750 00000000511 13361327516 011172  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   CC=gcc
FLAGS=-g
CFILES=main.c DoQuery.c DoTCP.c DoUDP.c DHCP_GetNS.c GetRecords.c GetAnswers.c ConvertName.c ConvertToPtr.c ConvertToPtr6.c ConvertNumberToE164.c HandleNAPTRrecord.c GetName.c GetOpCode.c GetQClass.c GetQType.c GetRCode.c PrintInfoDNS.c lib.c wrappers.c dns.h

orion: $(CFILES)
	$(CC) $(FLAGS) -o orion $(CFILES)
                                                                                                                                                                                       Networking_info                                                                                     0000600 0001750 0001750 00000043112 13361263617 012624  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   
					Networking

Classful Networking (Historical):

Class A:
	0.0.0.0/8 - 127.255.255.255/8
	# of networks:	128 (2^7)
	# of hosts:	16,777,216 (2^24)
Class B:
	128.0.0.0/16 - 191.255.255.255/16
	# of networks:	16384 (2^14)
	# of hosts:	65536 (2^16)
Class C:
	192.0.0.0/24 - 223.255.255.255/24
	# of networks:	2,097,152 (2^21)
	# of hosts:	256 (2^8)

Very wasteful. Millions of addresses wasted when a company given a class A network. Would never
need to use the entire (2^24)-2 possible usable host addresses (first and last are reserved).
--Subnetting used to try and mitigate the problem:
	192.80.109.0/24 11000000.01010000.01101100.00000000
	can be subdivided into two subnets, e.g.
	192.80.109.0/25 - 192.80.109.127, and
	192.80.109.128/25 - 192.80.109.255/25
	Software can simply check the 25th bit and send packet through appropriate interface.
--allows ISPs to be given a block which they can divide into various subnetworks.

CIDR - Classless Inter-Domain Routing (conceived in 1993):
Better solution: instead of the rigid 8, 16 and 24 network prefixes, one can have variable
network prefix lengths. This also allows for much better routing aggregation within
core/edge routers (aka: supernetting, prefix aggregation, route summarisation).

Route Aggregation:
A supernet = a block of contiguous subnetworks addressed as a single subnet in the larger network.
They always have a subnet mask that is smaller than the masks of the component networks.
Requires protocols that support CIDR; thus Interior Gateway Routing Protocol, Exterior Gateway
Protocol and Routing Information Protocol v1 CANNOT be used because they assume classful addressing.  The routing protocols that support CIDR are:
	RIPv2, Open Shortest Path First, Enhanced Interior Gateway Routing Protocol, IS-IS, and
	Border Gateway Protocol.

Example of Route Aggregation:

192.168.98.0		11000000.10101000.01100010.00000000
192.168.99.0		11000000.10101000.01100011.00000000
192.168.100.0		11000000.10101000.01100100.00000000
192.168.101.0		11000000.10101000.01100101.00000000
192.168.102.0		11000000.10101000.01100110.00000000
192.168.105.0		11000000.10101000.01101001.00000000
					     ^
		addresses all share bits until here
Can be aggregated thus:
192.168.96.0/20		11000000.10101000.01100000.00000000

Now, instead of a routing table containing all 6 entries, can forward data for any of these
6 networks through a single interface associated with the aggregated 192.168.96.0/20.

Special Use Addresses:

0.0.0.0/8: hosts on local net; can only be used as source ip;
10.0.0.0/8: for private nets (intranets); never appear on public inet
127.0.0.0/8: inet host loopback (same computer) [typically only 127.0.0.1 is used]
169.254.0.0/16: link-local; used only on a single link and generally assigned automatically
172.16.0.0/12: for private nets (intranets); never appear on public inet
192.0.0.0/24: IETF protocol assignments (IANA reserved)
192.0.2.0/24: TEST-NET-1 addresses approve for use in documentation; never on public inet
192.88.99.0/24: used for 6to4 relays (anycast addresses)
192.168.0.0/16: for private nets (intranets); never on public inet
198.18.0.0/15: used for benchmarks and performance testing
198.51.100.0/24: TEST-NET-2; approved for use in documentation
203.0.113.0/24: TEST-NET-3; approved for use in documentation
224.0.0.0/4: reserved space (formerly class E), except 255.255.255.255
255.255.255.255/32: local net (limited) broadcast address

::/0 default route entry; not used for addressing
::/128 unspec addr; may be used as a source IP addr
::1/128 ipv6 host loopback addr; not used in datagrams sent outside local host
::ffff:0:0/96 ipv4-mapped addresses; never appear in packet headers; internal host use only
::{ipv4-address}/96 ipv4-compatible addresses; DEPRECATED; NOT TO BE USED
2001:10::/28 overlay routable cryptographic hash IDs; never on public inet
2001:db8::/32 addr range used for documentation and for examples; never on public inet
2002::/16 6to4 addresses of 6to4 tunnel relays
3ffe::/16 used by 6bone experiments; DEPRECATED; NOT TO BE USED
5f00::/16 used by 6bone experimients; DEPRECATED; NOT TO BE USED
fc00::/7 unique, local unicast addresses; not used on global inet
fe80::/10 link-local unicast addresses
ff00::/8 ipv6 multicast addresses; only as destination addr

IPv4 private addresses very common for home nets and internal nets of moderately sized and large
enterprises. Frequently used in combination with NAT.
--they are aka "non routable addresses".


Non-routable IPv4 address range:
169.254/16 (first and last 256 addresses in range are RESERVED)
so: 169.254.1.0 - 169.254.254.255 can be used.

IANA owns the OUI 00:00:5e. IANA given right to use multicast MAC addresses starting with
01:00:5e - used as the high order 24 bits of the ethernet address. So this block includes
addresses in the range 01:00:5e:00:00:00 to 01:00:5e:ff:ff:ff.
	Half of the block is dedicated to multicast traffic on IEEE 802 LANs. So
01:00:5e:00:00:00 to 01:00:5e:7f:ff:ff is the multicast MAC address range.

All ipv4 multicast addresses share a common prefix of 1110 (224); so this means there are
32 - 4 = 28 bits for multicast addresses. So the entire 2^28 (268,435,456) ipv4 multicast
group IDs need to be mapped to a link-layer address containing only 2^23 (8,388,608) possibilities.
The mapping is therefore a surjection. 2^28/2^23 = 2^5 = 32 distinct ipv4 multicast addresses for
each link-layer multicast address.

e.g.
	both 224.128.64.32 (e0.80.40.20) and 224.0.64.32 (e0.00.40.20) map to the multicast
	link-layer address 01:00:5e:00:40:20.

Mapping of IPv4 and IPv6 addresses to a multicast address:

IPV4:
	Lower 23 bits of the IPv4 group address copied;
	Top 9 bits replaced by

	01005e[0] (the 0 in square brackets represents a single bit)

	e.g. 224.0.1.17 →  01:00:5e:01:11

IPV6 (prefix = 33:33)

	flags = 4 bits; scope = 4 bits;

	11111111|flags|scope|(80 bits)|		lower 32 bits		|

	lower 32 bits of ipv6 address copied.
	Higher 96 bits replaced by prefix 33:33.

	e.g.
		ff02::1:ff68:12cb →  33:33:ff68:12cb

	Because only 32 bits of the remaining 112 ipv6 bits are used, 2^80 ipv6 addresses
	are mapped to the same link-layer multicast address.

struct tcphdr {
	uint16_t	sport;
	uint16_t	dport;
	uint32_t	seqno;
	uint32_t	ackno;
#if __BYTE_ORDER == __LITTLE_ENDIAN
	uint16_t	res:4;
	uint16_t	doff:4;
	uint16_t	cwr:1;
	uint16_t	ece:1;
	uint16_t	urg:1;
	uint16_t	ack:1;
	uint16_t	psh:1;
	uint16_t	rst:1;
	uint16_t	syn:1;
	uint16_t	fin:1;
#elif __BYTE_ORDER == __BIG_ENDIAN
	uint16_t	doff:4;
	uint16_t	res:4;
	uint16_t	fin:1;
	uint16_t	syn:1;
	uint16_t	rst:1;
	uint16_t	psh:1;
	uint16_t	ack:1;
	uint16_t	urg:1;
	uint16_t	ece:1;
	uint16_t	cwr:1;
#else
# error "adjust <bits/endian.h>"
#endif
	uint16_t	winsz;
	uint16_t	checksum;
	uint16_t	urgptr;
};

struct iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	uint16_t	ihl:4;
	uint16_t	vers:4;
	uint16_t	tos:8;
#elif __BYTE_ORDER == __BIG_ENDIAN
	uint16_t	vers:4;
	uint16_t	ihl:4;
	uint16_t	tos:8;
#else
# error "adjust <bits/endian.h>"
#endif
	uint16_t	tlen;
	uint16_t	ident;
	uint16_t	fragoff;
	uint8_t		ttl;
	uint8_t		proto;
	uint16_t	checksum;
	struct in_addr	ip_dst, ip_src;
};

struct eth_addr {
	uint8_t		ether_addr_octet[ETH_ALEN];
} __attribute__ ((__packed__));

struct eth_hdr {
	uint8_t		ether_dhost[ETH_ALEN];
	uint8_t		ether_shost[ETH_ALEN];
	uint16_t	ether_type;
} __attribute__ ((__packed__));

Types of Ethernet Protocol:
ETHERTYPE_PUP		0x0200 /* xerox pup */
ETHERTYPE_SPRITE	0x0500	sprite
ETHERTYPE_IP		0x0800  ip
ETHERTYPE_ARP		0x0806  address resolution
ETHERTYPE_REVARP	0x8035  reverse arp
ETHERTYPE_AT		0x809B	appletalk protocol
ETHERTYPE_AARP		0x80F3  appletalk arp
ETHERTYPE_VLAN		0x8100	IEEE 802.1Q VLAN tagging
ETHERTYPE_IPX		0x8137	ipx
ETHERTYPE_IPV6		0x86dd	ipv6
ETHERTYPE_LOOPBACK	0x9000	used to test interfaces

	Socket I/O Control Options:
SIOCGIFINDEX - get the index of the interface within the array of if's
SIOCGIFHWADDR - get the hardware address of the interface
SIOCGIFADDR - get the internet protocol address associated with the interface
SIOCGIFNAME - get name of interface (based probably on the index?)
SIOCGIFFLAGS - get the flags associated with the interface
SIOCGIFDSTADDR - get the remote "PA" address
SIOCGIFBRDADDR - get the broadcast "PA" address
SIOCGIFNETMASK - get the netmask of the interface
SIOCGIFMETRIC - get the metric
SIOCGIFMEM - get the mem address (BSD!!!!)
SIOCGIFMTU - get the maximum transmission unit of the interface
SIOCGIFENCAP - get encapsulations
SIOCGIFSLAVE - driver slaving support
SIOGIFINDEX/SIOCGIFINDEX - for misprint compatibility
SIOCGIFPFLAGS - ?
SIOCGIFCOUNT - get number of devices
SIOCGIFBR - bridging support
SIOCGIFTXQLEN - get the tx queue length
SIOCGARP - get the arp table entry
SIOCGRARP - get the rarp table entry
SIOCGIFMAP - get device parameters

	Socket I/O Control Options related to wireless inet (/usr/include/linux/wireless.h)
SIOCGIWNAME - get name == wireless protocol; used to verify presence of wireless extensions
SIOCGIWNWID - get network id (the cell)
SIOCGIWFREQ - get channel/freq (Hz)
SIOCGIWMODE - get operation mode
SIOCGIWSENS - get sensitivity (dBm)
SIOCGIWRANGE - get range of parameters
SIOCGIWPRIV - get private ioctl interface info
SIOCGIWSTATS - get /proc/net/wireless stats (strictly used between userspace and kernel)
SIOCGIWSPY - get spy info (quality of link)
SIOCGIWTHRSPY - get spy threshold
SIOCGIWAP - get access point MAC addresses
SIOCGIWAPLIST - DEPRECATED in favour of scanning
SIOCGIWSCAN - get scanning results
SIOCGIWESSID - get ESSID
SIOCGIWNICKN - get node name/nickname
SIOCGIWRATE - get default bit rate (bps)
SIOCGIWRTS - get RTS/CTS threshold (bytes)
SIOCGIWFRAG - get fragmentation threshold (bytes)
SIOCGIWTXPOW - get transmit power (dBm)
SIOCGIWRETRY - get retry limits and life time
SIOCGIWENCODE - get encoding token and mode
SIOCGIWPOWER - get power management settings
SIOCGIWGENIE - get generic IE
SIOCGIWAUTH - get authentication mode params
SIOCGIWENCODEEXT - get encoding token and mode



				Universal Plug and Play
UPnP multicast address: 239.255.255.250:1900 (control points listen on this)
	advertisements:
	1. ssdp:alive (device available)
	2. ssdp:byebye (device unable)
	3. ssdp:update (device update)

upnp message formats:
device available:
NOTIFY * HTTP/1.1
HOST: 239.255.255.250:1900
CACHE-CONTROL: max-age = secs until adv expires
LOCATION: url for upnp description for root dev
NT: notif type
NTS: ssdp:alive
SERVER: 0S/version UPnP/2.0 product/version
USN: composite id for the adv
BOOTID.UPNP.ORG: num increased each time dev sends an initial announce or update msg
CONFIGID.UPNP.ORG: num used for caching description info
SEARCHPORT.UPNP.ORG: num identifies port on which dev responses to unicast M-SEARCH

device unavailable:
NOTIFY * HTTP/1.1
HOST: 239.255.255.250:1900
NT: notif type
NTS: ssdp:byebye
USN: composite id for the adv
BOOTID.UPNP.ORG:
CONFIGID.UPNP.ORG:

multicast discovery:
M-SEARCH * HTTP/1.1
HOST: 239.255.255.250:1900
MAN: "ssdp:discovery"
MX: secs to delay response
ST: search type
USER-AGENT: OS/Version UPnP/2.0 product/version
CPFN.UPNP.ORG: friendly name of the control point
CPUUID.UPNP.ORG: uuid of control point

multicast discovery response:
HTTP/1.1 200 OK
CACHE-CONTROL: max-age = secs until adv expires
DATE: when response was generated
EXT:
LOCATION: url for upnp description for root dev
SERVER: OS/Version UPnP/2.0 product/version
ST: search target
USN: composite id for the adv
BOOTID.UPNP.ORG:
CONFIGID.UPNP.ORG:
SEARCHPORT.UPNP.ORG:

			Point-to-Point Protocol over Ethernet (PPPoE)

	ETHERNET FRAME:

     6 bytes	     6 bytes	   2 bytes     variable       
destination addr | source addr | ether_type | payload ... | checksum |

ether_type will be set to 0x8863 (discovery stage) or 0x8864 (PPP session stage).

Ethernet payload for PPPoE:

 0                    15                      31
 ver  |  type  |  code  |      session id      |
      length            |       payload        ~


		DISCOVERY STAGE - four steps
0                  15                         31
     TAG_TYPE        |       TAG_LENGTH        |
     TAG_VALUE ...                             ~

	PPPoE Active Discovery Initiation packet (PADI):
DESTINATION_ADDR set to broadcast address;
CODE field set to 0x09
SESSION_ID set to 0x0000

Must contain exactly one TAG of TAG_TYPE Service-Name, indicating service host requesting, and any number of other TAG types. Entire PADI packet MUST NOT exceed 1484 bytes (leaves 16 bytes for relay agent to add a Relay-Session-ID TAG.

PPPoE Active Discovery Offer packet (PADO):

DESTINATION_ADDR = unicast host address
CODE = 0x07
SESSION_Id = 0x0000

Must contain one AC-Name TAG containing name of Access Concentrator, Service-Name TAG identical to one used in the PADI, and any number of other Service-Name TAGs indicating other services that the AC offers. AC MUST NOT respond with a PADO if cannot offer service sought by host.

A host may received several PADOs in response to a PADI; can choose whichever one it wants (choice may be based on name of AC and or services offered by the AC).

PPPoE Active Discovery Request packet (PADR):
DESTINATION_ADDR = unicast address of AC that sent the PADO
CODE = 0x19
SESSION_ID = 0x0000

Must contain exactly one TAG of TAG_TYPE Service-Name, indicating service host requesting (and any number of other TAG types).

PPPoE Active Discovery Session-confirmation packet (PADS):

DESTINATION_ADDR = unicast address of host
CODE = 0x65
SESSION_ID = generated (unique) by the AC for the PPP session.

Must contain exactly one TAG of TAG_TYPE Service-Name, indicating service under which the AC has
accepted the PPPoE session, and any number of other TAG types.

If the AC does not like the Service-Name in the PADR, MUST reply with a PADS containing a TAG of
TAG_TYPE Service-Name-Error (and any number of other TAG types); in this case,
SESSION_ID = 0x0000.

PPPoE Active Discovery Terminate packet (PADT):
DESTINATION_ADDR = unicast address
CODE = 0xA7
SESSION_ID = session id of PPPoE session that is to be terminated
-- no TAGs are required.

TAG_TYPE's and TAG_VALUE's:

0x0000 End of list: indicates no further TAGs in the list
0x0101 Service-Name: a service name TAG_VALUE is a UTF-8 string (non null-terminated). When 
	TAG_LENGTH is zero, this TAG is used to indicate that any service is acceptable.
0x0102 AC-Name: indicates that the following string uniquely this particular access concentrator
	unit from all others. May be a combination of trademark, model, and serial id info, or
	simply a UTF-8 rendition of the MAC address of the box. NOT null terminated.
0x0103 Host-Uniq: used by a host to uniquely associate an AC response (PADO or PADS) to a
	particular host request (PADI or PADR). TAG_VALUE is binary data of any value and length
	that the host desires. It is not interpreted by the AC. The host MAY include a Host-Uniq
	TAG in a PADI or PADR. If the AC receives this TAG, it MUST include the TAG unmodified
	in the associated PADO or PADS response.
0x0104 AC-Cookie: used by AC to aid in protecting against DoS attacks. MAY include it in the PADO
	packet. If a host receives the TAG, it MUST include it unmodified in the following PADR.
	TAG_VALUE is binary data of any value and length and is not interpreted by the host.
0x0105 Vendor-Specific: used to pass vendor proprietary information.
0x0110 Relay-Session-ID: This TAG MAY be added to any PADx packet by an intermediate agent that
	is relaying traffic. TAG_VALUE is opaque to both the host and the AC. If either the host
	or the AC receive it, they MUST Include it unmodified in any PADx packet they send as
	a response. All PADI packets MUST guarantee sufficient room for the addition of a
	Relay-Session-ID TAG with TAG_VALUE length of 12 octets.
0x0201 Service-Name-Error: typically has a zero-length data section; indicates that for one
	reason or another, the requested Service-Name request could not be honoured. If there
	is data, and the first octet is non-zero, then it MUST be a printable UTF-8 string which
	explains why the request was denied. The string MAY NOT be null terminated.
0x0202 AC-System-Error: AC experience some error in performing the host request (e.g. insufficient
	resources to create a virtual circuit). MAY be included in a PADS packet. IF there is data,
	same stipulations as the 0x0201 error string.
0x0203 Generic-Error: indicates error; can be added to a PADO, PADR, or PADS packet when an
	unrecoverable error occurs and no other error TAG is appropriate. If there is data, then
	the same stipulations as 0x0201 and 0x0202 error strings.



				iptables

Three types: input, forward, and output

--input: used to control the behaviour of incoming connections
--forward: used for incoming connections that aren't being delivered locally; e.g. a router: data
	is always being sent to it but rarely destined for the router itself; it just forwards it.
--output: used for outgoing connections; e.g. if you try to ping a site, iptables will check
	output chain for rules on ping and target site.



				MOBILE IP

Bidirectional Tunneling:
Mobile Nodes (MN) communicate with Correspondent Nodes (CN). MN given IP address chosen from the
network prefix used in its home net, aka Home Address (HoA). Roaming in visited net, given another
address called Care-of-Address (CoA). Whenever a CN communicates with a MN, communcation is
carried out via Home Agent (HA) -- special type of router in net infrastructure like other
important system (e.g. routers and web servers).
--Association between MN's HoA and CoA called a "binding".

MN ------ binding update ----> HA
HA ------ binding acknowledgement ----> MN
	Msgs protected with IPsec using Encapsulating Security Payload (ESP) (ensures that a HA
	is not fooled into accepting a binding from a fake MN).

See RFC6275 and RFC 4866 for detailed discussion on "route optimization" (strategy employed when
a MN and CN are in close proximity but the HA is far away [therefore inefficient]).
                                                                                                                                                                                                                                                                                                                                                                                                                                                      orion                                                                                               0000700 0001750 0001750 00000305420 13362125605 010607  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   ELF          >     @     @       �         @ 8 	 @ $ !       @       @ @     @ @     �      �                   8      8@     8@                                          @       @     ��      ��                    �      �`     �`     �      �                    (�      (�`     (�`     �      �                   T      T@     T@     D       D              P�td   ��      ��@     ��@                        Q�td                                                  R�td   �      �`     �`     �      �             /lib64/ld-linux-x86-64.so.2          GNU                        GNU f��ޔ�Y�wc���+����o   4              4   5   ���9�                        >                      �                     �                      f                      f                     �                      H                                            P                      N                     �                      X                     v                      4                      �                      �                      U                                            �                      q                     �                                          �                      ;                     x                     A                     �                     �                      p                     �                      �                      �                      }                      �                      n                                           $                     �                     �                      %                      �                     �                                            ,                      G                     �                                          �                      �                                                                 -    ��`            4    ��`             libc.so.6 socket exit htons strncmp perror connect inet_ntoa inet_ntop strncpy puts __stack_chk_fail putchar tolower getpid inet_pton strftime calloc strlen send sigemptyset memset __errno_location getopt shutdown sigaddset __sigsetjmp recv memcpy setsockopt malloc siglongjmp __ctype_b_loc recvfrom optarg stderr ioctl alarm fwrite sigaction clock_gettime localtime fprintf sendto strerror __libc_start_main ntohl ntohs snprintf sysconf free __gmon_start__ GLIBC_2.3 GLIBC_2.14 GLIBC_2.4 GLIBC_2.17 GLIBC_2.2.5                                                                        ii   �     ���   �     ii   �     ���   �     ui	   �      ��`                   ��`        4           ��`        5           �`                    �`                   (�`                   0�`                   8�`                   @�`                   H�`                   P�`                   X�`        	           `�`        
           h�`                   p�`                   x�`                   ��`                   ��`                   ��`                   ��`                   ��`                   ��`                   ��`                   ��`                   ��`                   Ȑ`                   А`                   ؐ`                   ��`                   �`                   �`                   ��`                    �`                   �`                    �`        !           �`        "            �`        #           (�`        $           0�`        %           8�`        &           @�`        '           H�`        (           P�`        )           X�`        *           `�`        +           h�`        ,           p�`        -           x�`        .           ��`        /           ��`        0           ��`        1           ��`        2           ��`        3           H��H�U�  H��t�C  H���              �5B�  �%D�  @ �%B�  h    ������%:�  h   ������%2�  h   ������%*�  h   �����%"�  h   �����%�  h   �����%�  h   �����%
�  h   �p����%�  h   �`����%�  h	   �P����%�  h
   �@����%�  h   �0����%�  h   � ����%�  h   �����%�  h   � ����%�  h   ������%�  h   ������%�  h   ������%�  h   ������%�  h   �����%�  h   �����%�  h   �����%�  h   �����%�  h   �p����%�  h   �`����%z  h   �P����%r  h   �@����%j  h   �0����%b  h   � ����%Z  h   �����%R  h   � ����%J  h   ������%B  h    ������%:  h!   ������%2  h"   ������%*  h#   �����%"  h$   �����%  h%   �����%  h&   �����%
  h'   �p����%  h(   �`����%�~  h)   �P����%�~  h*   �@����%�~  h+   �0����%�~  h,   � ����%�~  h-   �����%�~  h.   � ����%�~  h/   ������%�~  h0   ������%�~  h1   ������%}  f�        1�I��^H��H���PTI��Pn@ H���m@ H���@ �G����fD  ���` UH-��` H��H��v�    H��t]���` ��f�     ]�@ f.�     ���` UH�` H��H��H��H��?H�H��t�    H��t]���` �� ]�fD  �=1~   uUH���n���]�~  ��@ � �` H�? u� �    H��t�UH����]�z���UH��SH��(�}�H�uЃ}�u�bU  H�E�H�@H��}  ��   ������}  ��}  ��u
��}     H��}  H���m���H��}  H�H9���  H�r}  H���B   �   �hn@ ���������� $   �X  �`}  ����?��9�V  ��H��(o@ ����     �@  �T  � �` �  H���u��n@ �K����  �}     H��|  �   H�ƿ �` �#���H��u��n@ ������  ��|     ��  H��|  H������H��H�w|  H�ƿ �` �����H��u��n@ ������  �E�    �'�E�H��� �` �����W����E�H��� �` �E��E�Hc�H�|  H������H9�r��   ��n@ � �` �x�����u�^|     �+  �   ��n@ � �` �Q�����u�7|     �  �(|     ��  H��{  H������H��H��{  H�ƿ@�` ������E�    �'�E�H���@�` ���������E�H���@�` �E��E�Hcؿ@�` �I���H9�rĸ@�` ���n@ � 8�u?�   ��n@ �@�` ������t'�   ��n@ �@�` ������t��{     �+  �   ��n@ �@�` �]�����u�c{     �  �   ��n@ �@�` �6�����u�<{     ��  �   ��n@ �@�` ������u�{     �  �   ��n@ �@�` �������u��z     �  �   ��n@ �@�` �������u��z     �h  �   ��n@ �@�` ������u��z     �A  �   ��n@ �@�` �s�����u�yz     �  �   ��n@ �@�` �L�������  �Nz  #   H��y  H�Dz  �Bz      �8z      �d����H� H�U�H��H�
�z  Hc�H��H��H�H�� ��%   ��t����� G   ��n@ �����n  ��y  ����y  ��y  Hc�H�E�H��H� H������H9��w�����y      �5y  Hc�H��y  �   H��H����P  H��y  H�}y  H����  �pH�[y  � <-tH�My  � <+tH�?y  � < uH�1y  H��H�&y  �2H�-y  �y  �P�y  H�H�H�y  H�PH��x  � �H��x  H�|x  H���D���H�H��x  H9��f���H��x  ��x  H�H��  �S�   ��n@ �@�` ������u��x  �   �/�   �o@ �@�` �a�����u�gx  �   ��[x  �   ��
�rO  �mO  H�MЋEܺo@ H�Ή��4�����w  ��w  <��d�����w  ��u�   �o@ � �` �������w   ��w  ��u
��w     H��w  H��u(��w  ��w  H�gw  � �` H���|   H���u(�O��w  ��w  H��w  � �` H���T   H���t(H��w  H��tH��w  H�������    �w������H�tw  H��tH�hw  H�������������N���UH��AUATSH��(H�}�H�uЉỦMȿ�   �	����Cw  �=w  ��u
�/w     H�0w  �   �   H���5N  H�w  H�w  H��� 	  �w      ��   ��v  Hc�H��H�H�H��H��` H�     ��v  Hc�H��H�H�H��H��` H�     ��v  Hc�H��H�H�H��H��` H�     ��v  Hc�H��H�H�H��H��` H�     �nv  Hc�H��H�H�H��H`�` H�     �Kv  Hc�H��H�H�H��Hp�` H�     �(v  ���v  �v  ������H�v  H��{  H��u  H��H��{  H��{  ����������*���f�H��{  � �����C���f�|{  H�e{  �P���PH�T{  �P�⇈PH�C{  �P����PH�2{  �P����PH�!{  �P���PH�{  �P���PH��z  �P�⿈PH��z  �P��߈PH��z  �P���PH��z  �P����PH��z  �   �Q���f�CH��z  f�@  H��z  f�@  H��z  f�@
  �Ẽ���   ��z      �N�����H� �tz  Hc�H�U�H����H�H�� ��%   ��t�Iz     �)�=z  ���4z  �.z  Hc�H�E�H������H9�r��z  ��u*H��y  H�Mغ`�` H��H���/  H�����   �/  H��y  H�Mغ`�` H��H���L4  H�����   �  �Ẽ�#��   ��s  Hc�H��y  �   H��H����J  H��y  H��y  H����  H��y  H�Mغ`�` H��H����8  H���u��p@ �*����  H�Vy  H�/y  �`�` H��H���b-  H���u4�q@ ������_  H�y  H�Mغ`�` H��H���2-  H����$  H�s  H��x  H��H�H��x  H��x  �E������M���f�H��x  �E������6���f�CH��x  H��H��x  H��x      �   �    ���` �Q����   �    ���` �=������` �    �~�����y�*q@ � ����  �E�=�   uAH�Rx  H�[r  H�U�H��H����  H�=x  H�6x  H����6  �Rx     �5H�x  H�r  H�U�H��H����  H��w  H��w  H�����  ���` �    �������y�*q@ �~�����  H��w  f���H*��iQ  �^�H��w  f���H*��MQ  �^��\���w  ��w  ��u�Aq@ ��Eq@ H��w  H��H�E��E��Pq@ �   �����H�Mq  H��v  H�?q  H�pw  ��v  ��H�_w  H�Mо   H���D  H���u��q@ �����  H��p  H��v  H��H�H�w  H��v  �@�����X���f��tuL�-�v  L�%�p  H��v  H�gv  �@�����)�����A�Ș` M��L��H�ډƿ��` ��  H���u��q@ �����  H��v  H��v  H�H��v  H�v  �@���������f��tjL�--v  L�%.p  H�_v  H��u  �@����������A�Ș` M��L��H�ډƿ��` �7  H�����  H�v  H�v  H�H�
v  H��u  �@
�����E���f��tjL�-�u  L�%�o  H��u  H�Tu  �@
����������A�Ș` M��L��H�ډƿ`�` �  H����q  H��u  H��u  H�H��u  H��t  �@���������f��tI��q@ �����D�e�H�o  H��t  �@����������D��H�ډƿ��` ��  H�����   H��t  �@�����^���f��tI��q@ ����D�e�H��n  H�mt  �@�����/�����D��H�ډƿ��` �  H�����   H�9t  �@
���������f��tE�r@ �,���D�e�H�Yn  H�
t  �@
�����������D��H�ډƿ`�` �(  H���t=H�"n  H��tH�n  H���f����    �[������������
��������~���� �&t  H��m  H��tH��m  H�������V���H��s  �H������H��([A\A]]�UH��H���}��}�u-H��l  H���   �   �@r@ �j����   ���` ��������UH��H�� H�}�H�u�H�U�H�}� tH�}� tH�}� u������    H��������  ��   �    ���` ����H�9t  �$@ �ș` �����   �ș` � ����   �ș` ������t      �    ���` �   ������y�`r@ �U����N  �   �    �`�` �<���f�ct   �5   �����f�Tt  H�E�d�` H�ƿ   ������y��r@ �������  �   �   �   �����t  �t  ��y��r@ �������  �    ���` �7�������  �
   �������s  �   �`�` ���������t��r@ �����y  �    �����    ����H��s  �x�` �6���H��s  H��s  H���9  H��s  H����r@ �   ���` ����H�js  H�@0H�¾��` ��r@ �    ������Z�����H�M�H�E��    H���/>  H�����   �
   ������s  H�U�H�u��    ���C  H�Us  H�Ns  H���u�s@ �����   �    �����
   ������r  H�u��    �   ���D  H�s  H��r  H���u�s@ �D����@�    �h����rr  �   ������H��r  H��H�E�H��  H��r  ������H��������UH��H���}��}�u-H��i  H���   �   � s@ �����   ���` �������UH��H�� H�}�H�u�H�U�H�}� tH�}� tH�}�   wH�}� u�p����    H�������b  ��   �    ���` �Z���H��r  7(@ �]s      �ț` �����   �ț` �����   �ț` �����    ���` �   �9�����y�@s@ �������  �   �    �`�` �����f�	s   �5   ����f��r  H�E�d�` H�ƿ   �B�����y�qs@ �����  �   �   �   �;�����r  ��r  ��y��s@ �q����Y  �    ����H��r  �x�` �1���H��r  H��r  H���   H��r  H����s@ �   ���` ����H�er  H�@0H�¾��` ��s@ �    ������U�����H�M�H�E��    H���*;  H�����   �r  H�U�H�u�A�   A�`�` �    ���oB  H���u��s@ �����   �   ���` �	�����uq�
   ������q  H�u��    �   ��� ���H�r  H��q  H��y��s@ �C����.�    �g���H��q  H��H�E�H��  H��q  ������H��������UH��H���}��}�u-H��f  H���   �   ��s@ �+����   ���` �������UH��SH��H�}�H�}� u�����    H�������2  ��   �    ���` �����ȝ` ������   �ȝ` �����   �ȝ` ������pr      H��q  &+@ �    ���` �   �i�����y�t@ �+����  H�Gr  �   �   H���|=  H�.r  H�'r  H���^  �Ht@ �����   �    �`�` �����H��q  H�
r  H�r  �   �i����H��q  �   �V����CH��q  �   �B����CH��q  �@ H��q  ����������������CH��q  f�@
 H��q  f�@
  H��q  �@    H�yq  �@    H�kq  �@    H�]q  H��,�@   �    H������H�@q  H��l��   �    H�������H�q  H�   H�q  H�q  H�PH�q  � H�q  H�PH��p  � �H��p  �  f��p   �C   �?���f��p  �(   �    ���` �t�����p  wlp2f��p  s0��t@ �w����   �   �   �������p  ��p  ��y��t@ �)����  ��p  ���` �'�  �Ǹ    ������y��t@ ������g  H�.p  H���   ���` H���X����Jp  ���` ��  �Ǹ    �������y��t@ �����  H�p  H�p  H��o  H��o  H��o  H�P��o  ���o  ���` ��  �Ǹ    �n�����y��t@ �P����  H��o  H��o  H�fo  H�go  �`�` H��u��t@ �����  ��o     �~o  A�   ���` �   �   ���*�����y��t@ ������I  H�Lo  �   H�Eo  H�5�n  �0o  A�   A�`�` �    ���b=  H����  �   ���` ��������   �   ����H�5�n  ��n  �    �   ������H�����   �    �s���H�dn  H�   H�wn  H�pn  H��H�en  H�^n  H��n  H��n  H��n  � ���X����H�wn  �   ���` H�ƿ   �x���H��u�u@ ������9���` �u@ �    ����H�<n  H�=n  H�M�H�H�Q�
�������H��m  H��tH��m  H���#���H������H��[]�UH��SH��8H�}��H�U�H�M�L�E�L�M�f�E�H�}� t'f�}� t H�}� tH�}� tH�}� tH�}� ��   H�}� t?f�}� t1H�}� t#H�}� tH�}� u�9u@ �!�=u@ ��Bu@ ��Bu@ ��9u@ ��Bu@ H�}� t?f�}� t1H�}� t#H�}� tH�}� u�Iu@ �!�Nu@ ��Tu@ ��Xu@ ��\u@ ��`u@ H�='`  H��H�¾fu@ �    ������M����    H�������  ��   �Q�����l  ��l  ��u
��l     H�E�H�     H�E�H��l  ��l      �  ��l  Hc�H��H�H�H��H��H�E�H���l  Hcȋ�l  Hc�H��H�H�H��H��H�E�H�H� �   H��H���7  H�H�H����  �Pl  Hc�H��H�H�H��H��H�E�H�H�H�$l  H�M�H�u�H���*  H�����  H�l  H�E�H� H�H��k  ��k  Hc�H��H�H�H��H��H�E�H���k  Hc�H��H�H�H��H��H�E�H�H�@�
   H����6  H�CH�CH���  H��k  ��k  Hc�H��H�H�H��H��H�E�H�H�@�
   H��H������H��u�~u@ ������  H�9k  H��
H�.k  �0k  Hc�H��H�H�H��H��H�E�H��k  Hc�H��H�H�H��H��H�E�H�H�@�@������������Hcȋ�j  Hc�H��H�H�H��H��H�E�H�H�@�   H��H���U5  H�CH�CH���  ��j  Hc�H��H�H�H��H��H�E�H�H�@�@����������H�Qj  �Sj  Hc�H��H�H�H��H��H�E�H�H�@H��H��H�������H��u�~u@ �X����   H�j  �j  Hc�H��H�H�H��H��H�E�H�H�@�@�����������H�H��i  ��i  ����i  �U��i  9������H��i  H��H�E�H)�H��H��H�E�H��    ��������H������H��8[]�UH��SH��(H�}��H�U؉M�f�E�H�}� tf�}� tH�}� u�k����    H�������  ��   �o����)i  �#i  ��u
�i     �i  Hc�H�i  �   H��H���3  H��h  H��h  H����  ��h  Hc�H��h  �   H��H���]3  H��h  H��h  H����  ��h  Hc�H��h  �   H��H���&3  H��h  H��h  H����  ��h      �5  �{h  Hc�H��H�H�H��H��H�E�H�H�@�@�������=:	 ��   ��i  ����   �4h  Hc�H��H�H�H��H��H�E�H�H�@�@���|����ƿ�u@ �    �������g  Hc�H��H�H�H��H��H�E�H�H� H��������g  Hc�H��H�H�H��H��H�E�H�H�@H���d�����g  Hc�H��H�H�H��H��H�E�H�H�     �wg  Hc�H��H�H�H��H��H�E�H�H�@    ��  ��Ig  Hc�H��H�H�H��H��H�E�H�H�@� �������������)  ��8���  ����te����  �  ���Y  ����	  �z  ��#�o  ��#����  ���Y  �U  =�   �F  =   �~  �:  ��f  Hc�H��H�H�H��H��H�E�H�H�@�@�����S���f����  �Sf  Hc�H��H�H�H��H��H�E�H�H�@H�6f  H�f  H�(f  H�uع0�` H���X$  H����  H�f  H�f  H�H��e  H��e  H��e  H��e  � �؋�e  Hc�H��H�H�H��H��H�E�H�H�@�@��������H��e  A��A��u@ ��H�¾�u@ ��u@ �    �Y���H�ze  H��H�oe  �ae  Hc�H��H�H�H��H��H�E�H�H� H��������6e  Hc�H��H�H�H��H��H�E�H�H�@H��������
e  Hc�H��H�H�H��H��H�E�H�H�     ��d  Hc�H��H�H�H��H��H�E�H�H�@    �a  ��d  Hc�H��H�H�H��H��H�E�H�H�@H��d  H��d  H��d  H��d  �8�����H��d  H��d  H��u�v@ �h����k  �Md  Hc�H��H�H�H��H��H�E�H�H�@�@��������H�Dd  ��H�¾)v@ �6v@ �    �������c  Hc�H��H�H�H��H��H�E�H�H� H��������c  Hc�H��H�H�H��H��H�E�H�H�@H���l�����c  Hc�H��H�H�H��H��H�E�H�H�     �c  Hc�H��H�H�H��H��H�E�H�H�@    ��  �Rc  Hc�H��H�H�H��H��H�E�H�H�@H�5c  H�c  H�'c  H�uع0�` H���W!  H����	  H�c  H�c  H�H��b  ��b  Hc�H��H�H�H��H��H�E�H�H�@�@���.�����H��b  ��H�¾Ov@ �6v@ �    �|�����b  Hc�H��H�H�H��H��H�E�H�H� H���1����kb  Hc�H��H�H�H��H��H�E�H�H�@H�������?b  Hc�H��H�H�H��H��H�E�H�H�     �b  Hc�H��H�H�H��H��H�E�H�H�@    �  ��a  Hc�H��H�H�H��H��H�E�H�H�@H��a  H��a  H��a  H�uع0�` H����  H�����  H��a  H��a  H�H��a  �a  Hc�H��H�H�H��H��H�E�H�H�@�@���������H�6a  ��H�¾[v@ �6v@ �    �����/a  Hc�H��H�H�H��H��H�E�H�H� H��������a  Hc�H��H�H�H��H��H�E�H�H�@H��������`  Hc�H��H�H�H��H��H�E�H�H�     ��`  Hc�H��H�H�H��H��H�E�H�H�@    �/  ��`  Hc�H��H�H�H��H��H�E�H�H�@H�g`  H�H`  H�Y`  H�uع0�` H���  H����A  H�7`  H�8`  H�H�&`  H�`  H�`  H�uع0�` H���H  H����  H��_  H��_  H�H��_  H��_  H��_  H��_  H��_  � ���(����H��_  H��H��_  H��_  H��_  H��_  H��_  � ��������H�~_  H��H�s_  H�l_  H��_  H��_  H��_  � �������H�E_  H��H�:_  H�3_  H�l_  H�e_  H�^_  � ���}����H�_  H��H�_  H��^  H�;_  H�4_  H�-_  � ���D����H��^  H��H��^  ��^  Hc�H��H�H�H��H��H�E�H�H�@�@��������H��^  � A��H��^  � A��H��^  � A��H��^  � ��H��^  �H�I^  H�:^  H��Vhw@ ARhw@ AQh!w@ APh%w@ Wh*w@ RA�jv@ I�ȹmv@ H�¾rv@ ��v@ �    �����H��`��]  Hc�H��H�H�H��H��H�E�H�H� H��������]  Hc�H��H�H�H��H��H�E�H�H�@H���d�����]  Hc�H��H�H�H��H��H�E�H�H�     �w]  Hc�H��H�H�H��H��H�E�H�H�@    ��  �J]  Hc�H��H�H�H��H��H�E�H�H�@H�-]  H�]  H�]  H�uع0�` H���O  H����	  H��\  H��\  H�H��\  ��\  Hc�H��H�H�H��H��H�E�H�H�@�@���&�����H��\  ��H�¾/w@ �6v@ �    �t�����\  Hc�H��H�H�H��H��H�E�H�H� H���)����c\  Hc�H��H�H�H��H��H�E�H�H�@H��������7\  Hc�H��H�H�H��H��H�E�H�H�     �\  Hc�H��H�H�H��H��H�E�H�H�@    �  ��[  Hc�H��H�H�H��H��H�E�H�H�@H��[  H��[  H�\  H�\  H��[  � ��������f�H��[  H��H��[  H�a[  H�z[  H�uع0�` H���  H����k  H�X[  H�Y[  H�H�G[  �9[  Hc�H��H�H�H��H��H�E�H�H�@�@��������H�`[  � ��H��Z  A�ȉ�H�¾7w@ �Hw@ �    ������Z  Hc�H��H�H�H��H��H�E�H�H� H���t�����Z  Hc�H��H�H�H��H��H�E�H�H�@H���H�����Z  Hc�H��H�H�H��H��H�E�H�H�     �[Z  Hc�H��H�H�H��H��H�E�H�H�@    ��  �vw@ ��w@ �    � ����Z  Hc�H��H�H�H��H��H�E�H�H�@H��Y  �u����H�H��Y  � ��H�H�� ������t.H��Y  � <t H��Y  � <
tH��Y  H��H��Y  H��Y  H�PH��Y  � �����^����xY  Hc�H��H�H�H��H��H�E�H�H�X�TY  Hc�H��H�H�H��H��H�E�H�H�@�@����������H�H�"Y  H9������
   ������Y  Hc�H��H�H�H��H��H�E�H�H� H��������X  Hc�H��H�H�H��H��H�E�H�H�@H���p�����X  Hc�H��H�H�H��H��H�E�H�H�     ��X  Hc�H��H�H�H��H��H�E�H�H�@    �  �VX  Hc�H��H�H�H��H��H�E�H�H�@H�9X  H�2X  H��X  H�|X  �.   ���` H�ƿ
   ����H��u��w@ �����	  H��W  ��W  Hc�H��H�H�H��H��H�E�H�H�@�@����������H�H��W  ��W  Hc�H��H�H�H��H��H�E�H�H�@�@������������` ��w@ �6v@ �    �B����  �WW  Hc�H��H�H�H��H��H�E�H�H����  H�����  ��w@ �1����4  �W  Hc�H��H�H�H��H��H�E�H�H�@H��V  H��V  H��V  H�uع0�` H���  H�����  H��V  H��V  H�H��V  ��V  Hc�H��H�H�H��H��H�E�H�H�@�@���������H�aV  ��H�¾�w@ �6v@ �    �@����   �UV  Hc�H��H�H�H��H��H�E�H�H� H��t+�-V  Hc�H��H�H�H��H��H�E�H�H� H��������V  Hc�H��H�H�H��H��H�E�H�H�@H��t,��U  Hc�H��H�H�H��H��H�E�H�H�@H���s�����U  Hc�H��H�H�H��H��H�E�H�H�     ��U  Hc�H��H�H�H��H��H�E�H�H�@    �w  ����UU  ���LU  �U�BU  9������H�U  H��tH�U  H�������H�U  H��tH��T  H�������H��T  H��tH��T  H��������T      �   ��T  Hc�H��H�H�H��H��H�E�H�H� H��t+��T  Hc�H��H�H�H��H��H�E�H�H� H���G�����T  Hc�H��H�H�H��H��H�E�H�H�@H��t,�XT  Hc�H��H�H�H��H��H�E�H�H�@H��������,T  ���#T  �U�T  9��7����    �l  ��������������
������������� ��T  H��S  H��tH��S  H������H��S  H��tH��S  H���e���H��S  H��tH��S  H���J�����S      �   �uS  Hc�H��H�H�H��H��H�E�H�H� H��t+�MS  Hc�H��H�H�H��H��H�E�H�H� H��������"S  Hc�H��H�H�H��H��H�E�H�H�@H��t,��R  Hc�H��H�H�H��H��H�E�H�H�@H��������R  ����R  �U��R  9��7�������H�TS  �H������H�]���UH��H��@H�}�H�u�H�U�H�}� tH�}� u�n����    H�������9  �E�    �E�    �E�    �E�    H�E�H�������H�E�H�E�H�P�H�E�H�� <.t3H�E�H������H�¸    H���H��H��H�P�H�E�H�f� . H�E��E�    �   �E�Hc�H�E�H�� <.uk�E�P�U�Hc�H�E�HЋU���E�+E��E��E�    �0�E�P�U�Hc�H�E�H��E�P�U�Hc�H�E�H�� ��E��E�;E�|��E�    ��E��E��E�H�H;E��j����E�Hc�H�E�H��  �E�Hc�H�E�H��    ��UH��SH��   H��h���H��`���H��X���dH�%(   H�E�1�H��h��� t
H��`��� u�����H�������g  H�E�    H�E�    H��`���H���M���H�E���   �����MQ  �GQ  ��u
�9Q     �3Q  H�H;E�w�r���� $   ��  �Q  Hc�H�E��   H��H����  H�E�H�}� ��  H�E�H�PH�U�H��`���H�� .H��`���H�E��%��P  �B��P  H�E�H�HH�M��HcL�H�E�� <.tH��`���H�E�H�H;E�w���P  H��D� �rP      H�E��%�eP  �B�\P  H�E�H�HH�M��HcL�H�E�� <.tH��`���H�E�H�H;E�w��!P  H��D� �P      H�E��%�P  �B��O  H�E�H�HH�M��HcL�H�E�� <.tH��`���H�E�H�H;E�w���O  H��D� ��O      H�E��%��O  �B��O  H�E�H�HH�M��HcL�H�E�� <.tH�E�H�PH��`���H�H;E�w��YO  H��D� �HO      �>O  Hc�L�E�H�}�H�U�H�E�H��H�M�QM��I��H�Ѻ�w@ H�Ǹ    �C���H��H�E�H������H��H�E�H���Կ��H�H�E�H���ſ��H�H�E�H��趿��H�H��H�E�H�E�H�PH�U�H�U�H�� .H�U�H�E�H��  ǅx���    ǅ|���    �E�    �E�    H�E�H���Y���H�E��cN      �   �XN  Hc�H�E�H�� <.��   �=N  +�x�����|����E��P�U�Hc�H��h���HЋ�x�����E�    �9�E��P�U�Hc�H��h���H���|����P��|���Hc�H�E�H�� ��E��E�;�x���|�ǅx���    ���x�����M  ����M  ��M  H�H;E��4����E�Hc�H��h���H��  �E�Hc�H��X���H�H�}� tH�E�H���d����    �8�藽��� �KM  H�}� tH�E�H���<����w���H�*M  �H������H�u�dH34%(   t����H�]���UH��SH��(H�}�H�u�H�U�H�}� tH�}� tH�}� u�����    H��������  H��L  �   �I   H���p  H��L  H��L  H���_  H�E�H���n���H��L  ��L      H�E�H��L  H�E�H��L  H�`L  H��L  �_L      �H�^L  H��H�SL  H�LL  � <:tH�.L  H�E�H�H�0L  H9�w�H�$L  � <:t*H�L  H�E�H�H�L  H9�t�&���� G   �  H��K  H�E�H�H��K  H9�uH��K  H�E�H�H��K  H9���  H��K  H��H��K  H)�H��H���  �$H��K  H�PH��K  � 0�uK  ���lK  �fK  H�H�eK  H��H�cK  H)�H�ʹ   H)�H��H9�|��)H�LK  H�PH�AK  H�2K  H�JH�'K  ��H�K  H�K  H9�u�H��J  H�E�H�H��J  H9��x���H��J  H�PH��J  H��J  H�JH��J  ��H��J  H��H��J  �8���H��J  H�PH��J  H��J  H�JH��J  ��H��J  H�rJ  H9�u�H�VJ  H�E�H�H�XJ  H9������H�XJ  H�PH�MJ  H�6J  H�JH�+J  ��H�'J  H��H�J  �����H�J  �  H��I  H��轺��H��H�E�H�H��I  H��I  H��蝺��H��H�H��I  H�E�H��I  �^H��I  � <:uH��I  H��H��I  H��I  H�PH��I  H��I  H�J�H��I  ��H�jI  H�PH�_I  � .H�eI  H�6I  H9�s�H�*I  H�KI  H�4I  H�ip6-arpaH��@.H��u��w@ �L�����   H�I  H��	H��H  H��H  �  H��H  �I   �    H������H�E�H��薹��H��H��H  H�M�H��H���m���H�E�H���q���H��H�E�    H���͹��H�vH  H�U�H�E�H��H������H���t%H�VH  H��tH�JH  H���B����    �C����r���� �ZH  H�#H  H��tH�H  H�������J���H�1H  �H������H��([]�UH��H�� H�}�H�u�H�U�H�}� tH�}� tH�}� uL�����    ������ ���o���H��H��9  �x@ H�Ǹ    �C����η���    H�������  H��G  �   �   H���"  H��G  H��G  H���1  H�E�H��� ���H�yG  H�rG  H�P�H�E�H�H�hG  H�QG  H�bG  �>H�YG  H�PH�NG  H�?G  H�J�H�4G  ��H�0G  H�PH�%G  � .H�G  H;E�s�H�G  H�e164.arpH�f�@a H��F  H��	H��F  H��F  �  H��F  H���`���H��H��F  H�E�H��H��跶��H��u�x@ 訸���<H��F  H���'���H��H�E�H�H�nF  H��tH�bF  H���"����    �#�H�KF  H��tH�?F  H�������H��������UH��H��H�}�H�}� u5H��7  H���-   �   �8x@ �e���� ����    H�������5  H�E�H�@H��E  H�E�H�@�@����蹷����H��E  �fx@ ����H�E�H� H��E  ��x@ �    �x����H��E  H�PH��E  � �����W���H��E  � ��t*H��E  H��H�H�H��H��H�E�H�H�tE  H9�w���x@ �e���H�NE  � �����������ƿ�x@ �    �����H�&E  �@�����������ƿ�x@ �    �ĵ����x@ �    赵��H��D  H�@H��D  �H��D  H�PH��D  � ����肴��H��D  � ��t*H��D  H��H�H�H��H��H�E�H�H��D  H9�w���x@ 萴����x@ �    �1���H�jD  H�@H�oD  �H�fD  H�PH�[D  � ���������H�GD  � ��t*H�1D  H��H�H�H��H��H�E�H�H�D  H9�w���x@ ����H��C  H�@H��C  ��   ������C  ��C  ��u
��C     ��C  Hc�H��C  �   H��H���  H��C  H��C  H����   H��C      H��C  H�E�H�pH�~C  �h�` H���   H���u��x@ �A����TH�XC  H�iC  H�H�GC  H�PC  H�ƿy@ �    �޳��H�7C  H��tH�+C  H��裲���    �#�H�C  H��tH�C  H��耲��H��������UH��H�� H�}�H�u�H�U�H�M�H�}� tH�}� tH�}� tH�}� u�~����    H�������  H�E�H��B  ��B      ��B      H��B      H��B      H�E�H��B  �   H��B  � <�v[H��B  � ������H�pB  H��� ���- �  �EB  �?B  Hc�H�E�H�H�BB  � B     �B      H�B  H�PH�B  H�U�H�H�B  � ���A  ��uH��A  H��H��A  H��A  H��H��A  H��A  � ���2�����A  ��u*H��A  H��A  H�H��A  H��A  H��H��A  H��A  H��H��A  H�{A  H��H�QA  H)�H��H��H�E�H��_A      ��   �TA  Hc�H�E�HDA  H�H�HH�E�H�� ��e���H� �$A  Hc�H�U�H����H�H�� ��%   ��u]�2���H� ��@  Hc�H�U�H����H�H�� ��%   ��u*��@  Hc�H�E�H�� <-t��@  Hc�H�E�H�� .��@  ����@  ��@  Hc�H�k@  H9�� ����p@      H�Q@  H�E�H��  �5@      �    ��UH���f�E��E���w/��H��Py@ �� y@ �!�)y@ ��1y@ ��8y@ ��?y@ ��Fy@ ]�UH���f�E��E���t/����t��t�3=�   t=�   t�#��y@ �!��y@ ���y@ ���y@ ���y@ ���y@ ]�UH���f�E��E���+�,  ��+�  ����  ��}���X  ��3���,  �����  �  ���  ���  �  ���5  �����  ���  �i  ���  ���  �R  ��#�S  ��#<���'  �����  ���  �  ���  ��!�  �  ��'�'  ��'��$�  ��%�  ��  ��)�  ��*�  ��  ��<�o  ��<��   ��1�3  ��1<��.�  ��.��,��  ��-��  �  ��/��  ��0��  �p  ��4��  ��4��2��  ��3��  �K  ��7��  ��;��  �4  =�   ��  =�   D=�   ��  =�   ��=��  ��c��  ��  =�   ��  =�   ��  ��  =  ��  =  =�   ��  =   ��  �  = �  ��  =�  ��  �  ��y@ �  ��y@ �  ��y@ �}  ��y@ �s  ��y@ �i  ��y@ �_  ��y@ �U  ��y@ �K  ��y@ �A  ��y@ �7  ��y@ �-  ��y@ �#  �z@ �  �z@ �  �z@ �  �z@ ��   � z@ ��   �&z@ ��   �)z@ ��   �.z@ ��   �4z@ ��   �8z@ �   �<z@ �   �?z@ �   �Ez@ �   �Nz@ �   �Tz@ �   �Yz@ �   �`z@ �|�fz@ �u�lz@ �n�wz@ �g�|z@ �`��z@ �Y��z@ �R��z@ �K��z@ �D��z@ �=��z@ �6��z@ �/��z@ �(��z@ �!��z@ ���z@ ���z@ ���z@ ���z@ ]�UH���f�E��E���wn��H�Ř|@ ���z@ �`��z@ �Y�	{@ �R�"{@ �K�@{@ �D�Z{@ �=�x{@ �6��{@ �/��{@ �(��{@ �!�(|@ ��M|@ ��_|@ ��q|@ ���|@ ]�UH��AUATSH��HH�}��u���H�M�f�E�H�E�    H�E�    H�E�H�E�H�}� u�a����    H�������S  H�E�H�E�H�E�H�E��@
����� ���D��H�E��@�����
���D��H�E��@�����������H�E��@�����߫����E��D��ډƿ0}@ �    質���s}@ �    褪���o���H�H�E�� ��H�H�� ��%   ����   �D���H�H�E�� ��H�H�� ��%   ����   H�E��   ����H�H�E�� ��H�H�� ��%   ��uA����H�H�E�� ��H�H�� ��%   ��uH�E�� <-t�.   ����H�E�H�E�H�PH�U�� �����è��H�E�� ���p���H�E�H�ƿ}}@ �    螩��H�E�H�E�H�E�H�E�� ����萪��f;E�u
�9     �}� t� 9  ��t��}@ ���}@ ���}@ H�E�� �����L�����H�ډƿ�}@ �    �%���H�E��@����������I��H�E��@����������������H��H�E�� ������������������I��H�E��@����t��}@ ���}@ H�E��@�� ��t��}@ ���}@ H�E��@�����tA� ~@ �A��}@ H�E��@����tA�~@ �A��}@ H�E��@����t�~@ ���}@ H�E��@����t�	~@ ���}@ H�E��@�����t�~@ ���}@ H��ATSAPWVM��H�ƿ~@ �    ����H��0�
   �����    H�e�[A\A]]�UH��H��H�}������ ���W���H��H��(  H�U��X~@ H�Ǹ    �'����   �����UH��H�b(  H����  �   �x~@ �����   �����UH��H�� H�}�H�u�H�U�H�U�H�E�H��H��躧��H�E�H�}� uB�J���� ��6  H��'  H���   �   �d�@ 脨������H��6  ��    �H�E���UH��H��H�}�H�u�H�E�H��軧��H�E�H�}� uB�ۥ��� �S6  H��'  H���   �   �m�@ ����谥��H�'6  ��    �H�E���UH��H���}��u��U�M��U��M�E��Ή��2����E��}� yB�d���� ��5  H�'  H���   �   �v�@ 螧���9���H��5  ��������E���UH��H��@�}�H�u�H�UȉM�H�E�H�E�H�E�    H�E�    H�E�H�E��cH�}� yD�ޤ��� �V5  H��&  H���   �   ��@ ����賤��H�*5  �H�������PH�E�H)E�H�E�HE�H�E�HE�H�}� t-�M�H�U�H�u��E܉��;���H������H�E�H�}� �i���H�E���UH��H��@�}�H�u�H�UȉM�H�E�    H�E�    H�E�H�E��*H�}� y���@ ����H�������=H�E�HE�H�E�HE�M�H�U�H�u��E܉�讣��H������H�E�H�}� u�H�E���UH��H��P�}�H�u�H�UȉM�L�E�L�M�H�E�    H�E�    H�E�H�E��*H�}� y���@ 若��H�������BH�E�HE�H�E�HE�L�E�H�}��M�H�U�H�u��E�M��I��������H�E�H�}� �H�E���UH��H��P�}�H�u�H�UȉM�L�E�D�M�H�E�H�E�H�E�    H�E�    H�E�H�E��cH�}� yD����� �Z3  H��$  H���   �   ���@ ����跢��H�.3  �H�������UH�E�H)E�H�E�HE�H�E�HE�H�}� t2D�E�H�}��M�H�U�H�u��E�E��I����董��H�E�H�}� �d���H�E���f.�     f�AWAVA��AUATL�%   UH�-   SI��I��L)�H��H��臡��H��t 1��     L��L��D��A��H��H9�u�H��[]A\A]A^A_Ðf.�     ��  H��H���         [3;31mmain: hostname exceeds maximum number of chars allowed[m
 DHCP_GetNS strncpy in chaos a aaaa axfr ns cname soa ptr mx txt naptr invalid argument for NAPTR any hDS:t:c:x 127.0.1.1     �@     @     @     @     @     �@     @     @     @     @     @     @     @     @     @     @     @     @     @     @     �@     @     @     @     @     @     @     @     @     @     @     @     @     @     @     @     0@     @     @     @     @     �@     @     @     @     @     @     @     @     @     @     @     @     @     @     @     @     �@     DoQuery: ConvertNumberToE164 DoQuery: ConvertName doquery: clock_gettime TCP UDP        [3;02mDNS Response (%3.2lf ms; Protocol %s)[m
 PrintInfoDNS GetRecords       [1;02m[3;32m	ANSWER RESOURCE RECORDS[m      [1;02m[3;32m	AUTHORITATIVE RESOURCE RECORDS[m       [1;02m[3;32m	ADDITIONAL RESOURCE RECORDS[m      ��.A[3;31mrequest timed out[m
   do_tcp: failed to set signal handler for SIGALRM do_tcp: inet_pton do_tcp: socket do_tcp: connect %a %d %b %Y %H:%M:%S  
[3;02mDNS Query (sent %s [TZ %s])[m
 do_tcp: send_a do_tcp: recv_a [3;31mrequest timed out[m
   do_udp: failed to set signal handler for SIGALRM do_udp: inet_ntop do_udp: socket %a %d %b %Y %H:%M:%S  
[3;02mDNS Query (sent %s [TZ %s])[m
 do_udp: sendto_a do_udp: recv [3;31mrequest timed out[m
   DHCP_GetNS: failed to set signal handler for SIGALRM    [3;02mUsing DHCP to obtain primary/secondary DNS servers[m   [3;00mSending DHCP request for local DNS server[m DHCP_GetNS: socket DHCP_GetNS: ioctl DHCP_GetNS: memcpy DHCP_GetNS: socksetopt inet_ntop   [3;02mfound DNS server @%s[m
 <=0 NULL ==NULL size delta ptr buf cnt array [1;02m[3;31m%s%s[m
 GetRecords: memcpy        [3;31mResource record is stale (ttl:[m %u[3;02m)[m
 Chaos Address Chaos Name       [3;02m%18s[m %s [%u]
%18s %0o
 GetAnswers: inet_ntoa IPv4 Address [3;02m%18s[m %s [%u]
 Name Server Canonical Name S# mail domain        [3;02m%18s[m %s
[3;02m%18s[m %s
[3;02m%18s [m%u
[3;02m%18s [m%d
[3;02m%18s [m%d
[3;02m%18s [m%d
[3;02m%18s [m%d
[3;02m%18s[m %u
 TTL MIN EXP RETR REFR Pointer Mail Exchange    [3;02m%18s[m %s ([3;02mpref[m %hu) [%u]
 Txt Record [3;02m%s[m
 GetAnswers: inet_ntop IPv6 Address GetAnswers: HandleNAPTRrecord URI %s.%s.%s.%s.in-addr.arpa ConvertToPtr: strncpy  ConvertNumberToE164: %s
 ConvertNumberToE164      [3;31mHandleNAPTRrecord: record is NULL[m
 [3;02mNAPTR record[m
 [3;02mnumber[m   [3;02morder[m %hu
 [3;02mpreference[m %hu
 [3;02mservices[m  [3;02mregex[m  HandleNAPTRrecord: GetName [3;02mreplacement[m %s   standard inverse status notify update unknown   i`@     p`@     w`@     �`@     ~`@     �`@     internet chaos hesiod none all unknown A NS CNAME SOA PTR HINFO (obsolete) MX TXT RP (obsolete) AFSDB X25 (obsolete) SIG (obsolete) KEY (obsolete) AAAA LOC SRV NAPTR KX CERT DNAME OPT APL DS SSHFP IPSECKEY RRSIG NSEC DNSKEY DHCID NSEC3 NSEC3PARAM TLSA HIP CDS CDNSKEY OPENPGPKEY SPF (obsolete) TKEY TSIG IXFR AXFR ANY URI CAA TA DLV unknown    [1;02m[1;32mno error[m [3;31mformat error[m [3;31mserver failure[m [3;31mnon-existent domain[m [3;31mnot implemented[m [3;31mquery refused[m       [3;31mname exists but should not[m    [3;31mRRSet exists but should not[m   [3;31mRRSet does not exist but should[m       [3;31mserver not authorised for zone[m        [3;31mname not contained in zone[m [3;31mbad SIG[m [3;31mbad key[m [3;31mbad time[m [3;31munknown[m   e@     e@     "e@     )e@     0e@     7e@     >e@     Ee@     Le@     Se@     Ze@     ve@     ve@     ve@     ve@     ve@     ae@     he@     oe@     Questions %hu | Answers %hu | Authoritative %hu | Additional %hu
   Query " " @SERVER %s:53
  [1;02m[[1;32mVALID[m[1;02m][m      [1;02m[[1;31mINVALID[m[1;02m][m    Transaction ID 0x%hx %s
 cd ad ra rd tc aa qr    FLAGS: %s %s %s %s %s %s %s  QTYPE: %s  QCLASS: %s  STATUS: %s
      [1;02m[3;31m%s - %s[m
      usage: orion <hostname> [OPTIONS]
  -D	send DHCP request to find local DNS server(s)
  -S	specify DNS server [default is 127.0.1.1]
  -t	specify the type of query:
		A	IPv4 address
		AAAA	IPv6 address
		PTR	convert IPv4/IPv6 address to name
		NAPTR	find SIP URI for telephone number
		MX	mail exchange information
		TXT	server text file information
		SOA	zone authority information
		NS	name server information
		AXFR	request transfer of zone information
  -x	accept resource records with stale time-to-live


Examples:

orion example.com -t AAAA -S 8.8.8.8
	finds IPv6 address for "example.com"
	using Google's DNS server at 8.8.8.8#53

orion +447123456789 -t NAPTR
	finds SIP URI for +447123456789
	using default DNS resolver at 127.0.1.1#53

orion example.com -t AAAA -D
	finds IPv6 address for "example.com"
	using DNS server found via DHCP request

orion example.com -t NS -x
	finds name server for "example.com" and
	accepts records with stale time-to-live values
 calloc:  malloc:  socket:  send:  recv recvfrom sendto:     ;  "    ���h  `���8  V����  r����  G����  �����  ����  إ��8  ����X  Ǩ��x  +����  ����  �����  _���  5���8  O���`  J����  �����  �����  �����  Q���   U���   ����@  ����h  �����  �����  y����  �����  ^���  3���(  ����H  R���h  @����  �����             zR x�       ���*                  zR x�  $      ����0   FJw� ?;*3$"       D   ����   A�CE�    $   d   �����	   A�CI����	     �   g���A    A�C|      �   ����   A�C
    �   w���A    A�C|      �   �����   A�C�      &���A    A�C|   $   ,  G���d   A�CE�Z      $   T  �����   A�CE��      $   |  ���
   A�CE�          �  ����t   A�Co $   �  G����   A�CH��      $   �  ����   A�CE�           �����   A�C�    4  ����   A�Cz    T  !����   A�C�    t  ����I    A�CD     �  ���^    A�CY     �  I���   A�C�    �  -����    A�C�  $   �  �����   A�CI����       ���D    A�C          <  C���,    A�C          \  O���z    A�Cu     |  ����o    A�Cj     �  ����v    A�Cq     �  N����    A�C�     �  ����    A�C�     �  l����    A�C�       �����    A�C�  D   <  ����e    B�B�E �B(�H0�H8�M@r8A0A(B BBB    �  ����                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   �@     �@                                  �@            Tn@            �`                          �`                   ���o    �@            �@            �@     
                                                  �`            �                           �
@            �
@            H       	              ���o    @
@     ���o           ���o    �	@                                                                                                             (�`                     �@     �@     �@     @     @     &@     6@     F@     V@     f@     v@     �@     �@     �@     �@     �@     �@     �@     �@     @     @     &@     6@     F@     V@     f@     v@     �@     �@     �@     �@     �@     �@     �@     �@     @     @     &@     6@     F@     V@     f@     v@     �@     �@     �@     �@     �@     �@     �@                     GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609 ,             �@                           ,    
       @     �	                      ,    �       �$@     P                      ,    ?       7(@     �                      ,    �)       &+@     �                      ,    
6       �0@     �                      ,    �?       �5@     
                      ,    wI       �L@     t                      ,    �J       �M@     �                      ,    /R       �R@                           ,    �S       �W@     �                      ,    �W       �Y@                           ,    5a       i]@     �                      ,    oc       J`@     I                       ,    Fd       �`@     ^                       ,    Le       �`@                           ,    #f       �d@     �                       ,    �f       }e@     �                      ,    �i       /i@     p                       ,    �l       �i@     5                      

       �  �  q  �@               &  �  int �  $  �    �  9  �B   &  �B   "   �   -  �  �^   �  ;   �  �  W   /  �   m   �   �   �   �    ]   @V    �	0  	�  	�  	A   
�   "  {    �  (�   �  ��  ^  �;    ]  ��   �  ��   C  ��     ��    �  ��   (	  ��   0�	  ��   8�  ��   @�   �   H�  �   P�   �   X�  �  `  �  hM  ;   p  ;   t�  e   x"  P   �2  -   ��  �  �M  �  �  !p   ��	  )�   ��	  *�   ��	  +�   ��	  ,�   ��	  .�   �w
  /;   ��  1  �   �C  ��     ��   �	  ��  v  �;    �  -  
�   �  {     �  
�     {    W   H+  	o   	<   	J   	�  	c  	C  	R  	�   	S  	�
  		  
	f  	  	  	9  	~  	+   	�
  	�  	M  	�  	e  	�  	�  	g  	6  	  	�  	"  	�  	�  	Q  	�   	A  !	_   "	U  #	  $	N  %	�  &	F
  '	�	  (	�  )	'  *	�  +	(  ,	�  -	  .	�  /	�	  0	N  1	$  2	}
  3	�  4	.  5	�  6	&  7	�  8	  9	�  :	�  ;	�  <	j  <	  =	�  >	  ?	2  @	�	  A	�  B	A  C	M  D	  E	3  F	A	  G	�  H	�
  I	T	  J	�  K	p  L	f  M	�  N	X
  O	Y  P	�  Q	�  R	E  S	�  T	
  U	J  V	�  W	q  X	�   Y	�  Z	�  [	�  \	�  ]	[  ^	�  _	�  `	�  a	{  b	�  c	�  d	�  e	�   f	5  g	   h	�  i	�  j	g  k	�  l	q  m	%  n	a  o	�  p	Z  q	g  r		  s	�  t	�  u	
  v	"  w	�  x	  y	�  z	�  {	   |	�  }	�  ~	�  	�
  �	H  �	  �	�  �	h	  �	s  �	  �	S  �	'  �	�  �	v  �	{  �	�   �	�  �	[  �	�  �	�  �		  �	_  �	�  �	�  �	=  �	�  �	�  �	�  �	�  �	  �	t  �	�  �	�  �	1  �	v  �	�  �	i  �	�  �	�	  �	{  �	(	  �	�  �	s  �	c  �	�
  �	�  �	�  �	t  �	�  �	�  �	a  �	0  �	/  �	  �	�  �	4
  �	�  �	�  �	�   �	�
  �	�  �	C  �	  �	  �	�  �	�  �	  �	�  �	�  �	    �	7  �	�  �	t  �	�   �	�  �	+
  �	1  �	�  �	Q  �	3  �	  �	�  �	
  �	  �	�  �	<  �	U  �	p   �	$  � uc 	 I     	'@  "  +  �  ;   �@           ��	  �  ;   �L,  �	  �@c �   	��`     �    	@�`     �    	 �`     ns   	 �`       E  	h�`     p E  	X�`     �  E  	�`     B  ;   	�`     z	  	;   	��`     k 	;   	`�`     �	  
5  	T�`     �  
5  	4�`     �  ��@     �  ��@     j@     J       �	  k -;   �X 7@     E       k 8;   �\  �   �  
9�   �  ��  �  	);   	��`      �
   R  �  �  q  @     �	      �  &  �  int �  $  �  1[   �  �  3m     �  "   �  �B   �  �B   (  �B   -  �  m�   �  �t   �  ;   �  x�     z�    �  {�    �  �  m   	/[  �   m   �   �   �   �    ]   @V    �	0  	�  	�  	A   �  (�   m   
H}  	o   	<   	J   	�  	c  	C  	R  	�   	S  	�
  		  
	f  	  	  	9  	~  	+   	�
  	�  	M  	�  	e  	�  	�  	g  	6  	  	�  	"  	�  	�  	Q  	�   	A  !	_   "	U  #	  $	N  %	�  &	F
  '	�	  (	�  )	'  *	�  +	(  ,	�  -	  .	�  /	�	  0	N  1	$  2	}
  3	�  4	.  5	�  6	&  7	�  8	  9	�  :	�  ;	�  <	j  <	  =	�  >	  ?	2  @	�	  A	�  B	A  C	M  D	  E	3  F	A	  G	�  H	�
  I	T	  J	�  K	p  L	f  M	�  N	X
  O	Y  P	�  Q	�  R	E  S	�  T	
  U	J  V	�  W	q  X	�   Y	�  Z	�  [	�  \	�  ]	[  ^	�  _	�  `	�  a	{  b	�  c	�  d	�  e	�   f	5  g	   h	�  i	�  j	g  k	�  l	q  m	%  n	a  o	�  p	Z  q	g  r		  s	�  t	�  u	
  v	"  w	�  x	  y	�  z	�  {	   |	�  }	�  ~	�  	�
  �	H  �	  �	�  �	h	  �	s  �	  �	S  �	'  �	�  �	v  �	{  �	�   �	�  �	[  �	�  �	�  �		  �	_  �	�  �	�  �	=  �	�  �	�  �	�  �	�  �	  �	t  �	�  �	�  �	1  �	v  �	�  �	i  �	�  �	�	  �	{  �	(	  �	�  �	s  �	c  �	�
  �	�  �	�  �	t  �	�  �	�  �	a  �	0  �	/  �	  �	�  �	4
  �	�  �	�  �	�   �	�
  �	�  �	C  �	  �	  �	�  �	�  �	  �	�  �	�  �	    �	7  �	�  �	t  �	�   �	�  �	+
  �	1  �	�  �	Q  �	3  �	  �	�  �	
  �	  �	�  �	<  �	U  �	p   �	$  � 
uc  I   
us "[   
u16 $P   
u32 %b     '�  [  �  f�  @  h�   rd j�  tc k�  aa l�  �  m�  	qr n�  �  o�  cd p�  ad q�  z r�  ra s�     ��  �  ��  "  ��  �  ��  
 �  ��  ^  ��  ~  ��   �  ��   Z  ��  }  �  
�  �	  ��   �  ��  ttl ��  len ��   �  �<  D  ��   F  �<  �  ��   �  �  �  �  �   @     �	      ��
  �  �  �Hns �  �@�	  �  ���  �  ��  �  	P�`     buf �  	��`     :  �  	H�`     p �  	��`     �  �  	h�`     i ;   	T�`     err ;   	И`     z ;   	��`     s  ;   	��`     z	  ;   	��`     �  	�
  	��`     U  	�
  	��`     �  	�
  	`�`     dns 
�
  	@�`     q �
  	p�`     t1 �   	��`     t2 �   	��`     
  �   	`�`     �  �   	x�`     O  �   	��`     �  �   	Ș`     g  �
  	��`     �  ;   	X�`     �  ��$@      B  �
  {    �  �  l  �  );   	��`      d
   �  �  �  q  �$@     P      G  &  �  int �  $  �  1[   �  �  3m     �  �  }m   9  �B   &  �B     �;   "   /  �B   �  �B   �  �B   �   -  �  �m   �  m�   �  K�   �  �t   �  �      	t   /  
�    �  
  �  �  �  !�   �  m   �    �  �    �  �  W  
P     
    '  	[   �  ��  �  ��   �  ��   	�   �  
�    m   5�  E   �  ]   �  
b   �  
!  �  
!�    m   
)�  t   "  �  �  �  �  �  �    Q  D  !A  )�  .  /�  2�  3,  \*  ^{  b8  g�  l�  ��  �s  �   ��    �  
wP   g  
�  \  
�   �  
��  �  
�  ]  
�   	I   #  
�    )  �   4  ?  ;      J  	B   Z  
�    �  �"�  �  (?   ?  );   @  */  H 	Z  �  
�     �  \�  H   �    ";   ;  #�    t  $�  �  :�   J   O  L�    �  M{    Q-  �  S;      T;   E  U�   YZ  O  [�    �  \{   E  ]�    a�  O  c�    �  d{   c  e;   �  f�  �  g�   o�  t  q�      r�     k�    m�    9  n4   �  s�   w  �  yB    �  z;    ~;     ��    N  �;   �  �m    pE�  W  G�  �  N�  �  V   _rt ^-  �  hZ  �  t�  n  {�  9  �   	;   �  
�    �>�  �  @;    #  A;   �  C;   �  �;      ��  e  U.    �  �  �  !5   /  ;   /  �    �    �  �x  �  #�      +/  \  .;   �3  1y  � x  �  ���  ^  �;    ]  ��   �  ��   C  ��     ��    �  ��   (	  ��   0�	  ��   8�  ��   @�   �   H�  �   P�   �   X�  4  `  :  hM  ;   p  ;   t�  �   x"  [   �2  -   ��  @  �M  P  �  !�   ��	  )�   ��	  *�   ��	  +�   ��	  ,�   ��	  .�   �w
  /;   ��  1V  �   �C  �4     �4   �	  �:  v  �;        	�   P  
�     �  	�   f  
�    tm 8��  m  �;    g  �;   	  �;   3  �;   V  �;   �  �;   �  �;   w  �;   �  �;    M  �B   (+  �#  0 uc  I   �    �$@     A       �2	  �  ;   �l }  �   (%@           �
   buf  	  �h�  �   �` ns  	  �X!s4 �  	`�`     "  ;  	��`     !ret �   	ؚ`     !s ;   	p�`     "�  
  	��`     "�  �   	x�`     "�  ,
  	��`     #�  H.(@      	�   ,
  
�   1 f  "f  �  	��`     $�  �:  %�  );   	��`      E
   �  �    q  7(@     �      M  &  �  int �  $  �  1[   �  �  3m     �  �  }m   9  �B   &  �B     �;   "   /  �B   �  �B   �  �B   �   -  �  �m   �  m�   �  K�   �  �t   �  �      	t   /  
�    �  
  �  �  �  !�   �  m   �    �  �    �  �  W  
P     
    '  	[   �  ��  �  ��   �  ��   	�   �  
�    �  
b   �  
  �  
!�    m   
)�  t   "  �  �  �  �  �  �    Q  D  !A  )�  .  /�  2�  3,  \*  ^{  b8  g�  l�  ��  �s  �   ��    �  
wP   g  
��  \  
�   �  
�  �  
��  ]  
��   	I     
�    
  �        ;      +  	B   ;  
�    �  �"l  �  (    ?  );   @  */  H 	;  |  
�     �  \l  H   �    ";   ;  #�    t  $�  �  :�   J�  O  L�    �  M{    Q  �  S;      T;   E  U�   Y;  O  [�    �  \{   E  ]�    a�  O  c�    �  d{   c  e;   �  f�  �  g�   o�  t  q�      r�     k�    m�    9  n4   �  s�   w�  �  yB    �  z;    ~     ��    N  �;   �  �m    pE}  W  G}  �  N�  �  V�  _rt ^  �  h;  �  t�  n  {�  9  ��   	;   �  
�    �>�  �  @;    #  A;   �  C;   �  �      ��  e  U  �  �  �  �  !     ;     �    �  �  �  �Y  �  #�      +/  \  .;   �3  1Z  � Y  �  ���  ^  �;    ]  ��   �  ��   C  ��     ��    �  ��   (	  ��   0�	  ��   8�  ��   @�   �   H�  �   P�   �   X�    `    hM  ;   p  ;   t�  �   x"  [   �2  -   ��  !  �M  1  �  !�   ��	  )�   ��	  *�   ��	  +�   ��	  ,�   ��	  .�   �w
  /;   ��  17  �   �C  �     �   �	  �  v  �;    �  `  	�   1  
�     �  	�   G  
�    tm 8��  m  �;    g  �;   	  �;   3  �;   V  �;   �  �;   �  �;   w  �;   �  �;    M  �B   (+  �  0 uc  I   �    7(@     A       �	  �  ;   �l �  �   x(@     �      ��	   buf �  �h�  �   �` ns �  �X!s4 �  	`�`     "    	��`     !ret �   	؜`     !s ;   	p�`     "�  �   	x�`     "�  �	  	��`     "�  
  	��`     #�  A+@      G  	�   
  
�   1 "f  |  	��`     $�  �  %�  );   	��`      ~   v  �    q  &+@     �      C  &  �  int �  �  0T   $  �  1f   �  �  3x     �  �  }x   9  �B   &  �B     �;   "   /  �B   �  �B   �  ��   �   -  �  �x   �  m�   �  �   *  �T   �*  �  *    	   :  
�    �    �  �  �  !�   �  x   �    �  �    �  �  W  
P     
    '  f   �  ��  �  ��   �  ��   	�   �  
�    �  	m   �  	  �  	!�    x   	)�  t   "  �  �  �  �  �  �    Q  D  !A  )�  .  /�  2�  3,  \*  ^{  b8  g�  l�  ��  �s  �   ��    �  	w[   g  	��  \  	�   �  	�  �  	��  ]  	��   	T     
�         ;    d  
 9  �  
"9    	
  I  
�    	�   Y  
�    �  o�  {  q    J  r   �  sf   irq tT   dma uT   �  vT    ��  ?  �I   �O  �  ��  �  ��  U  ��  i  ��    ��  �  �4   o  �;   �  �;   �  �Y  )  �I  \  �I  R  ��    9  (~t  �  ��   �  ��       	B   �  
�    �  �"�  �  (t   ?  );   @  *:  H 	�  �  
�     �  \�  H   �    ";   ;  #�    t  $�  �  :�   J5  O  L�    �  M�    Qb  �  S;      T;   E  U�   Y�  O  [�    �  \�   E  ]�    a�  O  c�    �  d�   c  e;   �  f	  �  g	   o�  t  q�      r�     k"    m�    9  n4   �  s�   wC  �  yB    �  z;    ~p     ��    N  �;   �  �x    pE�  W  G�  �  N  �  V5  _rt ^b  �  h�  �  t�  n  {"  9  �C   	;   �  
�    �>  �  @;    #  A;   �  C;   �  �p      ��  e  U  O  �  %  �  !j   d  ;   d  �      O  �  ��  �  #0      +:  \  .;   �3  1�  � �  �  ��1	  ^  �;    ]  ��   �  ��   C  ��     ��    �  ��   (	  ��   0�	  ��   8�  ��   @�   �   H�  �   P�   �   X�  i	  `  o	  hM  ;   p  ;   t�  �   x"  f   �2  -   ��  u	  �M  �	  �  !�   ��	  )�   ��	  *�   ��	  +�   ��	  ,�   ��	  .�   �w
  /;   ��  1�	  �   �C  �i	     �i	   �	  �o	  v  �;    8	  �  	�   �	  
�     1	  	�   �	  
�    uc  T   u8 #I   u16 $[   u32 %m   �	    ��
  op �	   3  �	    �	  �  �	    �	  �  �	  _  �	  
I  �	  v  �	    �	  �  �	  4  �
  �  �
  ,  �
  l 	�	  �
  
�    	�	  �
  
�   ? 	�	  �
  
�      ��	    &+@     A       ��
  �  ;   �l ?  �   g+@     d      �0  ns �	  �X �  0  	p�`     !buf �	  	X�`     !p �	  	x�`     !s ;   	��`     !opt ;   	��`     !ifr O  	��`      �     	؞`     !s4 �  	`�`     !in4 6  	��`      �  �	  	О`      �  <  	��`      �  �   	��`        p  	��`     "�  q�0@      �
  �  	�	  L  
�     f  �  	��`     #�  �o	  $�  );   	��`      y	   W  �  �  q  �0@     �      �
  &  �  int �  $  �  1[   �  �  3m     �  9  �B   &  �B   "   �  �B   �   -  �  m�   �  �t   �  �  �  ��S  ^  �;    ]  ��   �  ��   C  ��     ��    �  ��   (	  ��   0�	  ��   8�  ��   @	�   �   H	�  �   P	�   �   X	�  �  `	  �  h	M  ;   p	  ;   t	�  {   x	"  [   �	2  -   �	�  �  �	M  �  �	  !�   �	�	  )�   �	�	  *�   �	�	  +�   �	�	  ,�   �	�	  .�   �	w
  /;   �	�  1�  � 
  �C  ��     ��   �	  ��  v  �;    Z  �   �   �  �     S  �   �  �    m   	H�  o   <   J   �  c  C  R  �   S  �
  	  
f      9  ~  +   �
  �  M  �  e  �  �  g  6    �  "  �  �  Q  �   A  !_   "U  #  $N  %�  &F
  '�	  (�  )'  *�  +(  ,�  -  .�  /�	  0N  1$  2}
  3�  4.  5�  6&  7�  8  9�  :�  ;�  <j  <  =�  >  ?2  @�	  A�  BA  CM  D  E3  FA	  G�  H�
  IT	  J�  Kp  Lf  M�  NX
  OY  P�  Q�  RE  S�  T
  UJ  V�  Wq  X�   Y�  Z�  [�  \�  ][  ^�  _�  `�  a{  b�  c�  d�  e�   f5  g   h�  i�  jg  k�  lq  m%  na  o�  pZ  qg  r	  s�  t�  u
  v"  w�  x  y�  z�  {   |�  }�  ~�  �
  �H  �  ��  �h	  �s  �  �S  �'  ��  �v  �{  ��   ��  �[  ��  ��  �	  �_  ��  ��  �=  ��  ��  ��  ��  �  �t  ��  ��  �1  �v  ��  �i  ��  ��	  �{  �(	  ��  �s  �c  ��
  ��  ��  �t  ��  ��  �a  �0  �/  �  ��  �4
  ��  ��  ��   ��
  ��  �C  �  �  ��  ��  �  ��  ��  �    �7  ��  �t  ��   ��  �+
  �1  ��  �Q  �3  �  ��  �
  �  ��  �<  �U  �p   �$  � uc  I   us "[   u16 $P   u32 %b   �  �  
�A  �	  ��   �  ��  ttl ��  len ��   �  �  �  �}  D  ��   F  �}  �  ��     �  �L  �  �   �0@     �      �P	  �  P	  �Xcnt �  �Tptr �  �Hbuf �  �@�  �   ���  V	  ��k ;   	�`     z	  ;   	��`     p �  	�`     �  +s5@      �  �   �  ��  �  );   	��`      �	   �	  �  �  q  �5@     
      |  &  �  int �  �  0T   $  �  1f   �  �  3x     �  "   �  �B   �   -  �  m�   �  �   �  ;   �  �  �  m   �  �   �  !�     �"  	   �"  	X  �2  	o  �B   
I   2  �    
[   B  �    
m   R  �    ;  �k  �  ��     x   	/�  �   m   �   �   �   �    ]   @V    �0  �  �  A   �  (�   x   
H�  o   <   J   �  c  C  R  �   S  �
  	  
f      9  ~  +   �
  �  M  �  e  �  �  g  6    �  "  �  �  Q  �   A  !_   "U  #  $N  %�  &F
  '�	  (�  )'  *�  +(  ,�  -  .�  /�	  0N  1$  2}
  3�  4.  5�  6&  7�  8  9�  :�  ;�  <j  <  =�  >  ?2  @�	  A�  BA  CM  D  E3  FA	  G�  H�
  IT	  J�  Kp  Lf  M�  NX
  OY  P�  Q�  RE  S�  T
  UJ  V�  Wq  X�   Y�  Z�  [�  \�  ][  ^�  _�  `�  a{  b�  c�  d�  e�   f5  g   h�  i�  jg  k�  lq  m%  na  o�  pZ  qg  r	  s�  t�  u
  v"  w�  x  y�  z�  {   |�  }�  ~�  �
  �H  �  ��  �h	  �s  �  �S  �'  ��  �v  �{  ��   ��  �[  ��  ��  �	  �_  ��  ��  �=  ��  ��  ��  ��  �  �t  ��  ��  �1  �v  ��  �i  ��  ��	  �{  �(	  ��  �s  �c  ��
  ��  ��  �t  ��  ��  �a  �0  �/  �  ��  �4
  ��  ��  ��   ��
  ��  �C  �  �  ��  ��  �  ��  ��  �    �7  ��  �t  ��   ��  �+
  �1  ��  �Q  �3  �  ��  �
  �  ��  �<  �U  �p   �$  � uc  T   us "f   u16 $[   u32 %m     '  �  �  �  
�g  �	  ��   �  ��  ttl �	  len ��   �  ��  D  �$   F  ��  �  �$   *  �  �g  d  �   �5@     
      ��	  �  �	  �XM  �  �Tbuf $  �H�	    �Pk ;   	 �`     z	  ;   	 �`     err ;   	П`     �  $  	�`     �  $  	�`       $  	�`     D  �	  	��`       �   	H�`     a32 �	  	@�`     6  �	  	��`     {  �	  	x�`     a16 �	  	8�`       �	  	P�`     �  �	  	X�`     R  �	  	`�`     �  �	  	h�`     min �	  	p�`     p $  	(�`     �  �   	0�`     �  �8K@     +  +�7@      �  
�   �	  �   - �   R  �  	  �  );   	��`      i   �
  �  �  q  �L@     t      �  &  �  int �  $  �    �  "   �  �B   -  �  ml   �  �^   �  �  uc  I   �   �  ~   �L@     t      �Q  :  �   �H�  �   �@
  Q  ��	�  ;   �T	  ;   �X	�  ;   �\
cnt ;   �`
k ;   �d
len �   �h �   �  );   	��`      G   �  �  �  q  �M@     �        &  �  int �  $  �    �  "   �  �B   -  �  ml   �  �^   �  �  W   H�  o   <   J   �  c  C  R  �   S  �
  	  
f      9  ~  +   �
  �  M  �  e  �  �  g  6    �  "  �  �  Q  �   A  !_   "U  #  $N  %�  &F
  '�	  (�  )'  *�  +(  ,�  -  .�  /�	  0N  1$  2}
  3�  4.  5�  6&  7�  8  9�  :�  ;�  <j  <  =�  >  ?2  @�	  A�  BA  CM  D  E3  FA	  G�  H�
  IT	  J�  Kp  Lf  M�  NX
  OY  P�  Q�  RE  S�  T
  UJ  V�  Wq  X�   Y�  Z�  [�  \�  ][  ^�  _�  `�  a{  b�  c�  d�  e�   f5  g   h�  i�  jg  k�  lq  m%  na  o�  pZ  qg  r	  s�  t�  u
  v"  w�  x  y�  z�  {   |�  }�  ~�  �
  �H  �  ��  �h	  �s  �  �S  �'  ��  �v  �{  ��   ��  �[  ��  ��  �	  �_  ��  ��  �=  ��  ��  ��  ��  �  �t  ��  ��  �1  �v  ��  �i  ��  ��	  �{  �(	  ��  �s  �c  ��
  ��  ��  �t  ��  ��  �a  �0  �/  �  ��  �4
  ��  ��  ��   ��
  ��  �C  �  �  ��  ��  �  ��  ��  �    �7  ��  �t  ��   ��  �+
  �1  ��  �Q  �3  �  ��  �
  �  ��  �<  �U  �p   �$  � uc  I   �  	�  ~   �M@     �      �  
D  �  ��~
�  �  ��~

    ��~err ;   	ܟ`     i ;   	؟`     z	  ;   	ԟ`     _1 %  ��_2 %  ��_3 %  �@_4 %  �Pptr �  ��~tmp �  ��  �   ���  H�R@     �  .�   ��cnt 1;   ��~  1;   ��~�  1;   ��~k 1;   ��~len 2�   �� �   �  5  e    �  );   	��`      �   l  �  �  q  �R@           �  &  �  int �  $  �    �  "   �  �B   -  �  ml   �  �^   �  �  uc  I   �   �  ~   �R@           ��  out �   �Xin �   �P	�  �  �H
tmp �   	��`     
p1 �   	��`     
p2 �   	 �`     
t �   	�`     
k ;   	�`     
err ;   	�`     
len �   	�`     �  P�W@      �   �  );   	��`      �      �     q  �W@     �        &  �  int �  $  �    �  9  �B   &  �B   "   �  �B   �   -  �  m�   �  �^   �  �  �  ��=  ^  �;    ]  ��   �  ��   C  ��     ��    �  ��   (	  ��   0�	  ��   8�  ��   @	�   �   H	�  �   P	�   �   X	�  u  `	  {  h	M  ;   p	  ;   t	�  e   x	"  P   �	2  -   �	�  �  �	M  �  �	  !p   �	�	  )�   �	�	  *�   �	�	  +�   �	�	  ,�   �	�	  .�   �	w
  /;   �	�  1�  � 
  �C  �u     �u   �	  �{  v  �;    D  �   �   �  {     =  �   �  {    uc  I   �    �   �W@     �      �u  1  �  �h8  �  �`�  u  �Xtmp �  	�`     p �  	(�`     t �  	0�`     *  �   	 �`     k ;   	8�`     �  +�Y@      �   �  �{  �  );   	��`      �	   6  �  Q  q  �Y@           a  &  �  int �  $  �  �  3b     �  9  �B   &  �B   "   �  �B   �   -  �  m�   �  �i   �  �  �  ��H  ^  �;    ]  ��   �  ��   C  ��     ��    �  ��   (	  ��   0�	  ��   8�  ��   @	�   �   H	�  �   P	�   �   X	�  �  `	  �  h	M  ;   p	  ;   t	�  p   x	"  P   �	2  -   �	�  �  �	M  �  �	  !{   �	�	  )�   �	�	  *�   �	�	  +�   �	�	  ,�   �	�	  .�   �	w
  /;   �	�  1�  � 
  �C  ��     ��   �	  ��  v  �;    O  �   �   �  �     H  �   �  �    b   	H�  o   <   J   �  c  C  R  �   S  �
  	  
f      9  ~  +   �
  �  M  �  e  �  �  g  6    �  "  �  �  Q  �   A  !_   "U  #  $N  %�  &F
  '�	  (�  )'  *�  +(  ,�  -  .�  /�	  0N  1$  2}
  3�  4.  5�  6&  7�  8  9�  :�  ;�  <j  <  =�  >  ?2  @�	  A�  BA  CM  D  E3  FA	  G�  H�
  IT	  J�  Kp  Lf  M�  NX
  OY  P�  Q�  RE  S�  T
  UJ  V�  Wq  X�   Y�  Z�  [�  \�  ][  ^�  _�  `�  a{  b�  c�  d�  e�   f5  g   h�  i�  jg  k�  lq  m%  na  o�  pZ  qg  r	  s�  t�  u
  v"  w�  x  y�  z�  {   |�  }�  ~�  �
  �H  �  ��  �h	  �s  �  �S  �'  ��  �v  �{  ��   ��  �[  ��  ��  �	  �_  ��  ��  �=  ��  ��  ��  ��  �  �t  ��  ��  �1  �v  ��  �i  ��  ��	  �{  �(	  ��  �s  �c  ��
  ��  ��  �t  ��  ��  �a  �0  �/  �  ��  �4
  ��  ��  ��   ��
  ��  �C  �  �  ��  ��  �  ��  ��  �    �7  ��  �t  ��   ��  �+
  �1  ��  �Q  �3  �  ��  �
  �  ��  �<  �U  �p   �$  � uc  I   us "P   u32 %W   �  �  
�+  �	  ��   �  ��  ttl ��  len ��   �  �\  D  ��   F  �\  �  ��   �  �  �+  m  $��  ~  ��   {  ��  _  ��  �  ��  x  ��  e  ��   m  �m  ?  �   �Y@           ��	  J  �	  �hp �  	P�`     D  �  	`�`     len �   	H�`     �  �   	h�`     z	  ;   	X�`     �  �	  	@�`     �  5E]@      b  �  �  ��  �  );   	��`      6   s  �  �  q  i]@     �      �  &  �  int �  $  �    �  "   �  �B   -  �  ml   �  �^   �  ;   �  �  W   /
  �   m   �   �   �   �    ]   @V    �0  �  �  A   �  (�   uc  I     '*  	
  
  �  ~   i]@     �      �  P  /  �hbuf /  �`1  /  �X�    �Pi ;   	��`       ;   	|�`     len �   	��`     �  �   	��`       /  	p�`     p /  	��`     �  	  	x�`      
�   �  );   	��`      �    Q  �  �  q  J`@     I       V  &  �  int �  $  �    �  "   -  �  �  uc  I   �   �  �   J`@     I       ��   �  P   �l �  );   	��`         �  �  �  q  �`@     ^       �  &  �  int �  $  �    �  "   -  �  �  uc  I     W   [�   �       �  ��  � �   �  �   �`@     ^       ��   	�  P   �l 
�  );   	��`      �    c  �  +  q  �`@           �  &  �  int �  $  �    �  "   -  �  �  uc  I   �   "  �   �`@           ��   ~  P   �l �  );   	��`      �    �  �  ?  q  �d@     �       n  &  �  int �  $  �    �  "   -  �  �  uc  I   �   6  �   �d@     �       ��   �  P   �l �  );   	��`      �   [  �  J  q  }e@     �      �  &  �  int �  $  �  1[   �    �  "   �  �B   -  �  mw   �  �  b   /�   �   m   �   �   �   �    ]   @V    �0  �  �  A   uc  I   us "[   u16 $P   	�  f�  
@  h	   rd j	  tc k	  aa l	  �  m	  	qr n	  �  o	  cd p	  ad q	  z r	  ra s	   
  �	  
�  �	  
"  �	  
�  �	  
 �  �  	^  �$  
~  �	   
�  �	   Z  ��  �   Y  �   }e@     �      ��  ptr /  ��f  ;   ��    ��ns /  ��d �  �@q �  �Hp /  ��ok 	;   	��`      �  $  �  );   	��`      �   k  �  w  q  /i@     p       �  &  �  int �  $  �    �  9  �B   &  �B   "   �   -  �  �^   �  �  �   �   �  ��2  	^  �;    	]  ��   	�  ��   	C  ��   	  ��    	�  ��   (		  ��   0	�	  ��   8	�  ��   @
�   �   H
�  �   P
�   �   X
�  j  `
  p  h
M  ;   p
  ;   t
�  e   x
"  P   �
2  -   �
�  v  �
M  �  �
  !p   �
�	  )�   �
�	  *�   �
�	  +�   �
�	  ,�   �
�	  .�   �
w
  /;   �
�  1�  �   �C  �j  	   �j   	�	  �p  	v  �;    9  �   �   �  {     2  �   �  {    k  /i@     D       ��  r  �   �h }  si@     ,       ��  �p  �  );   	��`      �   i  �  �  q  �i@     5      t  &  �  int �  $  �    �  9  �B   &  �B   "   �  �B   �   -  �  �W   �  m�   �  �^   �  �  �  !�   '  P   �  �  �  ��    �  �   	�     
{    �  ��  ^  �;    ]  ��   �  ��   C  ��     ��    �  ��   (	  ��   0�	  ��   8�  ��   @�   �   H�  �   P�   �   X�  �  `  �  hM  ;   p  ;   t�  e   x"  P   �2  -   ��  �  �M  �  �  !p   ��	  )�   ��	  *�   ��	  +�   ��	  ,�   ��	  .�   �w
  /;   ��  1�  �   �C  ��     ��   �	  ��  v  �;    �    	�   �  
{     �  	�   �  
{    uc 	 I   �  �  �   �i@     z       �W  p �   �h�  �   �`�	  �   �X �  �   j@     o       ��  p �   �h�	  �   �` �  ";   �j@     v       ��  s ";   �l�  ";   �h�	  ";   �d�  ";   �` �  0�   �j@     �       �|  �  0;   �Lbuf 0  �@�  0�   ��_  0;   �H�  2�   �P�  2�   �Xret 3�   �hp 4  �` �  G�   �k@     �       ��  �  G;   �Lbuf G  �@�  G�   ��_  G;   �Hret I�   �h�  J�   �Xp K  �` �  [�   \l@     �       ��  �  [;   �Lbuf [  �@�  [�   ��_  [;   �Hsa [�  ��ss [�  ��ret ]�   �h�  ]�   �Xp ^  �` �   �   �  n�   �l@     �       �W  �  n;   �Lbuf n  �@�  n�   ��_  n;   �Hsa n�  ��ss n�   ���  p�   �Pret q�   �h�  q�   �Xp r  �` err ;   	��`     �  ��  �  	);   	��`      %  $ >  $ >   :;I      I  I:;  (   	(   
I  ! I/  :;   :;I8   :;I8   :;   :;I  5 I  .?:;'I@�B   :;I  4 :;I  4 :;I  
 :;      4 :;I?<  4 :;I?   %  $ >  $ >   :;I  :;   :;I8  I:;  (   	(   
 :;I  5 I   :;I8   :;I8   I   :;I8  .?:;'I@�B   :;I   :;I  4 :;I  4 :;I  
 :;  I  ! I/  4 :;I?   %  $ >  $ >   :;I      I  :;   :;I8  	I  
! I/  I:;  (   (   (   :;  I:;  & I  '   I  :;   :;I  :;   :;I   '   :;I8   :;  :;   :;I  .:;'@�B   :;I  .?:;'I@�B    :;I  !4 :;I  "4 :;I  #
 :;  $4 :;I?<  %4 :;I?   %  $ >  $ >   :;I      I  :;   :;I8  	I  
! I/  I:;  (   (   (   :;  I:;  & I  '   I  :;   :;I  :;   :;I   '   :;I8   :;  :;   :;I  .:;'@�B   :;I  .?:;'I@�B    :;I  !4 :;I  "4 :;I  #
 :;  $4 :;I?<  %4 :;I?   %  $ >  $ >   :;I      I  :;   :;I8  	I  
! I/  I:;  (   (   (   :;  I:;  '   I   :;I8  :;   :;I  :;   :;I   '   :;I8   :;   :;I  .:;'@�B   :;I  .?:;'I@�B   :;I   4 :;I  !4 :;I  "
 :;  #4 :;I?<  $4 :;I?   %  $ >  $ >   :;I      I  :;   :;I8  	 :;I8  
 :;  I  ! I/  I:;  (    :;I   :;I8  .?:;'I@�B   :;I   :;I  4 :;I  4 :;I  
 :;  4 :;I?<  4 :;I?   %  $ >  $ >   :;I   I  :;   :;I8  :;  	 :;I  
I  ! I/  I:;  (   (    :;I  5 I   :;I8  .?:;'I@�B   :;I   :;I  4 :;I  4 :;I  
 :;  4 :;I?   %  $ >  $ >   :;I   :;I   I  .?:;'I@�B   :;I  	4 :;I  
4 :;I  4 :;I?   %  $ >  $ >   :;I  I:;  (    :;I   I  	.?:;'I@�B  
 :;I  4 :;I  4 :;I  
 :;  I  ! I/  4 :;I?   %  $ >  $ >   :;I   :;I   I  .?:;'I@�B   :;I  	 :;I  
4 :;I  
 :;  4 :;I?   %  $ >  $ >   :;I      I  :;   :;I8  	 :;I8  
 :;  I  ! I/   :;I  .?:;'I@�B   :;I  4 :;I  4 :;I  
 :;  4 :;I?<  4 :;I?   %  $ >  $ >   :;I      I  :;   :;I8  	 :;I8  
 :;  I  ! I/  I:;  (    :;I   :;I8  .?:;'I@�B   :;I  4 :;I  4 :;I  
 :;  4 :;I?<  4 :;I?   %  $ >  $ >   :;I  I:;  (   (    :;I  	5 I  
 I  .?:;'I@�B   :;I   :;I  4 :;I  4 :;I  4 :;I?   %  $ >  $ >   :;I   I  .?:;'I@�B   :;I  4 :;I?   %  $ >  $ >   :;I  I:;  (    I  .?:;'I@�B  	 :;I  
4 :;I?   %  $ >  $ >   :;I   I  .?:;'I@�B   :;I  4 :;I?   %  $ >  $ >   :;I   I  .?:;'I@�B   :;I  4 :;I?   %  $ >  $ >   :;I  I:;  (   (    :;I  	:;  
 :;I8   :;I8   :;I8   I  .?:;'I@�B   :;I   :;I  4 :;I  4 :;I?   %  $ >  $ >   :;I      I  & I  :;  	 :;I8  
 :;I8   :;  I  ! I/  .?:;'@�B   :;I  . ?:;'�@�B  4 :;I?<  4 :;I?   %  $ >  $ >   :;I      I  :;   :;I8  	I  
! I/   :;I8   :;   :;I  .?:;'I@�B   :;I   :;I  4 :;I  4 :;I  4 :;I?<  4 :;I?   �   �   �      /usr/include/x86_64-linux-gnu/bits /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include  main.c    types.h   stddef.h   sigset.h   signal.h   libio.h   ctype.h   confname.h   dns.h    getopt.h   stdio.h     	�@     �gY���#ɭ\"�Z[����Z+� � # J�u�u�Z& � # JL = ;L;M�Zv�Zv�Zv�Zv�Zv�Zv�Zv�Z��u�; � �%�70 � ��>2*K0v�0v�2�#0Z�X� +�=v���&2& ��J�2<' ��
 L     �      /usr/include /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include  DoQuery.c    stdint.h   types.h   types.h   stddef.h   sigset.h   time.h   signal.h   ctype.h   confname.h   dns.h      	@     ���0 � # # # # # y# �
��!��K��ɼ�3 � ���%^%\�7��$�%�gv!�==/�5�5/�Dd X t X " � �(���N�v�Rv�Ry��?��?��; ���~t7<
<<B<B
<BB'� ���u    �  �      /usr/include /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include/netinet  DoTCP.c    stdint.h   types.h   types.h   time.h   stddef.h   sigset.h   socket.h   sockaddr.h   in.h   socket.h   setjmp.h   setjmp.h   siginfo.h   signal.h   sigaction.h   libio.h   stdio.h   dns.h    socket_type.h     	�$@     �h��#2= t tv��=����u�=���$�h�����"'�.域/��!=i�CA u �   �  �      /usr/include /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include/netinet  DoUDP.c    stdint.h   types.h   types.h   time.h   stddef.h   sigset.h   socket.h   sockaddr.h   in.h   setjmp.h   setjmp.h   siginfo.h   signal.h   sigaction.h   libio.h   stdio.h   dns.h    socket_type.h     	7(@     �h��#2= t t �v��=�����u�=���$���"',�0�.��=q�A@	 u F   �  �      /usr/include /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include/netinet /usr/include/net  DHCP_GetNS.c    stdint.h   types.h   types.h   stddef.h   sigset.h   socket.h   sockaddr.h   in.h   ethernet.h   if.h   setjmp.h   setjmp.h   siginfo.h   signal.h   sigaction.h   libio.h   dns.h    stdio.h   socket_type.h     	&+@     �h��#2�v��=��埭u�0�=�/==���������=KK��=0�$�������1��&�&�6h�(�=!��#�=��.4<>?  ��v �   �   �      /usr/include /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include  GetRecords.c    stdint.h   types.h   types.h   stddef.h   libio.h   dns.h    stdio.h   confname.h     	�0@     # t t t t t�v t t t t t t t t r Xu t t t t t t t t s X � ����Z�lDuc@�!�s � pF � �ot>?A
 u k   "  �      /usr/include /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include/netinet  GetAnswers.c    stdint.h   types.h   types.h   stddef.h   sigset.h   in.h   signal.h   ctype.h   confname.h   dns.h      	�5@     � t tv����777�9�$�WOS(�<+)u�z�@$5!WOuX+���$,WOZ+)x$,WOZ+)x$,WOZ+)u)u��!��!��!��!�� r$�s�<t�<u�
<v�hWOZ+)x$,WOZ+��!)y$��&WOZ=+0 ( ��!�m�WOZ+�#�F$%Z2�+)x$,Z (+ ),OqX9 �~  ��  �� �� ��� (+ ) , �%�~�>><<<<?'<<,< � �� �� ��� (+ ) , �%�u    �   �      /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include  ConvertName.c    types.h   types.h   stddef.h   dns.h      	�L@     = tv����Y3�>u� � , J�u0 uJ J���Y �   �   �      /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include  ConvertToPtr.c    types.h   types.h   stddef.h   confname.h   dns.h      	�M@     �� ��Y��0����,��/% �>ɟY/% �>ɟY/% �>ɟY/% �vɟADY�"���!� � 5 J��0 ut � /� t�Pt3 � t�� u� q   �   �      /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include  ConvertToPtr6.c    types.h   types.h   stddef.h   dns.h      	�R@     K t tv��0/�$�/ �� ���Z ���/$//)0�@)0�;oX ��$�0�!)E5���!��%�� ��Ct;<%� ���u D   �   �      /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include  ConvertNumberToE164.c    types.h   types.h   stddef.h   libio.h   stdio.h   dns.h      	�W@     = t tv�*��0/��0)G��!�-�� ��jt  ��u �   �   �      /usr/include /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include  HandleNAPTRrecord.c    stdint.h   types.h   types.h   stddef.h   libio.h   dns.h    stdio.h   confname.h     	�Y@     �vɭ�����/� �*�()�!/� �*��!/� �*�!��7�)�u� ��wt  ��u m   �   �      /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include  GetName.c    types.h   types.h   stddef.h   sigset.h   signal.h   dns.h    ctype.h     	i]@     { t t tv��5��0g��%� u    �*!��( 3 3g * ����Y K    +   �       GetOpCode.c    dns.h      	J`@     �MwwwwwZ L    +   �       GetQClass.c    dns.h      	�`@     �*wwwwwZ u    *   �       GetQType.c    dns.h      	�`@     ������������������������������wwwwwwwwwwwwwwwwwwZ S    *   �       GetRCode.c    dns.h      	�d@     �MwwwwwwwwwwwwwwZ �   �   �      /usr/include /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys  PrintInfoDNS.c    stdint.h   types.h   types.h   dns.h    ctype.h     	}e@     Ƀ��v���]ML=�� ++YZ ' '��Zy�	�YY�g�h � t rY�fv�	�w �x�y� J t ^ z� J t ] � 	J 
� j � J � i � J t Z � J t Y � J t X $ �Y �    �   �      /usr/include/x86_64-linux-gnu/bits /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include  lib.c    types.h   stddef.h   libio.h   stdio.h   dns.h      	/i@     �.�K!�
 �   �   �      /usr/include/x86_64-linux-gnu/bits /usr/include/x86_64-linux-gnu/sys /usr/lib/gcc/x86_64-linux-gnu/5/include /usr/include  wrappers.c    types.h   types.h   stddef.h   socket.h   sockaddr.h   libio.h   stdio.h   dns.h      	�i@     =����wK2�h���wK2=����w=2Y�0v��󒃃u� t-K2Y���0v���x�
)K2��0v���x�
.K2Ƀ�0v��󒃃u� t2K _SC_LEVEL3_CACHE_SIZE _SC_INT_MAX sizetype _SC_MAPPED_FILES _SC_CHILD_MAX _SC_CLK_TCK _ISgraph _SC_SIGQUEUE_MAX _SC_THREAD_ROBUST_PRIO_INHERIT _SC_DEVICE_IO _IO_save_end _SC_LEVEL4_CACHE_ASSOC _SC_JOB_CONTROL _SC_CHAR_MAX _SC_TRACE_INHERIT _SC_XOPEN_VERSION _IO_write_base _SC_GETGR_R_SIZE_MAX _SC_PII_SOCKET _SC_CHAR_MIN DNS_SRV_FL _lock _SC_SAVED_IDS _SC_SCHAR_MAX _SC_ARG_MAX _SC_TIMEOUTS _IO_save_base _SC_CHAR_BIT _SC_AIO_LISTIO_MAX _SC_MB_LEN_MAX _SC_THREAD_PRIO_PROTECT _SC_PII_INTERNET _SC_THREAD_PROCESS_SHARED _chain _SC_TRACE_NAME_MAX _cur_column _SC_PII _SC_AIO_PRIO_DELTA_MAX _SC_MEMORY_PROTECTION _SC_OPEN_MAX _SC_THREAD_THREADS_MAX _SC_READER_WRITER_LOCKS _SC_STREAMS _SC_MQ_OPEN_MAX _SC_FIFO _SC_2_C_VERSION GNU C11 5.4.0 20160609 -mtune=generic -march=x86-64 -g -fstack-protector-strong _SC_THREAD_ROBUST_PRIO_PROTECT _IO_marker _SC_BC_DIM_MAX _ISprint _SC_THREAD_ATTR_STACKADDR _SC_V7_ILP32_OFF32 _SC_UINT_MAX _SC_2_PBS_LOCATE _SC_LEVEL1_DCACHE_LINESIZE _IO_FILE classStr _SC_TRACE_EVENT_FILTER _SC_NL_LANGMAX _SC_V6_LP64_OFF64 _SC_NL_MSGMAX unsigned char _SC_PII_OSI_CLTS _SC_LEVEL1_ICACHE_ASSOC _SC_DEVICE_SPECIFIC_R _SC_NZERO _SC_XOPEN_XPG2 _SC_XOPEN_XPG3 _SC_XOPEN_XPG4 _SC_ULONG_MAX _SC_XOPEN_ENH_I18N _SC_INT_MIN _SC_2_UPE _SC_PAGESIZE _SC_FILE_SYSTEM _SC_SINGLE_PROCESS _IO_lock_t _SC_2_VERSION _SC_RE_DUP_MAX _SC_LEVEL3_CACHE_ASSOC _SC_2_FORT_DEV _IO_read_ptr _SC_IOV_MAX _pos _SC_THREAD_CPUTIME _SC_SYSTEM_DATABASE _SC_PII_INTERNET_DGRAM _SC_FILE_LOCKING _markers _ISpunct _SC_SCHAR_MIN _SC_UIO_MAXIOV _SC_VERSION telno _SC_SPIN_LOCKS _SC_LEVEL1_DCACHE_SIZE _SC_RAW_SOCKETS _SC_THREADS _SC_THREAD_SAFE_FUNCTIONS _SC_WORD_BIT _SC_REGEXP _SC_ASYNCHRONOUS_IO _SC_CHARCLASS_NAME_MAX _SC_LEVEL1_DCACHE_ASSOC _SC_LEVEL3_CACHE_LINESIZE _SC_THREAD_STACK_MIN _SC_XOPEN_UNIX _SC_NL_TEXTMAX _flags2 _SC_PII_INTERNET_STREAM _SC_V6_ILP32_OFFBIG _IO_read_base _SC_V7_ILP32_OFFBIG _SC_SEMAPHORES _SC_LEVEL4_CACHE_SIZE _unused2 _SC_MULTI_PROCESS _SC_NPROCESSORS_ONLN _SC_USER_GROUPS _ISxdigit q_class argc _SC_2_LOCALEDEF _SC_LINE_MAX _old_offset _SC_PHYS_PAGES _SC_XOPEN_REALTIME argv _SC_SIGNALS _SC_MONOTONIC_CLOCK _SC_RTSIG_MAX _SC_FILE_ATTRIBUTES _SC_USER_GROUPS_R _SC_XOPEN_XCU_VERSION _SC_PII_XTI _SC_XOPEN_REALTIME_THREADS long long int _SC_LONG_BIT _SC_XBS5_ILP32_OFFBIG _ISupper main.c _IO_write_end _SC_UCHAR_MAX _SC_PIPE _SC_TYPED_MEMORY_OBJECTS _SC_LOGIN_NAME_MAX _SC_THREAD_KEYS_MAX _SC_ADVISORY_INFO hostmax _IO_buf_base q_type _SC_COLL_WEIGHTS_MAX _SC_PII_OSI_M _SC_2_C_DEV __pad1 __pad2 __pad3 __pad4 __pad5 _SC_SYSTEM_DATABASE_R _sbuf _SC_TRACE_EVENT_NAME_MAX _SC_USHRT_MAX _SC_IPV6 _SC_HOST_NAME_MAX _SC_BC_STRING_MAX _SC_THREAD_PRIORITY_SCHEDULING _mode _SC_2_SW_DEV _SC_REALTIME_SIGNALS _SC_XBS5_LPBIG_OFFBIG _SC_2_PBS_ACCOUNTING _SC_THREAD_DESTRUCTOR_ITERATIONS _SC_TRACE_LOG _SC_MEMLOCK _SC_PII_OSI _SC_DELAYTIMER_MAX _SC_2_FORT_RUN _SC_V7_LP64_OFF64 _SC_NPROCESSORS_CONF _SC_SHRT_MAX _SC_AIO_MAX _SC_BARRIERS _SC_REGEX_VERSION _SC_LEVEL2_CACHE_LINESIZE _SC_DEVICE_SPECIFIC _SC_SHARED_MEMORY_OBJECTS _ISalpha _SC_XBS5_LP64_OFF64 long long unsigned int _SC_V7_LPBIG_OFFBIG _SC_NL_ARGMAX _ISblank __off_t _ISalnum _SC_AVPHYS_PAGES _SC_XOPEN_SHM _SC_THREAD_SPORADIC_SERVER _ISspace _SC_SS_REPL_MAX _SC_NL_NMAX _IScntrl __sig_atomic_t _IO_backup_base _shortbuf _SC_2_PBS_MESSAGE _SC_SEM_NSEMS_MAX _SC_T_IOV_MAX _SC_PRIORITIZED_IO _next __off64_t _SC_V6_ILP32_OFF32 _SC_STREAM_MAX _SC_TZNAME_MAX _SC_2_PBS_CHECKPOINT _SC_SPAWN _SC_MEMLOCK_RANGE _SC_BC_SCALE_MAX _SC_TRACE_SYS_MAX _SC_SYMLOOP_MAX _IO_buf_end _SC_2_CHAR_TERM _SC_SHELL _SC_FD_MGMT host _SC_XOPEN_CRYPT _SC_LEVEL1_ICACHE_LINESIZE _SC_C_LANG_SUPPORT_R _SC_TRACE_USER_EVENT_MAX _SC_TIMER_MAX _SC_2_PBS _ISlower _SC_CPUTIME _SC_SPORADIC_SERVER stderr short int _SC_CLOCK_SELECTION _SC_XBS5_ILP32_OFF32 _SC_NETWORKING _SC_LEVEL4_CACHE_LINESIZE _SC_ATEXIT_MAX _atomic_ _SC_PII_OSI_COTS _SC_MQ_PRIO_MAX _vtable_offset _SC_SEM_VALUE_MAX _SC_C_LANG_SUPPORT _SC_TIMERS _SC_PASS_MAX _SC_FSYNC _SC_SELECT __got_type _SC_TTY_NAME_MAX STALE_OK typeStr _SC_POLL _SC_MESSAGE_PASSING _IO_read_end _SC_LEVEL2_CACHE_ASSOC _SC_BC_BASE_MAX _SC_LEVEL2_CACHE_SIZE _SC_EXPR_NEST_MAX _SC_SYNCHRONIZED_IO _fileno _SC_XOPEN_STREAMS _SC_SHRT_MIN _SC_2_PBS_TRACK optarg _SC_NGROUPS_MAX _SC_V6_LPBIG_OFFBIG _ISdigit short unsigned int _SC_LEVEL1_ICACHE_SIZE _SC_EQUIV_CLASS_MAX _IO_write_ptr _SC_BASE _SC_PRIORITY_SCHEDULING _SC_SSIZE_MAX _SC_GETPW_R_SIZE_MAX _SC_XOPEN_LEGACY _SC_THREAD_PRIO_INHERIT /home/oppa/Projects/Network/DNS _SC_TRACE _SC_NL_SETMAX __err _SC_2_C_BIND _SC_THREAD_ATTR_STACKSIZE __ssize_t answers DoQuery.c DNS_RRECORD rdata qdcnt qnamelen transaction_id nscnt __syscall_slong_t qname ident resource trcvd auth DNS_QUESTION diff double __USED_TCP qtype tosend DNS_HEADER timespec opcode uint16_t addit delta ancnt __time_t arcnt e164 DoQuery tv_nsec qclass V6_PTR uint32_t rcode tv_sec tm_hour __jmp_buf si_addr si_errno IPPROTO_MTP IPPROTO_PIM IPPROTO_DCCP IPPROTO_TP sa_flags __sighandler_t _lower IPPROTO_ENCAP IPPROTO_IGMP in_addr_t IPPROTO_AH _arch sigjmp_buf IPPROTO_RSVP si_stime IPPROTO_UDP __jmp_buf_tag size _sigchld __sigchld_clock_t _upper si_overrun SOCK_STREAM IPPROTO_BEETPH si_addr_lsb si_sigval si_pid tm_mon sin_zero __ALARM_ENV__ IPPROTO_IP SOCK_RDM tm_year SOCK_DCCP s_addr DoTCP.c __val seed si_utime IPPROTO_ESP __sigset_t SHUT_WR sin_addr si_uid tm_isdst SOCK_DGRAM siginfo_t SOCK_NONBLOCK sival_int IPPROTO_ICMP __clock_t _sigsys IPPROTO_IPV6 _syscall _pad sin_family tm_min _sigpoll tm_yday catch_sigalarm _sifields IPPROTO_MAX __jmpbuf si_tid tstring sin_port sa_family __socket_type IPPROTO_PUP _sigfault IPPROTO_SCTP IPPROTO_RAW IPPROTO_IDP __pid_t _call_addr tm_zone tm_mday sival_ptr SHUT_RD tm_gmtoff SOCK_PACKET si_status tm_sec sigval_t DoTCP IPPROTO_UDPLITE __socklen_t IPPROTO_EGP sockaddr tm_wday __uid_t __sigaction_handler si_signo _kill si_addr_bnd sa_sigaction sa_mask IPPROTO_GRE n_act __saved_mask sa_family_t sa_restorer __mask_was_saved SOCK_CLOEXEC SHUT_RDWR sockaddr_in IPPROTO_MPLS IPPROTO_TCP _timer si_fd IPPROTO_COMP sa_handler sa_data IPPROTO_IPIP SOCK_RAW _time si_band SOCK_SEQPACKET in_port_t si_code DoUDP DoUDP.c hlen server_ipv4 DHCP_GetNS.c u_int8_t htype ifreq DHCP_GetNS mem_end ifru_data ifru_newname ifru_netmask your_ipv4 base_addr router_ipv4 ipv4_str ifru_map ether_addr_octet server_hname ifmap ifru_dstaddr seconds eaddr ifru_flags __caddr_t ether_str DHCP_MSG ifru_hwaddr boot_filename ifru_slave client_hwd ifrn_name client_ipv4 ifru_broadaddr ether_addr ifru_ivalue mem_start ifru_mtu uint8_t dhcp ifru_addr hops ifr_ifrn ifr_ifru GetRecords DNS_RDATA GetRecords.c expire domain refresh ans_array GetAnswers.c a32_str mailn serial __u6_addr8 __stale_ok a128 in6_addr a128_str acnt retry __u6_addr16 GetAnswers __u6_addr32 pref __in6_u hidx ConvertName ConvertName.c qidx nidx ConvertToPtr ConvertToPtr.c ConvertToPtr6 ConvertToPtr6.c strptr_len targetlen ConvertNumberToE164.c ConvertNumberToE164 numlen target number HandleNAPTRrecord HandleNAPTRrecord.c replace NAPTR_DATA regex order services naptr GetName.c jmp_fl GetName delt GetOpCode.c GetOpCode GetQClass.c QCLASS_NONE QCLASS_INET QCLASS_ALL GetQClass QCLASS_CHAOS QCLASS QCLASS_HESIOD GetQType GetQType.c GetRCode GetRCode.c PrintInfoDNS.c PrintInfoDNS flag DieSys EMsg lib.c Usage calloc_e recv_a sendto_a wrappers.c protocol recvfrom_a send_a socket_e total malloc_e sock                                   8@                   T@                   t@                   �@                   �@                   �@                   �	@                   @
@                  	 �
@                  
 �
@                   �@                   �@                   �@                    @                   Tn@                   `n@                   ��@                   ��@                   �`                   �`                    �`                   (�`                   ��`                    �`                   ��`                   ��`                                                                                                                                                      ��                      �`                  0@                  p@             .     �@             D     �`            S     �`             z     �@             �     �`             �    ��                �     �`            �     ��`            �     ��`            �      �`            �     �`            �      �`            �     4�`            �     @�`            
    T�`                X�`                `�`            $    h�`            /   ��                9    ��`            F    ��`            O    ��`            V    ��`     �      c    ��`     �      m    `�`     �      x    @�`            �    H�`            �    P�`            �    T�`            �    X�`            �    `�`            �    h�`            �    p�`            �    x�`            �    ��`            �    ��`            �    ��`            �    ��`            	    ��`                ��`                Ș`                И`            '   ��                /    ��`     �       =    �$@     A       L    ��`     �       W    `�`            _    p�`            f    x�`            p    ��`            {    ��`     2       �    ؚ`            �   ��                /    ��`     �       =    7(@     A       L    ��`     �       W    `�`            _    p�`            �    x�`            �    ��`            �    ��`     2       �    ؜`            �   ��                /    ��`     �       =    &+@     A       �    ��`     �       �    X�`            �    `�`            �    p�`                x�`            �    ��`     (       �    ��`            �    ��`                ��`                ��`                ��`            *    О`            9    ؞`            D   ��                Q    ��`            ^    �`            e    �`            l   ��                y     �`            �    �`            �    �`            �    �`            �     �`            �    (�`            �    0�`            �    8�`            �    @�`            �    H�`            �    P�`            �    X�`            �    `�`                h�`                p�`                x�`            $    ��`            .    ��`     .       <    П`            E   ��                S   ��                y    ԟ`            b    ؟`            i    ܟ`            r   ��                �    ��`            �    �`            �    �`            �    ��`            �     �`            �    �`            �    �`            �   ��                �    �`            �     �`            �    (�`            G    0�`            �    8�`            �   ��                �    @�`            �    H�`                P�`            y    X�`                `�`                h�`            !   ��                +    p�`            6    x�`            B    |�`            �    ��`            N    ��`            X    ��`            b    ��`            _   ��                k   ��                w   ��                �   ��                �   ��                �    ��`            �   ��                �   ��                �    ��`                ��                �    ��@             �     �`                  ��                �     �`             �    (�`             �     �`                   ��@                  �`             )    Pn@            9                     P    �j@     v       Y                     k                     }                     �    �`@           �                     �                     �                     �                     �                      	     ��`                                  (                     ?                     W    \l@     �       b                     |                     �                     �    ��`             �    i]@     �      �                     3    Tn@             �                     �    J`@     I       �                                                               #	                     +                     A                     Y                     m    x(@     �      s    g+@     d      ~                     �                     �                     �    ��`            �    /i@     D       �                     �    si@     ,       �                     	    ��`             	    ��`            "	                     7	                     P	    �j@     �       W	                      f	    �d@     �       o	    �`@     ^       y	   ��`             �	                     �	    �Y@           �	    `n@            �	                     }
                     �	    �i@     z       �	                     �	    �m@     e       �	    }e@     �      
                      
                     �    ��`             	     @     *       6
    �5@     
      A
    ��`             M
    �@           R
                     e
    �W@     �      y
                     �
                     �
                     �
                      �
                     �
    �L@     t      �
    j@     o       �
    �k@     �       �
                                          #                     7   ��`             C                      ]    �0@     �      h                     {    (%@           �    @     �	      �                     �                     �    �M@     �      �	    �@             �                     �                     �    �R@               ��`                                 +    �l@     �        crtstuff.c __JCR_LIST__ deregister_tm_clones __do_global_dtors_aux completed.7594 __do_global_dtors_aux_fini_array_entry frame_dummy __frame_dummy_init_array_entry main.c host.5035 hostmax.5037 c.5029 ns.5032 DNS_SRV_FL.5036 classStr.5031 q_class.5040 typeStr.5030 q_type.5039 p.5034 k.5038 telno.5033 DoQuery.c hostmax.5040 buf.5032 z.5038 answers.5041 auth.5042 addit.5043 dns.5044 qname.5033 transaction_id.5031 i.5036 V6_PTR.5053 qnamelen.5048 e164.5035 q.5045 tosend.5049 trcvd.5050 t1.5046 t2.5047 __USED_TCP.5039 diff.5052 delta.5051 err.5037 DoTCP.c __ALARM_ENV__ catch_sigalarm n_act.5035 s4.5034 s.5037 seed.5039 _time.5040 tstring.5038 ret.5036 DoUDP.c seed.5038 _time.5039 tstring.5040 DHCP_GetNS.c n_act.5044 buf.5033 s4.5039 dhcp.5032 ifr.5037 s.5035 opt.5036 tosend.5043 in4.5040 ipv4_str.5042 ether_str.5041 eaddr.5038 GetRecords.c hostmax.5034 p.5035 k.5033 GetAnswers.c hostmax.5032 string.5034 domain.5035 mailn.5036 k.5031 p.5048 delta.5049 a16.5042 a32.5039 a32_str.5038 serial.5043 refresh.5044 retry.5045 expire.5046 min.5047 pref.5041 a128.5040 a128_str.5037 err.5033 ConvertName.c ConvertToPtr.c i.5031 err.5030 ConvertToPtr6.c tmp.5030 len.5036 p1.5031 p2.5032 t.5033 err.5035 ConvertNumberToE164.c numlen.5033 p.5031 HandleNAPTRrecord.c naptr.5033 len.5030 p.5028 name.5029 delta.5031 GetName.c start.5035 jmp_fl.5037 offset.5032 delt.5034 p.5036 GetOpCode.c GetQClass.c GetQType.c GetRCode.c PrintInfoDNS.c ok.5034 lib.c wrappers.c err __FRAME_END__ __JCR_END__ __init_array_end _DYNAMIC __init_array_start __GNU_EH_FRAME_HDR _GLOBAL_OFFSET_TABLE_ __libc_csu_fini inet_ntop@@GLIBC_2.2.5 socket_e free@@GLIBC_2.2.5 recv@@GLIBC_2.2.5 putchar@@GLIBC_2.2.5 GetQType localtime@@GLIBC_2.2.5 __errno_location@@GLIBC_2.2.5 strncpy@@GLIBC_2.2.5 strncmp@@GLIBC_2.2.5 _ITM_deregisterTMCloneTable puts@@GLIBC_2.2.5 sigaction@@GLIBC_2.2.5 setsockopt@@GLIBC_2.2.5 recvfrom_a clock_gettime@@GLIBC_2.17 getpid@@GLIBC_2.2.5 inet_ntoa@@GLIBC_2.2.5 _edata GetName shutdown@@GLIBC_2.2.5 strlen@@GLIBC_2.2.5 GetOpCode __stack_chk_fail@@GLIBC_2.4 htons@@GLIBC_2.2.5 send@@GLIBC_2.2.5 snprintf@@GLIBC_2.2.5 siglongjmp@@GLIBC_2.2.5 memset@@GLIBC_2.2.5 DoUDP DHCP_GetNS ioctl@@GLIBC_2.2.5 sendto@@GLIBC_2.2.5 alarm@@GLIBC_2.2.5 STALE_OK DieSys __libc_start_main@@GLIBC_2.2.5 Usage calloc@@GLIBC_2.2.5 __data_start optarg@@GLIBC_2.2.5 fprintf@@GLIBC_2.2.5 sigemptyset@@GLIBC_2.2.5 send_a __gmon_start__ GetRCode GetQClass __dso_handle memcpy@@GLIBC_2.14 HandleNAPTRrecord _IO_stdin_used inet_pton@@GLIBC_2.2.5 calloc_e tolower@@GLIBC_2.2.5 __libc_csu_init PrintInfoDNS malloc@@GLIBC_2.2.5 recvfrom@@GLIBC_2.2.5 GetAnswers __bss_start main ntohs@@GLIBC_2.2.5 ConvertNumberToE164 strftime@@GLIBC_2.2.5 perror@@GLIBC_2.2.5 sysconf@@GLIBC_2.2.5 _Jv_RegisterClasses getopt@@GLIBC_2.2.5 ConvertName malloc_e recv_a exit@@GLIBC_2.2.5 connect@@GLIBC_2.2.5 fwrite@@GLIBC_2.2.5 __TMC_END__ _ITM_registerTMCloneTable GetRecords ntohl@@GLIBC_2.2.5 DoTCP DoQuery strerror@@GLIBC_2.2.5 __sigsetjmp@@GLIBC_2.2.5 ConvertToPtr sigaddset@@GLIBC_2.2.5 __ctype_b_loc@@GLIBC_2.3 ConvertToPtr6 stderr@@GLIBC_2.2.5 socket@@GLIBC_2.2.5 sendto_a  .symtab .strtab .shstrtab .interp .note.ABI-tag .note.gnu.build-id .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt .init .plt.got .text .fini .rodata .eh_frame_hdr .eh_frame .init_array .fini_array .jcr .dynamic .got.plt .data .bss .comment .debug_aranges .debug_info .debug_abbrev .debug_line .debug_str                                                                              8@     8                                    #             T@     T                                     1             t@     t      $                              D   ���o       �@     �      (                             N             �@     �                                V             �@     �                                   ^   ���o       �	@     �	      l                            k   ���o       @
@     @
      `                            z             �
@     �
      H                            �      B       �
@     �
      �                          �             �@     �                                    �             �@     �      0                            �             �@     �                                    �              @            R[                             �             Tn@     Tn      	                              �             `n@     `n      =                             �             ��@     ��                                   �             ��@     ��      �                             �             �`     �                                    �             �`     �                                    �              �`      �                                    �             (�`     (�      �                           �             ��`     ��                                   �              �`      �      �                            �             ��`     ��                                    �             ��`     ��      �                                   0               ��      5                                                  �      �                                                  ��      vs                             '                     #	     �                             5                     �     	                             A     0               �<     �                                                  Ā     L                                                   �Y            #   �                 	                      �t     4                                                                                                                                                                                                                                                                             PrintInfoDNS.c                                                                                      0000600 0001750 0001750 00000002314 13361321315 012145  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

ssize_t
PrintInfoDNS(uc *ptr, int flag, u16 transaction_id, uc *ns)
{
	DNS_HEADER *d = NULL;
	DNS_QUESTION *q = NULL;
	uc *p = ptr;
	static int ok = 0;

	if (ptr == NULL)
	  {
		errno = EINVAL;
		return(-1);
	  }
	d = (DNS_HEADER *)ptr;
	p += sizeof(DNS_HEADER);
	printf("Questions %hu | Answers %hu | Authoritative %hu | Additional %hu\r\n",
		ntohs(d->qdcnt),
		ntohs(d->ancnt),
		ntohs(d->nscnt),
		ntohs(d->arcnt));
	printf("  Query \"");
	if (!isdigit(*p) && !isalpha(*p))
		++p;
	while (*p != 0)
	  {
		if (!isdigit(*p) && !isalpha(*p) && *p != 0x2d)
		  {
			putchar('.');
			p++;
		  }
		putchar(*p++);
	  }
	printf("\" @SERVER %s:53\r\n", ns);
	++p;
	q = (DNS_QUESTION *)p;
	if (ntohs(d->ident) == transaction_id)
		ok = 1;
	printf("  Transaction ID 0x%hx %s\r\n",
		ntohs(d->ident),
		(flag?(ok?"\e[1;02m[\e[1;32mVALID\e[m\e[1;02m]\e[m":"\e[1;02m[\e[1;31mINVALID\e[m\e[1;02m]\e[m"):""));
	printf("  FLAGS: %s %s %s %s %s %s %s  QTYPE: %s  QCLASS: %s  STATUS: %s\r\n",
		d->qr?"qr":"",
		d->aa?"aa":"",
		d->tc?"tc":"",
		d->rd?"rd":"",
		d->ra?"ra":"",
		d->ad?"ad":"",
		d->cd?"cd":"",
		GetQType(ntohs(q->qtype)),
		GetQClass(ntohs(q->qclass)),
		GetRCode(d->rcode));
	putchar('\n');
	return(0);
}
                                                                                                                                                                                                                                                                                                                    wrappers.c                                                                                          0000600 0001750 0001750 00000003726 13361263617 011555  0                                                                                                    ustar   oppa                            oppa                                                                                                                                                                                                                   #include "dns.h"

static int err;

void *
calloc_e(void *p, size_t size, size_t type)
{
	if (!(p = calloc(size, type)))
	  {
		err = errno;
		fprintf(stderr, "calloc: ");
		errno = err;
		return(NULL);
	  }
	else
		return(p);
}

void *
malloc_e(void *p, size_t type)
{
	if (!(p = malloc(type)))
	  {
		err = errno;
		fprintf(stderr, "malloc: ");
		errno = err;
		return(NULL);
	  }
	else
		return(p);
}

int
socket_e(int s, int domain, int type, int protocol)
{
	if ((s = socket(domain, type, protocol)) < 0)
	  {
		err = errno;
		fprintf(stderr, "socket: ");
		errno = err;
		return(-1);
	  }
	else
		return(s);
}

ssize_t
send_a(int sock, uc *buf, size_t size, int flags)
{
	size_t tosend = size, total = 0;
	ssize_t ret = 0;
	uc *p = buf;

	while (tosend > 0 && (ret = send(sock, p, tosend, flags) > 0))
	  {
		if (ret < 0)
		  {
			err = errno;
			fprintf(stderr, "send: ");
			errno = err;
			return(-1);
		  }
		tosend -= ret;
		p += ret;
		total += ret;
	  }
	return(total);
}

ssize_t
recv_a(int sock, uc *buf, size_t size, int flags)
{
	ssize_t ret = 0;
	size_t total = 0;
	uc *p = buf;

	while ((ret = recv(sock, p, size, flags) > 0))
	  {
		if (ret < 0)
		  {
			perror("recv");
			return(-1);
		  }
		p += ret;
		total += ret;
	  }
	return(total);
}

ssize_t
recvfrom_a(int sock, uc *buf, size_t size, int flags, struct sockaddr *sa, socklen_t *ss)
{
	ssize_t ret = 0, total = 0;
	uc *p = buf;

	while ((ret = recvfrom(sock, p, size, flags, sa, ss)) > 0)
	  {
		if (ret < 0)
		  {
			perror("recvfrom");
			return(-1);
		  }
		p += ret;
		total += ret;
	  }
	return(total);
}

ssize_t
sendto_a(int sock, uc *buf, size_t size, int flags, struct sockaddr *sa, socklen_t ss)
{
	size_t tosend = size;
	ssize_t ret = 0, total = 0;
	uc *p = buf;

	while (tosend > 0 && (ret = sendto(sock, p, tosend, flags, sa, ss)) > 0)
	  {
		if (ret < 0)
		  {
			err = errno;
			fprintf(stderr, "sendto: ");
			errno = err;
			return(-1);
		  }
		tosend -= ret;
		p += ret;
		total += ret;
	  }
	return(total);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          