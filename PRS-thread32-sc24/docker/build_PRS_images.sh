#!/bin/sh

docker build -t satcomp-prs:common ../ --file common/Dockerfile
docker build -t satcomp-prs:leader ../ --file leader/Dockerfile