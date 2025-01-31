#ifndef __BFTPD_OPTIONS_H
#define __BFTPD_OPTIONS_H

#include "commands.h"
#include "mypaths.h"

#include <pwd.h>
#include <grp.h>


/*
Define some default options
*/
#ifdef SUPPORT_LOGIN_CONTROL  	// 使FTP Server 不支持超时退出
#define CONTROL_TIMEOUT 0
#else
#define CONTROL_TIMEOUT 300
#endif


#if 0
#define DATA_TIMEOUT 300
#define XFER_BUFSIZE 4096
#else
#define DATA_TIMEOUT 100
#define XFER_BUFSIZE 64000
#endif
#define XFER_DELAY 0
//#define FTPSERVER_DEFAULT_PORT       21
#define DEFAULT_PORT 21
/*Added by yehuisheng00183935@20110702 添加ftpds功能*/
#ifdef SUPPORT_ATP_FTPDS
#define FTPDS_DEFAULT_PORT 990
#endif
/*Added by yehuisheng00183935@20110702 添加ftpds功能*/

struct bftpd_option {
  char *name, *value;
  struct bftpd_option *next;
};
struct list_of_struct_passwd {
    struct passwd pwd;
    struct list_of_struct_passwd *next;
};
struct list_of_struct_group {
    struct group grp;
    struct list_of_struct_group *next;
};

struct directory {
    char *path;
    struct bftpd_option *options;
    struct directory *next;
};
struct global {
    struct bftpd_option *options;
    struct directory *directories;
};
struct group_of_users {
    struct list_of_struct_passwd *users;
    struct list_of_struct_group *groups;
    char *temp_members;
    struct bftpd_option *options;
    struct directory *directories;
    struct group_of_users *next;
};

struct bftpd_user {
    char *name;
    struct bftpd_option *options;
    struct directory *directories;
    struct bftpd_user *next;
};

extern struct group_of_users *config_groups;
extern struct bftpd_user *config_users;

void expand_groups();
void config_init();
char *config_getoption(char *name);
char *config_getoption_reread( char *find_me );
void config_end();
void Reread_Config_File();

#endif
