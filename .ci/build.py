#! /usr/bin/env python
import json
import os
import sys
import shutil
from sys import stderr, stdout
from prettytable import PrettyTable
from colorama import init, Fore, Back, Style
from configparser import ConfigParser
import copy
import argparse
example = {"arc_feature_cache":"example/baremetal/arc_feature/cache",
		"arc_feature_timer_interrupt":"example/baremetal/arc_feature/timer_interrupt",
		"arc_feature_udma":"example/baremetal/arc_feature/udma",
		"ble_hm1x":"example/baremetal/ble_hm1x",
		"blinky":"example/baremetal/blinky",
		"cxx":"example/baremetal/cxx",
		"graphic_u8glib":"example/baremetal/graphic_u8glib",
		"kernel":"example/freertos/kernel"
		}
'''
"bootloader":"baremetal/bootloader",
"dma_spiflash":"baremetal/dma_spiflash",
"openthread_cli":"baremetal/openthread/cli",
"openthread_ncp":"baremetal/openthread/ncp",
"iot_coap_coap_server":"freertos/iot/coap/coap_server",
"iot_lwm2m_lwm2m_client":"freertos/iot/lwm2m/lwm2m_client",
"iot_lwm2m_lwm2m_server":"freertos/iot/lwm2m/lwm2m_server",
"kernel_secure":"freertos/kernel_secure",
"net_httpserver":"freertos/net/httpserver"
"net_ntshell":"freertos/net/ntshell",
"secureshield_secret_normal":"baremetal/secureshield/secret_normal",
"secureshield_secret_secure":"baremetal/secureshield/secret_secure",
"secureshield_secret_secure_sid":"baremetal/secureshield/secret_secure_sid",
"secureshield_test_case":"baremetal/secureshield/test_case",
"sec_mbedtls_dtls_client":"freertos/sec/mbedtls/dtls/client",
"sec_mbedtls_dtls_server":"freertos/sec/mbedtls/dtls/server",
"sec_mbedtls_ssl_client2":"freertos/sec/mbedtls/ssl/client2",
"sec_mbedtls_ssl_server2":"freertos/sec/mbedtls/ssl/server2"

'''

MakefileNames = ['Makefile', 'makefile', 'GNUMakefile']

class TailRecurseException:
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

def get_makefile(app_path):
	for makefile in MakefileNames:
		makefile_path = os.path.join(app_path, makefile)
		if os.path.exists(makefile_path) and os.path.isfile(makefile_path):
			return makefile_path
	return None

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

def get_config(config): # from input to get the config dict{"TOOLCHAIN":,"BOARD":,"BD_VER":,"CUR_CORE":}
	make_configs = dict()
	if type(config) == list:
		pass
	else:
		config = config.split(" ")
	for make_config in config:
		[config_name,value] = make_config.split("=")
		if value == "":
			value = None
		make_configs[config_name] = value
	return make_configs

def board_version_config(osp_root, board, bd_version=None):
	board_path = os.path.join(osp_root, "board", board)
	bd_vers = dict()
	if os.path.exists(board_path):
		files = os.listdir(board_path)
		if "configs" in files:
			versions = os.listdir(os.path.join(board_path, "configs"))
			for version in versions:
				version_path = os.path.join(board_path, "configs", version)
				if os.path.isdir(version_path):
					bd_vers[version] = version_path
		else:
			versions = os.listdir(board_path)
			for version in versions:
				path = os.path.join(board_path, version)
				if os.path.isdir(path) and "configs" in os.listdir(path):
					version_path = os.path.join(board_path, version, "configs")
					bd_vers[version] = version_path
	if bd_version is not None:
		if bd_vers.has_key(bd_version):
			bd_ver = {bd_version: bd_vers[bd_version]}
			return bd_ver
	return bd_vers

def get_tcf(osp_root, board, bd_version, cur_core):
	result = dict()
	tcf_name = cur_core + ".tcf"
	board_version_path_dict = copy.deepcopy(board_version_config(osp_root, board, bd_version))
	board_path = board_version_path_dict[bd_version]

	cur_core_file = cur_core + ".tcf"
	result[cur_core] = None
	if os.path.exists(board_path):
		for root, dirs, files in os.walk(board_path, topdown=True):
			if cur_core_file in files:
				result[tcf_name] = os.path.join(root, cur_core_file)
	return result


