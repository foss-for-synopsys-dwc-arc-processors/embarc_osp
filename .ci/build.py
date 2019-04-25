#! /usr/bin/env python
from __future__ import print_function, unicode_literals
import json
import os
import sys
import requests
from prettytable import PrettyTable
from colorama import Fore, Back, Style
from configparser import ConfigParser
import copy
import argparse
from xlsxwriter.workbook import Workbook
from xlsxwriter.worksheet import Worksheet
from xlsxwriter.worksheet import convert_cell_args
from embarc_tools.utils import cd, delete_dir_files
from embarc_tools.osp import osp
from embarc_tools.builder import build
import collections
import operator
example = {
    "arc_feature_cache": "example/baremetal/arc_feature/cache",
    "arc_feature_timer_interrupt": "example/baremetal/arc_feature/timer_interrupt",
    "arc_feature_udma": "example/baremetal/arc_feature/udma",
    "ble_hm1x": "example/baremetal/ble_hm1x",
    "blinky": "example/baremetal/blinky",
    "cxx": "example/baremetal/cxx",
    "graphic_u8glib": "example/baremetal/graphic_u8glib",
    "kernel": "example/freertos/kernel"
}


MakefileNames = ['Makefile', 'makefile', 'GNUMakefile']


def excel_string_width(str):
    string_width = len(str)
    if string_width == 0:
        return 0
    else:
        return string_width * 1.1


class MyWorksheet(Worksheet):

    @convert_cell_args
    def write_string(self, row, col, string, cell_format=None):
        if self._check_dimensions(row, col):
            return -1
        min_width = 0
        string_width = excel_string_width(string)
        if string_width > min_width:
            max_width = self.max_column_widths.get(col, min_width)
            if string_width > max_width:
                self.max_column_widths[col] = string_width
        return super(MyWorksheet, self).write_string(row, col, string, cell_format)


class MyWorkbook(Workbook):

    def add_worksheet(self, name=None):
        worksheet = super(MyWorkbook, self).add_worksheet(name, MyWorksheet)
        worksheet.max_column_widths = {}
        return worksheet

    def close(self):
        for worksheet in self.worksheets():
            for column, width in worksheet.max_column_widths.items():
                worksheet.set_column(column, column, width)

        return super(MyWorkbook, self).close()


def get_cache(osp_path):
    cache = ".cache/result"
    cache_path = os.path.join(osp_path, cache)
    if not os.path.exists(cache_path):
        os.makedirs(cache_path)
    return cache_path


def generate_json_file(data, path):
    with open(path, "w") as f:
        json.dump(data, f, indent=4)
        f.close()


def result_json_artifacts(osp_path, result, file=None):

    if file:
        job = file
    else:
        job = os.environ.get("CI_JOB_NAME")
        if not job:
            job = os.environ.get("NAME")
        if not job:
            job = os.environ.get("STAGE_NAME")
    if job:

        print(job)
        cache_path = get_cache(osp_path)
        file = job + ".json"
        file_path = os.path.join(cache_path, file)
        generate_json_file(result, file_path)


def takeApp(elem):
    return elem["app_path"]


