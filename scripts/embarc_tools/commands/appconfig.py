from __future__ import print_function, division, unicode_literals
import os
import sys
from ..notify import print_string
from ..utils import getcwd, read_json, cd
from ..osp import osp
from ..builder import build

help = "Get or set application config"
description = ("Show detail config.\n"
               "Currently supported options: board, board version, current core, toolchain, olevel and embarc root.\n"
               "Result is to look for embarc_app.json, default options in makefile will be overridden by it")


def run(args, remainder=None):
    if remainder:
        print("[embARC] embarc appconfig: error: invalid parameter %s" % remainder[0])
        sys.exit(1)
    root = getcwd()
    app_path = None
    recordBuildConfig = dict()
    osppath = osp.OSP()
    osp_root = None

    if not args.application:
        app_path = root
    else:
        app_path = os.path.join(root, args.application)
    osppath = osp.OSP()
    makefile = osppath.get_makefile(app_path)
    if makefile:
        embarc_config = os.path.join(app_path, "embarc_app.json")
        if os.path.exists(embarc_config):
            print("[embARC] Read embarc_app.json")
            recordBuildConfig = read_json(embarc_config)
        if args.board:
            recordBuildConfig["BOARD"] = args.board
        if args.bd_ver:
            recordBuildConfig["BD_VER"] = args.bd_ver
        if args.cur_core:
            recordBuildConfig["CUR_CORE"] = args.cur_core
        if args.toolchain:
            recordBuildConfig["TOOLCHAIN"] = args.toolchain
        if args.olevel:
            recordBuildConfig["OLEVEL"] = args.olevel
        if args.osp_root:
            osp_root, _ = osppath.check_osp(args.osp_root)
            recordBuildConfig["EMBARC_OSP_ROOT"] = osp_root.replace("\\", "/")
        builder = build.embARC_Builder(osp_root, recordBuildConfig)
        build_config_template = builder.get_build_template()
        with cd(app_path):
            builder.get_makefile_config(build_config_template)

    else:
        print_string("[embARC] Please set a valid application path")
        return


def setup(subparser):
    subparser.add_argument(
        "-a", "--application", help="Specify the path of the application")
    subparser.add_argument(
        "-b", "--board", help="Set board")
    subparser.add_argument(
        "--bd_ver", help="Set board version")
    subparser.add_argument(
        "--cur_core", help="Set core")
    subparser.add_argument(
        "--toolchain", help="Set toolchain")
    subparser.add_argument(
        "--osp_root", help="Set embARC osp root path")
    subparser.add_argument(
        "-o", "--olevel", help="Set olevel")
