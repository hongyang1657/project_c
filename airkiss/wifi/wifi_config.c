
#include <stdio.h>
#include <stdlib.h>
#include "./src/api_net_config.h"


static int app_net_callback(NET_CONFIG_TYPE_T type, NET_CONFIG_MSG_TYPE_T error_type, char *ssid, char *pwd)
{
    (void)type;
    (void)error_type;
    (void)ssid;
    (void)pwd;
    printf("type=%d,error=%d,ssid=%s,pwd=%s\n",type,error_type,ssid,pwd);
    return 0;
}

int main(int argc, char **argv){
    float f;

    api_net_config_regist_callback(app_net_callback);

    api_net_config_start(API_NET_CONFIG_AIR_KISS_TYPE);

    scanf("%f",&f);
    printf("Value = %f", f);
    return 0;
}
