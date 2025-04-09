# Dockerfile.debian
FROM debian:bullseye

# Install dependencies
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    g++ \
    make \
    && apt-get clean

WORKDIR /webserver

COPY . .

RUN make

#CMD [ "./webserv", "configuration_files/fancywebsite.conf" ]