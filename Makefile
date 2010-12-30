.PHONY: default getv8 getscons buildscons clean cmake

v8svn   := http://v8.googlecode.com/svn/trunk/
scons_v := 2.0.1
scons   := http://downloads.sourceforge.net/project/scons/scons/${scons_v}/scons-${scons_v}.tar.gz
arch    := x64

default: buildv8

.status:
	touch $@

scons/bin/scons: scons/build/setup.py
	cd scons/build && python setup.py install --prefix=$$PWD/..
	touch $@

scons/build/setup.py: .status
	mkdir -p scons/build && cd scons/build && wget ${scons} && \
	tar xzvf scons*.tar.gz && rm *.tar.gz && mv scons*/* .
	touch $@

v8/ChangeLog: .status
	svn checkout ${v8svn} v8
	touch $@

v8/libv8.so: v8/ChangeLog scons/bin/scons .status
	cd v8 && CXX=`which g++` ../scons/bin/scons library=shared arch=${arch}

v8/libv8.a: v8/ChangeLog scons/bin/scons .status
	cd v8 && CXX=`which g++` ../scons/bin/scons library=static arch=${arch}

getscons: scons/build/setup.py
buildscons: scons/bin/scons
getv8: v8/ChangeLog
buildv8: v8/libv8.so v8/libv8.a

clean:
	rm -rf scons v8

cmake:
	mkdir -p obj && cd obj && cmake ..
