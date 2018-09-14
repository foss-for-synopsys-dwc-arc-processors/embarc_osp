.. _add_new_middleware:

Adding a New Middleware Package
###############################

Follow the steps below to add support for a new middleware package to embARC OSP.

* Create a sub-folder named <target-middleware> in middleware.
* Create **<target-middleware>.mk** in the folder **<target-middleware>**. Note that the .mk file name must be the same as the folder's name so that the build system can find the new makefile.
* Add necessary definitions in **<target-middleware>.mk**, such as folders (MID_INCDIR, MID_CSRDIR, MID_ASM_DIR), source files (MID_XXX_CSRCS, MID_XXX_ASMSRCS), and MID_DEFINES.
* Please use existing middleware makefiles as a reference for additional information.
