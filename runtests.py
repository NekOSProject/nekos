#!/usr/bin/python3
import os
import sys
from importlib.machinery import SourceFileLoader
import timeit

root_dir = os.getcwd()
tests = []

# Collect tests from test folder (not recursively yet)
for test_folder in os.listdir("test"):
    test_folder_path = f"test/{test_folder}"
    test_file = f"{test_folder_path}/test.py"

    if not os.path.isdir(test_folder_path):
        continue

    if os.path.exists(test_file):
        tests.append(test_folder_path)

# Execute each test
for test in tests:
    test_dir = f"{root_dir}/{test}"

    os.chdir(test_dir)
    start = timeit.default_timer()
    result = SourceFileLoader("test", "test.py").load_module().run()
    finish = timeit.default_timer()
    result = "SUCCESS" if result else "FAILURE"
    print(f"{test}: {result} ({finish - start:.2f} seconds)")
    os.chdir(root_dir)

