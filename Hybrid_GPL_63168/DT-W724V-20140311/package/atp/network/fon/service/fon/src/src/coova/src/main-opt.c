/* 
 * Copyright (C) 2007-2011 Coova Technologies, LLC. <support@coova.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Author: David Bird <david@coova.com>
 */

#define MAIN_FILE

#include "chilli.h"
#include "cmdline.h"
#include "debug.h"
#include "atputil.h"

struct options_t _options;

static const char *description = 
  "CoovaChilli - A Wireless LAN Access Point Controller.\n"
  "  For more information on this project, visit: \n"
  "  http://www.coova.org/\n";

static const char *copyright = 
  "Copyright (c) 2003-2005 Mondru AB., 2006-2011 Coova Technologies LLC.\n"
  "Licensed under the GNU General Public License (GPL).\n";

static const char *usage =
  "Usage: chilli [OPTIONS]...\n";

static const char *compile_options = "Compiled with "
#ifdef ENABLE_BINSTATFILE
  "ENABLE_BINSTATFILE "
#endif
#ifdef ENABLE_CHILLIQUERY
  "ENABLE_CHILLIQUERY "
#endif
#ifdef ENABLE_CHILLIPROXY
  "ENABLE_CHILLIPROXY "
#endif
#ifdef ENABLE_RADPROXY
  "ENABLE_RADPROXY "
#endif
#ifdef ENABLE_CHILLIRADSEC
  "ENABLE_CHILLIRADSEC "
#endif
#ifdef ENABLE_CHILLIREDIR
  "ENABLE_CHILLIREDIR "
#endif
#ifdef ENABLE_CHILLIXML
  "ENABLE_CHILLIXML "
#endif
#ifdef ENABLE_CLUSTER
  "ENABLE_CLUSTER "
#endif
#ifdef ENABLE_DHCPRADIUS
  "ENABLE_DHCPRADIUS "
#endif
#ifdef ENABLE_DHCPOPT
  "ENABLE_DHCPOPT "
#endif
#ifdef ENABLE_DNSLOG
  "ENABLE_DNSLOG "
#endif
#ifdef ENABLE_UAMDOMAINFILE
  "ENABLE_UAMDOMAINFILE "
#endif
#ifdef ENABLE_EAPOL
  "ENABLE_EAPOL "
#endif
#ifdef ENABLE_EWTAPI
  "ENABLE_EWTAPI "
#endif
#ifdef ENABLE_IEEE8021Q
  "ENABLE_IEEE8021Q "
#endif
#ifdef ENABLE_IEEE8023
  "ENABLE_IEEE8023 "
#endif
#ifdef ENABLE_IPWHITELIST
  "ENABLE_IPWHITELIST "
#endif
#ifdef ENABLE_JSON
  "ENABLE_JSON "
#endif
#ifdef ENABLE_LARGELIMITS
  "ENABLE_LARGELIMITS "
#endif
#ifdef ENABLE_LAYER3
  "ENABLE_LAYER3 "
#endif
#ifdef ENABLE_LEAKYBUCKET
  "ENABLE_LEAKYBUCKET "
#endif
#ifdef ENABLE_MINIPORTAL
  "ENABLE_MINIPORTAL "
#endif
#ifdef ENABLE_MULTIROUTE
  "ENABLE_MULTIROUTE "
#endif
#ifdef ENABLE_NETNAT
  "ENABLE_NETNAT "
#endif
#ifdef ENABLE_PPPOE
  "ENABLE_PPPOE "
#endif
#ifdef ENABLE_PROXYVSA
  "ENABLE_PROXYVSA "
#endif
#ifdef ENABLE_REDIRDNSREQ
  "ENABLE_REDIRDNSREQ "
#endif
#ifdef ENABLE_SESSGARDEN
  "ENABLE_SESSGARDEN "
#endif
#ifdef ENABLE_SSDP
  "ENABLE_SSDP "
#endif
#ifdef ENABLE_STATFILE
  "ENABLE_STATFILE "
#endif
#ifdef HAVE_MATRIXSSL
  "HAVE_MATRIXSSL "
#endif
#ifdef HAVE_OPENSSL
  "HAVE_OPENSSL "
#endif
#ifdef USING_POLL
  "USING_POLL "
#endif
;

#define ATP_CBBID_RADCONFIG_NAME "radconfig"

char *STRDUP(char *s) {
  if (!s) return 0;
  while (isspace(*s)) s++;
  if (!*s) return 0;
  return s;
}

#ifdef ENABLE_MINICONFIG

#define cmdline_parser2 mini_cmdline_parser2
extern int mini_cmdline_parser2(int argc, char **argv, 
				struct gengetopt_args_info *args_info, 
				int, int, int);

#define cmdline_parser_free mini_cmdline_free
extern int mini_cmdline_free(struct gengetopt_args_info *args_info);

#define cmdline_parser_configfile mini_cmdline_file
extern int mini_cmdline_file(char *file, struct gengetopt_args_info *args_info,
			     int, int, int);

static void
options_print_version (void) {
  printf ("%s %s\n", PACKAGE, VERSION);
}

static void
options_print_help (void) {
  options_print_version();
  printf("\n%s", description);
  printf("\n%s\n", usage);
  printf("\n%s\n", compile_options);
  printf("\n%s\n", copyright);
}

#else

extern const char *gengetopt_args_info_help[];

static void
options_print_version (void) {
  printf ("%s %s\n", CMDLINE_PARSER_PACKAGE, CMDLINE_PARSER_VERSION);
}

