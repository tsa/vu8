.PHONY: default install test clean factory proto call callfrom

O := obj
PP_ARGS := -I/opt/tsa-env/64/include -I.. -I${O}/include -I${O}/v8/include -E -P

default:
	@if [ -d ${O} ] ; then \
		make -C ${O}; \
	else \
		rm -rf ${O}; \
		mkdir -p ${O}; \
		cd ${O} && cmake .. && ${MAKE}; \
	fi

install:
	make -C ${O} install

test:
	@cd ${O} && ctest --verbose

clean:
	${MAKE} -C ${O} clean

factory:
	g++ ${PP_ARGS} Factory.hpp

proto:
	g++ ${PP_ARGS} detail/Proto.hpp

call:
	g++ ${PP_ARGS} CallV8.hpp

callfrom:
	g++ ${PP_ARGS} CallFromV8.hpp
