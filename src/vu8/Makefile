.PHONY: default

BUILD_DIR := obj

default:
	@mkdir -p ${BUILD_DIR}
	${MAKE} -C ${BUILD_DIR} -f ../mk/v8
	cd ${BUILD_DIR} && cmake .. && ${MAKE}

