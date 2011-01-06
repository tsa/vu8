.PHONY: default test clean factory

BUILD_DIR := obj

default:
	@mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR} && cmake .. && ${MAKE}

test:
	@cd ${BUILD_DIR} && ctest --verbose

clean:
	${MAKE} -C ${BUILD_DIR} clean

factory:
	g++ -I/opt/tsa-env/64/include -I.. -E -P Factory.hpp