def get_tcfs(osp_root, board, bd_version, cur_core=None):
	result = []
	board_version_path_dict = board_version_config(osp_root, board, bd_version)
	board_path = board_version_path_dict [bd_version]
	
	if os.path.exists(board_path):
		if cur_core is not None:
			cur_core_file = cur_core + ".tcf"
			for root, dirs, files in os.walk(board_path, topdown=True):
				if cur_core_file in files:
					cur_core_path = os.path.join(root,cur_core_file)
					result.append(cur_core)
					
		else:
			for root, dirs, files in os.walk(board_path, topdown=True):
				for file in files:
					filename, filesuffix = os.path.splitext(file)
					if not filesuffix == ".tcf":
						continue
					result.append(filename)
	return result


def get_board_version(osp_root, board, bd_version=None):
	result = []
	board_path = "board/" + board
	ver_path = os.path.join(osp_root, board_path)
	if os.path.exists(ver_path):
		bd_vers_dict = board_version_config(osp_root, board, bd_version)
		result.extend(bd_vers_dict.keys())
	return result

def get_boards(osp_root, board=None):
	result = []
	board_path = os.path.join(osp_root, "board")
	if os.path.exists(board_path):
		if board is not None:
			if board in os.listdir(board_path):
				result.append(board)
				return result
		for file in os.listdir(board_path):
			if os.path.isdir(os.path.join(board_path, file)):
				result.append(file)
	return result

def build_makefile_project(app_path, config):
	result = dict()
	isMakeProject = False

	make_configs = config #get_config(config)
	print make_configs
	osp_root = make_configs["OSP_ROOT"]
	toolchain = make_configs["TOOLCHAIN"]
	board = make_configs["BOARD"]
	bd_ver = make_configs["BD_VER"]
	cur_core = make_configs["CUR_CORE"]
	toolchain_ver = make_configs["TOOLCHAIN_VER"]
	parallel = make_configs["PARALLEL"]
	conf_key = board + "_" + bd_ver + "_" + cur_core

	tcf_name = cur_core + ".tcf"
	tcf_found = get_tcf(osp_root, board, bd_ver, cur_core)
	print "tcf_path:%s" % (tcf_found)

	tcf_path = tcf_found[tcf_name]
	if tcf_path != None:
		makefile_found = get_makefile(app_path)
		
		if makefile_found != None:
			if is_embarc_makefile(makefile_found):
			    isMakeProject = True
			    # Record current folder
			    cur_dir = os.getcwd()
			    # Change to application makefile folder
			    os.chdir(app_path)
			    # Build application, clean it first
			    print Fore.GREEN + "Build Application {} with Configuration {} {}".format(app_path, conf_key, config)
			    print Style.RESET_ALL
			    sys.stdout.flush()
			    make_cmd = "make -j " + str(parallel) + " SILENT=1 " + " BOARD=" + board +" BD_VER=" + bd_ver + " CUR_CORE=" + cur_core +" TOOLCHAIN=" + toolchain
			    cleancommand = make_cmd + " clean"
			    os.system(cleancommand)
			    buildcommand = make_cmd
			    print Fore.GREEN + "{}".format(buildcommand)
			    result["status"] = os.system(buildcommand)
			    result["app"] = app_path
			    result["conf"] = conf_key
			    result["toolchain"] = toolchain
			    result["toolchain_ver"] = toolchain_ver
			    os.chdir(cur_dir)
			else:
				result["status"] = 1
				result["app"] = app_path
				result["conf"] = Fore.YELLOW + conf_key
				result["toolchain"] = toolchain
				result["toolchain_ver"] = toolchain_ver

	else:
		isMakeProject = False
	return isMakeProject, result

