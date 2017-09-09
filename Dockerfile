FROM debian:jessie

LABEL maintainer "opsxcq@strm.sh"

RUN apt-get update && \
        apt-get upgrade -y && \
        DEBIAN_FRONTEND=noninteractive apt-get install -y \
        acl attr autoconf bison build-essential \
        debhelper dnsutils docbook-xml docbook-xsl flex gdb krb5-user \
        libacl1-dev libaio-dev libattr1-dev libblkid-dev libbsd-dev \
        libcap-dev libcups2-dev libgnutls28-dev libjson-perl \
        libldap2-dev libncurses5-dev libpam0g-dev libparse-yapp-perl \
        libpopt-dev libreadline-dev perl perl-modules pkg-config \
        python-all-dev python-dev python-dnspython python-crypto \
        xsltproc zlib1g-dev&& \
        apt-get clean && \
        rm -rf /var/lib/apt/lists/*

# Add sources and compile it
COPY src /src
RUN cd /src && \
    ./configure && \
    make && \
    make install

# Samba configuration
COPY smb.conf /
RUN addgroup -gid 666 pwned && \
    (sleep 1;echo "nosambanocry"; sleep 1;echo "nosambanocry") |adduser --gid 666 --uid 666 sambacry && \
    (sleep 1;echo "nosambanocry"; sleep 1;echo "nosambanocry") | /usr/local/samba/bin/smbpasswd -a -s -c /smb.conf sambacry && \
    mkdir /data && \
    chown 666:666 /data

# Expose the samba to the network
EXPOSE 137/udp 138/udp 139 445

ENTRYPOINT ["/usr/local/samba/sbin/smbd"]
CMD ["-F","-S","-s","/smb.conf","--debuglevel=10"]
