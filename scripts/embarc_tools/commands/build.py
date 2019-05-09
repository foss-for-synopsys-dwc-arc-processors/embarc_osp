from __future__ import print_function, division, unicode_literals
import os
import sys
from ..project import Generator
from ..settings import get_input, get_config
from ..builder import build
from ..utils import cd, getcwd, read_json
help = "Build application"
description = ("Compile code using toolchain\n"
               "Currently supported Toolchain: GNU, MetaWare.")


def run(args, remainder=None):
    osproot = None
    curdir = args.outdir
    app_path = None
    recordBuildConfig = dict()
    if args.path:
        app_path = args.path
    else:
        app_path = getcwd()
    app_path = os.path.abspath(app_path)

    if not (os.path.exists(app_path) and os.path.isdir(app_path)):
        print("[embARC] This is not a valid application path")
        return

    embarc_config = args.app_config

    if not (embarc_config and os.path.exists(embarc_config)):
        embarc_config = os.path.join(app_path, "embarc_app.json")

    if os.path.exists(embarc_config):
        print("[embARC] Read embarc_app.json")
        recordBuildConfig = read_json(embarc_config)

    parallel = args.parallel
    if args.board:
        recordBuildConfig["BOARD"] = args.board
    if args.bd_ver:
        recordBuildConfig["BD_VER"] = args.bd_ver
    if args.core:
        recordBuildConfig["CUR_CORE"] = args.core
    if args.toolchain:
        recordBuildConfig["TOOLCHAIN"] = args.toolchain
    if args.olevel:
        recordBuildConfig["OLEVEL"] = args.olevel
    if remainder:
        if (remainder[0]).startswith("-"):
            print("embarc build: error: invalid parameter %s" % remainder[0])
            sys.exit(1)
        make_config, target = get_config(remainder)
        if target:
            args.target = target
        recordBuildConfig.update(make_config)

    builder = build.embARC_Builder(osproot, recordBuildConfig, curdir)
    if args.export:
        builder.get_build_cmd(app_path, target=None, parallel=parallel, silent=False)
        with cd(app_path):
            if os.path.exists(".project") and os.path.exists(".cproject"):
                while True:
                    yes = get_input("The IDE project already exists, recreate and overwrite the old files [Y/N]  ")
                    if yes in ["yes", "Y", "y"]:
                        break
                    elif yes in ["no", "n", "N"]:
                        return
                    else:
                        continue
            generator = Generator()
            recordBuildConfig = read_json(embarc_config)
            for project in generator.generate(buildopts=recordBuildConfig):
                project.generate()
        sys.exit(0)
    if args.target:
        information = None
        if args.target == "elf":
            information = builder.build_elf(app_path, parallel=parallel, pre_clean=False, post_clean=False)
        elif args.target == "bin":
            information = builder.build_bin(app_path, parallel=parallel, pre_clean=False, post_clean=False)
        elif args.target == "hex":
            information = builder.build_hex(app_path, parallel=parallel, pre_clean=False, post_clean=False)
        elif args.target == "clean":
            information = builder.clean(app_path, parallel=parallel)
        elif args.target == "distclean":
            information = builder.distclean(app_path, parallel=parallel)
        elif args.target == "boardclean":
            information = builder.boardclean(app_path, parallel=parallel)
        elif args.target == "info":
            information = builder.get_build_info(app_path, parallel=parallel)
        elif args.target == "size":
            information = builder.get_build_size(app_path, parallel=parallel)
        elif args.target:
            information = builder.build_target(app_path, target=args.target, parallel=parallel, coverity=False)
        else:
            print("[embARC] Please choose right target")
        if information:
            if information.get("result") is False:
                print("[embARC] Failed: {}".format(information.get("reason")))


def setup(subparser):
    subparser.add_argument(
        "-d", "--path", default=".", help="Application path")
    subparser.add_argument(
        "--outdir", help="Copy all files to this exported directory")
    subparser.add_argument(
        "-b", "--board", help="Build using the given BOARD")
    subparser.add_argument(
        "--bd_ver", help="Build using the given BOARD VERSION")
    subparser.add_argument(
        "--core", help="Build using the given CORE")
    subparser.add_argument(
        "-o", "--olevel", help="Build using the given OLEVEL")
    subparser.add_argument(
        "-t", "--toolchain", help="Build toolchain. Example: gnu, mw")
    subparser.add_argument(
        "-j", "--parallel", type=int, help="Build application with -j")
    subparser.add_argument(
        "--target", default="all", choices=["elf", "bin", "hex", "size", "info", "opt" "all", "run", "clean"], help="Choose build target, default target is all")
    subparser.add_argument(
        "-g", "--export", action="store_true", help="Generate IDE project file for your application")
    subparser.add_argument(
        "--app_config", help="Specify application configuration. Default is to look for 'embarc_app.json")
