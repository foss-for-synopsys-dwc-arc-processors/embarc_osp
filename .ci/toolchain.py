#! /usr/bin/env python
import zipfile
import tarfile
import urllib
import os
import sys
import shutil
from colorama import init, Fore, Back, Style
import argparse

def download_file(url, path):
    try:
    	urllib.urlretrieve(url, path)
    	return 0
    except Exception as e:
    	print e
    	print "This file from %s can't be download"%(url)
    	sys.stdout.flush()
    	return 1
    	
def download_gnu(version="2017.09", path=None):
	baseurl = "https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases/download/"
	url = baseurl + "arc-" + version + "-release/arc_gnu_"  + version+ "_prebuilt_elf32_le_linux_install.tar.gz"
	gnu_tgz = "arc_gnu_" + version + "_prebuilt_elf32_le_linux_install.tar.gz"
	if path is not None:
		gnu_tgz_path = os.path.join(path, "arc_gnu_" + version +"_prebuilt_elf32_le_linux_install.tar.gz")
	else:
		path = os.getcwd()
		gnu_tgz_path = os.path.join(path, "arc_gnu_" + version +"_prebuilt_elf32_le_linux_install.tar.gz")
	if not os.path.exists(path):
		os.makedirs(path)
	if gnu_tgz in os.listdir(path):
		print Fore.BLUE + "toochain cache"
		print os.listdir(path)
		print Style.RESET_ALL
		sys.stdout.flush()
	else:
		result = download_file(url, gnu_tgz_path)
		if result:
			gnu_tgz_path = None
	return gnu_tgz_path


def unzip(file, path):
	file_name = None
	try:
		pack = zipfile.ZipFile(file, "r")
		files = pack.namelist()
		file_name = files[0]
		pack.extractall(path)
		pack.close()
		return file_name
	except Exception:
		sys.exit(1)

def untar(file, path):
	file_name = None
	try:
		pack = tarfile.open(file, "r:gz")
		files = pack.getnames()
		file_name = files[0]
		for file in files:
			pack.extract(file, path)
		pack.close()
		return file_name
	except Exception as e:
		print e


def extract_file(file, path):
	extract_file_name = None
	extract_file_path = None
	filename, filesuffix = os.path.splitext(file)
	if filesuffix == ".gz":
		extract_file_name = untar(file, path)
	elif filesuffix == ".zip":
		extract_file_name = unzip(file, path)
	else:
		print "This file {} can't be extracted".format(file)
	if extract_file_name is not None:
		extract_file_path = os.path.join(path, extract_file_name)
	return extract_file_path




def store_gnu_toolchain(version, path=None):
	gnu_tgz_path = download_gnu(version, path)
	gnu_root_path = None
	gnu_file_path = None
	if gnu_tgz_path is None:
		print "Can't download gnu {} ".format(version)
	if not version in os.listdir(path):
		try:
			gnu_file_path = extract_file(gnu_tgz_path, path)
		except Exception as e:
			print e
		if gnu_file_path is not None:
			gnu_root_path = os.path.join(path, version)
			shutil.move(gnu_file_path, gnu_root_path)

def get_options_parser():
	configs = dict()
	parser = argparse.ArgumentParser()
	parser.add_argument("-v", "--gnu", dest="gnu", default="2017.09", help=("the version of gnu"), metavar="GNU_VERSION")
	parser.add_argument("-c", "--cache", dest="cache", default=".cache/toolchain", help=("the cache path"), metavar="TOOLCHAIN_CACHE_FOLDER")

	options = parser.parse_args()
	if options.gnu:
		configs["gnu"] = options.gnu
	if options.cache:
		configs["cache"] = options.cache

	return configs

if __name__ == '__main__':
	configs = get_options_parser()
	store_gnu_toolchain(configs["gnu"], configs["cache"])
