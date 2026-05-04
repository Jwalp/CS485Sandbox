# PLEASE READ
Ensure that your system has the correct packages for the `make` and `gcc` commands (vim is optional)

Please `sudo apt install debootstrap`
and do
```bash
sudo debootstrap --arch=amd64 --variant=minbase \
    --include=sudo,bash,coreutils,iputils-ping,procps \
    noble /srv/sandbox-rootfs http://archive.ubuntu.com/ubuntu
```
to get the desktop installed

For permissions, do `sudo chmod -R 755 /srv/sandbox-rootfs`

/srv/sandbox-rootfs/etc/shadow locked to permissions 000 for security

`sudo` command does not work, you are already root user (check using `id`)
