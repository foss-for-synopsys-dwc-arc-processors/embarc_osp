from __future__ import print_function, division, absolute_import, unicode_literals
from distutils.spawn import find_executable
import os
from ..settings import GNU_PATH, MW_PATH, SUPPORT_TOOLCHAIN, CURRENT_PLATFORM


TOOLCHAIN_PATHS = {
    "gnu": GNU_PATH,
    "mw": MW_PATH
}


class ProcessException(Exception):
    pass


class ARCtoolchain(object):

    def __init__():
        pass

    def check_executable(self, tool_key, executable_name):
        '''tool_key: toolchain name
        executable_name: command like ['arc-elf32-gcc', 'ccac']
        return True if the command is supported'''
        if not self.is_supported(tool_key):
            return False
        else:
            try:
                exe = find_executable(executable_name)
                if not exe:
                    print("can not execuate {}".format(executable_name))
                    return False
                toolchian_path = os.path.split(exe)
                TOOLCHAIN_PATHS[tool_key] = toolchian_path
                return True
            except Exception:
                return False

    @staticmethod
    def is_supported(tool_key):
        '''check if tool_key is supported'''
        if tool_key not in SUPPORT_TOOLCHAIN:
            print("This toolchian is not supported")
            return False
        return True

    @staticmethod
    def get_platform():
        return CURRENT_PLATFORM

    def set_toolchain_env(self, tool_key, toolchain_root):
        '''set toolchian's environmental variable
        tool_key: toolchian name "mw" or "gnu"
        toolchian_root: if current platform is windows, this is the bin path ,if current platform is Linux,this should be the root directory ,here input the bin path
        return False if failed
        '''
        platform = self.get_platform()
        if platform == "Windows":
            from embarc_tools.toolchain import windows_env_set_arc
            env_obj = windows_env_set_arc.Win32Environment(scope="user")
            windows_env_set_arc.set_env_path(env_obj, 'Path', toolchain_root)
            return True
        elif platform == "Linux":
            toolchain_root = os.path.dirname(toolchain_root)
            try:
                bashrc = os.path.join(os.path.expanduser("~"), '.bashrc')
                content = list()
                with open(bashrc) as f:
                    lines = f.read().splitlines()

                    toolchain_var_name = "ARC_%s_ROOT" % (tool_key.upper())
                    set_toolchain_var = False
                    set_toolchain_path = False
                    for line in lines:
                        if line.startswith(toolchain_var_name):
                            line = "%s=%s" % (toolchain_var_name, toolchain_root)
                            set_toolchain_var = True
                        if line.startswith("export PATH="):
                            if toolchain_root not in line:
                                line = "%s:%s/bin" % (line, toolchain_root)
                            set_toolchain_path = True
                        content.append(line)
                    if not set_toolchain_var:
                        content.append("%s=%s" % (toolchain_var_name, toolchain_root))
                    if not set_toolchain_path:
                        content.append("export PATH=$PATH:%s:%s/bin" % (line, toolchain_root))
                    f.close()
                with open(bashrc, "w") as f:
                    for line in content:
                        f.write("%s\n" % (line))
                    f.close()
            except Exception as e:
                print(e)
                return False
        else:
            print("this platform {} is not supported".format(platform))
            return False
