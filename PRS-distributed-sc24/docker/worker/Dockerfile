FROM satcomp-prs-distributed:common AS builder
USER root

FROM satcomp-infrastructure:worker
WORKDIR /

COPY --from=builder /cloud-sat/PRS-distributed /PRS-distributed

COPY --chown=ecs-user /docker/worker/init_solver.sh /competition/init_solver.sh
COPY --chown=ecs-user /docker/worker/worker /competition/worker

USER ecs-user
RUN chmod +x /competition/init_solver.sh
RUN chmod +x /competition/worker
