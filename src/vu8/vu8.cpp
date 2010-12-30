#include <v8.h>

#include <fstream>
#include <sstream>
#include <iostream>

namespace tsa { namespace vu8 {

// Extracts a C string from a V8 Utf8Value.
const char* toCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

v8::Handle<v8::Value> println(const v8::Arguments& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); ++i) {
        v8::HandleScope handle_scope;
        if (first) first = false;
        else std::cout << ' ';
        v8::String::Utf8Value str(args[i]);
        std::cout << toCString(str);
    }
    std::cout << std::endl;
    return v8::Undefined();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "must supply at least one argument\n";
        return 1;
    }

    std::ifstream stream(argv[1]);
    if (! stream) {
        std::cerr << "could not locate file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream scriptStream;
    std::string line;
    while (! stream.eof()) {
        std::getline(stream, line);
        scriptStream << line;
    }

    v8::HandleScope scope;
    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
    global->Set(v8::String::New("println"),
                v8::FunctionTemplate::New(&println));

    v8::Handle<v8::Context> context = v8::Context::New(0, global);
    v8::Context::Scope context_scope(context);

    v8::Handle<v8::Script> script =
        v8::Script::Compile(v8::String::New(scriptStream.str().c_str()));
    script->Run();
    return 0;
}

} }

int main(int argc, char *argv[]) {
    return tsa::vu8::main(argc, argv);
}
