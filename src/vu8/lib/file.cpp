#include <vu8/Class.hpp>
#include <vu8/Module.hpp>

#include <boost/multi_index/member.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>

namespace vu8 { namespace file {

struct FileBase {
    bool IsOpen() { return stream_.is_open(); }
    bool Good()   { return stream_.good(); }
    bool Eof()    { return stream_.eof(); }
    void Close()  { stream_.close(); }

  protected:
    std::fstream stream_;
};

struct FileWriter : FileBase {
    bool Open(const v8::Arguments& args) {
        if (1 != args.Length())
            throw std::runtime_error(
                "FileWriter.open: incorrect number of arguments");

        v8::String::Utf8Value utf8Str(args[0]);
        char const *str = *utf8Str;
        stream_.open(str, std::ios_base::out);
        return stream_.good();
    }

    void Print(const v8::Arguments& args) {
        bool first = true;
        for (int i = 0; i < args.Length(); ++i) {
            v8::HandleScope scope;
            if (first) first = false;
            else stream_ << ' ';
            v8::String::Utf8Value str(args[i]);
            stream_ << *str;
        }
    }

    void Println(const v8::Arguments& args) {
        Print(args);
        stream_ << std::endl;
    }

    FileWriter(const v8::Arguments& args) {
        if (1 == args.Length()) Open(args);
    }
};

struct FileReader : FileBase {
    bool Open(char const *str) {
        stream_.open(str, std::ios_base::in);
        return stream_.good();
    }

    v8::Handle<v8::Value> GetLine() {
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
        if (1 == args.Length()) {
            v8::String::Utf8Value utf8Str(args[0]);
            char const *str = *utf8Str;
            Open(str);
        }
    }
};

static inline v8::Handle<v8::Value> Open() {
    v8::HandleScope scope;
    BasicClass<FileBase> fileBase;
    fileBase.Set<void (), &FileBase::Close>("close")
            .Set<bool (), &FileBase::Good>("good")
            .Set<bool (), &FileBase::IsOpen>("is_open")
            .Set<bool (), &FileBase::Eof>("eof")
            ;

    Class<FileWriter> fileWriter(fileBase);
    fileWriter.Set<bool, &FileWriter::Open>("open")
              .Set<void, &FileWriter::Print>("print")
              .Set<void, &FileWriter::Println>("println")
              ;

    Class<FileReader> fileReader(fileBase);
    fileReader.Set<bool(char const *), &FileReader::Open>("open")
              .Set<v8::Handle<v8::Value>(), &FileReader::GetLine>("getln")
              ;

    Module mod;
    return mod("Writer", fileWriter)
              ("Reader", fileReader).NewInstance();
}

} }

extern "C" {
    v8::Handle<v8::Value> vu8_module_file() { return vu8::file::Open(); }
}
