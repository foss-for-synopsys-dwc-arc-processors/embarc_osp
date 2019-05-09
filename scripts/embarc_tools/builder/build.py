from __future__ import print_function, unicode_literals
import sys
import os
import time
import collections
from ..settings import BUILD_CONFIG_TEMPLATE, BUILD_OPTION_NAMES, BUILD_INFO_NAMES, BUILD_CFG_NAMES, BUILD_SIZE_SECTION_NAMES, get_config, MAKEFILENAMES
from ..utils import mkdir, delete_dir_files, cd, generate_json, pqueryOutputinline, pqueryTemporaryFile
from ..notify import (print_string, print_table)
from ..osp import osp
from ..builder import secureshield


class embARC_Builder(object):
    def __init__(self, osproot=None, buildopts=None, outdir=None, embarc_config="embarc_app.json"):
        self.buildopts = dict()
        make_options = ' '
        if osproot is not None and os.path.isdir(osproot):
            self.osproot = os.path.realpath(osproot)
            self.buildopts["EMBARC_OSP_ROOT"] = self.osproot
            # make_options += 'EMBARC_ROOT=' + str(self.osproot) + ' '
        else:
            self.osproot = None
        if outdir is not None:
            self.outdir = os.path.realpath(outdir)
            make_options += 'OUT_DIR_ROOT=' + str(self.outdir) + ' '
        else:
            self.outdir = None

        if buildopts is not None:
            self.buildopts.update(buildopts)
        self.make_options = make_options
        self.embarc_config = embarc_config

    @staticmethod
    def is_embarc_makefile(app):
        with open(app) as f:
            embarc_root = False
            appl = False
            lines = f.read().splitlines()
            for line in lines:
                if "EMBARC_ROOT" in line:
                    embarc_root = True
                if "APPL" in line:
                    appl = True
                if embarc_root and appl:
                    return True
            return False

    @staticmethod
    def build_common_check(app):
        build_status = {'result': True, 'reason': ''}
        app_normpath = os.path.normpath(app)
        if not os.path.isdir(app_normpath):
            build_status['reason'] = 'Application folder doesn\'t exist!'
            build_status['result'] = False
        current_makefile = None
        for makename in MAKEFILENAMES:
            if makename in os.listdir(app_normpath):
                current_makefile = os.path.join(app_normpath, makename)
                break
        if not current_makefile:
            build_status['reason'] = 'Application makefile donesn\'t exist!'
            build_status['result'] = False
        else:
            if not embARC_Builder.is_embarc_makefile(current_makefile):
                build_status['reason'] = 'Application makefile is invalid!'
                build_status['result'] = False

        app_realpath = os.path.realpath(app_normpath)
        build_status['app_path'] = app_realpath

        return app_realpath, build_status

    def configCoverity(self, toolchain):
        print_string("Config coverity")
        build_status = {'result': True, 'reason': ''}
        self.coverity_comptype = 'gcc'
        self.coverity_compiler = 'arc-elf32-gcc'
        if toolchain == "gnu":
            pass
        elif toolchain == "mw":
            self.coverity_comptype = 'clangcc'
            self.coverity_compiler = 'ccac'
        else:
            build_status["result"] = False
            build_status["reason"] = "Toolchian is not supported!"
            return build_status
        self.coverity_sa_version = os.environ.get("COVERITY_SA_VERSION", None)
        self.coverity_server = os.environ.get("COVERITY_SERVER", None)
        self.user = os.environ.get("AUTO_USER", None)
        self.password = os.environ.get("AUTO_PASSWORD", None)
        self.coverity_steam_pre = os.environ.get("COVERITY_STREAM_PRE", None)
        return build_status

    def _setCoverityDirs(self, app):
        app_path_list = app.split("/")
        self.coverity_steam = self.coverity_steam_pre + "_".join(app_path_list[1:])
        # print_string("The coverity stream: {} {} {} ".format(self.coverity_steam))
        self.coverity_data_dir = os.environ.get("COVERITY_DATA_DIR", "coverity-data")
        self.coverity_config = os.path.join(self.coverity_data_dir, "coverity-config.xml")
        self.coverity_html = "coverity_html"
        if os.path.exists(self.coverity_data_dir):
            delete_dir_files(self.coverity_data_dir, dir=True)
            mkdir(self.coverity_data_dir)
        if os.path.exists(self.coverity_html):
            delete_dir_files(self.coverity_html, dir=True)

    def build_coverity(self, make_cmd):
        build_status = {'result': True, 'reason': ''}
        print_string("BEGIN SECTION Configure Coverity to use the built-incompiler")
        config_compilercmd = "cov-configure --config {} --template --comptype {} --compiler {}".format(
            self.coverity_config,
            self.coverity_comptype, self.coverity_compiler
        )
        returncode = os.system(config_compilercmd)
        if returncode != 0:
            build_status["result"] = False
            build_status["reason"] = "Configure Coverity Failed!"
            return build_status

        print_string("BEGIN SECTION Build with Coverity {}".format(self.coverity_sa_version))
        coverity_build = "cov-build --config %s --dir %s %s" % (self.coverity_config, self.coverity_data_dir, make_cmd)
        try:
            build_proc = pqueryOutputinline(coverity_build, console=True)
            build_status['build_msg'] = build_proc
        except Exception as e:
            build_status["result"] = False
            build_status["reason"] = "Build with Coverity Failed! {}".format(e)
            return build_status

        print_string("BEGIN SECTION Coverity Analyze Defects")
        coverity_analyze = "cov-analyze --dir {}".format(self.coverity_data_dir)
        returncode = os.system(coverity_analyze)
        if returncode != 0:
            build_status["result"] = False
            build_status["reason"] = "Coverity Analyze Defects Failed!"
            return build_status

        print_string("BEGIN SECTION Coverity Format Errors into HTML")
        coverity_errors = "cov-format-errors --dir %s -x -X --html-output %s" % (self.coverity_data_dir, self.coverity_html)
        returncode = os.system(coverity_errors)
        if returncode != 0:
            build_status["result"] = False
            build_status["reason"] = "Coverity Format Errors into HTML Failed!"
            return build_status

        print_string("BEGIN SECTION Coverity Commit defects to {} steam {}".format(self.coverity_server, self.coverity_steam))
        coverity_commit = "cov-commit-defects --dir %s --host %s --stream %s --user %s --password %s" % (
            self.coverity_data_dir,
            self.coverity_server, self.coverity_steam, self.user, self.password
        )
        returncode = os.system(coverity_commit)
        if returncode != 0:
            build_status["result"] = False
            build_status["reason"] = "Coverity Commit defects Failed!"
            return build_status

        '''print_string("BEGIN SECTION Coverity Send E-mail Notifications")
        coverity_manage = "cov-manage-im --mode notification --execute --view 'Default' --host %s --user %s --password %s" % (
            self.coverity_server,
            self.user, self.password
        )
        returncode = os.system(coverity_manage)
        if returncode != 0:
            build_status["result"] = False
            build_status["reason"] = " Coverity Send E-mail Notifications Failed!"
            return build_status'''

        return build_status

    def get_build_cmd(self, app, target=None, parallel=8, silent=False):
        build_status = dict()
        build_precmd = "make "
        if parallel:
            build_precmd = "{} -j {}".format(build_precmd, str(parallel))

        if target != "info":
            build_config_template = self.get_build_template()
            with cd(app):
                self.get_makefile_config(build_config_template)
        build_precmd = "{} {}".format(build_precmd, self.make_options)
        if silent:
            if "SILENT=1" not in build_precmd:
                build_precmd = "{} SILENT=1 ".format(build_precmd)
        if isinstance(target, str) or target is not None:
            build_cmd = build_precmd + " " + str(target)
        else:
            build_status['reason'] = "Unrecognized build target"
            build_status['result'] = False
            return build_status
        build_cmd_list = build_cmd.split()
        for i in range(len(build_cmd_list)):
            if build_cmd_list[i].startswith("EMBARC_ROOT"):
                build_cmd_list[i] = "EMBARC_ROOT=" + self.osproot
                break
        build_cmd = " ".join(build_cmd_list)
        print_string("Build command: {} ".format(build_cmd))
        build_status['build_cmd'] = build_cmd
        return build_status

    def build_target(self, app, target=None, parallel=8, coverity=False, silent=False):
        app_realpath, build_status = self.build_common_check(app)
        build_status['build_target'] = target
        build_status['time_cost'] = 0
        print_string("Build target: {} " .format(target))

        if not build_status['result']:
            return build_status

        # Check and create output directory
        if (self.outdir is not None) and (not os.path.isdir(self.outdir)):
            print_string("Create application output directory: " + self.outdir)
            os.makedirs(self.outdir)

        current_build_cmd = self.get_build_cmd(app_realpath, target, parallel, silent)
        build_status.update(current_build_cmd)
        build_cmd = build_status.get('build_cmd', None)

        def start_build(build_cmd, build_status=None):
            print_string("Start to build application")
            return_code = 0
            time_pre = time.time()
            if coverity:
                with cd(app_realpath):
                    self._setCoverityDirs(app)
                    coverity_build_status = self.build_coverity(build_cmd)
                    if not coverity_build_status["result"]:
                        build_status["result"] = False
                        build_status["reason"] = coverity_build_status["reason"]
                        build_status["build_msg"] = coverity_build_status["build_msg"]
                    else:
                        build_status["build_msg"] = ["Build Coverity successfully"]
            else:
                if target not in ["opt", "info", "size", "all"]:
                    with cd(app_realpath):
                        try:
                            return_code = os.system(build_cmd)
                            if return_code == 0:
                                build_status["build_msg"] = ["Build successfully"]
                            else:
                                build_status["build_msg"] = ["Build failed"]
                                build_status['result'] = False
                                build_status["reason"] = "ProcessError: Run command {} failed".format(build_cmd)
                        except (KeyboardInterrupt):
                            print_string("Terminate batch job", "warning")
                            sys.exit(1)
                else:
                    try:
                        build_proc = pqueryOutputinline(build_cmd, cwd=app, console=True)
                        build_status['build_msg'] = build_proc
                    except Exception as e:
                        print("Run command({}) failed! {} ".format(build_cmd, e))
                        build_status["build_msg"] = ["Build failed"]
                        build_status["reason"] = "ProcessError: Run command {} failed".format(build_cmd)
                        build_status['result'] = False
            build_status['time_cost'] = (time.time() - time_pre)
            return build_status

        secureshield_config = secureshield.common_check(self.buildopts["TOOLCHAIN"], app_realpath)
        if secureshield_config:
            with secureshield.secureshield_appl_cfg_gen(self.buildopts["TOOLCHAIN"], secureshield_config, app_realpath):
                build_cmd_list = build_cmd.split()
                target = build_cmd_list[-1]
                build_cmd_list[-1] = "USE_SECURESHIELD_APPL_GEN=1"
                build_cmd_list.append(target)
                build_cmd = " ".join(build_cmd_list)
                build_status = start_build(build_cmd, build_status)
        else:
            build_status = start_build(build_cmd, build_status)
        print_string("Completed in: ({})s  ".format(build_status['time_cost']))
        return build_status

    def get_build_info(self, app, parallel=False):
        build_status = self.build_target(app, target=str('opt'), parallel=parallel)
        if not build_status['result']:
            return build_status

        build_cfg = dict()
        cfg_lines = build_status['build_msg']

        for cfg_line in cfg_lines:
            words = cfg_line.split(':')
            if len(words) == 2:
                key = words[0].strip()
                value = words[1].strip()
                if key in BUILD_CFG_NAMES or key in BUILD_OPTION_NAMES:
                    build_cfg[key] = value

        build_status['build_cfg'] = build_cfg

        # Get Build Info
        info_status = self.build_target(app, target=str('info'))
        build_out = info_status['build_msg']
        build_info = dict()
        if info_status['result']:
            for info_line in build_out:
                words = info_line.split(':')
                if len(words) == 2:
                    key = words[0].strip()
                    value = words[1].strip()
                    if key in BUILD_INFO_NAMES:
                        build_info[key] = value
                    if key == 'BUILD_OPTION':
                        build_cfgs_dict = value.split()
                        for cfg_dict in build_cfgs_dict:
                            cfg_pair = cfg_dict.split('=')
                            if len(cfg_pair) == 2 and cfg_pair[0] in BUILD_OPTION_NAMES:
                                build_status['build_cfg'][cfg_pair[0]] = cfg_pair[1]
                    if key == 'MIDDLEWARE' or key == 'PERIPHERAL':
                        build_info[key + 'S'] = value.split()
                    if key == 'APPLICATION_ELF':
                        build_info['APPLICATION_OUTDIR'] = os.path.dirname(value)
        build_status['build_info'] = build_info

        app_realpath = build_status['app_path']
        if 'EMBARC_ROOT' in build_status['build_cfg']:
            if not os.path.isabs((build_status['build_cfg']['EMBARC_ROOT'])):
                build_status['build_cfg']['EMBARC_ROOT'] = os.path.realpath(os.path.join(app_realpath, build_status['build_cfg']['EMBARC_ROOT']))
        if 'OUT_DIR_ROOT' in build_status['build_cfg']:
            if not os.path.isabs(build_status['build_cfg']['OUT_DIR_ROOT']):
                build_status['build_cfg']['OUT_DIR_ROOT'] = os.path.realpath(os.path.join(app_realpath, build_status['build_cfg']['OUT_DIR_ROOT']))
        if 'OUT_DIR_ROOT' in build_status['build_info']:
            if not os.path.isabs(build_status['build_info']['OUT_DIR_ROOT']):
                build_status['build_info']['OUT_DIR_ROOT'] = os.path.realpath(os.path.join(app_realpath, build_status['build_info']['OUT_DIR_ROOT']))
        if 'APPLICATION_ELF' in build_status['build_info']:
            if not os.path.isabs(build_status['build_info']['APPLICATION_ELF']):
                build_status['app_elf'] = os.path.realpath(os.path.join(app_realpath, build_status['build_info']['APPLICATION_ELF']))
            else:
                build_status['app_elf'] = build_status['build_info']['APPLICATION_ELF']
        if 'APPLICATION_HEX' in build_status['build_info']:
            if not os.path.isabs(build_status['build_info']['APPLICATION_HEX']):
                build_status['app_hex'] = os.path.realpath(os.path.join(app_realpath, build_status['build_info']['APPLICATION_HEX']))
            else:
                build_status['app_hex'] = build_status['build_info']['APPLICATION_HEX']
        if 'APPLICATION_BIN' in build_status['build_info']:
            if not os.path.isabs(build_status['build_info']['APPLICATION_BIN']):
                build_status['app_bin'] = os.path.realpath(os.path.join(app_realpath, build_status['build_info']['APPLICATION_BIN']))
            else:
                build_status['app_bin'] = build_status['build_info']['APPLICATION_BIN']
        if 'APPLICATION_OUTDIR' in build_status['build_info']:
            if not os.path.isabs(build_status['build_info']['APPLICATION_OUTDIR']):
                build_status['app_outdir'] = os.path.realpath(os.path.join(app_realpath, build_status['build_info']['APPLICATION_OUTDIR']))
            else:
                build_status['app_outdir'] = build_status['build_info']['APPLICATION_OUTDIR']

        return build_status

    def build_elf(self, app, parallel=False, pre_clean=False, post_clean=False, silent=False):
        # Clean Application before build if requested
        if pre_clean:
            build_status = self.build_target(app, parallel=parallel, target=str('clean'))
            if not build_status['result']:
                return build_status

        # Build Application
        build_status = self.build_target(app, parallel=parallel, target=str('all'), silent=silent)
        if not build_status['result']:
            return build_status
        # Clean Application after build if requested
        if post_clean:
            clean_status = self.build_target(app, parallel=parallel, target=str('clean'))
            if not clean_status['result']:
                return clean_status

        return build_status

    def build_bin(self, app, parallel=False, pre_clean=False, post_clean=False):
        # Clean Application before build if requested
        if pre_clean:
            build_status = self.build_target(app, parallel=parallel, target=str('clean'))
            if not build_status['result']:
                return build_status

        # Build Application
        build_status = self.build_target(app, parallel=parallel, target=str('bin'))
        if not build_status['result']:
            return build_status
        # Clean Application after build if requested
        if post_clean:
            clean_status = self.build_target(app, parallel=parallel, target=str('clean'))
            if not clean_status['result']:
                return clean_status

        return build_status

    def build_hex(self, app, parallel=False, pre_clean=False, post_clean=False):
        # Clean Application before build if requested
        if pre_clean:
            build_status = self.build_target(app, parallel=parallel, target=str('clean'))
            if not build_status['result']:
                return build_status

        # Build Application
        build_status = self.build_target(app, parallel=parallel, target=str('hex'))
        if not build_status['result']:
            return build_status
        # Clean Application after build if requested
        if post_clean:
            clean_status = self.build_target(app, parallel=parallel, target=str('clean'))
            if not clean_status['result']:
                return clean_status

        return build_status

    def get_build_size(self, app, parallel=False, silent=False):
        build_status = self.build_target(app, parallel=parallel, target=str('size'), silent=silent)
        build_size = dict()
        if build_status['result']:
            app_size_lines = build_status['build_msg']
            len_app_size_lines = len(app_size_lines)
            if len_app_size_lines >= 3:
                app_size_lines = app_size_lines[len_app_size_lines - 2:]
                section_names = app_size_lines[0].split()
                section_values = app_size_lines[1].split()
                for idx, section_name in enumerate(section_names):
                    if section_name in BUILD_SIZE_SECTION_NAMES:
                        build_size[section_name] = int(section_values[idx])
            else:
                build_status['result'] = False
        else:
            print_string("Build failed and there is no size information")
        build_status['build_size'] = build_size
        return build_status

    def clean(self, app, parallel=False):
        build_status = self.build_target(app, target=str('clean'), parallel=parallel)
        return build_status

    def distclean(self, app, parallel=False):
        build_status = self.build_target(app, target=str('distclean'), parallel=parallel)
        return build_status

    def boardclean(self, app, parallel=False):
        build_status = self.build_target(app, target=str('boardclean'), parallel=parallel)
        return build_status

    def get_makefile_config(self, build_template=None):
        # current_build_templates = dict()
        ospclass = osp.OSP()
        build_template["APPL"] = self.buildopts.get("APPL", False)
        build_template["BOARD"] = self.buildopts.get("BOARD", False)
        build_template["BD_VER"] = self.buildopts.get("BD_VER", False)
        build_template["CUR_CORE"] = self.buildopts.get("CUR_CORE", False)
        build_template["TOOLCHAIN"] = self.buildopts.get("TOOLCHAIN", False)
        build_template["OLEVEL"] = self.buildopts.get("OLEVEL", False)
        osp_root = self.buildopts.get("EMBARC_OSP_ROOT", False)

        if not all(build_template.values()):
            default_makefile_config = dict()
            _, default_makefile_config = ospclass.get_makefile_config(default_makefile_config)
            if not osp_root:
                osp_root = default_makefile_config.get("EMBARC_OSP_ROOT")
            for key, value in build_template.items():
                if not value:
                    build_template[key] = default_makefile_config.get(key, False)
            self.buildopts.update(build_template)

        osp_root, update = ospclass.check_osp(osp_root)
        self.make_options += 'EMBARC_ROOT=' + str(osp_root) + ' '
        self.buildopts["EMBARC_OSP_ROOT"] = osp_root
        build_template["EMBARC_OSP_ROOT"] = osp_root

        if not all(build_template.values()):
            try:
                returncode, cmd_output = pqueryTemporaryFile(["make", "EMBARC_ROOT=" + str(osp_root), "info"])
                default_build_option = None
                if not returncode and cmd_output:
                    for line in cmd_output:
                        if line.startswith("BUILD_OPTION"):
                            default_build_option = str(line.split(":", 1)[1]).split()
                            break
                        else:
                            pass
                    default_build_option_dict, _ = get_config(default_build_option)
                    for key, value in build_template.items():
                        if not value:
                            build_template[key] = default_build_option_dict[key]
                    self.buildopts.update(build_template)
            except Exception as e:
                print_string("Error: {}".format(e))
                sys.exit(1)

        current_build_list = ["%s=%s" % (opt, build_template[opt]) for opt in BUILD_CONFIG_TEMPLATE.keys()]
        self.make_options = self.make_options + " ".join(current_build_list)

        self.buildopts.update(build_template)
        generate_json(self.buildopts, self.embarc_config)

        print_string("Current configuration ")
        table_head = list()
        table_content = list()
        for key, value in build_template.items():
            table_head.append(key)
            table_content.append(value)
        msg = [table_head, [table_content]]
        print_table(msg)
        self.osproot = osp_root
        return build_template

    def get_build_template(self):

        build_template = BUILD_CONFIG_TEMPLATE
        build_template = collections.OrderedDict()
        return build_template
