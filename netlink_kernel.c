//Taken base from https://stackoverflow.com/questions/15215865/netlink-sockets-in-c-using-the-3-x-linux-kernel?lq=1
// Kernel module code 

#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#include <generated/utsrelease.h>  

#define NETLINK_USER 31

//#if (LINUX_VERSION_CODE <= KERNEL_VERSION(5,0,0))
//#panic "bad kernel version" UTS_RELEASE
//#endif 

struct sock *nl_sk = NULL;

////////////////////// send udp
// See Using FTrace https://lwn.net/Articles/365835/

// Code
// Declaration

#include <linux/netpoll.h>

static void send_to_syslog(char * message, int len)
{
#define MESSAGE_SIZE 1024
#define INADDR_LOCAL ((unsigned long int)0x7f000001)   //CLIENT IP 127.0.0.1
#define INADDR_SEND  ((unsigned long int)0x7f000001)   //SYSLOG    127.0.0.1
static struct netpoll* np = NULL;
static struct netpoll  np_t;

//#define INADDR_LOCAL ((unsigned long int)0xc0a80a54) //192.168.10.84
//#define INADDR_SEND  ((unsigned long int)0xc0a80a55) //192.168.10.85

// Initialization

np_t.name = "KERNEL_TRACER";
strlcpy(np_t.dev_name, "lo", IFNAMSIZ);

//np_t.local_ip  = (union inet_addr) 1; //htonl(INADDR_LOCAL);
//np_t.remote_ip = (union inet_addr) 2; //htonl(INADDR_SEND);

//np_t.local_ip.ip  = ifr_addr("127.0.0.1"); //htonl(INADDR_LOCAL);
//np_t.remote_ip.ip = inet_addr("127.0.0.1"); //htonl(INADDR_SEND);

//np_t.local_ip.in  =  (struct in_add) 1; //htonl(INADDR_LOCAL);

np_t.local_ip.ip  =   htonl(INADDR_LOCAL);
np_t.remote_ip.ip =   htonl(INADDR_SEND);

unsigned short rand_port;
get_random_bytes(&rand_port,2);

np_t.local_port  = 0xFD00 & rand_port; // random port > 1024
np_t.remote_port = 514;                // syslog destination port

memset(np_t.remote_mac, 0xff, ETH_ALEN);
netpoll_print_options(&np_t);
netpoll_setup(&np_t);
np = &np_t;

// send udp packet
 netpoll_send_udp(np,message,len);
}
////////// end send udp


static void send_ip(char * message, int len)
{
#define MESSAGE_SIZE 1024
#define INADDR_LOCAL ((unsigned long int)0x7f000001)   //127.0.0.1
#define INADDR_SEND    ((unsigned long int)0x7f000001) //127.0.0.1

}

//////////

static void hello_nl_recv_msg(struct sk_buff *skb) {

struct nlmsghdr *nlh;
int pid;
struct sk_buff *skb_out;
int msg_size;
char *msg="Hello from kernel inside";
int res;

printk(KERN_INFO "Entering: %s:%s\n", __FILE__, __FUNCTION__);

msg_size=strlen(msg);

nlh=(struct nlmsghdr*)skb->data;
printk(KERN_INFO "%s:%d received msg body:%s\n",__FILE__,__LINE__,(char*)nlmsg_data(nlh));
pid = nlh->nlmsg_pid; /*pid of sending process */

skb_out = nlmsg_new(msg_size,0);

if(!skb_out)
{

    printk(KERN_ERR "%s:%d Failed to allocate by nlmsg_new(...)\n",__FILE__,__LINE__);
    return;

} 
nlh=nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);  
NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
strncpy(nlmsg_data(nlh),msg,msg_size);

res=nlmsg_unicast(nl_sk,skb_out,pid);

if(res<0)
    printk(KERN_INFO "%s:%d Error while sending reply to user space by nlmsg_unicast(...)\n",__FILE__,__LINE__);
}

static int __init hello_init(void) {

 static char message[MESSAGE_SIZE];
 sprintf(message,"<%d> from kernel to syslog podkorytov test\n",0x1);
//static char *message = "<86> from kernel to syslog test\n";
 int len = strlen(message);

send_to_syslog(message,len);// send udp packet to remote syslog
printk("%s:%d load and start module %s inside kernel version %s\n",__FILE__,__LINE__,__FUNCTION__,UTS_RELEASE);
//This is for 3.6 kernels and above.
struct netlink_kernel_cfg cfg = {
    .input = hello_nl_recv_msg,
};

nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
//nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, 0, hello_nl_recv_msg,NULL,THIS_MODULE);
if(!nl_sk)
{

    printk(KERN_ALERT "%s:%d Error creating socket.\n",__FILE__,__LINE__);
    return -10;

}

return 0;
}

static void __exit hello_exit(void) {

printk(KERN_INFO "%s:%d stop module\n",__FILE__,__LINE__);
netlink_kernel_release(nl_sk);
}

module_init(hello_init); module_exit(hello_exit);

MODULE_LICENSE("GPL");
