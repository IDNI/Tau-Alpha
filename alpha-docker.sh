docker build . --tag alpha
docker run --rm -ti -v$(pwd):/alpha alpha
