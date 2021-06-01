#! /usr/bin/env python

import os
import re
import shutil
import argparse
from embarc_tools.toolchain import gnu


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        pass

    try:
        import unicodedata
        unicodedata.numeric(s)
        return True
    except (TypeError, ValueError):
        pass

    return False

def store_gnu_toolchain(version, path):
    gnu_version = version
    if not is_number(version):
        version = None
    gnu_toolchain = gnu.Gnu()
    gnu_tgz_path = gnu_toolchain.download(version, path)
    gnu_root_path = None
    gnu_file_path = None
    if gnu_tgz_path is None:
        print("Can't download gnu {} ".format(version))

    new_path = os.path.join(path, gnu_version)
    if gnu_version not in os.listdir(path):
        gnu_file_path = gnu_toolchain.extract_file(gnu_tgz_path, path)
        if gnu_file_path:
            shutil.move(gnu_file_path, new_path)

def get_options_parser():
    configs = dict()
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--gnu", dest="gnu", help=("the version of gnu"), metavar="GNU_VERSION")
    parser.add_argument("-c", "--cache", dest="cache", default=".cache/toolchain", help=("the cache path"), metavar="TOOLCHAIN_CACHE_FOLDER")

    options = parser.parse_args()
    if options.gnu:
        configs["gnu"] = options.gnu

    if options.cache:
        configs["cache"] = options.cache

    return configs


if __name__ == '__main__':
    configs = get_options_parser()
    store_gnu_toolchain(configs["gnu"], configs["cache"])
