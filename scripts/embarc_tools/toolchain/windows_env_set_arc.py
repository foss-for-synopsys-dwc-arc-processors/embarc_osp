from __future__ import print_function, division, unicode_literals
import os
import sys
from subprocess import check_call

if sys.hexversion > 0x03000000:
    import winreg
else:
    import _winreg as winreg


class Win32Environment(object):
    """Utility class to get/set windows environment variable"""
    def __init__(self, scope):
        # assert scope in ('user', 'system')
        self.scope = scope
        if scope == 'user':
            self.root = winreg.HKEY_CURRENT_USER
            self.subkey = 'Environment'
        else:
            self.root = winreg.HKEY_LOCAL_MACHINE
            self.subkey = r'SYSTEM\CurrentControlSet\Control\Session Manager\Environment'

    def search(self, name):
        key = winreg.OpenKey(self.root, self.subkey, 0, winreg.KEY_READ)
        key_value = ''
        try:
            i = 0
            while i >= 0:
                key_value, _, _ = winreg.EnumValue(key, i)
                i += 1
                if key_value == name:
                    break
        except Exception:
            key_value = ''
        return key_value

    def getenv(self, name):
        key = winreg.OpenKey(self.root, self.subkey, 0, winreg.KEY_READ)
        try:
            value, _ = winreg.QueryValueEx(key, name)
        except Exception:
            value = ''
        return value

    def setenv(self, name, value):
        # Note: for 'system' scope, you must run this as Administrator
        key = winreg.OpenKey(self.root, self.subkey, 0, winreg.KEY_ALL_ACCESS)
        winreg.SetValueEx(key, name, 0, winreg.REG_EXPAND_SZ, value)  # store value in the name field of an open registry key
        winreg.CloseKey(key)
        # For some strange reason, calling SendMessage from the current process
        # doesn't propagate environment changes at all.
        # TODO: handle CalledProcessError (for assert)
        check_call(
            '''\"%s" -c "import win32api, win32con;assert win32api.SendMessage(win32con.HWND_BROADCAST, win32con.WM_SETTINGCHANGE,0, 'Environment')"''' % sys.executable)

    def get_userenv(self, name):
        # Note: for 'system' scope, you must run this as Administrator
        key = winreg.OpenKey(self.root, self.subkey, 0, winreg.KEY_ALL_ACCESS)
        value, _ = winreg.QueryValueEx(key, name)
        return value


def set_env_path(env_obj, env_name, env_path, refresh=False):
    need_add = False
    path_values = None
    exist_path = None
    if env_obj.search(env_name):
        exist_path = env_obj.get_userenv(env_name)
    if not exist_path and env_obj.search(env_name.upper()):
        exist_path = env_obj.get_userenv(env_name.upper())

    if refresh:
        exist_path = None
    if exist_path:
        path_values = [i for i in exist_path.split(';')]
        for i in env_path.split(';'):
            if i not in path_values:
                path_values.append(i)
                need_add = True
    if not need_add and path_values:
        return 'environment variable {}:{} already exists '.format(env_name, exist_path)

    if path_values:
        env_path = ';'.join(path_values)

    env_obj.setenv(env_name, os.path.expanduser(env_path))
    path_value = env_obj.get_userenv(env_name)
    return 'set environment variable {}:{}'.format(env_name, path_value)
