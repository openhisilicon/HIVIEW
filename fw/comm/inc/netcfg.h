#ifndef __NETCFG_H__
#define __NETCFG_H__

#ifdef __cplusplus
extern "C" {
#endif

int netcfg_active(char *name, char *action); 
int netcfg_netlink_status(const char * const if_name);
int netcfg_get_ip_addr(char *name,char *net_ip);
int netcfg_set_ip_addr(char *name, char *net_ip);
int netcfg_get_mac_addr(char *name, char *net_mac);
int netcfg_set_mac_addr(char *name,char *addr);
int netcfg_get_gw_addr(char *name,char *gateway_addr);
int netcfg_set_def_gw_addr(char *name,char *gateway_addr);
int netcfg_get_mask_addr(char *name,char *net_mask);
int netcfg_set_mask_addr(char *name,char *mask_ip); 
int netcfg_get_boardcast_addr(char *name,char *net_brdaddr);
int netcfg_del_def_gw_addr(char *name,char *gateway_addr);
int netcfg_update_dns(char *cfgFile, char *dnsaddr, char *dnsaddr2);
int netcfg_get_dns(char *cfgFile, char *dnsaddr, char *dnsaddr2);
int netcfg_find_listen_port(int nPort);
int netcfg_add_seg_route(const char *name, const char *route, const char *net_mask);
int netcfg_del_seg_route(const char *name, const char *route, const char *net_mask);
int netcfg_same_subnet(char *sub_mask, char *ip_a, char *ip_b);


#ifdef __cplusplus
}
#endif 

#endif // __NETCFG_H__