def dict_to_excel(datas):

    print("Start to generate excel")
    failed_summarize = dict()
    for toolchain_config, results in datas.items():
        results_board_config = dict()
        filename = toolchain_config + ".xlsx"
        workbook = MyWorkbook(filename)
        sheet_dict = dict()
        merge_format = workbook.add_format({'bold': True, "align": "center", "valign": "vcenter"})

        for build_result in results:
            config = build_result.get("config", None)
            if config:
                board_config = config["BOARD"] + "_" + config["BD_VER"]
                if not results_board_config.get(board_config, None):
                    results_board_config[board_config] = [build_result]
                else:
                    results_board_config[board_config].append(build_result)
        for board_config in results_board_config:
            worksheet = workbook.add_worksheet(board_config)
            sheet_dict[board_config] = worksheet
        for board_config, build_result in results_board_config.items():
            build_result.sort(key=takeApp)
            worksheet = sheet_dict[board_config]
            worksheet.merge_range(0, 0, 1, 0, "APP", merge_format)
            worksheet.set_column(0, 7, 10)
            worksheet.merge_range(0, 1, 1, 1, "CORE", merge_format)
            worksheet.merge_range(0, 2, 1, 2, "TIME (s)", merge_format)
            worksheet.merge_range(0, 3, 1, 3, "GIT COMMIT SHA", merge_format)
            worksheet.merge_range(0, 4, 0, 6, "SIZE (bytes)", merge_format)
            worksheet.write(1, 4, "text", merge_format)
            worksheet.write(1, 5, "data", merge_format)
            worksheet.write(1, 6, "bss", merge_format)
            worksheet.merge_range(0, 7, 1, 7, "CI JOB", merge_format)
            worksheet.merge_range(0, 8, 1, 8, "PASS", merge_format)
            worksheet.freeze_panes(1, 0)
            worksheet.freeze_panes(2, 0)
            row = 2

            same_app = dict()
            failed_count = dict()
            same_app = collections.OrderedDict()
            for app_build in build_result:
                app_name = app_build["app_path"]
                if same_app.get(app_name, None):
                    same_app[app_name] = same_app[app_name] + 1

                else:
                    same_app[app_name] = 1
                    failed_count[app_name] = 0

                cell_format = workbook.add_format({"align": "center", "valign": "vcenter"})
                failed = "No"
                if not app_build["code"]:
                    failed = "Yes"
                else:
                    cell_format.set_bg_color("#FFC7CE")
                    failed_count[app_name] += 1
                worksheet.write(row, 0, app_build["app_path"])
                worksheet.write(row, 1, app_build["config"]["CUR_CORE"], cell_format)
                worksheet.write(row, 2, round(app_build["time_cost"], 2), cell_format)
                worksheet.write(row, 3, app_build["commit_sha"][0:8], cell_format)
                worksheet.write(row, 4, str(app_build["size"].get("text", " ")), cell_format)
                worksheet.write(row, 5, str(app_build["size"].get("data", " ")), cell_format)
                worksheet.write(row, 6, str(app_build["size"].get("bss", " ")), cell_format)
                worksheet.write(row, 7, app_build["JOB"], cell_format)
                worksheet.write(row, 8, failed, cell_format)
                row += 1
            s = 2
            for app, value in same_app.items():
                merge_format_app = workbook.add_format({"align": "center", "valign": "vcenter"})
                if value == failed_count[app]:
                    if not failed_summarize.get(toolchain_config, None):
                        failed_summarize[toolchain_config] = dict()
                    if not failed_summarize[toolchain_config].get(board_config, None):
                        failed_summarize[toolchain_config][board_config] = list()
                    failed_summarize[toolchain_config][board_config].append(app)
                    merge_format_app.set_font_color("red")
                if value > 1:
                    worksheet.merge_range(s, 0, s + value - 1, 0, app, merge_format_app)
                s += value
        workbook.close()
    return failed_summarize


def result_excel_artifacts(osp_path):
    cache_path = get_cache(osp_path)
    files = os.listdir(cache_path)
    datas = dict()
    failed_summarize = None
    for file in files:
        filename, filesuffix = os.path.splitext(file)
        if not filesuffix == ".json" or filename == "results":
            continue
        file_path = os.path.join(cache_path, file)
        with open(file_path, "r") as f:
            results = json.load(f)
            for app, build_status in results.items():
                for build_result in build_status:
                    config = build_result.get("config", None)
                    if config:
                        toolchain_config = config["TOOLCHAIN"] + "_" + config["TOOLCHAIN_VER"]
                        if not datas.get(toolchain_config, None):
                            datas[toolchain_config] = [build_result]
                        else:
                            datas[toolchain_config].append(build_result)
                    else:
                        continue
            f.close()

    result_json_artifacts(osp_path, datas, file="results")
    with cd(cache_path):
        failed_summarize = dict_to_excel(datas)
    result_json_artifacts(osp_path, failed_summarize, file="failed_results")


class TailRecurseException(Exception):
    def __init__(self, args, kwargs):
        self.args = args
        self.kwargs = kwargs


