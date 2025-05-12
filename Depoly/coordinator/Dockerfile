FROM python:3.9-slim

RUN apt-get update

RUN apt-get install -y curl

RUN curl -fsSL https://get.docker.com | sh

COPY . /

WORKDIR /coordinator

ENTRYPOINT [ "python", "coordinator.py"]