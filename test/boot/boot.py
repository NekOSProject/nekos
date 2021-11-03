#!/usr/bin/python3

import os
import subprocess
import signal

p = subprocess.Popen("qemu-system-i386 -kernel '../../kernel/kernel.elf' -debugcon stdio", shell=True, stderr=subprocess.PIPE, stdout=subprocess.PIPE, preexec_fn=os.setsid)

try:
    out, err = p.communicate(timeout = 1)
except subprocess.TimeoutExpired:
    os.killpg(os.getpgid(p.pid), signal.SIGTERM)
    out, err = p.communicate()

if b"Hello, world!" not in out:
    print("FAILURE: No expected message logged")
    print("stdout: ", out)
    exit()

print("SUCCESS")