def tail_call_optimized(g):
    def func(*args, **kwargs):
        f = sys._getframe()
        if f.f_back and f.f_back.f_back and f.f_back.f_back.f_code == f.f_code:
            raise TailRecurseException(args, kwargs)
        else:
            while 1:
                try:
                    return g(*args, **kwargs)
                except TailRecurseException as e:
                    args = e.args
                    kwargs = e.kwargs
    func.__doc__ = g.__doc__
    return func


def parse_config(config):
    make_configs = config
    core_key = "CUR_CORE"
    osp_root = None
    board_input = None
    bd_ver_input = None
    cur_core_input = None
    toolchain_ver = "2017.09"
    bd_vers = dict()
    cur_cores = dict()
    toolchain = "gnu"
    expected_file = None
    parallel = ""
    coverity = False
    json_path = os.path.join(os.path.expanduser("~"), '.embarc_cli')
    delete_dir_files(json_path, dir=True)
    embarc_osp = osp.OSP()

    core_key = "CORE" if "CORE" in make_configs else "CUR_CORE"
    if "PARALLEL" in make_configs and make_configs["PARALLEL"] is not None:
        parallel = make_configs["PARALLEL"]
    if "EXPECTED" in make_configs and make_configs["EXPECTED"] is not None:
        expected_file = make_configs["EXPECTED"]
    if "COVERITY" in make_configs and make_configs["COVERITY"] is not None:
        if make_configs["COVERITY"] == "true":
            coverity = True
    if "TOOLCHAIN_VER" in make_configs and make_configs["TOOLCHAIN_VER"] is not None:
        toolchain_ver = make_configs["TOOLCHAIN_VER"]

    if "TOOLCHAIN" in make_configs and make_configs["TOOLCHAIN"] is not None:
        toolchain = make_configs["TOOLCHAIN"]
    if "OSP_ROOT" in make_configs and make_configs["OSP_ROOT"] is not None:
        osp_root = make_configs["OSP_ROOT"]
    if "BOARD" in make_configs and make_configs["BOARD"] is not None:
        board_input = make_configs["BOARD"]
    support_boards = embarc_osp.supported_boards(osp_root)

    boards = [board_input] if board_input in support_boards else support_boards
    if "BD_VER" in make_configs and make_configs["BD_VER"] is not None:
        bd_ver_input = make_configs["BD_VER"]
    for board in boards:
        version = embarc_osp.supported_bd_versions(osp_root, board, bd_version=bd_ver_input)
        bd_vers[board] = version
    if core_key in make_configs and make_configs[core_key] is not None:
        cur_core_input = make_configs[core_key]
    for (board, versions) in bd_vers.items():
        cur_cores[board] = dict()
        for version in versions:
            cores = embarc_osp.supported_cores(osp_root, board, version, cur_core=cur_core_input)
            cur_cores[board][version] = cores

    current_configs = dict()
    current_configs["make_options"] = cur_cores
    current_configs["core_key"] = core_key
    current_configs["parallel"] = parallel
    current_configs["osp_root"] = osp_root
    current_configs["toolchain"] = {"name": toolchain, "version": toolchain_ver}
    current_configs["EXPECTED"] = expected_file
    current_configs["COVERITY"] = coverity
    return current_configs


def is_embarc_makefile(makefile_path):
    with open(makefile_path) as f:
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


def get_config(config):
    make_configs = dict()
    if type(config) == list:
        pass
    else:
        config = config.split(" ")
    for make_config in config:
        [config_name, value] = make_config.split("=")
        if value == "":
            value = None
        make_configs[config_name] = value
    return make_configs


def get_expected_result(expected_file, app_path, board, bd_ver):
    result = False
    filesuffix = os.path.splitext(expected_file)[1]
    if filesuffix == ".json":
        with open(expected_file, "r") as f:
            expected_result = json.load(f)
            if app_path in expected_result:
                if board in expected_result[app_path]:
                    if bd_ver not in expected_result[app_path][board]:
                        result = True

    elif filesuffix == ".ini":
        conf = ConfigParser()
        conf.read(expected_file)
        sections = conf.sections()
        if app_path in sections:
            options = conf.options(app_path)
            if board in options:
                items = conf.get(app_path, board)
                if bd_ver not in items:
                    result = True
    return result


