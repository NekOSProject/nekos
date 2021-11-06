# Copyright 2021 Nekos Team
# Published under MIT License

import os
import sys
import re
import pickle
import time

class DependencyInfo:
    def __init__(self, check_time, dependencies):
        assert(isinstance(check_time, float)) # Returned from os.path.getmtime
        assert(isinstance(dependencies, list)) # List of files
        self.check_time = check_time
        self.dependencies = dependencies
        # To prevent recursive search of newer include (updated dependency)
        # If we checked the include and got that it's newer - we just return
        # If we checked the include and got that it's older - we put the (time
        # aganist which we checked the include) into was_tested_on. Then check
        # its dependencies, if they are newer than the time we look for.
        # At some point we can return to this file cause it's recursively included
        # For this case we have time aganist what we checked the include in 
        # was_tested_on field. So we can look at this and know that the file was
        # already tested aganist this specific time. So we don't need to check its
        # dependencies.
        # This resolves recursive include analisys.
        self.was_tested_on = None

def collect_files(ext, folder = "kernel", recursive = True):
    files = []
    for item in os.listdir(folder):
        item = f"{folder}/{item}"
        if item.endswith(ext) and os.path.isfile(item):
            files.append(item)
        if recursive and os.path.isdir(item) and not item.endswith(".") and not item.endswith(".."):
            files += collect_files(ext, folder = item, recursive = recursive)
    return files

def collect_c_files(folder = "kernel", recursive = True):
    return collect_files(".c", folder = folder, recursive = recursive)

def collect_S_files(folder = "kernel", recursive = True):
    return collect_files(".S", folder = folder, recursive = recursive)

def collect_sources(folder = "kernel", recursive = True):
    return (collect_files(".c", folder = folder, recursive = recursive) +
            collect_files(".S", folder = folder, recursive = recursive))

def remove_arch_sources(files):
    return [f for f in files if not f.startswith("kernel/arch")]

def filter_c_sources(files):
    return [f for f in files if f.endswith(".c")]

def filter_S_sources(files):
    return [f for f in files if f.endswith(".S")]

def source_path_to_object_path(source):
    return f"build/objects/{source}.o"

def get_file_directory(path):
    path = path.replace("\\", "/")
    if "/" in path:
        folder = "/".join(path.split("/")[:-1])
        if folder == "":
            return "/" # It was a file in the root folder
        return folder
    else:
        return "." # Just a filename, let's return current folder

def create_dirs_for_file(file):
    dir_path = get_file_directory(file)
    os.makedirs(dir_path, exist_ok = True)

def make_boot_first(objects):
    boot_file_index = 0
    for i in range(len(objects)):
        filename = objects[i]
        if f"kernel/arch/{ARCH_FAMILY}/{ARCH}/boot." in filename:
            boot_file_index = i
            break
    objects[0], objects[i] = objects[i], objects[0]
    return objects

def check_included_files_exist(paths):
    was_error = False
    for path in paths:
        if not os.path.exists(path):
            print(f"Can't find included file: {path}")
            was_error = True
    if was_error:
        exit(1)

# Takes "includes" and source file name and returns
# paths of the includes relative to current working directory
def translate_local_includes_of_file(includes, filename):
    # Get path relative to filename
    includes = [ f"{get_file_directory(filename)}/{i}" for i in includes ]
    # Remove stuff like path/../path
    includes = [ os.path.normpath(i) for i in includes ]
    # Return UNIX-like slashes back (windows-only)
    includes = [ i.replace("\\", "/") for i in includes ]
    return includes

freestanding_includes = [
    "limits.h",
    "stdint.h",
    "stddef.h",
    "stdbool.h",
    "stdarg.h",
]

# Takes <includes> and returns paths of the includes
# relative to current working directory
def translate_global_includes(includes):
    return [ i for i in includes if i not in freestanding_includes ]

def parse_c_dependencies(filename):
    with open(filename) as c_file:
        contents = c_file.read()
    include_local = re.findall(r'#include\s*"([\w\./\\]+)"', contents)
    include_global = re.findall(r'#include\s*<([\w\./\\]+)>', contents)
    dependencies = []
    dependencies += translate_local_includes_of_file(include_local, filename)
    dependencies += translate_global_includes(include_global)
    check_included_files_exist(dependencies)
    check_time = time.time()
    return DependencyInfo(check_time, dependencies)

def parse_gas_dependencies(filename):
    # GNU Assembler uses C preprocessor
    return parse_c_dependencies(filename)

def parse_dependencies(filename):
    if filename.endswith(".c") or filename.endswith(".h"):
        return parse_c_dependencies(filename)
    elif filename.endswith(".S") or filename.endswith(".s"):
        return parse_gas_dependencies(filename)
    else:
        print(f"How to parse dependencies of {filename}?")
        exit(-1)

