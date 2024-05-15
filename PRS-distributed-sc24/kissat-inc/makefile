all:
	$(MAKE) -C "/root/PRS-sc23/kissat-inc/build"
kissat:
	$(MAKE) -C "/root/PRS-sc23/kissat-inc/build" kissat
tissat:
	$(MAKE) -C "/root/PRS-sc23/kissat-inc/build" tissat
clean:
	rm -f "/root/PRS-sc23/kissat-inc"/makefile
	-$(MAKE) -C "/root/PRS-sc23/kissat-inc/build" clean
	rm -rf "/root/PRS-sc23/kissat-inc/build"
coverage:
	$(MAKE) -C "/root/PRS-sc23/kissat-inc/build" coverage
indent:
	$(MAKE) -C "/root/PRS-sc23/kissat-inc/build" indent
test:
	$(MAKE) -C "/root/PRS-sc23/kissat-inc/build" test
.PHONY: all clean coverage indent kissat test tissat