def send_pull_request_comment(columns, results):
    job = os.environ.get("NAME")
    pr_number = os.environ.get("TRAVIS_PULL_REQUEST")
    if all([job, pr_number]):
        comment_job = "## " + job + "\n"
        if len(results) > 0:
            head = "|".join(columns) + "\n"
            table_format = "|".join(["---"]*len(columns)) + "\n"
            table_head = head + table_format
            comments = ""
            comment = ""
            for result in results:
                for k in result:
                    comment += (k.replace(Fore.RED, "")).replace("\n", "<br>") + " |"
                comment = comment.rstrip("|") + "\n"
                comments += comment
            comment_on_pull_request(comment_job + table_head + comments)

    else:
        print("WARNING:Only send pull request comment in travis ci!")
    pass


def show_results(results, expected=None):
    columns = ['TOOLCHAIN', "TOOLCHAIN_VER", 'APP', 'CONF', 'PASS']
    failed_pt = PrettyTable(columns)
    failed_results = []
    success_results = []
    expected_results = None
    success_pt = PrettyTable(columns)
    expected_pt = PrettyTable(columns)

    for result in results:
        status = result.pop("status")
        if status != 0:
            result["PASS"] = "NO"
            t = result["conf"]
            t.replace("\n", "\n" + Fore.RED)
            t = Fore.RED + t
            result["conf"] = t
            failed_results.append([v for (k, v) in result.items()])

        else:
            result["PASS"] = "YES"
            success_results.append([v for (k, v) in result.items()])
    if expected is not None:
        expected_results = failed_results
        send_pull_request_comment(columns, expected_results)
        for result in expected_results:
            if len(result) > 0:
                expected_pt.add_row(result)
        if len(expected_results) > 0:
            print("these applications failed with some bd_ver: ")
            print(expected_pt)
            sys.stdout.flush()
            return

    else:
        for result in success_results:
            if len(result) > 0:
                success_pt.add_row(result)
        print(Fore.GREEN + "Successfull results")
        print(success_pt)
        print(Style.RESET_ALL)
        sys.stdout.flush()
        for result in failed_results:
            if len(result) > 0:
                list_key = range(len(result))
                for i in list_key:
                    result[i] = Fore.RED + result[i]
                failed_pt.add_row(result)

        print(Fore.RED + "Failed result:")
        print(failed_pt)
        print(Style.RESET_ALL)
        sys.stdout.flush()


@tail_call_optimized
def build_result_combine(results=None, formal_result=None):

    first_result = results[0]
    t = first_result.pop("conf")  # first_result = results[0]
    other_results = []
    if formal_result is None:
        formal_result = []
    for result in results[1:]:
        conf = result.pop("conf")
        if operator.eq(first_result, result):
            t = t + "\n" + conf
        else:
            result["conf"] = conf
            other_results.append(result)
        first_result = results[0]
    results[0]["conf"] = t
    formal_result.append(results[0])

    if len(other_results) > 0:
        return build_result_combine(other_results, formal_result)
    else:
        return formal_result


def get_apps(path):
    result = []
    for root, dirs, files in os.walk(path):
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        for f in files:
            if f in MakefileNames:
                result.append(root)
    return result


def get_exampes_from_input(paths):

    results = []
    paths_list = paths.split(",")
    for path in paths_list:
        apps = get_apps(path)
        results.extend(apps)
    sorted(set(results), key=results.index)
    return results


def build_result_combine_tail(results):
    results_list = []
    for (app_path, result) in results.items():
        formal_results = build_result_combine(result)
        results_list.extend(formal_results)
    return results_list


def get_applications(config):
    app_paths = None
    if "EXAMPLES" in config and config["EXAMPLES"]:
        examples_path = config.pop("EXAMPLES")
        app_paths_list = get_exampes_from_input(examples_path)
        key_list = range(len(app_paths_list))
        app_paths = dict(zip(key_list, app_paths_list))
    else:
        app_paths = example
    return app_paths


