#include <v8.h>

#include <fstream>
#include <sstream>
#include <iostream>

namespace tsa { namespace v8 {

namespace v8 = ::v8;

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

    v8::Context::New();

    v8::Handle<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);
    v8::HandleScope scope;

    v8::Handle<v8::Script> script =
        v8::Script::Compile(v8::String::New(scriptStream.str().c_str()));
    script->Run();
    return 0;
}

} }

int main(int argc, char *argv[]) {
    return tsa::v8::main(argc, argv);
}
