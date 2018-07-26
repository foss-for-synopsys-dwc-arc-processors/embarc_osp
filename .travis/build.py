#! /usr/bin/env python
import json
import os
import sys
import shutil
import zipfile
import tarfile
import urllib
from sys import stderr, stdout
from prettytable import PrettyTable
from colorama import init,Fore
example = {"arc_feature_cache":"baremetal/arc_feature/cache",
		"arc_feature_timer_interrupt":"baremetal/arc_feature/timer_interrupt",
		"arc_feature_udma":"baremetal/arc_feature/udma",
		"ble_hm1x":"baremetal/ble_hm1x",
		"blinky":"baremetal/blinky",
		"cxx":"baremetal/cxx",
		"graphic_u8glib":"baremetal/graphic_u8glib",
		"kernel":"freertos/kernel"
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
default_root = "."

def download_file(url, path):
    try:
    	urllib.urlretrieve(url, path)
    except Exception:
    	
    	print "This file from {} can't be download".format(url)
    	sys.exit(1)
    	
def download_gnu(version="2017.09", path=None):
	baseurl = "https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases/download/"
	url = baseurl + "arc-" + version + "-release/arc_gnu_"  + version+ "_prebuilt_elf32_le_linux_install.tar.gz"
	if path is not None:
		path = os.path.join(path , "arc_gnu_" + version +"_prebuilt_elf32_le_linux_install.tar.gz")
	else:
		path = os.path.join(os.getcwd(), "arc_gnu_" + version +"_prebuilt_elf32_le_linux_install.tar.gz")
	download_file(url, path)
	gnu = "arc_gnu_" + version + "_prebuilt_elf32_le_linux_install.tar.gz"
	return gnu

def unzip(file, path):
	try:
		pack = zipfile.ZipFile(file, "r")
		pack.extractall(path)
		pack.close()
	except Exception:
		sys.exit(1)

def untar(file, path):
	try:
		pack = tarfile.open(file, "r:gz")
		files = pack.getnames()
		for file in files:
			pack.extract(file, path)
		pack.close()
	except Exception:
		sys.exit(1)

def extract_file(file, path):
	filename, filesuffix = os.path.splitext(file)
	if filesuffix == ".gz":
		untar(file, path)
	elif filesuffix == ".zip":
		unzip(file, path)
	else:
		print "This file {} can't be extracted".format(file)


def add_gnu(version, path=None):
	os.chdir("/")
	os.chdir("tmp")
	gnu = download_gnu(version, path)
	extract_file(gnu, os.getcwd())
	gnu_bin_path = os.path.join(gnu.split(".")[0],"bin")
	# add_env_path(os.path.join(os.getcwd(), gnu_bin_path))
	os.system("arc-elf32-gcc --version")

def get_makefile(app_path):
	for makefile in MakefileNames:
		makefile_path = os.path.join(app_path, makefile)
		if os.path.exists(makefile_path) and os.path.isfile(makefile_path):
			return makefile_path
	return None

def get_config(config): # from input to get the config dict{"TOOLCHAIN":,"BOARD":,"BD_VER":,"CUR_CORE":}
	make_configs = dict()
	for make_config in config.split(" "):
		[config_name,value] = make_config.split("=")
		make_configs[config_name] = value
	return make_configs

def get_tcf(osp_root, board, bd_version, cur_core):
	result = dict()
	tcf_name = cur_core + ".tcf"
	board_path = "board/" + board + "/configs/" + bd_version + "/tcf/"
	tcf_path = os.path.join(osp_root, board_path, tcf_name)
	result[tcf_name] = None
	if os.path.exists(tcf_path) and os.path.isfile(tcf_path):
		result[tcf_name] = tcf_path
	return result

def get_tcfs(osp_root, board, bd_version, cur_core=None):
	result = []
	board_path = "board/" + board + "/configs/" + bd_version + "/tcf/"
	tcfs_path = os.path.join(osp_root, board_path)
	if os.path.exists(tcfs_path):
		if cur_core is not None:
			cur_core_file = cur_core + ".tcf"
			if cur_core_file in os.listdir(tcfs_path):
				result.append(cur_core)
				return result
		for file in os.listdir(tcfs_path):
			filename, filesuffix = os.path.splitext(file)
			if not filesuffix == ".tcf":
				continue
			result.append(filename)
	return result

def get_board_version(osp_root, board, bd_version=None):
	result = []
	board_path = "board/" + board + "/configs/"
	ver_path = os.path.join(osp_root, board_path)
	if os.path.exists(ver_path):
		if bd_version is not None:
			if bd_version in os.listdir(board_path):
				result.append(bd_version)
				return result
		for file in os.listdir(ver_path):
			if os.path.isdir(os.path.join(ver_path, file)):
				result.append(file)
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
	gnu_ver = make_configs["GNU_VER"]

	print os.getcwd()

	conf_key = board + "_" + bd_ver + "_" + cur_core

	tcf_name = cur_core + ".tcf"
	tcf_found = get_tcf(osp_root, board, bd_ver, cur_core)
	print "tcf_path:%s" % (tcf_found)

	tcf_path = tcf_found[tcf_name]
	if tcf_path != None:
		makefile_found = get_makefile(app_path)
		if makefile_found != None:
			isMakeProject = True
			# Record current folder
			cur_dir = os.getcwd()
			# Change to application makefile folder
			os.chdir(app_path)
			# Build application, clean it first
			print "Build Application {} with Configuration {}".format(app_path, config)
			sys.stdout.flush()

			os.system("make " + " clean")
			result["status"] = os.system("make" + " BOARD=" + board +" BD_VER=" + bd_ver + " CUR_CORE=" + cur_core +" TOOLCHAIN=" + toolchain)
			result["app"] = app_path
			result["conf"] = conf_key
			result["toolchain"] = toolchain
			result["gnu_ver"] = gnu_ver
			os.chdir(cur_dir)
	else:
		isMakeProject = False
	return isMakeProject, result

def build_project_configs(app_path, config):
	work_path = os.getcwd()
	make_configs = config
	osp_root = default_root
	board_input = None
	bd_ver_input = None
	cur_core_input = None
	gnu_ver = "2017.09"
	bd_vers = dict()
	cur_cors = dict()
	make_config = dict()
	results = []
	toolchain = "gnu"
	build_count = 0
	status = True
	if "GNU_VER" in make_configs:
		gnu_ver = make_configs["GNU_VER"]
	add_gnu(gnu_ver)
	os.chdir(work_path)
	if "TOOLCHAIN" in make_configs:
		toolchain = make_configs["TOOLCHAIN"]
	if "OSP_ROOT" in make_configs:
		osp_root = make_configs["OSP_ROOT"]
	if "BOARD" in make_configs:
		board_input = make_configs["BOARD"]
	boards = get_boards(osp_root, board_input)
	if "BD_VER" in make_configs: 
		bd_ver_input = make_configs["BD_VER"]
	for board in boards:
		version = get_board_version(osp_root, board, bd_version=bd_ver_input)
		bd_vers[board] = version
	if "CUR_CORE" in make_configs:
		cur_core_input = make_configs["CUR_CORE"]
	for (board, versions) in bd_vers.items():
		cur_cors[board] = dict()
		for version in versions:
			cors = get_tcfs(osp_root, board, version, cur_core=cur_core_input)
			cur_cors[board][version] = cors
	for board in cur_cors:
		for bd_ver in cur_cors[board]:
			for cur_core in cur_cors[board][bd_ver]:
				make_config["OSP_ROOT"] = osp_root
				make_config["BOARD"] = board
				make_config["BD_VER"] = bd_ver
				make_config["CUR_CORE"] = cur_core
				make_config["TOOLCHAIN"] = toolchain
				make_config["GNU_VER"] = gnu_ver
				isMakefileProject, result = build_makefile_project(app_path, make_config)
				if isMakefileProject is False:
					print "Application {} doesn't have makefile".format(app_path)
				else:
					build_count += 1
					if result["status"] != 0:
						status = False

					results.append(result)
	return status, results, build_count

def show_results(results):
	columns = ['TOOLCHAIN', 'APP', "GNU_VER", 'CONF', 'PASS']
	pt = PrettyTable(columns)
	failed_pt = PrettyTable(columns)
	failed_results = []
	for result in results:
		status = result.pop("status")
		if status != 0:
			result["PASS"] = "NO"
			failed_results.append([v for (k, v) in result.items()])

		else:
			result["PASS"] = "YES"

		result_list = [v for (k, v) in result.items()]

		
		pt.add_row(result_list)
	print "ALL results:"
	print pt

	init(autoreset=True)
	for result in failed_results:
		if len(result) > 0:
			i = 0
			if i < len(result):

				result[i] = Fore.RED + result[i]
				i += 1
			failed_pt.add_row(result)

	print "Failed result:"
	print failed_pt

def build_result_combine(results,formal_result=None):
	first_result = results[0]
	t = first_result.pop("conf")
	first_result = results[0]
	other_results = []
	if formal_result is None:
		formal_result = []
	
	for result in results[1:]:
		conf = result.pop("conf")
		if cmp(first_result, result) == 0:
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

def build_makefiles_project(app_paths, config):
	apps_results = {}
	apps_status = []
	count = 0
	app_count = 0
	results_list = []
	for (app, app_path) in app_paths.items():
		app_path = os.path.join("example", app_path)
		status, results, build_count = build_project_configs(app_path, config)
		apps_results[app_path] = results
		apps_status.append(status)
		count += build_count
		app_count += 1
	print "There are {} projects, and they are compiled for {} times".format(app_count, count)
	for (app_path, result) in apps_results.items():
		print "{} : ".format(app_path)
		formal_result = build_result_combine(result)
		results_list.extend(formal_result)

	show_results(results_list)
	sys.stdout.flush()
	return apps_status

if __name__ == '__main__':

	cwd_path = os.getcwd()
	osp_path = os.path.dirname(cwd_path)
	make_config = get_config(sys.argv[1])
	print make_config
	os.chdir(osp_path)
	apps_status = build_makefiles_project(example, make_config)
	os.chdir(cwd_path)
	for status in apps_status:
		if status is False:
			print "build failed"
			sys.exit(1)
	sys.exit(0)