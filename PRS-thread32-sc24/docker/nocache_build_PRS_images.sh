#!/bin/sh

docker build --no-cache -t satcomp-prs32:common ../ --file common/Dockerfile
docker build --no-cache -t satcomp-prs32:leader ../ --file leader/Dockerfile