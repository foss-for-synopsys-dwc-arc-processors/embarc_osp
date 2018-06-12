.. _add_new_middleware:

Add New Middleware
##################

Follow the steps below to add new middleware.

* Create a sub-folder named <target-middleware> in middleware.
* Create **<target-middleware>.mk** in the folder **<target-middlware>**. Note that the .mk file name must be the same as the folder's so that the build system can find the new makefile.
* Add necessary definitions in **<target-middleware>.mk**, such as folders (MID_INCDIR, MID_CSRDIR,  MID_ASM_DIR), source files (MID_XXX_CSRCS,
  MID_XXX_ASMSRCS), and MID_DEFINES.
* Use the other middleware makefiles for reference.
