.PHONY: default build install test clean config dist

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
		cd ${O} && cmake .. ${args} && ${MAKE}; \
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

dist:
	ver=vu8-`git tag | tail -n 1` && \
	cd ../.. && git archive --prefix $$ver/ master | gzip > src/vu8/$$ver.tar.gz
