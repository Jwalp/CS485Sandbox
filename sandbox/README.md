# PLEASE READ
this folder is meant for our sandbox program to be developed in
- this file is for an initial commit only
- please remove once finished
- ensure that you understand how the makefile works (because I sure forgot)

Please sudo apt install debootstrap
and do
sudo debootstrap --arch=amd64 --variant=minbase \
    --include=bash,coreutils,iputils-ping,procps \
    noble /srv/sandbox-rootfs http://archive.ubuntu.com/ubuntu
    
    to get the desktop installed

For permissions, do sudo chmod -R 755 /srv/sandbox-rootfs
