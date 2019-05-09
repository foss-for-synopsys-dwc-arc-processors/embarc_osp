from __future__ import print_function, division, unicode_literals

help = "Get, set or unset configuration options."

description = ("Currently supported options: osp, toolchain, build_cfg")


def run(args, remainder=None):
    pass


def setup(subparser):
    subparser.usage = ("\n    embarc config osp --add <name> <url/path> [<dest>]\n"
                       "    embarc config osp --rename <oldname> <newname>\n"
                       "    embarc config osp --remove <name>\n"
                       "    embarc config osp --list\n"
                       "    embarc config osp --set <name>\n"
                       "    embarc config toolchain [--version] [--download] gnu\n"
                       "    embarc config toolchain [--version] mw\n"
                       "    embarc config toolchain --set <gnu/mw>\n"
                       "    embarc config build_cfg BOARD <value>\n"
                       "    embarc config build_cfg BD_VER <value>\n"
                       "    embarc config build_cfg CUR_CORE <value>\n")
