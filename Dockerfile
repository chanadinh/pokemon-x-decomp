FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
	&& apt-get install --no-install-recommends -y \
		build-essential \
		ca-certificates \
		git \
		libsdl2-dev \
		python3 \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /opt/pokemon-x-decomp
COPY . .

CMD ["bash"]
