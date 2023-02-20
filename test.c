#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <errno.h>
#include <asm-generic/int-ll64.h>

#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error message frame */
#define CAN_EXT_FLAG 0x10000000U /* extendend message frame */
char *can_read(int fd)
{
    struct can_frame rx;
    static char databuf[4096];
    char     *s;
    unsigned char     type;
    unsigned char     rtr;
    int i, got;

    s = &databuf[0];
    databuf[0] = '\0';
    /* retrieve only one message */ 
    got = read(fd, &rx , sizeof(rx));
    printf("got = %ld\n",got);
    printf("rx.can_id & CAN_EXT_FLAG = %d\n",rx.can_id & CAN_EXT_FLAG);
    printf("rx.can_id & CAN_RTR_FLAG = %d\n",rx.can_id & CAN_RTR_FLAG);
    if( got > 0) {
        if(rx.can_id & CAN_ERR_FLAG){
            printf("error message\n");
        }else{
            printf("data frame \n");
        }
        if(rx.can_id & CAN_RTR_FLAG){
            printf("发送的数据远程帧\n");
            rtr = 'R';
        }else{
            rtr = 'D';
        }

        if(rx.can_id & CAN_EFF_FLAG){
            printf("发送的数据是扩展帧\n");
            type = 'e';
        } else{
            type = 's';
        }
        s += sprintf(s, "%ld :%c%c(%d): ",
            rx.can_id, type, rtr, rx.can_dlc );      
        if(rtr == 'D') {
            for(i = 0; i < rx.can_dlc; i++) {
                s += sprintf(s, " %d", rx.data[i]); 
            }
        }
    }
    return databuf;
}

static int sock_fd;
struct ifreq ifr;
struct sockaddr_can addr;

int main(){
	/*******************创建套接字***********************/
	sock_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
	if (sock_fd < 0) {
        perror("socket create error ! \n");
        return -1;
    }
	//将套接字与can0绑定
	strcpy(ifr.ifr_name,"can0");
	ioctl(sock_fd, SIOCGIFINDEX, &ifr);  //设置设备为can0
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
    {
        perror("bind error!\n");
        return -1;
    }


    //调用can_read 函数
    char *p;
    p = can_read(sock_fd);
    printf("%s\n", p);    
    return 0;
}