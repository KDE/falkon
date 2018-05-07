# Build:
#   docker build -t falkon-appimage-build .
# Run:
#   docker run -v $OUT_DIRECTORY:/out -it falkon-appimage-build $FALKON_TARBALL_URL

FROM centos:7

RUN yum -y install \
    wget \
    fontconfig \
    xz \
    openssl-devel \
    libX11-devel \
    xcb-util-devel \
    centos-release-scl \
    make \
    gettext \
    squashfs-tools \
    chrpath \
    which \
    mesa-libGL-devel \
    mesa-libEGL-devel \
    libXcomposite-devel \
    libXcursor-devel \
    alsa-lib-devel \
    libXi-devel \
    libXtst-devel \
    libXrandr-devel \
    xcb-util-keysyms-devel \
    gperf \
    flex \
    bison \
    libgcrypt-devel \
    systemd-devel

RUN yum -y install devtoolset-4-gcc-c++

RUN wget -O cmake-install https://cmake.org/files/v3.11/cmake-3.11.1-Linux-x86_64.sh && \
    chmod +x cmake-install && \
    ./cmake-install --skip-license --prefix=/usr && \
    rm cmake-install

COPY setup.sh /root/setup.sh
COPY qt-installer-noninteractive.qs /root/qt-installer-noninteractive.qs
RUN scl enable devtoolset-4 /root/setup.sh

COPY build.sh /root/build.sh
COPY build-appimage.sh /root/build-appimage.sh

ENTRYPOINT ["/root/build.sh"]
