#define _GNU_SOURCE
#include <string.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <syscall.h>
static char child_stack[1048576];

static int child_fn(void* arg) {
    system("mount -t proc proc /proc --make-private");
    system("mount --make-private -o remount /");
    system("ifconfig veth1 10.1.1.1/24 up");
    
    // setting up virt fs
    system("dd if=/dev/zero of=image.img bs=32K count=90000");
    system("losetup /dev/loop25 image.img");
    system("mkfs.ext4 image.img");
    system("mkdir -p /home/fs");
    system("mount -o loop /dev/loop25 /home/fs");

    system("ps aux");

    system("cd /home/fs && bash");
    
    // exiting from container
    system("umount /home/fs");
    system("losetup -d /dev/loop25");
    system("sudo -S umount /proc");
    return 0;
}

int main() {
    pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_NEWNS|CLONE_NEWNET | CLONE_NEWPID | SIGCHLD, NULL);
    char str[80];
    sprintf(str, "echo %d", child_pid);
    system(str);
    // settign cgroup for container
    sprintf(str, "echo %d > /sys/fs/cgroup/memory/container/tasks", child_pid);
    system(str);

    sprintf(str, "ip link add name veth0 type veth peer name veth1 netns %d", child_pid);
    system(str);
    system("ifconfig veth0 10.1.1.2/24 up");
    system("ping -I veth0 -c 3 10.1.1.1");

    waitpid(child_pid, NULL, 0);

    _exit(EXIT_SUCCESS);
}
