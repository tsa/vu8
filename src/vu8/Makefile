.PHONY: default test

BUILD_DIR := obj

default:
	@mkdir -p ${BUILD_DIR}
	${MAKE} -C ${BUILD_DIR} -f ../mk/v8
	cd ${BUILD_DIR} && cmake .. && ${MAKE}

test:
	@cd ${BUILD_DIR} && ctest --verbose
