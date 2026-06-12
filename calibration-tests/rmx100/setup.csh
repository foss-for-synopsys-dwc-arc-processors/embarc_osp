#! /usr/bin/env tcsh
# ------------------------------------------------
# Author:       hastakh
# Created:      Tue Jan 16 16:55:13 IST 2024
#
# Modifier:     natalya
# Modified:     Tue Apr 09 14:20:00 GMT+4 2024
#
# Modifier:     lavg
# Modified:     Tue Nov 5 15:22:45 IST 2024
# Usage:
#       source setup.csh
# ------------------------------------------------
source /global/etc/modules.csh
set CUR_DIR=/slowfs/de02dwt2p011/zhup/rattler_mini_ga/repo/11_16         

setenv GENIP_ROOT              $CUR_DIR/genip_root
setenv JAVA_VPP_HOME           $GENIP_ROOT
setenv JAVA_JS_HOME            $GENIP_ROOT
setenv ARCV5EM                 $CUR_DIR/arcv5em
setenv ARCV2HS                 $ARCV5EM
setenv ARC_COSIM               $CUR_DIR/ARC_COSIM/main/not_released/arcv5
setenv ARC_SAFETY              $CUR_DIR/arc_safety
setenv ARCV2MSS                $CUR_DIR/arcv2mss
setenv ARCV5MPTB               $CUR_DIR/arcv5mptb
setenv ARC_TRACE               $CUR_DIR/arc_trace
setenv ARC_SOC_TRACE           $CUR_DIR/arc_soc_trace
setenv BCM                     $CUR_DIR/bcm
setenv ARC_COMMON_FUNCTIONS    $BCM
setenv UTILS_MPTB              $CUR_DIR/utils_mptb/hhd
setenv UTILS_HOME              $CUR_DIR/utils
setenv CCTRV                   $CUR_DIR/cctrv
setenv GEN_HOME                $CUR_DIR/isg3
setenv GEN_TEST                $CUR_DIR/isg3test
setenv NSIM_ROOT               "/SCRATCH/jingru/nsim/Tools/nsim/main/dev/build/out"
setenv PROJECT_NAME            RATTLER
setenv PROJECT_LIBS            "ARCV5EM,IMPLEMENTATION,CCTRV,ARC_SAFETY,ARC_COSIM,ARCV2MSS,BCM,ARC_TRACE,ARC_SOC_TRACE"
setenv FAMILY                   ARCV
setenv MASTER_IPLIBRARY        $ARCV5EM
setenv COSIM_RV                1
setenv IPLIBS                  $CUR_DIR/iplibs.lst

set siteId = `/usr/local/bin/siteid`

setenv PATH $ARCV5EM/bin:${PATH}
setenv PATH $ARCV5MPTB/bin/scripts:${PATH}

switch ($siteId)
  case "us01":
    set INFRA_HOME=/remote/us01sgnfs00113/infra
    breaksw
  case "in01":
    set INFRA_HOME=/slowfs/in01dwt3p029/dwc_verif
    breaksw
  case "eudc":
    set INFRA_HOME=/slowfs/de02dwt2p011/dwc_verif
    breaksw
  default:
    echo "UNKNOWN SITEID FOUND :: $siteId"
    exit
endsw

module purge
module use -a ${INFRA_HOME}/modulefiles
module load arcprojects/rattler

if (!($?METAWARE_HOME)) then
  setenv METAWARE_ROOT /u/relauto/.TOOLS_ROOT/SiteSpecific/eng_builds/arc_MWDT/latest/linux/ARC/MetaWare
  setenv METAWARE_HOME ${METAWARE_ROOT}/arc
  setenv PATH ${METAWARE_HOME}/bin:${PATH}
  setenv LD_LIBRARY_PATH ${METAWARE_HOME}/bin:${LD_LIBRARY_PATH}
endif

setenv LD_LIBRARY_PATH ${ARCHITECT_ROOT}/lib/linux_x86_64:${LD_LIBRARY_PATH}

echo "PROJECT_NAME    :$PROJECT_NAME"
echo "FAMILY          :$FAMILY"
echo "ARCV5MPTB       :$ARCV5MPTB"
echo "ARCV2MSS        :$ARCV2MSS"
echo "ARCV5EM         :$ARCV5EM"
echo "ARC_COSIM       :$ARC_COSIM"
echo "ARC_SAFETY      :$ARC_SAFETY"
echo "GENIP_ROOT      :$GENIP_ROOT"
echo "BCM             :$BCM"
echo "CCTRV           :$CCTRV"
echo "UTILS_HOME      :$UTILS_HOME"
echo "UTILS_MPTB      :$UTILS_MPTB"
echo "GEN_HOME        :$GEN_HOME"
echo "GEN_TEST        :$GEN_TEST"
echo "JAVA_VPP_HOME   :$JAVA_VPP_HOME"
echo "NSIM_HOME       :$NSIM_HOME"
echo "NSIM_ROOT       :$NSIM_ROOT"
echo "JAVA_JS_HOME    :$JAVA_JS_HOME"
echo "ARCHITECT_ROOT  :$ARCHITECT_ROOT"
echo "METAWARE_HOME   :$METAWARE_HOME"
echo "IPLIBS          :$IPLIBS"

module list

