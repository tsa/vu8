#include <vu8/Context.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>
#include <stdexcept>

namespace tsa { namespace vu8 {

static inline std::string find_in_path(char const *search) {
    char const *path_env = std::getenv("PATH");
    char const *nextColon = path_env;

    std::string path;
    if ('\0' != *search) {
        for (char const *firstSlash = search; *firstSlash != '\0';
             ++firstSlash)
        {
            if ('/' == *firstSlash) {
                if (access(search, X_OK)) return "";
                else return search;
            }
        }

        while ('\0' != *nextColon) {
            for (++nextColon;
                 *nextColon != ':' && '\0' != *nextColon; ++nextColon) {}

            path.assign(path_env, nextColon);
            path.append("/").append(search);

            if (! access(path.c_str(), X_OK)) break;

            path_env = nextColon + 1;
        }
    }
    else path = "";

    return path;
}

namespace po = boost::program_options;
void usage(po::options_description const& desc) {
    std::cout << desc << std::endl;
}

int main(int argc, char *argv[]) {
    po::options_description desc("allowed options");

    std::string libPath;
    typedef std::vector<std::string> scripts_t;
    scripts_t scripts;

    po::positional_options_description positionals;
    positionals.add("script", -1);
    desc.add_options()
        ("help,h", "print this help message.")
        ("script,s", po::value(&scripts), "script to run.")
        ("library-path,l", po::value(&libPath), "path containing plugin libraries.")
        ;

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv)
            .options(desc).positional(positionals).run(), vm);
    }
    catch (po::invalid_option_value& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (po::multiple_occurrences& e) {
        usage(desc);
        std::cerr << "multiple occurences of same option" << std::endl;
        return 1;
    }
    catch (po::unknown_option& e) {
        usage(desc);
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        usage(desc);
        return 1;
    }

    po::notify(vm);

    if (libPath.empty()) {
        std::string root = find_in_path(argv[0]);
        boost::filesystem::path libPtr(root);
        libPtr = libPtr.parent_path();
        if (libPtr.has_parent_path()) {
            libPath = libPtr.parent_path().string();
            libPath.append("/lib");
        }
        else {
            libPath = "../lib";
        }
    }

    if (scripts.empty()) {
        std::cerr << "must supply at least one script\n";
        usage(desc);
        return 1;
    }

    try {
        Context ctxt(libPath);
        for (scripts_t::const_iterator it = scripts.begin();
             it != scripts.end(); ++it)
        { ctxt.RunFile(it->c_str()); }
    }
    catch (std::runtime_error const& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    v8::V8::Dispose();
    return 0;
}

} }

int main(int argc, char *argv[]) {
    return tsa::vu8::main(argc, argv);
}
