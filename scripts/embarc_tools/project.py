from __future__ import print_function, unicode_literals
import os
import random
from .utils import cd, getcwd, merge_recursive, uniqify, pquery
from .exporter import Exporter
from .osp import osp
from .notify import (print_string)
from .toolchain import gnu
from .settings import BUILD_CONFIG_TEMPLATE, PYTHON_VERSION


class Generator(object):

    def generate(self, buildopts=None):
        yield Ide(buildopts=buildopts)


class Ide(object):
    def __init__(self, path=None, buildopts=None):
        self.ide = {}
        self.ide["common"] = {}
        self.ide["common"] = self._get_project_file_template()
        self.buildopts = buildopts
        if path:
            self.ide["common"]["path"] = path
        else:
            self.ide["common"]["path"] = getcwd().replace("\\", "/")
        print_string("Start to generate IDE project ")

    def _get_project_file_template(self, name="Default"):
        project_template = {
            "name": name,
            "path": "",
            "folder": "",
            "root": "",
            "outdir": "${ProjDirPath}",
        }
        return project_template

    def _get_cproject_template(self):

        cproject_template = {
            "core": {},
            "includes": [],
            "defines": [],
            "build": "",
            "clean": "",
            "toolchain": "",
        }
        return cproject_template

    def _get_build_template(self):
        return BUILD_CONFIG_TEMPLATE

    def _set_debug_configuration(self):
        common = self.ide["common"]
        nsim_home = os.environ.get("NSIM_HOME", "").replace("\\", "/")
        gnu_class = gnu.Gnu()
        gnu_root_path = os.path.dirname(gnu_class.path)
        openocd_bin = os.path.join(gnu_root_path, "bin", "openocd.exe")
        openocd_cfg = None
        if common["board"] == "emsk":
            openocd_cfg = "snps_em_sk"
        else:
            openocd_cfg = "snps_" + common["board"]

        osppath = osp.OSP()

        # support_board = osppath.support_board(common["root"])
        openocd_cfg_dir = os.path.join(gnu_root_path,
                                       "share",
                                       "openocd",
                                       "scripts",
                                       "board")
        in_openocd_cfg_dir = os.listdir(openocd_cfg_dir)

        openocd_cfg_list = list()
        for cfg in in_openocd_cfg_dir:
            if cfg.startswith(openocd_cfg):
                openocd_cfg_list.append(cfg)
        if not openocd_cfg_list:
            if common["board"] == "emsdp":
                openocd_cfg = os.path.join(common["root"], "board", "emsdp", openocd_cfg)
            else:
                openocd_cfg = os.path.join(openocd_cfg_dir, "snps_em_sk.cfg")
        elif len(openocd_cfg_list) == 1:
            openocd_cfg = os.path.join(openocd_cfg_dir, openocd_cfg_list[0])
        else:
            for cfg in openocd_cfg_list:
                if common["bd_ver"] in cfg:
                    openocd_cfg = os.path.join(openocd_cfg_dir, cfg)
                    break
                elif ".".join(list(common["bd_ver"])) in cfg:
                    openocd_cfg = os.path.join(openocd_cfg_dir, cfg)
                    break
                else:
                    openocd_cfg = os.path.join(openocd_cfg_dir, "snps_em_sk.cfg")
        self.ide["common"]["openocd_cfg"] = openocd_cfg.replace("\\", "/")
        self.ide["common"]["openocd_bin"] = openocd_bin.replace("\\", "/")

        nsim_tcf = "/".join(["etc", "tcf", "templates", "em4_dmips.tcf"])
        nsim_tcf = "/".join([nsim_home, nsim_tcf])
        if common["board"] == "nsim":
            nsim_tcf = osppath.get_tcf(common["root"], common["board"], common["bd_ver"], common["cur_core"]).replace("\\", "/")

        self.ide["common"]["nsim"] = "/".join([nsim_home, "bin", "nsimdrv.exe"])
        self.ide["common"]["nsim_tcf"] = nsim_tcf
        self.ide["common"]["nsim_port"] = 49105

    def _get_project_conf_template(self):
        cproject_template = self._get_cproject_template()

        osp_root = self.buildopts["EMBARC_OSP_ROOT"]
        self.ide["common"]["root"] = osp_root
        self.ide["common"]["folder"] = os.path.relpath(
            getcwd(), osp_root
        ).replace("\\", "/").strip("../")

        make_tool = "make"
        opt_command = [make_tool]
        self.buildopts.update(EMBARC_ROOT=self.buildopts.pop("EMBARC_OSP_ROOT"))
        build_command_list = ["%s=%s" % (key, value) for (key, value) in self.buildopts.items()]
        self.ide["common"]["build_command"] = " ".join(build_command_list)

        opt_command.extend(build_command_list)
        opt_command.append("opt")
        print_string("Run command [make opt]")
        cmd_output = pquery(opt_command)
        includes = list()
        compile_opts = ""
        relative_root = ""
        if cmd_output:
            opt_lines = cmd_output.splitlines()
            for opt_line in opt_lines:
                if opt_line.startswith("COMPILE_OPT"):
                    compile_opt_line = opt_line.split(":", 1)[1]
                    compile_opts = compile_opt_line.split()
                if opt_line.startswith("EMBARC_ROOT"):
                    relative_root = opt_line.split(":", 1)[1].strip()
                if compile_opts != "" and relative_root != "":
                    break

        print_string("Get inculdes and defines ")
        if compile_opts != "" and relative_root != "":
            for comp_opt in compile_opts:
                if comp_opt.startswith("-I"):

                    inc_path = comp_opt.replace("-I", "", 1)
                    if inc_path.startswith(relative_root):
                        inc_path = os.path.relpath(inc_path, relative_root)
                    inc_path = inc_path.replace("\\", "/")

                    includes.append(inc_path)
                if comp_opt.startswith("-D"):
                    define = comp_opt.replace("-D", "", 1)

                    define = define.replace('\\"', '&quot;')
                    define = define.replace('"', '&quot;')

                    cproject_template["defines"].append(define)

        self.buildopts["OUT_DIR_ROOT"] = "${ProjDirPath}"

        cur_core = self.buildopts["CUR_CORE"]
        self.ide["common"]["name"] = self.buildopts["APPL"]
        core_description = "ARC {}".format(cur_core.upper())
        cproject_template["core"] = {
            cur_core: {
                "description": core_description
            }
        }
        for core, _ in cproject_template["core"].items():
            core_id = random.randint(1000000000, 2000000000)
            cproject_template["core"][core]["id"] = core_id

        for path in includes:
            if not path.startswith("obj_"):
                include = os.path.join(self.ide['common']["name"], "embARC", path)
            else:
                include = os.path.join(self.ide['common']["name"], path)

            if path == ".":
                include = self.ide["common"]["name"]

            include = include.replace("\\", "/")
            cproject_template["includes"].append(include)

        self.ide["common"]["toolchain"] = self.buildopts["TOOLCHAIN"]
        self.ide["common"]["board"] = self.buildopts.get("BOARD", None)
        self.ide["common"]["bd_ver"] = self.buildopts.get("BD_VER", None)
        self.ide["common"]["cur_core"] = self.buildopts.get("CUR_CORE", None)
        self.ide["common"]["links_folder"] = list()
        self.ide["common"]["links_file"] = list()

        link_folders, link_files, virtual_folders = self.set_link_folders(
            includes,
            self.buildopts
        )

        self.ide["common"]["links_folder"].extend(link_folders)
        self.ide["common"]["links_file"].extend(link_files)

        self.ide["common"]["virtual_folders"] = uniqify(virtual_folders)
        self._set_debug_configuration()

        return cproject_template

    def set_link_folders(self, includes, makefile_config=None):
        link_folders = list()
        for path in includes:
            if path != "." and "embARC_generated" not in path:
                if "/" in path:
                    if path.startswith("board"):
                        link_folders.append(path.rsplit("/", 2)[0])
                    else:
                        link_folders.append(path)

        link_folders = uniqify(link_folders)

        need_pop = list()
        for link in link_folders:
            for path in link_folders:
                if path != link and path.startswith(link):
                    need_pop.append(path)
        for path in link_folders:
            if path.startswith("inc"):
                need_pop.append(path)
            if path.startswith("options"):
                need_pop.append(path)
            if path.startswith("arc"):
                need_pop.append(path)

        for pop_path in need_pop:
            if pop_path in link_folders:
                link_folders.remove(pop_path)

        link_folders.append("inc")
        link_folders.append("options")
        link_folders.append("arc")
        link_folders.append("device/ip")
        link_folders.append("device/inc")
        if makefile_config:
            os_sel = makefile_config.get("os", None)
            lib_sel = makefile_config.get("lib", None)
            if os_sel:
                link_folders.append("os/" + os_sel)
            if lib_sel:
                link_folders.append("library/" + lib_sel)
        virtual_folders = list()
        for link in link_folders:
            if "/" in link:
                link_depth = len(link.split("/"))
                link_list = [
                    link.rsplit("/", i)[0] for i in range(link_depth, 0, -1)
                ]
                virtual_folders.extend(link_list)
        link_files = list()
        osp_root = self.ide["common"]["root"]
        for folder in virtual_folders:
            folder_path = os.path.join(osp_root, folder)
            for file_name in os.listdir(folder_path):
                file_path = os.path.join(folder_path, file_name)
                if os.path.isfile(file_path):
                    relative_path = os.path.join(
                        folder,
                        file_name
                    ).replace("\\", "/")
                    link_files.append(relative_path)
        current_virtual_folders = list()
        for link in link_folders:
            link = "embARC/" + link
            if "/" in link:
                link_depth = len(link.split("/"))
                link_list = [
                    link.rsplit("/", i)[0] for i in range(link_depth, 0, -1)
                ]
                current_virtual_folders.extend(link_list)
        return link_folders, link_files, current_virtual_folders

    @staticmethod
    def _list_elim_none(list_to_clean):
        return [l for l in list_to_clean if l]

    @staticmethod
    def _dict_elim_none(dic_to_clean):
        dic = dic_to_clean
        try:
            for key, value in dic_to_clean.items():
                if isinstance(value, list):
                    dic[key] = Ide._list_elim_none(value)
                elif isinstance(value, dict):
                    dic[key] = Ide._dict_elim_none(value)
        except AttributeError:
            pass
        return dic

    def _set_project_attributes(self, key_values, destination, source):
        if key_values in source:
            for attribute, data in source[key_values].items():
                if attribute in destination:
                    if isinstance(destination[attribute], list):
                        if isinstance(data, list) is list:
                            destination[attribute].extend(data)
                        else:
                            destination[attribute].append(data)
                    elif isinstance(destination[attribute], list):
                        destination[attribute] = Ide._dict_elim_none(
                            merge_recursive(destination[attribute], data)
                        )
                    else:
                        if isinstance(data, list) is list:
                            if data[0]:
                                destination[attribute] = data[0]
                        else:
                            if data:
                                destination[attribute] = data[0]

    def get_asm_c_include(self):
        self.ide["exporter"] = self._get_project_conf_template()
        self.ide["exporter"].update(self.ide["common"])
        '''
        updata app.json
        '''

    def generate(self):
        with cd(self.ide["common"]["path"]):

            self.get_asm_c_include()
            outdir = "."
            cores = self.ide["exporter"]["core"].keys()
            if PYTHON_VERSION.startswith("3"):
                cores = list(cores)
            launch = self.ide["common"]["name"] + "-" + cores[0] + ".launch"

            exporter = Exporter(self.ide["common"]["toolchain"])
            print_string(
                "Start to generate IDE project accroding to templates"
                + " (.project.tmpl and .cproject.tmpl)"
            )
            exporter.gen_file_jinja(
                "project.tmpl", self.ide["common"], ".project", outdir
            )
            exporter.gen_file_jinja(
                ".cproject.tmpl", self.ide["exporter"], ".cproject", outdir
            )
            exporter.gen_file_jinja(
                ".launch.tmpl", self.ide["exporter"], launch, outdir
            )
            print_string(
                "Finish generate IDE project and the files are in "
                + os.path.abspath(outdir)
            )
            print_string(
                "Open ARC GNU IDE (version) Eclipse "
                + "- >File >Open Projects from File System >Paste\n"
                + os.path.abspath(outdir)
            )