def startBuild(app, config, builder):

    for opt in build.BUILD_OPTION_NAMES:
        if opt in config:
            builder.buildopts[opt] = config[opt]
    CurrentBuildConfigString = ["%s=%s" % (key, value) for (key, value) in builder.buildopts.items()]

    build_conf = str()
    build_status = None
    builder.make_options = " "

    # builder.build_target(app, target=str("clean"), parallel=config["PARALLEL"])
    if os.environ.get("COVERITY", None) == "true":
        build_status = builder.build_target(app, target=str("all"), parallel=config["PARALLEL"], coverity=True, silent=True)
    else:
        build_status = builder.get_build_size(app, parallel=config["PARALLEL"], silent=True)

    board = builder.buildopts.get("BOARD")
    bd_ver = builder.buildopts.get("BD_VER")
    core = builder.buildopts.get("CUR_CORE")
    build_conf = board + "_" + bd_ver + "_" + core

    build_status["commit_sha"] = os.environ.get("CI_COMMIT_SHA") or os.environ.get("TRAVIS_COMMIT")
    build_status["JOB"] = os.environ.get("CI_JOB_NAME") or os.environ.get("NAME")
    build_status["status"] = 0 if build_status["result"] else 1
    build_status["conf"] = build_conf
    return build_status


def BuildApp(app, config):

    BuildOptions = config["make_options"]
    core_key = config["core_key"]
    toolchain = config["toolchain"]
    parallel = config["parallel"]
    osp_root = config["osp_root"]
    app_build_status = dict()
    app_build_results = list()
    CurrentBuildConfig = dict()
    expected_different = dict()
    expected_different[app] = []
    builder = build.embARC_Builder(osproot=osp_root)

    if os.environ.get("COVERITY", None) == "true":
        build_status = builder.configCoverity(toolchain["name"])
        if not build_status["result"]:
            print("Config Coverity failed")
    for board in BuildOptions:
        for bd_ver in BuildOptions[board]:
            core_num = len(BuildOptions[board][bd_ver])
            failed_count = 0
            may_compare = []
            for cur_core in BuildOptions[board][bd_ver]:
                # CurrentBuildConfig["OSP_ROOT"] = osp_root
                CurrentBuildConfig["BOARD"] = board
                CurrentBuildConfig["BD_VER"] = bd_ver
                CurrentBuildConfig[core_key] = cur_core
                CurrentBuildConfig["TOOLCHAIN"] = toolchain["name"]
                CurrentBuildConfig["TOOLCHAIN_VER"] = toolchain["version"]
                CurrentBuildConfig["PARALLEL"] = None if parallel == "" else parallel

                build_status = startBuild(app, CurrentBuildConfig, builder)
                if not app_build_status.get(app, None):
                    app_build_status[app] = [build_status]

                else:
                    app_build_status[app].append(build_status)
                if build_status["status"] != 0:
                    failed_count += 1
                current_result = dict()
                current_result = collections.OrderedDict()
                current_result["toolchain"] = toolchain["name"]
                current_result["toolchain_ver"] = toolchain["version"]
                current_result["app"] = app
                current_result["conf"] = build_status["conf"]
                current_result["status"] = build_status["status"]
                app_build_results.append(current_result)
                may_compare.append(current_result)

                if failed_count == core_num:
                    expected_file = config.get("EXPECTED", None)
                    if expected_file is not None:
                        expected_status = get_expected_result(expected_file, app, board, bd_ver)
                        if not expected_status:
                            print("This application compile failed, but the expected result is pass")
                            expected_different[app].extend(may_compare)

                    else:
                        print("now not have expected file")
                        expected_different[app].extend(may_compare)

    return app_build_results, expected_different, app_build_status


def comment_on_pull_request(comment):

    pr_number = os.environ.get("TRAVIS_PULL_REQUEST")
    slug = os.environ.get("TRAVIS_REPO_SLUG")
    token = os.environ.get("GH_TOKEN")
    request_config = [pr_number, slug, token, comment]
    for i in range(len(request_config)):
        if request_config[i] == "false":
            request_config[i] = False
    if all(request_config):
        url = 'https://api.github.com/repos/{slug}/issues/{number}/comments'.format(
            slug=slug, number=pr_number)
        response = requests.post(url, data=json.dumps({'body': comment}),
            headers={'Authorization': 'token ' + token})
        print(">>>>Travis send pull request comment to {}, repsonse status code {}.".format(url, response.status_code))
        return response.json()


