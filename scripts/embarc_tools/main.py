from __future__ import print_function, unicode_literals
import argparse
import sys
import pkgutil
import importlib
import pkg_resources
from embarc_tools import commands
from embarc_tools.commands import config_commands


def import_submodules(package, recursive=True):
    """ Import all submodules of a module, recursively, including subpackages

    :param package: package (name or actual module)
    :type package: str | module
    :rtype: dict[str, types.ModuleType]
    """
    results = {}
    for _, name, is_pkg in pkgutil.walk_packages(package.__path__):
        full_name = package.__name__ + '.' + name
        if not is_pkg:
            results[name] = importlib.import_module(full_name)
        if recursive and is_pkg:
            results.update(import_submodules(full_name))
    return results


SUBCOMMANDS = import_submodules(commands, recursive=False)
CONFIG_SUBCOMMANDS = import_submodules(config_commands)
ver = pkg_resources.require("embarc_cli")[0].version


def main():
    parser = argparse.ArgumentParser(
        prog='embarc',
        description="Command-line tool for embARC OSP - https://embarc.org/embarc_osp\nversion %s\n\nUse \"embarc <command> -h|--help\" for detailed help.\nOnline manual and guide available at https://github.com/foss-for-synopsys-dwc-arc-processors/embarc-cli" % ver,
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument(
        "--version", action='version',
        version=pkg_resources.require("embarc_cli")[0].version,
        help="Display version"
    )
    subparsers = parser.add_subparsers(help='commands')

    current_command = ["new", "build", "appconfig", "config"]
    subcommand = SUBCOMMANDS.keys()
    for key in subcommand:
        if key not in current_command:
            current_command.append(key)
    for key in current_command:
        subparser = subparsers.add_parser(key, help=SUBCOMMANDS[key].help, description=SUBCOMMANDS[key].description)
        if key == "config":
            config_subparsers = subparser.add_subparsers(help='commands')
            for name, module in CONFIG_SUBCOMMANDS.items():
                cfg_subparser = config_subparsers.add_parser(name, help=module.help)
                module.setup(cfg_subparser)
                cfg_subparser.set_defaults(func=module.run)

        SUBCOMMANDS[key].setup(subparser)
        subparser.set_defaults(func=SUBCOMMANDS[key].run)
    args = None
    if len(sys.argv) == 1:
        return parser.print_help()

    args, remainder = parser.parse_known_args()
    return args.func(args, remainder)


if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt):
        print("Terminate batch job")
        sys.exit(255)
