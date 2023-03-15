FROM ubuntu:latest AS myserver

RUN apt-get update -y && \
    apt upgrade -y \
    apt-get make \
    apt-get -y build-essential \
    apt-get -y gcc-10 g++-10 cpp-10 \
    cmake \
    libevent-dev \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100 --slave /usr/bin/g++ g++ /usr/bin/g++-10 --slave /usr/bin/gcov gcov /usr/bin/gcov-10

WORKDIR server

COPY . .

RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

ENTRYPOINT [ "./build/StaticServer" ]

EXPOSE 80