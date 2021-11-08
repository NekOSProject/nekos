# Copyright 2021 Nekos Team
# Published under MIT License

import io
import os
import sys
import subprocess
import timeit
import time
import shlex
import shutil
import signal

sys.path.append('../../')
import build

is_win32 = True if sys.platform == "win32" else False
is_linux = True if sys.platform == "linux" or sys.platform == "linux2" else False
is_osx = True if sys.platform == "darwin" else False

class TestTimeoutException(Exception):
    pass

class TestFailureException(Exception):
    pass

class Qemu:
    def __init__(self, popen):
        self.popen = popen
        # Qemu needs time to create log files
        while not os.path.exists("debug.log"):
            self.wait(0.1)
        self.debug = open("debug.log", "rb")

    def wait_for_debug_log(self, needle, timeout = 1):
        needle = bytes(needle, "utf-8")
        start = timeit.default_timer()
        log = b""

        # While no timeout, read and search logs
        while timeit.default_timer() - start < timeout:
            log += self.debug.read(1)
            if needle in log:
                return

            # We don't have to read whole logs to find the neddle
            # If we read len(needle) * 2 bytes of log then we
            # already can say that if there's no needle in the data
            # then it can't be in first len(needle) bytes of the data
            # so first len(needle) bytes of saved logs may be thrown away
            #
            # So we consume lessser memory and don't search all the previous
            # logs every single time
            if len(log) > len(needle) * 2:
                log = log[len(needle):]

        self.timeout()

    def kill(self):
        if is_win32:
            # FIXME: This is shit, isn't there anything better?
            subprocess.Popen(f"TASKKILL /F /PID {self.popen.pid} /T", stdout = subprocess.DEVNULL, stderr = subprocess.DEVNULL, stdin = subprocess.DEVNULL)
        else:
            os.killpg(os.getpgid(self.popen.pid), signal.SIGTERM)

    def failure(self):
        self.kill()
        raise TestFailureException()

    def timeout(self):
        self.kill()
        raise TestTimeoutException()

    def wait(self, seconds = 0.25):
        time.sleep(seconds)

def run_qemu(arch):
    qemu_command = f"qemu-system-{arch}"
    flags = ""
    if is_win32:
        qemu_full_path = shutil.which(qemu_command)
        qemu_directory = build.get_file_directory(qemu_full_path)
        flags += f"-L {qemu_directory}"
    s = f"{qemu_command} -nographic {flags} -kernel ../../kernel/kernel.elf -debugcon file:debug.log"
    a = shlex.split(s)
    if is_win32:
        return subprocess.Popen(s, bufsize = 0, stdout = subprocess.DEVNULL, stderr = subprocess.DEVNULL, stdin = subprocess.DEVNULL, shell = True, start_new_session = True)
    else:
        return subprocess.Popen(a, bufsize = 0, stdout = subprocess.DEVNULL, stderr = subprocess.DEVNULL, stdin = subprocess.DEVNULL, start_new_session = True)

def run_i686():
    if os.path.exists("debug.log"):
        os.remove("debug.log")
    popen = run_qemu("i386")
    return Qemu(popen)

