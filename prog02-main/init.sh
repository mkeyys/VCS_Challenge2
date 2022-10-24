#!/bin/bash

# add user A to system
useradd A

# set default password is "vcs" to user A
echo 'A:vcs' | chpasswd

# add user A to group sudo to run sudoer files
usermod -aG sudo A