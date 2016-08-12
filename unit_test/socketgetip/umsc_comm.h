/**
filename: umsc_comm.c
compiled as a common used library for other process modules
#date:2013-08-28
#author:ljyang
*/

#ifndef UMSC_COMM_INC_H
#define UMSC_COMM_INC_H

#define UPGRADE_URL "upgrade.uncplay.com"
//#define UPGRADE_URL_FORCE "http://%s/upgrade?version=%s&force=true"
//#define UPGRADE_URL_MANUAL "http://%s/upgrade?version=%s"
#define UPGRADE_URL_FORCE "http://%s/upgrade/v2/upgrade.php?version=%s&force=true"
#define UPGRADE_URL_MANUAL "http://%s/upgrade/v2/upgrade.php?version=%s"
#define FORCE_FLAG "/tmp/force_flag"
#define MANUAL_FLAG "/tmp/manual_flag"
#define MANUAL_UPGRADE "/tmp/manual_upgrade"
#define FORCE_UPGRADE "/tmp/force_upgrade"


/**
func:get_operation_mode
param:none
return :
	workmode 
note: 查询当前的工作模式
  */
int get_operation_mode(void);

/**
func:set_operation_mode
param:
	int mode: wifi operation mode
return :
	0==success
	-1==fail
	workmode 
note: 查询当前的工作模式
  */
int set_operation_mode(int mode);

void set_manual_upgrade_flag(void);
void set_force_upgrade_flag(void);
int is_manual_upgrade(char *url);
int is_force_upgrade(char *url);
char *get_upgrade_server_addr(char *addr, int max_len);
void set_http_request_url_manual(char *url);
void set_http_request_url_force(char *url);

char *get_msc_dev_nam(char *nam, int max_len);
void set_msc_dev_nam(char *nam);
char *get_software_version(char *version, int max_len);
void set_image_new_flag(void);
void set_image_ok_flag(void);
int get_running_image(void);

enum{
	GATEWAY_MOD=1,
	APCLI_MOD = 3
};

#ifndef STA_CTRL_RELEASE
#define STA_CTRL_RELEASE
#endif


int get_net_if_info(char *net_if, char *ipaddr, char *broadaddr, char *macaddr);
int disable_ap(void);
int enable_ap(void);
void set_2860_env_val(char *env, char *value);
void set_uboot_env_val(char *env, char *value);
char *get_2860_env_val(char *env, char *value, int max_len);
char *get_uboot_env_val(char *env, char *value, int max_len);
int is_disconnect_from_internet(void);
int is_disconnect_from_ap(void);
void open_debug_input(void);
#endif
