FROM satcomp-prs-distributed:common AS builder
USER root

FROM satcomp-infrastructure:leader

WORKDIR /

COPY --from=builder /cloud-sat/PRS-distributed /PRS-distributed

COPY --chown=ecs-user /docker/leader/init_solver.sh /competition/init_solver.sh
COPY --chown=ecs-user /docker/leader/run_solver.sh /competition/run_solver.sh
COPY --chown=ecs-user /docker/leader/solver /competition/solver

USER ecs-user
RUN chmod +x /competition/init_solver.sh
RUN chmod +x /competition/run_solver.sh
RUN chmod +x /competition/solver
