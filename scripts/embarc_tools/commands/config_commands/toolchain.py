from __future__ import print_function, division, unicode_literals
from ...notify import print_string
from ...settings import get_input, SUPPORT_TOOLCHAIN
from ...osp import osp
from ...toolchain import gnu, metaware
help = "Get, set toolchain configuration options."
usage = ("\n    embarc config toolchain [--version] [--download] gnu\n"
         "    embarc config toolchain mw\n"
         "    embarc config toolchain --set <toolchain>\n")


def run(args, remainder=None):
    if len(remainder) == 1 and remainder[0] in SUPPORT_TOOLCHAIN:
        gnu_verion = gnu.Gnu.check_version()
        mw_verion = metaware.Mw.check_version()
        toolchain = remainder[0]
        if toolchain == "gnu":
            if gnu_verion:
                print_string("Current GNU verion: {}".format(gnu_verion))
            else:
                toolchain_class = gnu.Gnu()
                input_ = None
                if not args.download:
                    input_ = get_input("[embARC] Download the latest gnu [Y/N]")
                if input_ in ["Y", "y", "yes"]:
                    input_ = True
                if input_ or args.download:
                    tgz_path = toolchain_class.download(version=args.version, path=args.download_path)
                    print_string("Download it to : {}, please install it and set environment.".format(tgz_path))
                else:
                    print_string("You can get GNU Toolchain from (%s)" % (toolchain_class.root_url))

        elif toolchain == "mw":
            if mw_verion:
                print_string("Current MetaWare version: {}".format(mw_verion))
            else:
                print_string("There is no MetaWare in this platform, please install it")
        else:
            print_string("This toolchain {} is not supported now".format(toolchain))
    elif not remainder:
        if args.set:
            if args.set in SUPPORT_TOOLCHAIN:
                print_string("Set %s as global TOOLCHAIN" % args.set)
                osppath = osp.OSP()
                osppath.set_global("TOOLCHAIN", args.set)
            else:
                print_string("Only support GNU and MetaWare now")
        else:
            print("usage: " + usage)
    else:
        print("usage: " + usage)
        return


def setup(subparser):
    subparser.usage = usage
    subparser.add_argument(
        "--version", action='store_true', help="Choose toolchain version.")
    subparser.add_argument(
        "--download", action='store_true', help="Downlad the latested toolchain only support gnu.")
    subparser.add_argument(
        "--set", help="Set a toolchain as global setting.")
