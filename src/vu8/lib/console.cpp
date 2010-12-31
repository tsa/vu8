#include <vu8/Module.hpp>
#include <vu8/String.hpp>

#include <iostream>

namespace tsa { namespace vu8 { namespace console {

v8::Handle<v8::Value> Println(const v8::Arguments& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); ++i) {
        v8::HandleScope handle_scope;
        if (first) first = false;
        else std::cout << ' ';
        v8::String::Utf8Value str(args[i]);
        std::cout << ToCString(str);
    }
    std::cout << std::endl;
    return v8::Undefined();
}

} } }

extern "C" {

using namespace tsa::vu8::console;
using namespace tsa::vu8;

v8::Handle<v8::Value> vu8_module_console() {
    Module mod;
    mod("println", &Println);
    return mod.NewInstance();
}

}
