#! /bin/sh
# genlsm.sh.  Generated from genlsm.sh.in by configure.

# WARNING:
# WARNING: If you change the description, also change specgen.sh!
# WARNING:

cat <<EOF
Begin4
Title:		fetchmail
Version:	6.3.21
Entered-date:	`date -u +%Y-%m-%d`
Description:	Fetchmail is a free, full-featured, robust, and well-documented
		remote mail retrieval and forwarding utility intended to be used
		over on-demand TCP/IP links (such as SLIP or PPP connections).
		It retrieves mail from remote mail servers and forwards it
		to your local (client) machine's delivery system, so it can
		then be be read by normal mail user agents such as mutt, elm,
		pine, (x)emacs/gnus, or mailx.  Comes with an interactive GUI
		configurator (in Python/Tk) suitable for end-users.
Keywords:	mail, client, POP3, APOP, KPOP, IMAP, ETRN, ODMR, SMTP, ESMTP, GSSAPI, RPA, NTLM, CRAM-MD5, SASL
Author:		matthias.andree@gmx.de (Matthias Andree)
		shetye@bombay.retortsoft.com (Sunil Shetye)
		rfunk@funknet.net (Rob Funk)
Maintained-by:	fetchmail-devel@lists.berlios.de
Primary-site:	http://developer.berlios.de/projects/fetchmail
		`ls -sk fetchmail-6.3.21.tar.bz2 | \
		 gawk '{ printf "%4dkB", $1 }'` fetchmail-6.3.21.tar.bz2
		   1kB fetchmail-6.3.21.tar.bz2.asc
Platforms:	All
Copying-policy: GPL
End
EOF
