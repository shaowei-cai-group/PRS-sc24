#!/bin/sh

docker build --no-cache -t satcomp-prs64:common ../ --file common/Dockerfile
docker build --no-cache -t satcomp-prs64:leader ../ --file leader/Dockerfile