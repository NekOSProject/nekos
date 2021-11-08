#!/usr/bin/python3
import sys

sys.path.append('../')
import common

def run():
    os = common.run_i686()
    os.wait_for_debug_log("Hello, world!", timeout = 1)
    os.wait_for_debug_log("Hello from generic_main", timeout = 0.1)
    os.kill()

