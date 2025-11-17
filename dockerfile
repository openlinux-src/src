FROM scratch
ADD build/x86_64/openlinux-1.0.0-alpha-x86_64-rootfs.tar.gz /
CMD ["/bin/sh"]