static void
options_print_help (void) {
  int i = 0;

  options_print_version();

  printf("\n%s", description);
  printf("\n%s\n", usage);

  while (gengetopt_args_info_help[i])
    printf("%s\n", gengetopt_args_info_help[i++]);

  printf("\n%s\n", compile_options);

  printf("\n%s\n", copyright);
}

#endif

int main(int argc, char **argv) {
  struct gengetopt_args_info args_info;
  struct hostent *host;
  char hostname[USERURLSIZE];
  int numargs;
  int ret = -1;
  int radius1valid = 0;
  int radius2valid = 0;
  int i = 0;
  unsigned int radiusipaddr[2] = {0};
  unsigned int uamserveripaddr[UAMSERVER_MAX] = {0};

  FILE *fp = NULL;
  char acLine[128] = {0};

  options_init();

  ATP_MSG_Init(ATP_CBBID_RADCONFIG_NAME);

  memset(&args_info, 0, sizeof(args_info));

  if (cmdline_parser2(argc, argv, &args_info, 1, 1, 1) != 0) {
    log_err(0, "Failed to parse command line options");
    goto end_processing;
  }

  if (args_info.version_given) {
    options_print_version();
    exit(2);
  }

  if (args_info.help_given) {
    options_print_help();
    exit(2);
  }

  if (cmdline_parser_configfile(args_info.conf_arg ? 
				args_info.conf_arg : 
				DEFCHILLICONF, 
				&args_info, 0, 0, 0)) {
    log_err(0, "Failed to parse configuration file: %s!", 
	    args_info.conf_arg);
    goto end_processing;
  }

  /* Get the system default DNS entries */
  if (res_init()) {
    log_err(0, "Failed to update system DNS settings (res_init()!");
    goto end_processing;
  }

  /* Handle each option */
  _options.initialized = 1;

  if (args_info.debug_flag) 
    _options.debug = args_info.debugfacility_arg;
  else 
    _options.debug = 0;

  /** simple configuration parameters **/
  _options.layer3 = args_info.layer3_flag;
#if(_debug_ && !defined(ENABLE_LAYER3))
  if (_options.layer3) 
    log_warn(0, "layer3 not implemented. build with --enable-layer3");
#endif
  _options.uid = args_info.uid_arg;
  _options.gid = args_info.gid_arg;
  _options.mtu = args_info.mtu_arg;
  _options.usetap = args_info.usetap_flag;
#if(_debug_ && !defined(ENABLE_TAP))
  if (_options.usetap) 
    log_warn(0, "tap not implemented. build with --enable-tap");
#endif
  _options.foreground = args_info.fg_flag;
  _options.interval = args_info.interval_arg;
  _options.lease = args_info.lease_arg;
  _options.leaseplus = args_info.leaseplus_arg;
  _options.dhcpstart = args_info.dhcpstart_arg;
  _options.dhcpend = args_info.dhcpend_arg;
  _options.eapolenable = args_info.eapolenable_flag;
#if(_debug_ && !defined(ENABLE_EAPOL))
  if (_options.eapolenable) 
    log_warn(0, "EAPOL not implemented. build with --enable-eapol");
#endif
  _options.swapoctets = args_info.swapoctets_flag;
  _options.logfacility = args_info.logfacility_arg;
  _options.chillixml = args_info.chillixml_flag;
  _options.macauth = args_info.macauth_flag;
  _options.macreauth = args_info.macreauth_flag;
  _options.macauthdeny = args_info.macauthdeny_flag;
  _options.uamport = args_info.uamport_arg;
  _options.uamuiport = args_info.uamuiport_arg;
  _options.macallowlocal = args_info.macallowlocal_flag;
  _options.strictmacauth = args_info.strictmacauth_flag;
  _options.strictdhcp = args_info.strictdhcp_flag;
  _options.no_wispr1 = args_info.nowispr1_flag;
  _options.no_wispr2 = args_info.nowispr2_flag;
  _options.wpaguests = args_info.wpaguests_flag;
  _options.openidauth = args_info.openidauth_flag;
  _options.challengetimeout = args_info.challengetimeout_arg;
  _options.challengetimeout2 = args_info.challengetimeout2_arg;
  _options.defsessiontimeout = args_info.defsessiontimeout_arg;
  _options.definteriminterval = args_info.definteriminterval_arg;
  _options.defbandwidthmaxdown = args_info.defbandwidthmaxdown_arg;
  _options.defbandwidthmaxup = args_info.defbandwidthmaxup_arg;
  _options.defidletimeout = args_info.defidletimeout_arg;
  _options.radiusnasporttype = args_info.radiusnasporttype_arg;
  _options.radiusauthport = args_info.radiusauthport_arg;
  _options.radiusacctport = args_info.radiusacctport_arg;
  _options.coaport = args_info.coaport_arg;
  _options.coanoipcheck = args_info.coanoipcheck_flag;
  _options.radiustimeout = args_info.radiustimeout_arg;
  _options.radiusretry = args_info.radiusretry_arg;
  _options.radiusretrysec = args_info.radiusretrysec_arg;
  _options.radiusretryiteration = args_info.radiusretryiteration_arg;  
  _options.proxyport = args_info.proxyport_arg;
#if(_debug_ && !defined(ENABLE_RADPROXY))
  if (_options.proxyport)
    log_err(0,"radproxy not implemented. build with --enable-radproxy");
#endif
  _options.txqlen = args_info.txqlen_arg;
  _options.ringsize = args_info.ringsize_arg;
  _options.sndbuf = args_info.sndbuf_arg;
  _options.rcvbuf = args_info.rcvbuf_arg;
  _options.postauth_proxyport = args_info.postauthproxyport_arg;
  _options.pap_always_ok = args_info.papalwaysok_flag;
  _options.mschapv2 = args_info.mschapv2_flag;
  _options.acct_update = args_info.acctupdate_flag;
  _options.dhcpradius = args_info.dhcpradius_flag;
  _options.ieee8021q = args_info.ieee8021q_flag;
  _options.dhcp_broadcast = args_info.dhcpbroadcast_flag;
  _options.dhcpgwport = args_info.dhcpgatewayport_arg;
  _options.noc2c = args_info.noc2c_flag;
  _options.tcpwin = args_info.tcpwin_arg;
  _options.tcpmss = args_info.tcpmss_arg;
  _options.max_clients = args_info.maxclients_arg;
  _options.seskeepalive = args_info.seskeepalive_flag;
  _options.uamallowpost = args_info.uamallowpost_flag;
  _options.redir = args_info.redir_flag;
#if(_debug_ && !defined(ENABLE_CHILLIREDIR))
  if (_options.redir) 
    log_err(0, "chilli_redir not implemented. build with --enable-chilliredir");
#endif
  _options.redirssl = args_info.redirssl_flag;
  _options.uamuissl = args_info.uamuissl_flag;
  _options.domaindnslocal = args_info.domaindnslocal_flag;
  _options.framedservice = args_info.framedservice_flag;
  _options.radsec = args_info.radsec_flag;
#if(_debug_ && !defined(ENABLE_CHILLIRADSEC))
  if (_options.radsec) 
    log_err(0, "chilli_radsec not implemented. build with --enable-chilliradsec");
#endif
  _options.proxymacaccept = args_info.proxymacaccept_flag;
  _options.noradallow = args_info.noradallow_flag;
  _options.peerid = args_info.peerid_arg;
#if(_debug_ && !defined(ENABLE_CLUSTER))
  if (_options.peerid) 
    log_err(0, "clustering not implemented. build with --enable-cluster");
#endif
  _options.redirdnsreq = args_info.redirdnsreq_flag;
#if(_debug_ && !defined(ENABLE_REDIRDNSREQ))
  if (_options.redirdnsreq) 
    log_err(0, "redirdnsreq not implemented. build with --enable-redirdnsreq");
#endif

#ifdef ENABLE_LEAKYBUCKET
  _options.bwbucketupsize = args_info.bwbucketupsize_arg;
  _options.bwbucketdnsize = args_info.bwbucketdnsize_arg;
  _options.bwbucketminsize = args_info.bwbucketminsize_arg;
#endif

#ifdef ENABLE_PROXYVSA
  if (args_info.proxylocattr_given) {
    for (numargs = 0; numargs < args_info.proxylocattr_given 
	   && numargs < PROXYVSA_ATTR_CNT; ++numargs)  {
      unsigned int i[2];

      switch (sscanf(args_info.proxylocattr_arg[numargs], 
		     "%u,%u", &i[0], &i[1])) {
      case 0:
	log_err(0, "invalid input %s", args_info.proxylocattr_arg[numargs]);
	break;
      case 1:
	_options.proxy_loc[numargs].attr = i[0];
	break;
      case 2:
	_options.proxy_loc[numargs].attr_vsa = i[0];
	_options.proxy_loc[numargs].attr = i[1];
	break;
      }
      
      log_dbg("Proxy location attr %d %d", 
	      (int)_options.proxy_loc[numargs].attr_vsa, 
	      (int)_options.proxy_loc[numargs].attr);
    }
  }
#endif

  if (args_info.dhcpgateway_arg &&
      !inet_aton(args_info.dhcpgateway_arg, &_options.dhcpgwip)) {
    log_err(0, "Invalid DHCP gateway IP address: %s!", args_info.dhcpgateway_arg);
    goto end_processing;
  }

  if (args_info.dhcprelayagent_arg &&
      !inet_aton(args_info.dhcprelayagent_arg, &_options.dhcprelayip)) {
    log_err(0, "Invalid DHCP gateway relay IP address: %s!", args_info.dhcprelayagent_arg);
    goto end_processing;
  }

  _options.dhcpif = STRDUP(args_info.dhcpif_arg);

  if (!args_info.radiussecret_arg) {
    log_err(0, "radiussecret must be specified!");
    goto end_processing;
  }

  if (!args_info.nexthop_arg) {
    memset(_options.nexthop, 0, PKT_ETH_ALEN);
    _options.has_nexthop = 0;
  }
  else {
    unsigned int temp[PKT_ETH_ALEN];
    char macstr[RADIUS_ATTR_VLEN];
    int macstrlen;
    int	i;

    if ((macstrlen = strlen(args_info.nexthop_arg)) >= (RADIUS_ATTR_VLEN-1)) {
      log_err(0, "MAC address too long");
      goto end_processing;
    }

    memcpy(macstr, args_info.nexthop_arg, macstrlen);
    macstr[macstrlen] = 0;

    /* Replace anything but hex with space */
    for (i=0; i<macstrlen; i++) 
      if (!isxdigit(macstr[i])) 
	macstr[i] = 0x20;

    if (sscanf (macstr, "%2x %2x %2x %2x %2x %2x", 
		&temp[0], &temp[1], &temp[2], 
		&temp[3], &temp[4], &temp[5]) != 6) {
      log_err(0, "MAC conversion failed!");
      return -1;
    }
    
    for (i = 0; i < PKT_ETH_ALEN; i++) 
      _options.nexthop[i] = temp[i];

    _options.has_nexthop = 1;
  }

  if (!args_info.dhcpmac_arg) {
    memset(_options.dhcpmac, 0, PKT_ETH_ALEN);
    _options.dhcpusemac = 0;
    _options.dhcpmacset = 0;
  }
  else {
    unsigned int temp[PKT_ETH_ALEN];
    char macstr[RADIUS_ATTR_VLEN];
    int macstrlen;
    int	i;

    if ((macstrlen = strlen(args_info.dhcpmac_arg)) >= (RADIUS_ATTR_VLEN-1)) {
      log_err(0, "MAC address too long");
      goto end_processing;
    }

    memcpy(macstr, args_info.dhcpmac_arg, macstrlen);
    macstr[macstrlen] = 0;

    /* Replace anything but hex with space */
    for (i=0; i<macstrlen; i++) 
      if (!isxdigit(macstr[i])) macstr[i] = 0x20;

    if (sscanf (macstr, "%2x %2x %2x %2x %2x %2x", 
		&temp[0], &temp[1], &temp[2], 
		&temp[3], &temp[4], &temp[5]) != 6) {
      log_err(0, "MAC conversion failed!");
      return -1;
    }
    
    for (i = 0; i < PKT_ETH_ALEN; i++) 
      _options.dhcpmac[i] = temp[i];

    _options.dhcpusemac = 1;
    _options.dhcpmacset = args_info.dhcpmacset_flag;
  }

  if (args_info.net_arg) {
    if (option_aton(&_options.net, &_options.mask, args_info.net_arg, 0)) {
      log_err(0, "Invalid network address: %s!", args_info.net_arg);
      goto end_processing;
    }
    if (!args_info.uamlisten_arg) {
      _options.uamlisten.s_addr = htonl(ntohl(_options.net.s_addr)+1);
    }
    else if (!inet_aton(args_info.uamlisten_arg, &_options.uamlisten)) {
      log_err(0, "Invalid UAM IP address: %s!", args_info.uamlisten_arg);
      goto end_processing;
    }
    if (!args_info.dhcplisten_arg) {
      _options.dhcplisten.s_addr = _options.uamlisten.s_addr;
    }
    else if (!inet_aton(args_info.dhcplisten_arg, &_options.dhcplisten)) {
      log_err(0, "Invalid DHCP IP address: %s!", args_info.dhcplisten_arg);
      goto end_processing;
    }
  }
  else {
    log_err(0, "Network address must be specified ('net' parameter)!");
    goto end_processing;
  }


  log_dbg("DHCP Listen: %s", inet_ntoa(_options.dhcplisten));
  log_dbg("UAM Listen: %s", inet_ntoa(_options.uamlisten));

  if (!args_info.uamserver_arg) {
    log_err(0, "WARNING: No uamserver defiend!");
  }

  if (args_info.uamserver_arg) {
    if (_options.debug & DEBUG_CONF) {
      log_dbg("Uamserver: %s\n", args_info.uamserver_arg);
    }
    memset(_options.uamserver, 0, sizeof(_options.uamserver));
    _options.uamserverlen = 0;
    if (get_urlparts(args_info.uamserver_arg, hostname, USERURLSIZE, 
		     &_options.uamserverport, 0)) {
      log_err(0, "Failed to parse uamserver: %s!", args_info.uamserver_arg);
      goto end_processing;
    }

    if (!args_info.uamaliasname_arg ||
	strncmp(args_info.uamaliasname_arg, hostname, strlen(args_info.uamaliasname_arg))) {
      if (!(host = ATP_FON_Gethostbyname(hostname, AF_INET, NULL))) {
	log_err(0, "Could not resolve IP address of uamserver: %s!", 
		args_info.uamserver_arg);
	goto end_processing;
      }
      else {
	int j = 0;
	while (host->h_addr_list[j] != NULL) {
	  if (_options.debug & DEBUG_CONF) {
	    log_dbg("Uamserver IP address #%d: %s\n", j,
		    inet_ntoa(*(struct in_addr*) host->h_addr_list[j]));
	  }
	  if (_options.uamserverlen>=UAMSERVER_MAX) {
	    log_err(0,
		    "Too many IPs in uamserver %s!",
		    args_info.uamserver_arg);
	    goto end_processing;
	  }
	  else {
	    _options.uamserver[_options.uamserverlen++] = 
	      *((struct in_addr*) host->h_addr_list[j++]);

        uamserveripaddr[_options.uamserverlen - 1] = _options.uamserver[_options.uamserverlen - 1].s_addr;
	  }
	}
      }
    }
    memset(acLine, 0, sizeof(acLine));

    snprintf(acLine, sizeof(acLine), "%8x %8x %8x %8x %8x %8x %8x %8x", uamserveripaddr[0], uamserveripaddr[1], uamserveripaddr[2], 
        uamserveripaddr[3], uamserveripaddr[4], uamserveripaddr[5], uamserveripaddr[6], uamserveripaddr[7]);
    if ((fp = fopen(ATP_FILE_PORTAL_IPADDR, "w+")))
    {
        fputs(acLine, fp);
        fclose(fp);
    }

    ATP_MSG_SendMsgData(ATP_CBBID_WEB_NAME, ATP_MSG_WEB_UPDATE_PORTAL, (void*)uamserveripaddr, sizeof(uamserveripaddr));
    
  }

  if (args_info.uamhomepage_arg && *args_info.uamhomepage_arg) {
    if (get_urlparts(args_info.uamhomepage_arg, hostname, USERURLSIZE, 
		     &_options.uamhomepageport, 0)) {
      log_err(0,"Failed to parse uamhomepage: %s!", 
	      args_info.uamhomepage_arg);
      goto end_processing;
    }

    if (!args_info.uamaliasname_arg ||
	strncmp(args_info.uamaliasname_arg, hostname, strlen(args_info.uamaliasname_arg))) {
      if (!(host = ATP_FON_Gethostbyname(hostname, AF_INET, NULL))) {
	log_err(0,"Could not resolve uamhomepage server IP %s!", 
		args_info.uamhomepage_arg);
	/*goto end_processing;*/
      }
      else {
	int j = 0;
	while (host->h_addr_list[j] != NULL) {
	  if (_options.uamserverlen>=UAMSERVER_MAX) {
	    log_err(0,"Too many IPs in uamhomepage %s!",
		    args_info.uamhomepage_arg);
	    goto end_processing;
	  }
	  else {
	    _options.uamserver[_options.uamserverlen++] = 
	      *((struct in_addr*) host->h_addr_list[j++]);
	  }
	}
      }
    }
  }

  _options.uamanydns = args_info.uamanydns_flag;
  _options.uamanyip = args_info.uamanyip_flag;
  _options.uamnatanyip = args_info.uamnatanyip_flag;
  _options.dnsparanoia = args_info.dnsparanoia_flag;
  _options.radiusoriginalurl = args_info.radiusoriginalurl_flag;
  _options.routeonetone = args_info.routeonetone_flag;

  /* pass-throughs */
  memset(_options.pass_throughs, 0, sizeof(_options.pass_throughs));
  _options.num_pass_throughs = 0;

  for (numargs = 0; numargs < args_info.uamallowed_given; ++numargs) {
    pass_throughs_from_string(_options.pass_throughs,
			      MAX_PASS_THROUGHS,
			      &_options.num_pass_throughs,
			      args_info.uamallowed_arg[numargs]);
  }

#ifdef ENABLE_DHCPOPT
  _options.dhcp_options_len = 0;
  for (numargs = 0; numargs < args_info.dhcpopt_given; ++numargs) {
    unsigned char binopt[128];
    int hex_length = strlen(args_info.dhcpopt_arg[numargs]);
    int bin_length = hex_length / 2;
    if (hex_length > 0 && (bin_length * 2) == hex_length &&
	bin_length < sizeof(binopt)) {
      log_dbg("DHCP Options %s", args_info.dhcpopt_arg[numargs]);
      if (redir_hextochar((unsigned char *)args_info.dhcpopt_arg[numargs],
			  hex_length, binopt, bin_length) == 0) {
	if (_options.dhcp_options_len + bin_length < 
	    sizeof(_options.dhcp_options)) {
	  memcpy(_options.dhcp_options + 
		 _options.dhcp_options_len, 
		 binopt, bin_length);
	  _options.dhcp_options_len += bin_length;
	} else {
	  log_dbg("No room for DHCP option %d", (int)binopt[0]);
	}
      } else {
	log_dbg("Bad DHCP option hex encoding");
      }
    } else {
      log_dbg("DHCP options are hex encoded binary");
    }
  }
#endif

#ifdef ENABLE_MODULES
  memset(_options.modules, 0, sizeof(_options.modules));
  for (numargs = 0; numargs < args_info.module_given; ++numargs) {
    if (numargs < MAX_MODULES) {
      char *n, *sc;
      int len, nlen;

      n = args_info.module_arg[numargs];
      len = strlen(n);
      sc = strchr(n, ';');
      if (!sc) sc = strchr(n, ':');
      nlen = sc ? (sc - n) : len;

      safe_snprintf(_options.modules[numargs].name, 
		    sizeof(_options.modules[numargs].name),
		    "%.*s", nlen, n);
      if (sc && len > (nlen + 1)) {
	safe_snprintf(_options.modules[numargs].conf, 
		      sizeof(_options.modules[numargs].conf),
		      "%.*s", len - nlen - 1, sc + 1);
      }
    }
  }
#endif

#ifdef ENABLE_CHILLIREDIR
  /*
  for (numargs = 0; numargs < MAX_REGEX_PASS_THROUGHS; ++numargs) {
    if (_options.regex_pass_throughs[numargs].re_host.allocated)
      regfree(&_options.regex_pass_throughs[numargs].re_host);
    if (_options.regex_pass_throughs[numargs].re_path.allocated)
      regfree(&_options.regex_pass_throughs[numargs].re_path);
    if (_options.regex_pass_throughs[numargs].re_qs.allocated)
      regfree(&_options.regex_pass_throughs[numargs].re_qs);
  }
  */
  
  memset(_options.regex_pass_throughs, 0, sizeof(_options.regex_pass_throughs));
  _options.regex_num_pass_throughs = 0;
  
  for (numargs = 0; numargs < args_info.uamregex_given; ++numargs) {
    regex_pass_throughs_from_string(_options.regex_pass_throughs,
				    MAX_REGEX_PASS_THROUGHS,
				    &_options.regex_num_pass_throughs,
				    args_info.uamregex_arg[numargs]);
  }
#endif

  for (numargs = 0; numargs < MAX_UAM_DOMAINS; ++numargs) {
    if (_options.uamdomains[numargs])
      free(_options.uamdomains[numargs]);
    _options.uamdomains[numargs] = 0;
  }

  if (args_info.uamdomain_given) {
    for (numargs = 0; numargs < args_info.uamdomain_given && numargs < MAX_UAM_DOMAINS; ++numargs) 
      _options.uamdomains[numargs] = STRDUP(args_info.uamdomain_arg[numargs]);
  }

  _options.allowdyn = 1;
  
  _options.autostatip = args_info.autostatip_arg;
  if (_options.autostatip)
    _options.uamanyip = 1;
  
  if (args_info.nodynip_flag) {
    _options.allowdyn = 0;
  } else {
    if (!args_info.dynip_arg) {
      _options.dynip = STRDUP(args_info.net_arg);
    }
    else {
      struct in_addr addr;
      struct in_addr mask;
      _options.dynip = STRDUP(args_info.dynip_arg);
      if (option_aton(&addr, &mask, _options.dynip, 0)) {
	log_err(0, "Failed to parse dynamic IP address pool!");
	goto end_processing;
      }
    }
  }
  
  /* statip */
  if (args_info.statip_arg) {
    struct in_addr addr;
    struct in_addr mask;
    _options.statip = STRDUP(args_info.statip_arg);
    if (option_aton(&addr, &mask, _options.statip, 0)) {
      log_err(0, "Failed to parse static IP address pool!");
      return -1;
    }
    _options.allowstat = 1;
  } else {
    _options.allowstat = 0;
  }

  /* anyipexclude */
  if (args_info.anyipexclude_arg) {
    if (option_aton(&_options.anyipexclude_addr, 
		    &_options.anyipexclude_mask, 
		    args_info.anyipexclude_arg, 0)) {
      log_err(0, "Failed to parse anyip exclude network!");
      return -1;
    }
  }
  
  if (args_info.dns1_arg) {
    if (!inet_aton(args_info.dns1_arg, &_options.dns1)) {
      log_err(0,"Invalid primary DNS address: %s!", 
	      args_info.dns1_arg);
      goto end_processing;
    }
  }
  else if (_res.nscount >= 1) {
    _options.dns1 = _res.nsaddr_list[0].sin_addr;
  }
  else {
    _options.dns1.s_addr = 0;
  }

  if (args_info.dns2_arg) {
    if (!inet_aton(args_info.dns2_arg, &_options.dns2)) {
      log_err(0,"Invalid secondary DNS address: %s!", 
	      args_info.dns1_arg);
      goto end_processing;
    }
  }
  else if (_res.nscount >= 2) {
    _options.dns2 = _res.nsaddr_list[1].sin_addr;
  }
  else {
    _options.dns2.s_addr = _options.dns1.s_addr;
  }


  /* If no listen option is specified listen to any local port    */
  /* Do hostname lookup to translate hostname to IP address       */
  if (args_info.radiuslisten_arg) {
    if (!(host = ATP_FON_Gethostbyname(args_info.radiuslisten_arg, AF_INET, NULL))) {
      log_err(0, "Invalid listening address: %s! [%s]", 
	      args_info.radiuslisten_arg, strerror(errno));
      goto end_processing;
    }
    else {
      memcpy(&_options.radiuslisten.s_addr, host->h_addr, host->h_length);
    }
  }
  else {
    _options.radiuslisten.s_addr = htonl(INADDR_ANY);
  }

#ifdef ENABLE_NETNAT
  if (args_info.natip_arg) {
    if (!(host = ATP_FON_Gethostbyname(args_info.natip_arg, AF_INET, NULL))) {
      log_warn(0, "Invalid natip address: %s! [%s]", 
	       args_info.natip_arg, strerror(errno));
    }
    else {
      memcpy(&_options.natip.s_addr, host->h_addr, host->h_length);
    }
  }
#endif

  if (args_info.uamlogoutip_arg) {
    if (!(host = ATP_FON_Gethostbyname(args_info.uamlogoutip_arg, AF_INET, NULL))) {
      log_warn(0, "Invalid uamlogoutup address: %s! [%s]", 
	       args_info.uamlogoutip_arg, strerror(errno));
    }
    else {
      memcpy(&_options.uamlogout.s_addr, host->h_addr, host->h_length);
    }
  }

  if (args_info.uamaliasip_arg) {
    if (!(host = ATP_FON_Gethostbyname(args_info.uamaliasip_arg, AF_INET, NULL))) {
      log_warn(0, "Invalid uamaliasip address: %s! [%s]", 
	       args_info.uamlogoutip_arg, strerror(errno));
    }
    else {
      memcpy(&_options.uamalias.s_addr, host->h_addr, host->h_length);
    }
  }

  if (args_info.postauthproxy_arg) {
    if (!(host = ATP_FON_Gethostbyname(args_info.postauthproxy_arg, AF_INET, NULL))) {
      log_warn(0, "Invalid postauthproxy address: %s! [%s]", 
	       args_info.postauthproxy_arg, strerror(errno));
    }
    else {
      memcpy(&_options.postauth_proxyip.s_addr, host->h_addr, host->h_length);
    }
  }

  /* If no option is specified terminate                          */
  /* Do hostname lookup to translate hostname to IP address       */
  if (args_info.radiusserver1_arg) {
    for(i=0; i<3; i++)
    {
        if (strcmp(args_info.radiusserver1_arg, "127.0.0.1"))
        {
            radius_trigger();
        }
            
        if((host = ATP_FON_Gethostbyname(args_info.radiusserver1_arg, AF_INET, NULL)))
        {
            memcpy(&_options.radiusserver1.s_addr, host->h_addr, host->h_length);
            radiusipaddr[0] = (unsigned int)_options.radiusserver1.s_addr;
            radius1valid = 1;
            break;
        }

        sleep(2);
    }

    if(!host)
    {
        log_err(0, "Invalid radiusserver1 address: %s! [%s]", 
	      args_info.radiusserver1_arg, strerror(errno));
        radius1valid = 0;
    }
  }
  else {
    log_err(0,"No radiusserver1 address given!");
    goto end_processing;
  }

  /* radiusserver2 */
  /* If no option is specified terminate                          */
  /* Do hostname lookup to translate hostname to IP address       */
  if (args_info.radiusserver2_arg) {
    for(i=0; i<3; i++)
    {
        if (strcmp(args_info.radiusserver2_arg, "127.0.0.1"))
        {
            radius_trigger();
        }
            
        if((host = ATP_FON_Gethostbyname(args_info.radiusserver2_arg, AF_INET, NULL)))
        {
            memcpy(&_options.radiusserver2.s_addr, host->h_addr, host->h_length);
            radiusipaddr[1] = (unsigned int)_options.radiusserver2.s_addr;
            radius2valid = 1;      
            break;
        }

        sleep(2);
    }

    if(!host)
    {
        log_err(0, "Invalid radiusserver2 address: %s! [%s]", 
          args_info.radiusserver2_arg, strerror(errno));
        radius2valid = 0;        
    }
  }
  else {
    _options.radiusserver2.s_addr = 0;
  }

  if (0x7f000001 != radiusipaddr[0] && 0x7f000001 != radiusipaddr[1])
  {
      /* Save Radius IP Addr */
      if ((fp = fopen(ATP_FILE_RADIUS_IPADDR, "w+")))
      {
        memset(acLine, 0, sizeof(acLine));
        snprintf(acLine, sizeof(acLine), "%8x %8x", radiusipaddr[0], radiusipaddr[1]);
        fputs(acLine, fp);    
        fclose(fp);
      }
      
      /* Send Radius IP Addr to web */
      ATP_MSG_SendMsgData(ATP_CBBID_WEB_NAME, ATP_MSG_WEB_UPDATE_RADIUS, (void*)radiusipaddr, sizeof(radiusipaddr));
  }

  if(!radius1valid && !radius2valid)
    goto end_processing;    

  /* If no listen option is specified listen to any local port    */
  /* Do hostname lookup to translate hostname to IP address       */
  if (args_info.proxylisten_arg) {
#ifdef ENABLE_RADPROXY
    if (!(host = ATP_FON_Gethostbyname(args_info.proxylisten_arg, AF_INET, NULL))) {
      log_err(0, "Invalid listening address: %s! [%s]", 
	      args_info.proxylisten_arg, strerror(errno));
      goto end_processing;
    }
    else {
      memcpy(&_options.proxylisten.s_addr, host->h_addr, host->h_length);
    }
  }
  else {
    _options.proxylisten.s_addr = htonl(INADDR_ANY);
#elif (_debug_)
    log_warn(0,"radproxy not implemented. build with --enable-radproxy");
#endif
  }
  
  /* Store proxyclient as in_addr net and mask                       */
  if (args_info.proxyclient_arg) {
#ifdef ENABLE_RADPROXY
    if(option_aton(&_options.proxyaddr, &_options.proxymask, 
		   args_info.proxyclient_arg, 0)) {
      log_err(0,"Invalid proxy client address: %s!", args_info.proxyclient_arg);
      goto end_processing;
    }
  }
  else {
    _options.proxyaddr.s_addr = ~0; /* Let nobody through */
    _options.proxymask.s_addr = 0; 
#elif (_debug_)
    log_warn(0,"radproxy not implemented. build with --enable-radproxy");
#endif
  }

  memset(_options.macok, 0, sizeof(_options.macok));
  _options.macoklen = 0;

  for (numargs = 0; numargs < args_info.macallowed_given; ++numargs) {

    char *p1 = NULL;
    char *p2 = NULL;
    char *p3 = malloc(strlen(args_info.macallowed_arg[numargs])+1);
    int i;

    unsigned int mac[6];

    log_dbg("Macallowed #%d: %s", numargs, args_info.macallowed_arg[numargs]);

    strcpy(p3, args_info.macallowed_arg[numargs]);
    p1 = p3;
    if ((p2 = strchr(p1, ','))) {
      *p2 = '\0';
    }
    while (p1) {
      if (_options.macoklen>=MACOK_MAX) {
	log_err(0,"Too many addresses in macallowed %s!",
		args_info.macallowed_arg);
      }
      else {
	/* Replace anything but hex and comma with space */
	for (i=0; i<strlen(p1); i++) 
	  if (!isxdigit(p1[i])) p1[i] = 0x20;
      
	if (sscanf (p1, "%2x %2x %2x %2x %2x %2x",
		    &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6) {
	  log_err(0, "Failed to convert macallowed option to MAC Address");
	}
	else {

	  log_dbg("Macallowed address #%d: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X", 
		  _options.macoklen,
		  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	  for (i = 0; i < 6; i++)
	    _options.macok[_options.macoklen][i] = (unsigned char) mac[i]; 

	  _options.macoklen++;
	}
      }
      
      if (p2) {
	p1 = p2+1;
	if ((p2 = strchr(p1, ','))) {
	  *p2 = 0;
	}
      }
      else {
	p1 = NULL;
      }
    }
    free(p3);
  }

  /** string parameters **/
#ifdef HAVE_SSL
  _options.sslkeyfile = STRDUP(args_info.sslkeyfile_arg);
  _options.sslkeypass = STRDUP(args_info.sslkeypass_arg);
  _options.sslcertfile = STRDUP(args_info.sslcertfile_arg);
  _options.sslcafile = STRDUP(args_info.sslcafile_arg);
#endif

#ifdef USING_IPC_UNIX
  _options.unixipc = STRDUP(args_info.unixipc_arg);
#endif

#ifdef HAVE_NETFILTER_COOVA
  _options.kname = STRDUP(args_info.kname_arg);
#endif

#ifdef ENABLE_DNSLOG
  _options.dnslog = STRDUP(args_info.dnslog_arg);
#else
  if (args_info.dnslog_arg)
    log_err(0, "option dnslog given when no support built-in");
#endif

#ifdef ENABLE_IPWHITELIST
  _options.ipwhitelist = STRDUP(args_info.ipwhitelist_arg);
#else
  if (args_info.ipwhitelist_arg)
    log_err(0, "option ipwhitelist given when no support built-in");
#endif

#ifdef ENABLE_UAMDOMAINFILE
  _options.uamdomainfile = STRDUP(args_info.uamdomainfile_arg);
#else
  if (args_info.uamdomainfile_arg)
    log_err(0, "option uamdomainfile given when no support built-in");
#endif

#ifdef ENABLE_MODULES
  _options.moddir = STRDUP(args_info.moddir_arg);
#else
  if (args_info.moddir_arg)
    log_err(0, "option moddir given when no support built-in");
#endif
  
  if (!args_info.proxysecret_arg) {
    _options.proxysecret = STRDUP(args_info.radiussecret_arg);
  }
  else {
    _options.proxysecret = STRDUP(args_info.proxysecret_arg);
  }

  _options.peerkey = STRDUP(args_info.peerkey_arg);
  _options.routeif = STRDUP(args_info.routeif_arg);
  _options.wwwdir = STRDUP(args_info.wwwdir_arg);
  _options.wwwbin = STRDUP(args_info.wwwbin_arg);
  _options.uamui = STRDUP(args_info.uamui_arg);
  _options.localusers = STRDUP(args_info.localusers_arg);
  _options.uamurl = STRDUP(args_info.uamserver_arg);
  _options.uamaaaurl = STRDUP(args_info.uamaaaurl_arg);
  _options.uamhomepage = STRDUP(args_info.uamhomepage_arg);
  _options.wisprlogin = STRDUP(args_info.wisprlogin_arg);
  _options.uamsecret = STRDUP(args_info.uamsecret_arg);
  _options.macsuffix = STRDUP(args_info.macsuffix_arg);
  _options.macpasswd = STRDUP(args_info.macpasswd_arg);
  _options.adminuser = STRDUP(args_info.adminuser_arg);
  _options.adminpasswd = STRDUP(args_info.adminpasswd_arg);
  _options.adminupdatefile = STRDUP(args_info.adminupdatefile_arg);
  _options.rtmonfile = STRDUP(args_info.rtmonfile_arg);
  _options.ssid = STRDUP(args_info.ssid_arg);
  _options.vlan = STRDUP(args_info.vlan_arg);
  _options.nasmac = STRDUP(args_info.nasmac_arg);
  _options.nasip = STRDUP(args_info.nasip_arg);
  _options.tundev = STRDUP(args_info.tundev_arg);
  _options.radiusnasid = STRDUP(args_info.radiusnasid_arg);
  _options.radiuslocationid = STRDUP(args_info.radiuslocationid_arg);
  _options.radiuslocationname = STRDUP(args_info.radiuslocationname_arg);
  _options.locationname = STRDUP(args_info.locationname_arg);
  _options.radiussecret = STRDUP(args_info.radiussecret_arg);
  _options.cmdsocket = STRDUP(args_info.cmdsocket_arg);
  _options.domain = STRDUP(args_info.domain_arg);
  _options.ipup = STRDUP(args_info.ipup_arg);
  _options.ipdown = STRDUP(args_info.ipdown_arg);
  _options.conup = STRDUP(args_info.conup_arg);
  _options.condown = STRDUP(args_info.condown_arg);
  _options.macup = STRDUP(args_info.macup_arg);
  _options.macdown = STRDUP(args_info.macdown_arg);
  _options.pidfile = STRDUP(args_info.pidfile_arg);
  _options.statedir = STRDUP(args_info.statedir_arg);
  _options.usestatusfile = STRDUP(args_info.usestatusfile_arg);
  _options.uamaliasname = STRDUP(args_info.uamaliasname_arg);
  _options.uamhostname = STRDUP(args_info.uamhostname_arg);
  _options.binconfig = STRDUP(args_info.bin_arg);
  _options.ethers = STRDUP(args_info.ethers_arg);

  ret = 0;

  if (_options.binconfig) { /* save out the configuration */
    bstring bt = bfromcstr("");
    int ok = options_save(_options.binconfig, bt);
    if (!ok) log_err(0, "could not save configuration options!");
    bdestroy(bt);
  }

  if (args_info.reload_flag) {
    if (execl(SBINDIR "/chilli_query", "chilli_query", 
	      args_info.cmdsocket_arg, "reload", (char *) 0) != 0) {
      log_err(errno, "execl() did not return 0!");
      exit(2);
    }
  }

 end_processing:
  cmdline_parser_free (&args_info);

  return ret;
}

