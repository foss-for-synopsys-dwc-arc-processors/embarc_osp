from __future__ import print_function, division, unicode_literals
import os
from ...notify import print_string
from ...settings import EMBARC_OSP_URL
from ...osp import repo, osp
from ...utils import getcwd, read_json, unzip

help = "Get, set or unset osp configuration."

usage = ("\n    embarc config osp --add <name> <url/path> [<dest>]\n"
         "    embarc config osp --set <name>\n"
         "    embarc config osp --rename <oldname> <newname>\n"
         "    embarc config osp --remove <name>\n"
         "    embarc config osp --list")


def run(args, remainder=None):
    osppath = osp.OSP()
    num = [args.add, args.rename, args.remove]
    if num.count(False) + num.count(None) < 2:
        print("usage: " + usage)
        return
    if args.add:
        if len(remainder) < 2:
            print("usage: " + usage)
            return
        else:
            name = remainder[0]
            url = remainder[1]
            dest = None
            if len(remainder) >= 3:
                dest = remainder[2]
                if not (os.path.exists(url) and os.path.isdir(url)):
                    print_string("% is not a valid path" % dest)
                    dest = None
            if os.path.exists(url) and os.path.isdir(url):
                source_type = "local"
                url = os.path.abspath(url)
                msg = "Add this local (%s) to user profile osp.json" % (url)
                print_string(msg)
                osppath.set_path(name, source_type, url)
                args.list = True

            elif os.path.exists(url) and os.path.isfile(url):
                if url.endswith("zip"):
                    dest_dir = dest if dest else getcwd()
                    path = os.path.join(dest_dir, name)
                    result = unzip(url, name)
                    if not result:
                        msg = "Unzip zip failed"
                        print_string(msg, level="error")
                    else:
                        source_type = "zip"
                        osppath.set_path(name, source_type, path, url)
                        print_string("Add (%s) to user profile osp.json" % path)
                        args.list = True
            elif url == EMBARC_OSP_URL:
                osprepo = repo.Repo.fromurl(url)
                path = dest if dest else getcwd()
                source_type = "git"
                if not os.path.exists(name):
                    print_string("Start clone {}".format(osprepo.name))
                    osprepo.clone(osprepo.url, path=os.path.join(path, name), rev=None, depth=None, protocol=None, offline=False)
                    print_string("Finish clone {}".format(osprepo.name))
                    osppath.set_path(name, source_type, os.path.join(path, name), url)
                    print_string("Add (%s) to user profile osp.json" % os.path.join(path, osprepo.name))
                    args.list = True
                else:
                    print_string("There is already a folder or file named '%s' under current path" % name)
                    return
            else:
                print("usage: " + usage)
                return
    elif args.rename:
        if len(remainder) != 2:
            print("usage: " + usage)
        else:
            old = remainder[0]
            new = remainder[1]
            print_string("Start rename {} to {}".format(old, new))
            osppath.rename(old, new)
            args.list = True
    elif args.remove:
        name = args.remove
        print_string("Start remove {} ".format(name))
        osppath.remove_path(name)
        args.list = True
    elif args.set:
        name = args.set
        print_string("Set %s as global EMBARC_OSP_ROOT" % name)
        if osppath.get_path(name):
            config = "EMBARC_OSP_ROOT"
            osppath.set_global(config, name)
        else:
            print_string("This is not a valid osp path")
            args.list = True
    else:
        if remainder:
            print("usage: " + usage)
            return

    if args.list:
        print_string("Current recored embARC source code")
        current_paths = osppath.list_path()
        makefile = osppath.get_makefile(getcwd())
        app_setting = dict()
        current_osp = None
        if makefile:
            if os.path.exists("embarc_app.json"):
                app_setting = read_json("embarc_app.json")
                current_osp = app_setting.get("EMBARC_OSP_ROOT", False)
            else:
                _, app_setting = osppath.get_makefile_config(app_setting)
                current_osp = app_setting.get("EMBARC_OSP_ROOT", False)
        if current_paths:
            osppath.list_path(show=True, current=current_osp)


def setup(subparser):
    subparser.usage = usage
    mutualex_group = subparser.add_mutually_exclusive_group()
    mutualex_group.add_argument(
        "--add", action='store_true', help='Fetch the remote source code and add it to osp.json')
    mutualex_group.add_argument(
        '-s', '--set', help="Set a global EMBARC_OSP_ROOT, make sure you have added it to osp.json")
    mutualex_group.add_argument(
        "--rename", action='store_true', help="Rename osp source code.")
    mutualex_group.add_argument(
        '-rm', '--remove', help="Remove the specified osp source code.")
    subparser.add_argument(
        '-l', '--list', action='store_true', help="Show all recored embARC OSP source code")
