from __future__ import print_function, unicode_literals
from functools import reduce
import random
import time
import io
import sys
import operator
import subprocess
import errno
import os
from os import listdir, remove, makedirs
import yaml
import stat
import urllib
import contextlib
from shutil import copyfile
import shutil
import zipfile
import tarfile
import json

cwd_root = ""
_cwd = os.getcwd()


def getcwd():
    '''return current working path '''
    global _cwd
    return _cwd


@contextlib.contextmanager
def cd(newdir):
    '''A function that does cd'''
    global _cwd
    prevdir = getcwd()
    os.chdir(newdir)
    _cwd = newdir
    try:
        yield
    finally:
        os.chdir(prevdir)
        _cwd = prevdir


def relpath(root, path):
    '''return the relative path of root and path'''
    return path[len(root) + 1:]


def mkdir(path):
    '''A function that does mkdir'''
    if not os.path.exists(path):
        makedirs(path)


def rmtree_readonly(directory):
    '''change permission and delete directory'''
    if os.path.islink(directory):
        os.remove(directory)
    else:
        def remove_readonly(func, path, _):
            os.chmod(path, stat.S_IWRITE)
            func(path)
        shutil.rmtree(directory, onerror=remove_readonly)


def delete_dir_files(directory, dir=False):
    """ A function that does rm -rf

    Positional arguments:
    directory - the directory to remove
    """
    if not os.path.exists(directory):
        return
    if os.path.isfile(directory):
        remove(directory)
    else:
        if not dir:
            for element in listdir(directory):
                to_remove = os.path.join(directory, element)
                if not os.path.isdir(to_remove):
                    remove(to_remove)
        else:
            shutil.rmtree(directory)


def generate_file(filename, data, path=None):
    file = None
    if path and os.path.exists(path) and os.path.isdir(path):
        file = os.path.join(path, filename)
    else:
        file = os.path.join(os.getcwd(), filename)
    if os.path.isfile(file):
        os.remove(file)
    try:
        with open(file, 'w+') as f:
            f.write(data)
        f.close()
    except EnvironmentError:
        print("[embARC] Unable to open %s for writing!" % file)
        return -1
    print("[embARC] Write to file %s" % file)
    return 0


def generate_yaml(filename, data):
    file = os.path.join(os.getcwd(), filename)
    if os.path.isfile(file):
        os.remove(file)
    try:
        with open(file, 'w+') as f:
            f.write(yaml.dump(data, default_flow_style=False))
        f.close()
    except EnvironmentError:
        print("[embARC] Unable to open %s for writing!" % file)
        return -1
    print("[embARC] Write to file %s" % file)
    return 0


def edit_yaml(filename, data):
    file = os.path.join(os.getcwd(), filename)
    if not os.path.isfile(file):
        generate_yaml(filename, data)
        return
    else:
        try:
            with open(file, 'w+') as f:
                f.write(yaml.dump(data, default_flow_style=False))
        except EnvironmentError:
            print("[embARC] Unable to open %s for writing!" % file)
            return -1
        print("[embARC] Write to file %s" % file)
        return 0


def read_json(path):
    result = None
    with open(path, "r") as f:
        result = json.load(f)
    return result


def generate_json(data, path):
    with open(path, "w") as f:
        json.dump(data, f, indent=4)
        f.close()


def copy_file(src, dst):
    """ Implement the behavior of "shutil.copy(src, dst)" without copying the
    permissions (this was causing errors with directories mounted with samba)

    Positional arguments:
    src - the source of the copy operation
    dst - the destination of the copy operation
    """
    if os.path.isdir(dst):
        _, base = os.path.split(src)
        dst = os.path.join(dst, base)
    copyfile(src, dst)


def download_file(url, path):
    '''from url download file to path. if failed ,return False. else return True'''
    try:
        urllib.urlretrieve(url, path)
    except AttributeError:
        from urllib import request
        request.urlretrieve(url, path)
    except Exception as e:
        print("[embARC] This file from %s can't be download for %s" % (url, e))
        sys.stdout.flush()
        return False
    return True


def unzip(file, path):
    '''extract file from .zip to path
    file - the path of zip
    path - the dest path
    return directory name after decompression'''
    file_name = None
    try:
        pack = zipfile.ZipFile(file, "r")
        files = pack.namelist()
        file_name = files[0]
        pack.extractall(path)
        pack.close()

    except Exception as e:
        print(e)
    return file_name


def untar(file, path):
    file_name = None
    try:
        pack = tarfile.open(file, "r:gz")
        files = pack.getnames()
        file_name = files[0]
        for file in files:
            pack.extract(file, path)
        pack.close()
    except Exception as e:
        print(e)
    return file_name


def extract_file(file, path):
    extract_file_name = None
    extract_file_path = None
    _, filesuffix = os.path.splitext(file)
    if filesuffix == ".gz":
        extract_file_name = untar(file, path)
    elif filesuffix == ".zip":
        extract_file_name = unzip(file, path)
    else:
        print("[embARC] This file {} can't be extracted".format(file))
    if extract_file_name is not None:
        extract_file_path = os.path.join(path, extract_file_name)
    return extract_file_path


