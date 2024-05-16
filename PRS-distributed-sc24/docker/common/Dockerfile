FROM satcomp-infrastructure:common
USER root
#  Install required softwares

# RUN sed -i s@/archive.ubuntu.com/@/mirrors.bfsu.edu.cn/@g /etc/apt/sources.list
RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y vim cmake build-essential zlib1g-dev libopenmpi-dev wget unzip python3 gfortran curl
RUN apt install -y libboost-all-dev

WORKDIR /
COPY / /cloud-sat

WORKDIR /cloud-sat/kissat-inc
RUN make clean
RUN ./configure
RUN make -j

WORKDIR /cloud-sat/mapleCOMSPS/m4ri-20140914
RUN ./configure
RUN make clean
RUN make -j

WORKDIR /cloud-sat/mapleCOMSPS
RUN make clean
RUN make -j

WORKDIR /cloud-sat/yalsat
RUN ./configure.sh
RUN make -j

WORKDIR /cloud-sat/lingeling
RUN ./configure.sh
RUN make clean
RUN ./configure.sh
RUN make -j

WORKDIR /cloud-sat/
RUN make clean
RUN make -j
