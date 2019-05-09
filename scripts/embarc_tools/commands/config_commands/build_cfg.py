from __future__ import print_function, division, unicode_literals
from ...notify import print_string
from ...osp import osp

help = "Set global build configuration."
usage = ("\n    embarc config build_cfg BOARD <value>\n"
         "    embarc config build_cfg BD_VER <value>\n"
         "    embarc config build_cfg CUR_CORE <value>\n")


def run(args, remainder=None):
    if len(remainder) != 2:
        print("usage: " + usage)
    else:
        config = remainder[0]
        if config not in ["BOARD", "BD_VER", "CUR_CORE"]:
            print("usage: " + usage)
            return
        value = remainder[1]
        osppath = osp.OSP()
        print_string("Set %s = %s as global setting" % (config, value))
        osppath.set_global(config, value)


def setup(subparser):
    subparser.usage = usage
