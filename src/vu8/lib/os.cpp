#include <vu8/Class.hpp>
#include <vu8/Module.hpp>

#include <boost/multi_index/member.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdio>

#include <sys/stat.h>
#include <signal.h>

namespace vu8 { namespace os {

char const *Getenv(char const *name) {
    return std::getenv(name);
}

bool Signal(int const pid, int const signal) {
    return ! ::kill(pid, signal);
}

static inline v8::Handle<v8::Value> Open() {
    v8::HandleScope scope;
    Module mod;
    return scope.Close(mod
        .Set<char const *(char const *), &Getenv>("getenv")
        .Set<bool(int const, int const), &Signal>("signal")
        .NewInstance());
}

} }

extern "C" {
    v8::Handle<v8::Value> vu8_module_init() { return vu8::os::Open(); }
}
