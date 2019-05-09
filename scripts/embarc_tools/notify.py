from __future__ import print_function, unicode_literals
import re
import sys
from prettytable import PrettyTable
from colorama import init, Fore, Back, Style

COLOR = False
CLI_COLOR_MAP = {
    "info": "white",
    "warning": "yellow",
    "error": "red",
    "success": "green"
}

COLOR_MATCHER = re.compile(r"(\w+)(\W+on\W+\w+)?")
init()
COLORS = {
    'none': "",
    'default': Style.RESET_ALL,
    'black': Fore.BLACK,
    'red': Fore.RED,
    'green': Fore.GREEN,
    'yellow': Fore.YELLOW,
    'blue': Fore.BLUE,
    'magenta': Fore.MAGENTA,
    'cyan': Fore.CYAN,
    'white': Fore.WHITE,

    'on_black': Back.BLACK,
    'on_red': Back.RED,
    'on_green': Back.GREEN,
    'on_yellow': Back.YELLOW,
    'on_blue': Back.BLUE,
    'on_magenta': Back.MAGENTA,
    'on_cyan': Back.CYAN,
    'on_white': Back.WHITE,
}


def colorstring_to_escapecode(color_string):
    match = re.match(COLOR_MATCHER, color_string)
    if match:
        return COLORS[match.group(1)] + \
            (COLORS[match.group(2).strip().replace(" ", "_")]
             if match.group(2) else "")
    else:
        return COLORS['default']


def print_string(msg, level=None):
    if not level:
        level = "info"
    string_color = CLI_COLOR_MAP.get(level)
    if string_color != "white":
        sys.stdout.write(colorstring_to_escapecode(string_color))
        sys.stdout.flush()
    sys.stdout.write("[embARC] {} \n".format(msg))
    if string_color != "white":
        sys.stdout.write(colorstring_to_escapecode('default'))
    sys.stdout.flush()


def print_table(msg, level=None):
    if not level:
        level = "info"
    if isinstance(msg, list):
        if len(msg) > 1:
            table_head = msg[0]
            table_content = msg[1]
            string_color = CLI_COLOR_MAP[level]
            sys.stdout.write(colorstring_to_escapecode(string_color))
            sys.stdout.flush()
            pretty_table = PrettyTable(table_head)
            pretty_table.align = "l"
            for content in table_content:
                if content:
                    pretty_table.add_row(content)
            print(pretty_table)
            sys.stdout.write(colorstring_to_escapecode('default'))
            sys.stdout.flush()
