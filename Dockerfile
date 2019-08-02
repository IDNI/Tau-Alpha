FROM debian:buster-slim

RUN apt update && apt -y upgrade && apt -y install g++ cmake make git

EXPOSE 6286
VOLUME /alpha
WORKDIR /alpha
ENTRYPOINT ["/alpha/alpha.sh"]