def show_progress(title, percent, max_width=80):
    '''show progress when download file'''
    if sys.stdout.isatty():
        percent = round(float(percent), 2)
        show_percent = '%.2f' % percent
        bwidth = max_width - len(str(title)) - len(show_percent) - 6  # 6 equals the spaces and paddings between title, progress bar and percentage
        sys.stdout.write('%s |%s%s| %s%%\r' % (str(title), '#' * int(percent * bwidth // 100), '-' * (bwidth - int(percent * bwidth // 100)), show_percent))
        sys.stdout.flush()


def hide_progress(max_width=80):
    if sys.stdout.isatty():
        sys.stdout.write("\r%s\r" % (' ' * max_width))


def uniqify(_list):
    return reduce(lambda r, v: v in r[1] and r or (r[0].append(v) or r[1].add(v)) or r, _list, ([], set()))[0]


def flatten(S):
    if S == []:
        return S
    if isinstance(S[0], list):
        return flatten(S[0]) + flatten(S[1:])
    return S[:1] + flatten(S[1:])


def load_yaml_records(yaml_files):
    dictionaries = []
    for yaml_file in yaml_files:
        try:
            f = open(yaml_file, 'rt')
            dictionaries.append(yaml.load(f))
        except IOError:
            raise IOError("The file %s referenced in main yaml doesn't exist." % yaml_file)
    return dictionaries


def merge_recursive(*args):
    if all(isinstance(x, dict) for x in args):
        output = {}
        keys = reduce(operator.or_, [set(x) for x in args])

        for key in keys:
            # merge all of the ones that have them
            output[key] = merge_recursive(*[x[key] for x in args if key in x])

        return output
    return reduce(operator.add, args)


class ProcessException(Exception):
    pass


def popen(command, **kwargs):
    proc = None
    try:
        proc = subprocess.Popen(command, **kwargs)
    except OSError as e:
        if e.args[0] == errno.ENOENT:
            print(
                "Could not execute \"%s\".\n"
                "Please verify that it's installed and accessible from your current path by executing \"%s\".\n" % (command[0], command[0]), e.args[0])
        else:
            raise e

    if proc and proc.wait() != 0:
        raise ProcessException(proc.returncode, command[0], ' '.join(command), getcwd())


def processcall(command, **kwargs):
    returncode = 0
    try:
        returncode = subprocess.call(command, **kwargs)
    except OSError as e:
        if e.args[0] == errno.ENOENT:
            print(
                "Could not execute \"%s\".\n"
                "Please verify that it's installed and accessible from your current path by executing \"%s\".\n" % (command[0], command[0]), e.args[0])
        else:
            raise e
    return returncode


def pquery(command, output_callback=None, stdin=None, **kwargs):
    proc = None
    try:
        proc = subprocess.Popen(command, bufsize=0, stdout=subprocess.PIPE, stderr=subprocess.PIPE, **kwargs)
    except OSError as e:
        if e.args[0] == errno.ENOENT:
            print(
                "Could not execute \"%s\" in \"%s\".\n"
                "You can verify that it's installed and accessible from your current path by executing \"%s\".\n" % (' '.join(command), getcwd(), command[0]), e.args[0])
        else:
            raise e

    if output_callback:
        line = ""
        while 1:
            s = str(proc.stderr.read(1))
            line += s
            if s == '\r' or s == '\n':
                output_callback(line, s)
                line = ""

            if proc.returncode is None:
                proc.poll()
            else:
                break

    stdout, _ = proc.communicate(stdin)
    if proc.returncode != 0:
        print("[embARC] Run command {} return code:{} ".format(' '.join(command), proc.returncode))

    return stdout.decode("utf-8")


def pqueryOutputinline(command, console=False, **kwargs):
    proc = None
    build_out = list()
    file_num = random.randint(100000, 200000)
    file_name = "message" + str(file_num) + ".log"
    try:
        with io.open(file_name, "wb") as writer, io.open(file_name, "rb", 1) as reader:
            proc = subprocess.Popen(
                command, stdout=writer, stderr=subprocess.PIPE, shell=True, bufsize=1, **kwargs
            )
            end = ""
            # if PYTHON_VERSION.startswith("3"):
            #    end = "\n"
            try:
                while True:
                    decodeline = reader.read().decode()
                    if decodeline == str() and proc.poll() is not None:
                        break
                    if decodeline != str():
                        build_out.append(decodeline)
                        if console:
                            print(decodeline, end=end)
                            time.sleep(0.1)
            except (KeyboardInterrupt):
                print("[embARC] Terminate batch job")
                sys.exit(1)

    except OSError as e:
        if e.args[0] == errno.ENOENT:
            print(
                "Could not execute \"%s\".\n"
                "Please verify that it's installed and accessible from your current path by \
                executing \"%s\".\n" % (command[0], command[0]), e.args[0])
        else:
            raise e
    except Exception as e:
        print(e)
    proc.wait()
    if os.path.exists(file_name):
        remove(file_name)
    if proc.stdout:
        proc.stdout.close()
    if proc.stderr:
        proc.stderr.close()
    del proc
    return build_out


def pqueryTemporaryFile(command):
    current_command = None
    if isinstance(command, list):
        current_command = " ".join(command)
    else:
        current_command = command
    print("[embARC] Run command {}".format(current_command))
    proc = None
    returncode = 0
    rt_list = None
    file_num = random.randint(100000, 200000)
    file_name = "message" + str(file_num) + ".log"
    try:
        log_file = open(file_name, "w")
        proc = subprocess.Popen(current_command, stdout=log_file, stderr=None, shell=True)
        log_file.close()
        returncode = proc.wait()

    except Exception as e:
        print("[embARC] Run command {} failed : {}".format(current_command, e))
    if os.path.exists(file_name):
        with open(file_name) as f:
            rt_list = f.read().splitlines()
        remove(file_name)
    del proc
    return returncode, rt_list
