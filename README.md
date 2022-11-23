## Linux kernel module tracing and iteraction by netlink

A simple example for show how to use netlink for user space <=> kernel IPC
also it show how trace kernel module activity to remote syslog server.
It can pass UDP messages directly to remote SYSLOG daemon

# Preparation for kernel development

Run: 

$./before.sh

# Compilation

$make

# Test

$make test

echo ==== Module info: `modinfo netlink_kernel.ko` =========
==== Module info: filename: /root/c.w/linux_kernel.w/netlink_example/netlink_kernel.ko license: GPL depends: retpoline: Y name: netlink_kernel vermagic: 5.10.0-19-686 SMP mod_unload modversions GEODE =========
sudo insmod 	netlink_kernel.ko || echo can not load module netlink_kernel.ko 
./netlink_user
netlink_user.c:51 Sending message to kernel
netlink_user.c:53 Waiting for message from kernel
netlink_user.c:57 Received message payload: Hello from kernel inside
sudo rmmod  netlink_kernel.ko
sudo dmesg | tail	
[ 9830.382548] netpoll: KERNEL_TRACER: local port 17664
[ 9830.382559] netpoll: KERNEL_TRACER: local IPv4 address 127.0.0.1
[ 9830.382563] netpoll: KERNEL_TRACER: interface 'lo'
[ 9830.382566] netpoll: KERNEL_TRACER: remote port 514
[ 9830.382569] netpoll: KERNEL_TRACER: remote IPv4 address 127.0.0.1
[ 9830.382573] netpoll: KERNEL_TRACER: remote ethernet address ff:ff:ff:ff:ff:ff
[ 9830.382629] /root/c.w/linux_kernel.w/netlink_example/netlink_kernel.c:124 load and start module hello_init inside kernel version 5.10.0-19-686
[ 9830.391579] Entering: /root/c.w/linux_kernel.w/netlink_example/netlink_kernel.c:hello_nl_recv_msg
[ 9830.391591] /root/c.w/linux_kernel.w/netlink_example/netlink_kernel.c:94 received msg body:Hello
[ 9830.428283] /root/c.w/linux_kernel.w/netlink_example/netlink_kernel.c:145 stop module
 
