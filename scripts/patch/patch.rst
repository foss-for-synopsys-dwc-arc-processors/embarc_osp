
.. patch script:

embARC apply patch
###################

This script scans for the set of patches in this git repository and attempts to apply them to the `embARC` branch.

The default options will scan in directories ``<EMBARC_ROOT>/library``, ``<EMBARC_ROOT>/middleware`` and ``<EMBARC_ROOT>/os``  for patches.

To run the script in the local tree, follow the steps below:
::
		$ python patch.py --embarc-root < EMBARC_ROOT >

The embARC patch script accepts the following optional arguments:

	-h, --help         show this help message
	
	--patch-roots      Directories to search for patch files.
    	                   All .patch files in these directories 
    	                   will be processed.
	--embarc-root      Specify the embARC_osp directory


.. note:: When generate patches, use the option ``--add-header`` to add the repo url and commit. For example, ``--add-header="repo_link:https://github.com/ARMmbed/mbedtls.git repo_commit:mbedtls-2.4.1"``
