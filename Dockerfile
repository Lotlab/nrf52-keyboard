FROM python:3.7-slim-buster
WORKDIR /work

RUN apt-get update && apt-get install -y make git wget unzip && rm -rf /var/lib/apt/lists/* && pip3 install --upgrade pip && pip3 install nrfutil
COPY docker/pre.sh .
RUN ./pre.sh
COPY . .
RUN ./docker/post.sh

ENTRYPOINT ["/bin/bash"]
