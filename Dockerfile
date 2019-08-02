FROM debian:buster-slim

RUN apt update && apt -y upgrade && apt -y install g++ cmake make git python2.7\
 && update-alternatives --install /usr/bin/python python /usr/bin/python2.7 1 \
 && cd / && git clone https://github.com/emscripten-core/emsdk.git \
 && cd /emsdk && ./emsdk install latest && ./emsdk activate latest

EXPOSE 6286
VOLUME /alpha
WORKDIR /alpha
ENTRYPOINT ["/alpha/alpha.sh"]