def get_options_parser():
    configs = dict()
    toolchainlist = ["gnu", "mw"]
    boardlist = ["emsk", "nsim", "axs", "hsdk"]
    parser = argparse.ArgumentParser()
    parser.add_argument("--osp_root", dest="osp_root", default=".", help=("the path of embarc_osp"), metavar="OSP_ROOT")
    parser.add_argument("--toolchain", dest="toolchain", default=None, help=("build using the given TOOLCHAIN (%s)" % ', '.join(toolchainlist)), metavar="TOOLCHAIN")
    parser.add_argument("--board", dest="board", default=None, help=("build using the given BOARD (%s)" % ', '.join(boardlist)), metavar="BOARD")
    parser.add_argument("--bd_ver", dest="bd_ver", default=None, help=("build using the given BOARD VERSION"), metavar="BOARD VERSION")
    parser.add_argument("--core", dest="cur_core", default=None, help=("build using the given core"), metavar="CUR_CORE")
    parser.add_argument("--toolchain_ver", dest="toolchain_ver", default=None, help=("build using the given toolchian verion"), metavar="TOOLCHAIN_VER")
    parser.add_argument("--examples", dest="examples", default=None, help=("the path of applications that will be built"), metavar="EXAMPLES")
    parser.add_argument("--expected", dest="expected", default=None, help=("the path of the expected file that include the results"), metavar="EXPECTED")
    parser.add_argument("-j", "--parallel", dest="parallel", type=int, help=("Compile the application in parallel"), metavar="NUMBER")
    options = parser.parse_args()
    if options.osp_root:
        configs["OSP_ROOT"] = options.osp_root
    if options.toolchain:
        configs["TOOLCHAIN"] = options.toolchain
    if options.board:
        configs["BOARD"] = options.board
    if options.bd_ver:
        configs["BD_VER"] = options.bd_ver
    if options.cur_core:
        configs["CORE"] = options.cur_core
    if options.toolchain_ver:
        configs["TOOLCHAIN_VER"] = options.toolchain_ver
    if options.examples:
        configs["EXAMPLES"] = options.examples
    if options.expected:
        configs["EXPECTED"] = options.expected
    if options.parallel:
        configs["parallel"] = options.parallel
    return configs


def main(config):

    current_input_config = copy.deepcopy(config)
    examples = get_applications(config)
    BuildConfigs = parse_config(current_input_config)

    if os.environ.get("CI_JOB_NAME") == "deploy_prod" or os.environ.get("STAGE_NAME") == "Deploy":
        print("Build finish and generate excel")
        result_excel_artifacts(BuildConfigs.get("OSP_ROOT", "."))
        sys.exit(0)

    apps_expected_different = dict()
    all_results = dict()
    apps_build_status = dict()
    build_count = 0
    for (app, app_path) in examples.items():
        app_build_results, expected_different, app_build_status = BuildApp(app_path, BuildConfigs)
        if all([i["status"] for i in app_build_results]):
            print(Back.RED + "{} failed with all configurations".format(app_path))
            print(Style.RESET_ALL)
        build_count = build_count + len(app_build_results)
        all_results[app_path] = app_build_results
        apps_build_status.update(app_build_status)

        if expected_different.get(app_path, False) and len(expected_different[app_path]) > 0:
                    apps_expected_different[app_path] = copy.deepcopy(expected_different[app_path])
    print("There are {} projects, and they are compiled for {} times".format(len(examples), build_count))
    all_results_list = copy.deepcopy(build_result_combine_tail(all_results))
    show_results(all_results_list)
    expected_differents_list = build_result_combine_tail(apps_expected_different)
    show_results(expected_differents_list, expected=True)
    print("Generate JSON for every job")
    result_json_artifacts(BuildConfigs["osp_root"], apps_build_status)

    if not len(apps_expected_different) > 0:
        print("All the applications build as expected")
    else:
        print("these applications failed with some configuration: ")
        print(apps_expected_different.keys())
        comment = "applications failed with some configuration: \n" + "\n".join(apps_expected_different.keys())
        comment_on_pull_request(comment)
        sys.exit(1)


if __name__ == '__main__':

    cwd_path = os.getcwd()
    osp_path = os.path.dirname(cwd_path)
    make_config = get_config(sys.argv[1:])
    with cd(osp_path):
        main(make_config)
    print("The end")
