FROM satcomp-prs:common AS builder
USER root

FROM satcomp-infrastructure:leader

WORKDIR /

COPY --from=builder /PRS/PRS /PRS

COPY --chown=ecs-user /docker/leader/init_solver.sh /competition
COPY --chown=ecs-user /docker/leader/solver /competition

USER ecs-user

RUN chmod +x /competition/init_solver.sh
RUN chmod +x /competition/solver