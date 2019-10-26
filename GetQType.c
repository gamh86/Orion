#include "dns.h"

uc *
get_qtype(unsigned short qtype)
{
	switch(qtype)
	{
		case 1:
			return "A";
			break;
		case 2:
			return "NS";
			break;
		case 5:
			return "CNAME";
			break;
		case 6:
			return "SOA";
			break;
		case 12:
			return "PTR";
			break;
		case 13:
			return "HINFO (obsolete)";
			break;
		case 15:
			return "MX";
			break;
		case 16:
			return "TXT";
			break;
		case 17:
			return "RP (obsolete)";
			break;
		case 18:
			return "AFSDB";
			break;
		case 19:
			return "X25 (obsolete)";
			break;
		case 24:
			return "SIG (obsolete)";
			break;
		case 25:
			return "KEY (obsolete)";
			break;
		case 28:
			return "AAAA";
			break;
		case 29:
			return "LOC";
			break;
		case 33:
			return "SRV";
			break;
		case 35:
			return "NAPTR";
			break;
		case 36:
			return "KX";
			break;
		case 37:
			return "CERT";
			break;
		case 39:
			return "DNAME";
			break;
		case 41:
			return "OPT";
			break;
		case 42:
			return "APL";
			break;
		case 43:
			return "DS";
			break;
		case 44:
			return "SSHFP";
			break;
		case 45:
			return "IPSECKEY";
			break;
		case 46:
			return "RRSIG";
			break;
		case 47:
			return "NSEC";
			break;
		case 48:
			return "DNSKEY";
			break;
		case 49:
			return "DHCID";
			break;
		case 50:
			return "NSEC3";
			break;
		case 51:
			return "NSEC3PARAM";
			break;
		case 52:
			return "TLSA";
			break;
		case 55:
			return "HIP";
			break;
		case 59:
			return "CDS";
			break;
		case 60:
			return "CDNSKEY";
			break;
		case 61:
			return "OPENPGPKEY";
			break;
		case 99:
			return "SPF (obsolete)";
			break;
		case 249:
			return "TKEY";
			break;
		case 250:
			return "TSIG";
			break;
		case 251:
			return "IXFR";
			break;
		case 252:
			return "AXFR";
			break;
		case 255:
			return "ANY";
			break;
		case 256:
			return "URI";
			break;
		case 257:
			return "CAA";
			break;
		case 32768:
			return "TA";
			break;
		case 32769:
			return "DLV";
			break;
		default:
		return "unknown";
	}
}
