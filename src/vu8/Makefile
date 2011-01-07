.PHONY: default test clean factory proto

O := obj
PP_ARGS := -I/opt/tsa-env/64/include -I.. -I${O}/include -E -P

default:
	@if [ -d ${O} ] ; then \
		make -C ${O}; \
	else \
		rm -rf ${O}; \
		mkdir -p ${O}; \
		cd ${O} && cmake .. && ${MAKE}; \
	fi

test:
	@cd ${O} && ctest --verbose

clean:
	${MAKE} -C ${O} clean

factory:
	g++ ${PP_ARGS} Factory.hpp

proto:
	g++ ${PP_ARGS} detail/Proto.hpp
