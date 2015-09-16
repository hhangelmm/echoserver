# run a tcp echoserver 
FROM ubuntu:14.04
MAINTAINER Cheng xuemin <xuemin.cheng@gmail.com>

RUN apt-get update && apt-get install -y \
    git \
    ca-certificates \
    build-essential \
    libevent-dev \
    --no-install-recommends
RUN git clone https://github.com/snowcrystall/echoserver.git /usr/local/echoserver
RUN cd /usr/local/echoserver \
    && make 
EXPOSE 8000
CMD     ["/usr/local/echoserver/echoserver", ""]
