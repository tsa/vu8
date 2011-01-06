#include <vu8/Module.hpp>

#include <iostream>

namespace vu8 { namespace console {

v8::Handle<v8::Value> Println(const v8::Arguments& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); ++i) {
        v8::HandleScope handle_scope;
        if (first) first = false;
        else std::cout << ' ';
        v8::String::Utf8Value str(args[i]);
        std::cout << *str;
    }
    std::cout << std::endl;
    return v8::Undefined();
}

static inline v8::Handle<v8::Value> Open() {
    v8::HandleScope scope;
    Module mod;
    mod("println", &Println);
    return mod.NewInstance();
}

} }

extern "C" {
    v8::Handle<v8::Value> vu8_module_console() {
        return vu8::console::Open();
    }
}
