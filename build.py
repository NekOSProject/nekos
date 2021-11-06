import os
import sys

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
lflags = f"-ffreestanding -nostdlib" # -lgcc

if not os.path.exists("build"):
    os.mkdir("build")

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

sources = collect_sources()
sources = remove_arch_sources(sources)
sources += collect_sources(f"kernel/arch/{ARCH_FAMILY}", recursive = False)
sources += collect_sources(f"kernel/arch/{ARCH_FAMILY}/{ARCH}")

c_sources = filter_c_sources(sources)
S_sources = filter_S_sources(sources)

source_counter = 1
for source in c_sources:
    source_object = source_path_to_object_path(source)
    create_dirs_for_file(source_object)
    print(f"[{source_counter}/{len(sources)}] Building {source}...")
    os.system(f"clang {cflags} -I . -c {source} -o {source_object}")
    source_counter += 1

for source in S_sources:
    source_object = source_path_to_object_path(source)
    create_dirs_for_file(source_object)
    print(f"[{source_counter}/{len(sources)}] Building {source}...")
    os.system(f"clang {aflags} -I . -c {source} -o {source_object}")
    source_counter += 1

objects = [ source_path_to_object_path(s) for s in sources ]
objects = " ".join(objects)
linker_script = f"kernel/arch/{ARCH_FAMILY}/{ARCH}/link.ld"
command = f"ld.lld -T {linker_script} -o kernel/kernel.elf {objects}"
print(command)
os.system(command)