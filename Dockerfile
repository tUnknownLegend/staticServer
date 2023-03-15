FROM ubuntu:20.04 AS myserver

RUN apt-get -y update && apt-get -y install g++ make cmake libevent-dev

WORKDIR server

COPY . .

RUN rm -r build && mkdir -p build && cd build && cmake .. && make

EXPOSE 8080

WORKDIR build

CMD [ "./StaticServer" ]