def build_project_configs(app_path, config):
	work_path = os.getcwd()
	make_configs = config
	osp_root = None
	board_input = None
	bd_ver_input = None
	cur_core_input = None
	toolchain_ver = "2017.09"
	bd_vers = dict()
	cur_cors = dict()
	make_config = dict()
	results = []
	toolchain = "gnu"
	build_count = 0
	status = True
	expected_file = None
	expected_different = dict()
	expected_different[app_path] = []
	parallel = ""

	if "PARALLEL" in make_configs and make_configs["PARALLEL"] is not None:
		parallel = make_configs["PARALLEL"]
	if "EXPECTED" in make_configs and make_configs["EXPECTED"] is not None:
		expected_file = make_configs["EXPECTED"]

	if "TOOLCHAIN_VER" in make_configs and make_configs["TOOLCHAIN_VER"] is not None:
		toolchain_ver = make_configs["TOOLCHAIN_VER"]

	if "TOOLCHAIN" in make_configs and make_configs["TOOLCHAIN"] is not None:
		toolchain = make_configs["TOOLCHAIN"]
	if "OSP_ROOT" in make_configs and make_configs["OSP_ROOT"] is not None:
		osp_root = make_configs["OSP_ROOT"]
	if "BOARD" in make_configs and make_configs["BOARD"] is not None:
		board_input = make_configs["BOARD"]
	boards = get_boards(osp_root, board_input)
	if "BD_VER" in make_configs and make_configs["BD_VER"] is not None: 
		bd_ver_input = make_configs["BD_VER"]
	for board in boards:
		version = get_board_version(osp_root, board, bd_version=bd_ver_input)
		bd_vers[board] = version
	if "CORE" in make_configs and make_configs["CORE"] is not None:
		cur_core_input = make_configs["CORE"]
	else:
		if "CUR_CORE" in make_configs and make_configs["CUR_CORE"] is not None:
			cur_core_input = make_configs["CUR_CORE"]
	for (board, versions) in bd_vers.items():
		cur_cors[board] = dict()
		for version in versions:
			cors = get_tcfs(osp_root, board, version, cur_core=cur_core_input)
			cur_cors[board][version] = cors

	for board in cur_cors:
		for bd_ver in cur_cors[board]:
			core_failed = 0
			core_num = len(cur_cors[board][bd_ver])
			may_compare = []
			for cur_core in cur_cors[board][bd_ver]:
				make_config["OSP_ROOT"] = osp_root
				make_config["BOARD"] = board
				make_config["BD_VER"] = bd_ver
				make_config["CUR_CORE"] = cur_core
				make_config["TOOLCHAIN"] = toolchain
				make_config["TOOLCHAIN_VER"] = toolchain_ver
				make_config["PARALLEL"] = parallel
				isMakefileProject, result = build_makefile_project(app_path, make_config)
				if isMakefileProject is False:
					print "Application {} doesn't have makefile".format(app_path)
				else:
					build_count += 1
					if result["status"] != 0:
						status = False
						core_failed += 1
					results.append(result)
					may_compare.append(result)

			if core_failed == core_num:
				if expected_file is not None:
					expected_status = get_expected_result(expected_file, app_path, board, bd_ver)
					if not expected_status:
						print "This application compile failed, but the expected result is pass"
						expected_different[app_path].extend(may_compare)
				else:
					print "now not have expected file"
					expected_different[app_path].extend(may_compare)

						

	return status, results, build_count, expected_different

def get_expected_result(expected_file, app_path, board, bd_ver):
	result = False
	filesuffix = os.path.splitext(expected_file)[1]
	if filesuffix == ".json":
		with open(expected_file, "r") as f:
			expected_result = json.load(f)
			if app_path in expected_result:
				if board in expected_result[app_path]:
					if bd_ver in expected_result[app_path][board]:
						result = False
					else:
						result = True
				else:
					result = False
			else:
				result = False
			
	elif filesuffix == ".ini":
		conf = ConfigParser()
		conf.read(expected_file)
		sections = conf.sections()
		if app_path in sections:
			options = conf.options(app_path)
			if board in options:
				items = conf.get(app_path, board)
				if bd_ver in items:
					result = False
				else:
					result = True
			else:
				result = False
		else:
			result = False
	return result

def show_results(results, expected=None):
	columns = ['TOOLCHAIN', 'APP', "TOOLCHAIN_VER", 'CONF', 'PASS']
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
		for result in expected_results:
			if len(result) > 0:
				expected_pt.add_row(result)
		if len(expected_results) > 0:
			print "these applications failed with some bd_ver: "
			print expected_pt
			sys.stdout.flush()
			return
	else:

		for result in success_results:
			if len(result) > 0:
				success_pt.add_row(result)
		print Fore.GREEN + "Successfull results"
		print success_pt
		print Style.RESET_ALL
		sys.stdout.flush()

		for result in failed_results:
			if len(result) > 0:
				list_key = range(len(result))
				for i in list_key:

					result[i] = Fore.RED + result[i]
					
				failed_pt.add_row(result)
				

		print Fore.RED + "Failed result:"
		print failed_pt
		print Style.RESET_ALL
		sys.stdout.flush()

