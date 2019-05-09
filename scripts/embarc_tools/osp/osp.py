from __future__ import print_function, division, unicode_literals
import os
import sys
from ..notify import (print_string, colorstring_to_escapecode)
from ..settings import MAKEFILENAMES, get_input, OSP_DIRS
from ..exporter import Exporter
from ..utils import cd, read_json, generate_json, pquery


class OSP(object):
    def __init__(self, osp_file="osp.json"):
        self.path = os.path.join(os.path.expanduser("~"), '.embarc_cli')
        if not os.path.exists(self.path):
            try:
                os.mkdir(self.path)
            except (IOError, OSError):
                pass
        self.file = osp_file

        fl = os.path.join(self.path, self.file)
        if not os.path.exists(fl):
            self.cfg_dict = dict()
            try:
                generate_json(self.cfg_dict, fl)
            except Exception as e:
                print("[embARC] Write file failed: {}".format(e))
        else:
            self.cfg_dict = read_json(fl)

    def get_path(self, path):  # find if path exists in osp.json
        fl = os.path.join(self.path, self.file)
        if not os.path.exists(fl):
            self.cfg_dict = dict()
            generate_json(self.cfg_dict, fl)
        try:
            self.cfg_dict = read_json(fl)
            current_paths = self.cfg_dict.get(path, False)
            if current_paths:
                path = current_paths.get("directory", False)
                return path
            return False

        except IOError:
            raise IOError("Can not open file %s ." % fl)

    def generate_global(self):
        file = "global_config.json"
        config_file = os.path.join(self.path, file)
        global_config = dict()
        global_config["EMBARC_OSP_ROOT"] = str()
        global_config["TOOLCHAIN"] = str()
        global_config["BUILD_CONFIG"] = dict()
        global_config["BUILD_CONFIG"]["BOARD"] = str()
        global_config["BUILD_CONFIG"]["BD_VER"] = str()
        global_config["BUILD_CONFIG"]["CUR_CORE"] = str()
        generate_json(global_config, config_file)

    def set_global(self, config, value):
        file = "global_config.json"
        config_file = os.path.join(self.path, file)
        if not os.path.exists(config_file):
            self.generate_global()
        try:
            global_config = read_json(config_file)
            if global_config.get(config, False) is not False:
                global_config[config] = value
            elif global_config.get("BUILD_CONFIG", False) is not False:
                build_config = global_config["BUILD_CONFIG"]
                if build_config.get(config, False) is not False:
                    global_config["BUILD_CONFIG"][config] = value
                else:
                    return False
            else:
                return False
            generate_json(global_config, config_file)
        except IOError:
            raise IOError("Can not open file %s ." % config_file)

    def get_global(self, name):
        file = "global_config.json"
        config_file = os.path.join(self.path, file)
        if not os.path.exists(config_file):
            self.generate_global()
        else:
            global_config = read_json(config_file)
            if global_config.get(name, False):
                return global_config[name]
            elif global_config.get("BUILD_CONFIG", False):
                build_config = global_config["BUILD_CONFIG"]
                if build_config.get(name, False):
                    return global_config["BUILD_CONFIG"][name]
                else:
                    return False
            else:
                return False

    def set_path(self, name, source_type, path, url=None):
        fl = os.path.join(self.path, self.file)
        if not os.path.exists(fl):
            self.cfg_dict = dict()
            generate_json(self.cfg_dict, fl)
        path = path.replace("\\", "/")
        if not self.is_osp(path):
            print_string("This is not a valid EMBARC_OSP_ROOT")
            return False
        try:
            self.cfg_dict = read_json(fl)

            current_paths = self.cfg_dict.get(name, False)
            if current_paths:
                print_string("%s already exists" % name)
                return False
            else:
                self.cfg_dict[name] = {"type": source_type, "directory": path}
                if url:
                    self.cfg_dict[name]["source"] = url
                generate_json(self.cfg_dict, fl)
                return True

        except IOError:
            raise IOError("Can not open file %s ." % fl)

    def rename(self, old, new):
        fl = os.path.join(self.path, self.file)
        if not os.path.exists(fl):
            self.cfg_dict = dict()
            generate_json(self.cfg_dict, fl)
        try:
            self.cfg_dict = read_json(fl)
            if self.cfg_dict.get(old, False):
                self.cfg_dict[new] = self.cfg_dict[old]
                self.cfg_dict.pop(old)
                generate_json(self.cfg_dict, fl)
                return True
            else:
                print_string("%s not exists" % old)
                return False

        except IOError:
            raise IOError("Can not open file %s ." % fl)

    def remove_path(self, name):
        fl = os.path.join(self.path, self.file)
        if not os.path.exists(fl):
            self.cfg_dict = dict()
            generate_json(self.cfg_dict, fl)
        try:
            self.cfg_dict = read_json(fl)
            if self.cfg_dict.get(name, False):
                self.cfg_dict.pop(name)
                generate_json(self.cfg_dict, fl)
            else:
                print_string("%s not exists" % name)
        except IOError:
            raise IOError("Can not open file %s ." % fl)

    def clear_path(self):
        fl = os.path.join(self.path, self.file)
        try:
            self.cfg_dict = dict()
            generate_json(self.cfg_dict, fl)
        except IOError:
            raise IOError("Can not open file %s ." % fl)

    def list_path(self, show=False, current=None):
        fl = os.path.join(self.path, self.file)
        if not os.path.exists(fl):
            self.cfg_dict = dict()
            generate_json(self.cfg_dict, fl)
        if current:
            current = current.replace("\\", "/")
        try:
            self.cfg_dict = read_json(fl)
            if self.cfg_dict:
                if show:
                    for key, value in self.cfg_dict.items():
                        if value.get("directory", False) == current:
                            sys.stdout.write(colorstring_to_escapecode('green'))
                            sys.stdout.write("  * %s\n" % key)
                            for source_type, source_value in value.items():
                                sys.stdout.write("    %s: %s\n" % (source_type, source_value))
                            sys.stdout.write(colorstring_to_escapecode('default'))
                        else:
                            sys.stdout.write("  %s\n" % key)
                            for source_type, source_value in value.items():
                                sys.stdout.write("    %s: %s\n" % (source_type, source_value))
                        sys.stdout.write("\n")
                return self.cfg_dict
            else:
                print_string("There is no valid osp root path in record")

        except IOError:
            raise IOError("Can not open file %s ." % fl)

    def is_osp(self, path):
        if os.path.exists(path) and os.path.isdir(path):
            for files in OSP_DIRS:
                files_path = os.path.join(path, files)
                if os.path.exists(files_path) and os.path.isdir(files_path):
                    pass
                else:
                    return False
            return True
        else:
            return False

    def support_board(self, root):
        result = []
        board_path = os.path.join(root, "board")
        if os.path.exists(board_path):
            for file in os.listdir(board_path):
                if os.path.isdir(os.path.join(board_path, file)):
                    result.append(file)
        return result

    def supported_olevels(self, root):
        app = "example/baremetal/arc_feature/timer_interrupt"
        app_path = os.path.join(root, app)
        result = list()
        with cd(app_path):
            cmd_output = pquery(["make", "spopt"])
            if cmd_output:
                opt_lines = cmd_output.splitlines()
                for opt_line in opt_lines:
                    if opt_line.startswith("SUPPORTED_OLEVELS"):
                        board_opt_line = opt_line.split(":", 1)[1]
                        result.extend(board_opt_line.split())
                        break
        return result

    def supported_toolchains(self, root):
        app = "example/baremetal/arc_feature/timer_interrupt"
        app_path = os.path.join(root, app)
        result = list()
        with cd(app_path):
            cmd_output = pquery(["make", "spopt"])
            if cmd_output:
                opt_lines = cmd_output.splitlines()
                for opt_line in opt_lines:
                    if opt_line.startswith("SUPPORTED_TOOLCHAINS"):
                        board_opt_line = opt_line.split(":", 1)[1]
                        result.extend(board_opt_line.split())
                        break
        return result

    def supported_jtags(self, root):
        app = "example/baremetal/arc_feature/timer_interrupt"
        app_path = os.path.join(root, app)
        result = list()
        with cd(app_path):
            cmd_output = pquery(["make", "spopt"])
            if cmd_output:
                opt_lines = cmd_output.splitlines()
                for opt_line in opt_lines:
                    if opt_line.startswith("SUPPORTED_JTAGS"):
                        board_opt_line = opt_line.split(":", 1)[1]
                        result.extend(board_opt_line.split())
                        break
        return result

    def supported_boards(self, root):
        app = "example/baremetal/arc_feature/timer_interrupt"
        app_path = os.path.join(root, app)
        result = list()

        with cd(app_path):
            cmd_output = pquery(["make", "spopt"])
            if cmd_output:
                opt_lines = cmd_output.splitlines()
                for opt_line in opt_lines:
                    if opt_line.startswith("SUPPORTED_BOARDS"):
                        board_opt_line = opt_line.split(":", 1)[1]
                        result.extend(board_opt_line.split())
                        break
        return result

    def supported_bd_versions(self, root, board, bd_version=None):
        app = "example/baremetal/arc_feature/timer_interrupt"
        app_path = os.path.join(root, app)
        result = list()
        with cd(app_path):
            cmd = ["make"]
            cmd.append("BOARD=%s" % (board))
            cmd.append("spopt")
            cmd_output = pquery(cmd)
            if cmd_output:
                opt_lines = cmd_output.splitlines()
                for opt_line in opt_lines:
                    if opt_line.startswith("SUPPORTED_BD_VERS"):
                        board_opt_line = opt_line.split(":", 1)[1]
                        result.extend(board_opt_line.split())
                        if bd_version and bd_version in result:
                            result = [bd_version]
                        break
        return result

    def supported_cores(self, root, board, bd_version, cur_core=None):
        app = "example/baremetal/arc_feature/timer_interrupt"
        app_path = os.path.join(root, app)
        result = list()
        with cd(app_path):
            cmd = ["make"]
            cmd.append("BOARD=%s" % (board))
            cmd.append("BD_VER=%s" % (bd_version))
            cmd.append("spopt")
            cmd_output = pquery(cmd)
            if cmd_output:
                opt_lines = cmd_output.splitlines()
                for opt_line in opt_lines:
                    if opt_line.startswith("SUPPORTED_CORES"):
                        board_opt_line = opt_line.split(":", 1)[1]
                        result.extend(board_opt_line.split())
                        if cur_core and cur_core in result:
                            result = [cur_core]
                        break
        return result

    def get_board_version(self, root, board, bd_version=None):
        result = []
        board_path = "board/" + board
        ver_path = os.path.join(root, board_path)
        if os.path.exists(ver_path):
            bd_vers_dict = self._board_version_config(root, board, bd_version)
            result.extend(bd_vers_dict.keys())
        return result

    def _board_version_config(self, root, board, bd_version=None):
        board_path = os.path.join(root, "board", board)
        bd_vers = dict()
        if os.path.exists(board_path):
            files = os.listdir(board_path)
            if "configs" in files:
                versions = os.listdir(os.path.join(board_path, "configs"))
                for version in versions:
                    version_path = os.path.join(board_path, "configs", version)
                    if os.path.isdir(version_path):
                        bd_vers[version] = version_path
            else:
                versions = os.listdir(board_path)
                for version in versions:
                    path = os.path.join(board_path, version)
                    if os.path.isdir(path) and "configs" in os.listdir(path):
                        version_path = os.path.join(board_path, version, "configs")
                        bd_vers[version] = version_path
        if bd_version is not None:
            if bd_version in bd_vers:
                bd_ver = {bd_version: bd_vers[bd_version]}
                return bd_ver
        return bd_vers

    def get_tcfs(self, root, board, bd_version, cur_core=None):
        result = []
        board_version_path_dict = self._board_version_config(root, board, bd_version)
        board_path = board_version_path_dict.get(bd_version, False)
        cur_core_file = cur_core + ".tcf" if cur_core else cur_core
        if board_path and os.path.exists(board_path):
            for root, _, files in os.walk(board_path, topdown=True):
                if cur_core_file in files:
                    result.append(cur_core)
                    break
                for file in files:
                    filename, filesuffix = os.path.splitext(file)
                    if filesuffix != ".tcf":
                        continue
                    else:
                        result.append(filename)

        return result

    def get_tcf(self, root, board, bd_version, cur_core):
        result = None
        board_version_path_dict = self._board_version_config(root, board, bd_version)
        board_path = board_version_path_dict[bd_version]
        cur_core_file = cur_core + ".tcf"
        if os.path.exists(board_path):
            for root, _, files in os.walk(board_path, topdown=True):
                if cur_core_file in files:
                    result = os.path.join(root, cur_core_file)
                    break
        return result

    def get_makefile(self, app_path):
        for makefile in MAKEFILENAMES:
            makefile_path = os.path.join(app_path, makefile)
            if os.path.exists(makefile_path) and os.path.isfile(makefile_path):
                return makefile_path
        return None

    def check_osp(self, path):
        update = False
        if path:
            osp_root = path.replace("\\", "/")
        else:
            osp_root = path
        if not osp_root or not self.is_osp(osp_root):
            update = True
            msg = "Current EMBARC_ROOT is not a valid osp root"
            level = "warning"
            print_string(msg, level)
            self.list_path(show=True, current=None)
            while True:
                input_root = get_input("Choose osp root or set another path as osp root: ")
                if not self.is_osp(input_root):
                    input_root = self.get_path(input_root)

                if not self.is_osp(input_root):
                    msg = "What you choose is not a valid osp root"
                    level = "warning"
                    print_string(msg, level)
                    print_string("Please set a valid osp root or download embarc osp first")
                    continue

                break
            osp_root = input_root.replace("\\", "/")
        return osp_root, update

    def get_makefile_config(self, build_template=None, verbose=False):
        '''
        Get config from makefile
        '''
        print_string("Read makefile and get configuration ")
        makefile = None
        for file in MAKEFILENAMES:
            if os.path.exists(file) and os.path.isfile(file):
                makefile = file

        if not makefile:
            print_string("Makefile doesn't exists")
            return makefile, build_template

        with open(makefile) as f:
            lines = f.read().splitlines()
            for line in lines:
                if line.startswith("APPL "):
                    build_template["APPL"] = (line.split("=")[1]).strip()

                if line.startswith("BOARD"):
                    build_template["BOARD"] = (line.split("=")[1]).strip()
                if line.startswith("BD_VER"):
                    build_template["BD_VER"] = (line.split("=")[1]).strip()
                if line.startswith("CUR_CORE"):
                    build_template["CUR_CORE"] = (line.split("=")[1]).strip()
                if line.startswith("TOOLCHAIN"):
                    build_template["TOOLCHAIN"] = (line.split("=")[1]).strip()
                    if build_template["TOOLCHAIN"] and build_template["TOOLCHAIN"] != "":
                        pass
                    else:
                        build_template["TOOLCHAIN"] = "gnu"
                if line.startswith("EMBARC_ROOT"):
                    relative_root = (line.split("=")[1]).strip()
                    osp_root = os.path.normpath(os.path.join(os.getcwd(), relative_root))
                    build_template["EMBARC_OSP_ROOT"] = osp_root
                if verbose:
                    if line.startswith("MID_SEL"):
                        build_template["middleware"] = (line.split("=")[1]).strip()
                    if line.startswith("APPL_CSRC_DIR"):
                        build_template["csrc"] = (line.split("=")[1]).strip()
                    if line.startswith("APPL_ASMSRC_DIR"):
                        build_template["asmsrc"] = (line.split("=")[1]).strip()
                    if line.startswith("APPL_INC_DIR"):
                        build_template["include"] = (line.split("=")[1]).strip()
                    if line.startswith("APPL_DEFINES"):
                        build_template["defines"] = (line.split("=")[1]).strip()
                    if line.startswith("OS_SEL"):
                        os_sel = (line.split("=")[1]).strip()
                        if os_sel and os_sel != "":
                            build_template["os"] = os_sel
                    if line.startswith("LIB_SEL"):
                        lib_sel = (line.split("=")[1]).strip()
                        if lib_sel and lib_sel != "":
                            build_template["lib"] = lib_sel

        return makefile, build_template

    def update_makefile(self, value, path):
        """ update embarc_root in makefile"""
        with cd(path):
            print_string("Update makefile {}".format(value))
            build_template = dict()
            makefile, build_template = self.get_makefile_config(build_template, verbose=True)
            build_template.update(value)
            exporter = Exporter("application")
            exporter.gen_file_jinja("makefile.tmpl", build_template, makefile, os.getcwd())