def analise_dependencies(dependency_infos, sources):
    analised_sources = []

    for source in sources:
        # Dependencies of each source are cached in dependency_infos.pickle file
        filemtime = os.path.getmtime(source)
        if source not in dependency_infos or filemtime > dependency_infos[source].check_time:
            dependency_infos[source] = parse_dependencies(source)
            analised_sources.append(source)

    for source in analised_sources:
        # Update dependencies of the dependencies
        analise_dependencies(dependency_infos, dependency_infos[source].dependencies)

def has_dependency_newer_than_time(dependency_infos, source, time):
    dependencies = dependency_infos[source].dependencies
    for dependency in dependencies:
        if dependency_infos[dependency].was_tested_on == time:
            # We got to this inculude second time
            # And it's older than time
            # Let's skip it
            continue
        if os.path.getmtime(dependency) > time:
            return True
        else:
            dependency_infos[dependency].was_tested_on = time
            result = has_dependency_newer_than_time(dependency_infos, dependency, time)
            if result:
                # Some dependency of the dependency is newer than time
                return True
            else:
                # Found no dependency of the dependency which is newer than time
                # Let's analise next dependency in given dependency list
                continue
    return False

def find_sources_to_be_compiled(dependency_infos, sources):
    to_be_compiled = []
    for source in sources:
        source_object = source_path_to_object_path(source)
        # If no object file created - it obviously should be compiled
        if not os.path.exists(source_object):
            to_be_compiled.append(source)
            continue
        compile_time = os.path.getmtime(source_object)
        source_mtime = os.path.getmtime(source)
        if source_mtime < compile_time and not has_dependency_newer_than_time(dependency_infos, source, compile_time):
            continue
        to_be_compiled.append(source)
    return to_be_compiled

if __name__ == '__main__':
    is_win32 = True if sys.platform == "win32" else False
    is_linux = True if sys.platform == "linux" or sys.platform == "linux2" else False
    is_osx = True if sys.platform == "darwin" else False

    arch_to_family = {
        "i686": "x86",
    }

    ARCH="i686"
    ARCH_FAMILY = arch_to_family[ARCH]
    cflags = f"-std=gnu11 -ffreestanding -Wall -Wextra -g -DARCH_{ARCH} --target={ARCH}-unknown-linux-gnu"
    aflags = f"-ffreestanding -g --target={ARCH}-unknown-linux-gnu"
    lflags = f"-ffreestanding -nostdlib"

    sources = collect_sources()
    sources = remove_arch_sources(sources)
    sources += collect_sources(f"kernel/arch/{ARCH_FAMILY}", recursive = False)
    sources += collect_sources(f"kernel/arch/{ARCH_FAMILY}/{ARCH}")

    # Cached dict of "filename" -> DependencyInfo
    dependency_infos = {}
    actual_dependency_infos_version = 1
    if os.path.exists("build/dependency_infos.pickle"):
        dependency_infos = pickle.load(open("build/dependency_infos.pickle", "rb"))
        if dependency_infos["\\version//"] < actual_dependency_infos_version:
            os.remove("build/dependency_infos.pickle") # The file is old, get rid of it
            dependency_infos = {}
    analise_dependencies(dependency_infos, sources)
    if not os.path.exists("build"):
        os.mkdir("build")
    dependency_infos["\\version//"] = actual_dependency_infos_version
    pickle.dump(dependency_infos, open("build/dependency_infos.pickle", "wb"))

    sources_to_compile = find_sources_to_be_compiled(dependency_infos, sources)

    if len(sources_to_compile) == 0 and os.path.exists("kernel/kernel.elf"):
        print("Nothing to be done")
        exit()

    c_sources = filter_c_sources(sources_to_compile)
    S_sources = filter_S_sources(sources_to_compile)

    source_counter = 1
    for source in c_sources:
        source_object = source_path_to_object_path(source)
        create_dirs_for_file(source_object)
        print(f"[{source_counter}/{len(sources_to_compile)}] Building {source}...")
        os.system(f"clang {cflags} -I . -c {source} -o {source_object}")
        source_counter += 1

    for source in S_sources:
        source_object = source_path_to_object_path(source)
        create_dirs_for_file(source_object)
        print(f"[{source_counter}/{len(sources_to_compile)}] Building {source}...")
        os.system(f"clang {aflags} -I . -c {source} -o {source_object}")
        source_counter += 1

    objects = [ source_path_to_object_path(s) for s in sources ]
    make_boot_first(objects)
    objects = " ".join(objects)
    linker_script = f"kernel/arch/{ARCH_FAMILY}/{ARCH}/link.ld"
    print("Linkng...")
    linker = "ld.lld" if is_win32 else "lld"
    os.system(f"{linker} -T {linker_script} -o kernel/kernel.elf {objects}")
