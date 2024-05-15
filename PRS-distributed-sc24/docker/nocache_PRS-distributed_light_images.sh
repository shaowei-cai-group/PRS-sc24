#!/bin/sh

docker build --no-cache -t satcomp-prs-distributed:common ../ --file common/Dockerfile
docker build --no-cache -t satcomp-prs-distributed:leader ../ --file leader/Dockerfile
docker build --no-cache -t satcomp-prs-distributed:worker ../ --file worker/Dockerfile