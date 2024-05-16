#!/bin/sh

docker build -t satcomp-prs64:common ../ --file common/Dockerfile
docker build -t satcomp-prs64:leader ../ --file leader/Dockerfile