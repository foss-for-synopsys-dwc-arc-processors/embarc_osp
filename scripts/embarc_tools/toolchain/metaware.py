from __future__ import print_function, division, unicode_literals
from distutils.spawn import find_executable
import re
import os
from ..toolchain import ARCtoolchain
from ..utils import pquery


class Mw(ARCtoolchain):
    version = "2017.09"
    path = None
    executable_name = "ccac"

    def __init__(self):
        exe = find_executable(self.executable_name)
        if exe:
            self.path = os.path.split(exe)[0]
            self.version = self.check_version()

    @staticmethod
    def check_version():
        cmd = ["ccac", "-v"]
        try:
            exe = pquery(cmd)
            version = re.search(r"[0-9]*\.[0-9]*", exe).group(0)
            if version:
                return version
        except Exception as e:
            print(e)
            return None

    def _set_version(self):
        version = self.check_version()
        if version:
            self.version = version

    def download(self, version=None, path=None):
        print("Can not download metaware using cli {} {}".format(version, path))
        return

    def extract_file(self):
        '''extract gnu file from pack to path;
        pack - the path of the compressed package
        path - the compressed package is extracted to this path
        return the root path of gnu and set self.path'''
        print("Extract gnu file from pack to path")

    def set_env(self, path=None):
        self.set_toolchain_env("mw", path)
