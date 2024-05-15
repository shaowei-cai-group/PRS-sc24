#!/bin/sh


docker build -t satcomp-prs-distributed:common ../ --file common/Dockerfile
docker build -t satcomp-prs-distributed:leader ../ --file leader/Dockerfile
docker build -t satcomp-prs-distributed:worker ../ --file worker/Dockerfile