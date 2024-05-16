#!/bin/sh

docker build -t satcomp-prs32:common ../ --file common/Dockerfile
docker build -t satcomp-prs32:leader ../ --file leader/Dockerfile