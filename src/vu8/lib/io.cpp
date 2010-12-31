#include <vu8/Class.hpp>
#include <vu8/String.hpp>
#include <vu8/Throw.hpp>
#include <vu8/Module.hpp>

#include <boost/multi_index/member.hpp>

#include <iostream>
#include <fstream>

namespace tsa { namespace vu8 { namespace io {

struct FileBase {
    v8::Handle<v8::Value> IsOpen(const v8::Arguments& args) {
        return v8::Boolean::New(stream_.is_open());
    }

    v8::Handle<v8::Value> Good(const v8::Arguments& args) {
        return v8::Boolean::New(stream_.good());
    }

    v8::Handle<v8::Value> Eof(const v8::Arguments& args) {
        return v8::Boolean::New(stream_.eof());
    }

    v8::Handle<v8::Value> Close(const v8::Arguments& args) {
        stream_.close();
        return v8::Undefined();
    }

  protected:
    std::fstream stream_;
};

struct FileWriter : FileBase {
    v8::Handle<v8::Value> Open(const v8::Arguments& args) {
        if (1 != args.Length())
            return Throw("FileWriter.open: incorrect number of arguments");

        v8::HandleScope scope;
        v8::String::Utf8Value utf8Str(args[0]);
        char const *str = ToCString(utf8Str);
        stream_.open(str, std::ios_base::out);
        return scope.Close(v8::Boolean::New(stream_.good()));
    }

    v8::Handle<v8::Value> Print(const v8::Arguments& args) {
        bool first = true;
        for (int i = 0; i < args.Length(); ++i) {
            v8::HandleScope scope;
            if (first) first = false;
            else stream_ << ' ';
            v8::String::Utf8Value str(args[i]);
            stream_ << ToCString(str);
        }
        return v8::Undefined();
    }

    v8::Handle<v8::Value> Println(const v8::Arguments& args) {
        Print(args);
        stream_ << std::endl;
        return v8::Undefined();
    }

    FileWriter(const v8::Arguments& args) {
        if (1 == args.Length()) Open(args);
    }
};

struct FileReader : FileBase {
    v8::Handle<v8::Value> Open(const v8::Arguments& args) {
        if (1 != args.Length())
            return Throw("FileWriter.open: incorrect number of arguments");

        v8::HandleScope scope;
        v8::String::Utf8Value utf8Str(args[0]);
        char const *str = ToCString(utf8Str);
        stream_.open(str, std::ios_base::in);
        return scope.Close(v8::Boolean::New(stream_.good()));
    }

    v8::Handle<v8::Value> GetLine(const v8::Arguments& args) {
        if (stream_.good() && ! stream_.eof()) {
            std::string line;
            std::getline(stream_, line);
            return v8::String::New(line.c_str());
        }
        else {
            return v8::Undefined();
        }
    }

    FileReader(const v8::Arguments& args) {
        if (1 == args.Length()) Open(args);
    }
};

v8::Handle<v8::Value> Open() {
    Module mod;

    BasicClass<FileBase> fileBase;
    fileBase.Method<&FileBase::Close>("close")
            .Method<&FileBase::Good>("good")
            .Method<&FileBase::IsOpen>("is_open")
            .Method<&FileBase::Eof>("eof")
            ;

    Class<FileWriter> fileWriter(fileBase);
    fileWriter.Method<&FileWriter::Open>("open")
              .Method<&FileWriter::Print>("print")
              .Method<&FileWriter::Println>("println")
              ;

    Class<FileReader> fileReader(fileBase);
    fileReader.Method<&FileReader::Open>("open")
              .Method<&FileReader::GetLine>("getln")
              ;

    mod("FileWriter", fileWriter);
    mod("FileReader", fileReader);
    return mod.NewInstance();
}

} } }

extern "C" {
    v8::Handle<v8::Value> vu8_module_io() { return tsa::vu8::io::Open(); }
}
