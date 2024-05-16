#!/bin/sh

docker build --no-cache -t satcomp-prs:common ../ --file common/Dockerfile
docker build --no-cache -t satcomp-prs:leader ../ --file leader/Dockerfile