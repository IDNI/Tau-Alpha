FROM debian:buster-slim

RUN \
	apt update \
	&& apt -y upgrade \
	&& apt -y install \
		wget git cmake make llvm g++ python2.7 \
		libboost-program-options-dev libboost-thread-dev \
		libboost-atomic-dev libboost-system-dev libboost-filesystem-dev\
		libboost-chrono-dev libboost-date-time-dev \
	&& update-alternatives --install \
		/usr/bin/python python /usr/bin/python2.7 1

EXPOSE 6286
VOLUME /alpha
WORKDIR /alpha
ENTRYPOINT ["/alpha/alpha.sh"]
