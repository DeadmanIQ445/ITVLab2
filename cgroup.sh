#!/bin/bash

if [ ! -d /sys/fs/cgroup/memory/container ]
then
    sudo mkdir /sys/fs/cgroup/memory/container
    echo 1953000 > /sys/fs/cgroup/memory/container/memory.max_usage_in_bytes
    echo "created container cgroup"
fi
