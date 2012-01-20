.PHONY: default build install test clean config

-include Makefile.local

O := obj
PP_ARGS += -I.. -I${O}/include -I${O}/v8/include -E -P

default: build

build:
	@if [ -d ${O} ] ; then \
		make -C ${O}; \
	else \
		rm -rf ${O}; \
		mkdir -p ${O}; \
		cd ${O} && cmake .. && ${MAKE}; \
	fi

config:
	cd ${O} && ccmake .

install:
	make -C ${O} install

test:
	@cd ${O} && ctest --verbose

clean:
	@${MAKE} -C ${O} clean
	@rm -rf obj