@tail_call_optimized
def build_result_combine(results=None, formal_result=None):
	first_result = results[0]
	t = first_result.pop("conf")#first_result = results[0]
	other_results = []
	if formal_result is None:
		formal_result = []
	
	for result in results[1:]:
		conf = result.pop("conf")
		if cmp(first_result, result) == 0:
			t = t + "\n"  + conf
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
		dirs[:] = [d for d in dirs  if not d.startswith('.')]
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

def application_all_failed(results):
	count = len(results)
	i = 0
	for result in results:
		if result["status"] != 0:
			i += 1
	if i == count:
		return 1
	else:
		return 0

def build_result_combine_tail(results):
	results_list = []
	for (app_path, result) in results.items():
		formal_results = build_result_combine(result)
		results_list.extend(formal_results)
	return results_list


def build_makefiles_project(config):
	apps_results = {}
	apps_status = []
	count = 0
	app_count = 0
	results_list = []
	applications_failed =[]
	toolchain_ver = "2017.09"
	work_path = os.getcwd()
	update_json = None
	diff_expected_differents = dict()
	expected_differents_list = []
	if "EXAMPLES" in config and config["EXAMPLES"]:
		examples_path = config.pop("EXAMPLES")
		app_paths_list = get_exampes_from_input(examples_path)
		key_list = range(len(app_paths_list))
		app_paths = dict(zip(key_list, app_paths_list))
	else:
		app_paths = example
	if "TOOLCHAIN_VER" in config and config["TOOLCHAIN_VER"] is not None:
		toolchain_ver = config["TOOLCHAIN_VER"]
	if "UPDATE_JSON" in config and config["UPDATE_JSON"] is not None:
		update_json = config["UPDATE_JSON"]

	for (app, app_path) in app_paths.items():
		
		status, results, build_count, expected_different = build_project_configs(app_path, config)
		application_failed = application_all_failed(results)
		if application_failed == 1:
			print Back.RED + "{} failed with all configurations".format(app_path)
			print Style.RESET_ALL
		applications_failed.append(application_failed)
		apps_results[app_path] = results
		apps_status.append(status)
		count += build_count
		app_count += 1
		if app_path in expected_different and len(expected_different[app_path]) > 0:
			diff_expected_differents[app_path] = copy.deepcopy(expected_different[app_path])



	print "There are {} projects, and they are compiled for {} times".format(app_count, count)
	results_list = build_result_combine_tail(apps_results)
	show_results(results_list)
	expected_differents_list = build_result_combine_tail(diff_expected_differents)
	show_results(expected_differents_list, expected=True)

	return applications_failed, diff_expected_differents

def get_options_parser():
	configs = dict()
	toolchainlist = ["gnu", "mw"]
	boardlist = ["emsk", "nsim", "axs", "hsdk"]
	parser = argparse.ArgumentParser()
	parser.add_argument("--osp_root", dest="osp_root",default=".", help=("the path of embarc_osp"), metavar="OSP_ROOT")
	parser.add_argument("--toolchain", dest="toolchain", default=None, help=("build using the given TOOLCHAIN (%s)" %', '.join(toolchainlist)), metavar="TOOLCHAIN")
	parser.add_argument("--board", dest="board", default=None, help=("build using the given BOARD (%s)" %', '.join(boardlist)),metavar="BOARD")
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

if __name__ == '__main__':

	cwd_path = os.getcwd()
	osp_path = os.path.dirname(cwd_path)
	make_config = get_config(sys.argv[1:]) #get_options_parser() #
	sys.stdout.flush()
	os.chdir(osp_path)
	applications_failed, expected_differents = build_makefiles_project(make_config)
	os.chdir(cwd_path)
	if "embarc_applications" in os.listdir(os.getcwd()):
		os.chdir(os.path.dirname(cwd_path))

	if  not len(expected_differents) > 0:
		print "All the applications build as expected"
	else:
		print "these applications failed with some configuration: "
		print expected_differents.keys()
		sys.exit(1)
