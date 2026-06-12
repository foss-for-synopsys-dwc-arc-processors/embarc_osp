import os
import re

# def extract_context(trace1, trace2):
#     ncam_regex_cycles = r'[0-9]+\s+([0-9]+)\s+'  # group 1 (cycles)
#     ncam_regex_pipeline = r'(\[[^\]]*])?\s+'  # group 2 (pipeline)
#     ncam_regex_pc = r'[[<|-](0x[0-9a-f]{8,16})[\]|>|-] '  # group 3 (pc)
#     ncam_regex_bits = r'0x[0-9a-f]{4,8}(\s+0x[0-9a-f]{4,8})*\s*'  # group 4 (instruction data, ignore)
#     ncam_regex_lanes = r'(\[.*])*'  # group 5 (ignore)
#     ncam_regex_cond = r'[A-Z ]+'
#     ncam_regex_mnemonic = r'(\[?[^:]+)'  # group 6 (disassembly)
#     ncam_regex = re.compile(ncam_regex_cycles + ncam_regex_pipeline + ncam_regex_pc + ncam_regex_bits + ncam_regex_lanes + ncam_regex_cond + ncam_regex_mnemonic)
#     ncam_pipe_warmed_up = re.compile(r".*\(w0\) x5 \<= 0x[0]+5")

#     xcam_regex_core = r'([.|/*0-9a-f ]* )?'  # group 1
#     xcam_regex_cycles = r'([0-9]*)[F|\:]?'  # group 2
#     xcam_regex_flags = r'.{8}[ .:]'  # nothing
#     xcam_regex_pc = r'([0-9a-f]{8,16})'  # group 3
#     xcam_regex_insn = r'(\s+[-0-9a-f]{4,8})+[| ]'  # group 4 : encoding
#     xcam_regex_mnemonic = r'([a-z0-9_.,\[\]\(\) ]*)'  # group 5 : mnemonic

#     xcam_regex_debug_mnemonic = r'-debug'

#     xcam_regex_debug_text = xcam_regex_core + xcam_regex_cycles + xcam_regex_flags + xcam_regex_debug_mnemonic
#     xcam_regex_debug = re.compile(xcam_regex_debug_text)

#     xcam_regex_full = xcam_regex_core + xcam_regex_cycles + xcam_regex_flags + xcam_regex_pc + xcam_regex_insn + xcam_regex_mnemonic
#     xcam_regex = re.compile(xcam_regex_full)
#     xcam_pipe_warmed_up = re.compile(r".*w[0-9]: t0 <- [0]+5")

#     with open(trace1) as f1, open(trace2) as f2:
#         line = 0
#         for line1, line2 in zip(f1, f2):
#             line = line + 1
#             trace_res1 = ncam_regex.match(line1)
#             trace_res2 = ncam_regex.match(line2)
#             if not (trace_res1 and trace_res2):
#                 if not("Misses at" in line1 and "Misses at" in line2) and line1 != line2:
#                     print(f"not align {line} : {line1}, {line2}")
#                     break
#             if trace_res1 and trace_res2:
#                 ncam_pc1 = trace_res1.group(3)
#                 ncam_pc2 = trace_res2.group(3)
#                 if ncam_pc1 != ncam_pc2:
#                     print(f"{line} : {line1}, {line2}")
#                     break
# extract_context("/SCRATCH/jingru/calibration/calibration-tests/rhx100/dhrystone_fusion.ll.latst", "/SCRATCH/jingru/calibration/calibration-tests/rhx100/dhrystone_fusion.ll.tmp1")

# import os
# from harness import VPXBenchmarks, EEMBC

# def _output_reader(self, proc, harness):
#     log_out_fp = open(self.log, "wt")
#     for line in iter(proc.stdout.readline, b''):
#         line = line.decode('utf-8').rstrip()
#         logger.debug("OUTPUT: {0}".format(line))
#         log_out_fp.write(line + "\n")
#         log_out_fp.flush()
#         harness.handle(line)
#         if harness.capture_end:
#             try:
#                 proc.wait(0.1)
#             except subprocess.TimeoutExpired:
#                 self.terminate(proc)
#             break

#     log_out_fp.close()

# def read_metrics(root, results):
#     for dirpath, _, filenames in os.walk(root, topdown=True):
#         if "handler.log" in filenames:
#             root_info = dirpath.split(os.path.sep)
#             benchmark = root_info[-1]
#             sim = root_info[-2]
#             platform = root_info[-3]
#             print(f"{benchmark} {sim} {platform}")

#             with open(os.path.join(dirpath, "hanler.log")) as file:
#                 for line in file:
#                     harness.handle(line)

# read_metrics("/SCRATCH/jingru/test-out-1", None)
# import re
# import os
# import json
# data_re = re.compile(".*RHX100 (.*)  passed \(([a-z]+) ([0-9]+).([0-9]+)s\)")
# data =  {}

# class RVBench:
#     EXCEPTION_RE = re.compile("(.*)Encountered '(.*)' exception")
#     CRUN_RE = re.compile(".*Cycle count is ([0-9]+)")
#     IALL_RE = re.compile(".*Inst count is ([0-9]+)")
#     RESULT_RE = re.compile("\(lldb.*\) exit")

#     def __init__(self, instance):
#         self.state = None
#         self.capture_end = False
#         self.id = instance #   .testcase.id
#         self.tests = {}
#         self.pct = {}
#         self.reason = None

#     def handle(self, line):
#         exception = self.EXCEPTION_RE.match(line)
#         if exception:
#             self.tests[self.id] = "failed"
#             self.state = "failed"
#             self.reason = exception.group(2)
#         if self.RESULT_RE.match(line):
#             self.capture_end = True
#         cycle_re = self.CRUN_RE.match(line)
#         if cycle_re:
#             self.pct["crun"] = int(cycle_re.group(1))
#         iall_re = self.IALL_RE.match(line)
#         if iall_re:
#             self.pct["iall"] = int(iall_re.group(1))
#         if self.capture_end:
#             self.pct["cpi"] = round(float(self.pct["crun"] / self.pct["iall"]), 2)
#             if self.state is None:
#                 self.tests[self.id] = "passed"
#                 self.state = "passed"

# with open('test-out/test.log') as f:
#     lines = f.readlines()
#     for line in lines:

#         res = data_re.match(str(line))
#         if res:

#             sim  = res.group(2)
#             if "nsim"  in sim:
#                 sim = "ncam"
#             bmark = res.group(1) 
#             time = f"{res.group(3)}.{res.group(4)}"
#             print(f"{sim} {bmark} {time}")
#             if not data.get(sim.strip()):
#                 data[sim.strip()] = {bmark.strip(): {"handler_time": time.strip()}}
#             else:
#                 data[sim.strip()][bmark.strip()] = {"handler_time": time.strip()}
# print(json.dumps(data, indent=4))



# for dirpath, _, filenames in os.walk("test-out", topdown=True):
#     if "handler.log" in filenames:
#         bmark = os.path.basename(dirpath)
#         sim = os.path.basename(os.path.dirname(dirpath))
#         harness  = RVBench(bmark)
#         print(f"{bmark} {sim}")
#         if  data[sim].get(bmark):
#             with open(os.path.join(dirpath,"handler.log")) as f:
#                 lines = f.readlines()
#                 for line in lines:
#                     harness.handle(line)
#                     if harness.capture_end:
#                         break
#                 print(harness.pct)
                
#                 data[sim][bmark].update(harness.pct)
# print(json.dumps(data, indent=4))
# from collections import OrderedDict
# for sim, sim_data   in data.items():
#     print(f"{sim} ............")
#     data1 = OrderedDict(sorted(sim_data.items()))
#     for bmark, bmark_data in  data1.items():
#         print(f"{bmark},eembc,{bmark_data['iall']},{bmark_data['crun']},{bmark_data['cpi']},{bmark_data['handler_time']},10")
#         # print(f"{bmark},{bmark_data['crun']}")
#         # print(f"{bmark_data['crun']}")

# configs =  {
#                     "name": "RHX100",
#                     "type": "na",
#                     "simulation": "ncam",
#                     "runner": "lldbac",
#                     "runner_args": "scripts/support/run_ncam.sh",
#                     "runner_cmd": "/slowfs/us01dwt2p323/de02-arcjenkins2/slaves/us01-gp-automationlinux06/workspace/ncam-calibration-jingru/calibration-tests/ncam-calibration/scripts/support/hwc.mdb",
#                     "tcf": "/slowfs/us01dwt2p323/de02-arcjenkins2/slaves/us01-gp-automationlinux06/workspace/ncam-calibration-jingru/calibration-tests/ncam-calibration/AV5/RHX100/tcf/arc.tcf",
#                     "model_root": "AV5/RHX100/vcs",
#                     "arc_version": "av5",
#                     "build_configuration": "",
#                     "vpx_tag": None,
#                     "extra_args": "isa_counters=1 nsim_isa_pct_counters=32 nsim_isa_rbd_option=0 nsim_system_cfg=128,400,1,64 nsim_isa_dccm_ecc_option=1",
#                     "special_args": "isa_counters=1 nsim_isa_pct_counters=32 nsim_isa_rbd_option=0 nsim_system_cfg=128,400,1,64 nsim_isa_dccm_ecc_option=1",
#                     "nsim_arch": "",
#                     "clock_speed": 0,
#                     "link_script": "/slowfs/us01dwt2p323/de02-arcjenkins2/slaves/us01-gp-automationlinux06/workspace/ncam-calibration-jingru/calibration-tests/ncam-calibration/AV5/RHX100/link/link.ld",
#                     "nsim_version": " TIP@d0f9f36e14 (Build: TIP@d0f9f36e14)",
#                     "nsim_home": "/slowfs/us01dwt2p323/de02-arcjenkins2/slaves/us01-gp-automationlinux06/workspace/ncam-calibration-jingru/calibration-tests/nsim/Tools/nsim/main/dev/build/out/nSIM_64/",
#                     "version": "2fb774075b1",
#                     "url": "git@gitsnps.internal.synopsys.com:dwc-sim/ncam-calibration.git",
#                     "root": "/slowfs/us01dwt2p323/de02-arcjenkins2/slaves/us01-gp-automationlinux06/workspace/ncam-calibration-jingru/calibration-tests/ncam-calibration",
#                     "binary": "AV5/RHX100/bmark/stage2/semihosting/gcc/fpbench",
#                     "nsim_prop": "cpunum=1 dcache=0x4000,32,2,l dccm_base=0x200000 dccm_size=0x80000 debugger_64=1 flattener_version=3 gverbose=0 icache=0x4000,32,2,a iccm0_base=0x0 iccm0_size=0x80000 invalid_instruction_interrupt=0 isa_counters=1 ncam_profiling=1 nsim_bpu_bc_entries=512 nsim_bpu_debug=1 nsim_bpu_fb_entries=4 nsim_bpu_pt_entries=8192 nsim_bpu_rs_entries=4 nsim_bpu_tosq_entries=5 nsim_cycle=1 nsim_has_l0_dcache=1 nsim_hostlink=0 nsim_is_under_debugger=1 nsim_isa_core=2 nsim_isa_dc_feature_level=2 nsim_isa_dc_mem_cycles=1 nsim_isa_dc_version=1 nsim_isa_dccm_ecc_option=1 nsim_isa_dccm_mem_cycles=1 nsim_isa_dual_issue_option=1 nsim_isa_ext=+smrnmi nsim_isa_family=rv32 nsim_isa_fusion_option=1 nsim_isa_halt_on_reset=1 nsim_isa_has_wb_mem=1 nsim_isa_ic_disable_on_reset=1 nsim_isa_ic_version=1 nsim_isa_nvm_base=0x80000000 nsim_isa_nvm_enabled=1 nsim_isa_nvm_size=0x800000 nsim_isa_pct_counters=32 nsim_isa_rbd_option=0 nsim_isa_rv_marchid=0x80000402 nsim_isa_rv_mimpid=0x20100 nsim_isa_unaligned_option=1 nsim_isa_wb_mem_max_outstanding=8 nsim_isa_wb_mem_num_entries=16 nsim_mem-interface=ARCV_SPMP,num_regions=32,granule=5,csr_base=0x4 nsim_ncam_experimental_option=1 nsim_print_stats_on_exit=1 nsim_props_output=properties.props nsim_rnmi_int_vec=0x800 nsim_rnmi_vec_ext=0 nsim_rtia_hart_major_prio=1 nsim_rtia_hart_major_prio_width=6 nsim_rv_reset_pc=0x0 nsim_rv_reset_pc_ext=1 nsim_semihosting=1 nsim_system_cfg=128,400,1,64 nsim_table_directory=./ nsim_table_format=csv nsim_trace=0 nsim_trace-output=trace.txt profiling_enabled=0 run_cycles=10000 tcf=/slowfs/us01dwt2p323/de02-arcjenkins2/slaves/us01-gp-automationlinux06/workspace/ncam-calibration-jingru/calibration-tests/ncam-calibration/AV5/RHX100/tcf/arc.tcf trace_enabled=0",
#                     "pre_report": "/slowfs/us01dwt2p323/de02-arcjenkins2/slaves/us01-gp-automationlinux06/workspace/ncam-calibration-jingru/calibration-tests/ncam-calibration/pm/AV5/RHX100/fpbench/gcc/RHX100-Iteration-10.xlsx",
#                     "pre_report_root": "pm/AV5/RHX100",
#                     "compiler_version": "(RISC-V elf toolchain - build 9161) 14.2.0",
#                     "category": "fpbench"
# }
# from jinja2 import FileSystemLoader
# from jinja2.environment import Environment

# TEMPLATE_DIR = os.path.abspath("/SCRATCH/jingru/calibration/calibration-tests/test_tmpl")
# env = Environment()
# template_file_path = "/SCRATCH/jingru/calibration/calibration-tests/test_tmpl"
# env.loader = FileSystemLoader(template_file_path)
# template = env.get_template('test.tmpl')
# target_text = template.render(data=configs)
# open("/SCRATCH/jingru/calibration/calibration-tests/rhx100/resultsdb_metrics_file.log", "w").write(target_text)
# import numpy as np
# from scipy.optimize import curve_fit
# import math
# xx = []
# yy = []
# cc = []
# rem_5 = "/SCRATCH/jingru/calibration/calibration-tests/rmx500/test_div.log"
# div_5 = "/SCRATCH/jingru/calibration/calibration-tests/rmx500/test_div5.log"
# div_51 = "/SCRATCH/jingru/calibration/calibration-tests/rmx500/test_5div.log"
# dived = 134217733
# with open(os.path.join(div_5)) as file:
#     i = -65536
#     for line in file:
#         xx.append(i)
#         rtl_latency = int(line)
#         yy.append(rtl_latency)
#         if i != 0:
#             data0 = math.floor(math.log2(dived))
#             data1 = math.floor(math.log2(abs(i)))
#             latency = math.floor((data0 - data1)/2) + 5
#             quotient = int(dived/i)
#             bit_check = 0
#             bit_count = bin(quotient).count('1')
#             remainer = dived % i
#             if ((remainer) == 0) and ((quotient % 2 == 0)) and ((bit_count >= 1)):
#                 bit_check = 1
#             #     latency = latency + 1
#             # print(f"{latency - rtl_latency}: {i} - {latency}- {rtl_latency}, {bit_check}, {bit_count}")
#             # if latency != rtl_latency:
#             nn = math.floor(math.log2(abs(quotient)))
#             if remainer == 0:
#                 mm = 0
#             else:
#                 mm = math.floor(math.log2(abs(remainer)))
#             # if (nn > 10) and ((nn %2 ) != 0) and ((quotient % 2 == 0)) and  ((nn + mm) == 26):
#             #     bit_check = 1
#             if (i <0) and (math.pow(2, data1) == abs(i)) and ((data1 % 2) == 0):
#                 latency = latency + 1
#             if (latency - rtl_latency) != 0:
#                 print(f"{latency - rtl_latency}: {i} - {latency}- {rtl_latency}, {bit_check}, x/{i} = {quotient} {nn} {remainer} {data1}")
#             # if (bit_check) != 0:
#             #     print(f"bit {latency - rtl_latency}: {i} - {latency}- {rtl_latency}, {bit_check}, x/{i} = {quotient}-{nn} {remainer}")
#             cc.append(latency - rtl_latency)
#         i = i + 1
# cc.sort()
# print(list(set(cc)))
# with open(os.path.join(div_51)) as file:
#     i = 0
#     for line in file:
#         xx.append(i)
#         rtl_latency = int(line)
#         yy.append(rtl_latency)
#         if i != 0:
#             data0 = math.floor(math.log2(dived))
#             data1 = math.floor(math.log2(abs(i)))
#             latency = math.floor((data0 - data1)/2) + 5
#             quotient = int(dived/i)
#             bit_check = 0
#             bit_count = bin(quotient).count('1')
#             remainer = dived % i
#             if ((remainer) == 0) and ((quotient % 2 == 0)) and ((bit_count >= 1)):
#                 bit_check = 1
#             #     latency = latency + 1
#             # print(f"{latency - rtl_latency}: {i} - {latency}- {rtl_latency}, {bit_check}, {bit_count}")
#             # if latency != rtl_latency:
#             nn = math.floor(math.log2(abs(quotient)))
#             if remainer == 0:
#                 mm = 0
#             else:
#                 mm = math.floor(math.log2(abs(remainer)))
#             # if (nn > 10) and ((nn %2 ) != 0) and ((quotient % 2 == 0)) and  ((nn + mm) == 26):
#             #     bit_check = 1
#             if (i <0) and (math.pow(2, data1) == abs(i)) and ((data1 % 2) == 0):
#                 latency = latency + 1
#             if (latency - rtl_latency) != 0:
#                 print(f"{latency - rtl_latency}: {i} - {latency}- {rtl_latency}, {bit_check}, x/{i} = {quotient} {nn} {remainer} {data1}")
#             # if (bit_check) != 0:
#             #     print(f"bit {latency - rtl_latency}: {i} - {latency}- {rtl_latency}, {bit_check}, x/{i} = {quotient}-{nn} {remainer}")
#             cc.append(latency - rtl_latency)
#         i = i + 1
# cc.sort()
# print(list(set(cc)))
# rem_2 = "/SCRATCH/jingru/calibration/calibration-tests/rmx500/test_rem2.log"
# with open(os.path.join(rem_2)) as file:
#     i = -65536
#     for line in file:
#         xx.append(i)
#         rtl_latency = int(line)
#         yy.append(rtl_latency)
#         if i != 0:
#             data0 = math.floor(math.log2(abs(i)))
#             data1 = math.floor(math.log2(abs(1)))
#             latency = math.floor((data0 - data1)/2) + 6
#             quotient = int(i/1)

#             # if (nn > 10) and ((nn %2 ) != 0) and ((quotient % 2 == 0)) and  ((nn + mm) == 26):
#             #     bit_check = 1
#             if (i <0) and (math.pow(2, data0) == abs(i)) and ((data0 % 2) == 0):
#                 latency = latency - 1
#             if (latency - rtl_latency) != 0:
#                 print(f"{latency - rtl_latency}: {i} - {latency}- {rtl_latency}, {i}/1 = {quotient}")
#             # if (bit_check) != 0:
#             #     print(f"bit {latency - rtl_latency}: {i} - {latency}- {rtl_latency}, {bit_check}, x/{i} = {quotient}-{nn} {remainer}")
#             cc.append(latency - rtl_latency)
#         i = i + 1
# cc.sort()
# print(list(set(cc)))

# # 示例数据 (简单线性关系)
# x_data = np.array([4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12])
# y_data = np.array([4, 3, 2, 1, 0, 5, 4, 3, 2, 1, 0, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 8, 7, 6, 5, 4, 3, 2, 1, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0])
# d_data = np.array([0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10])
# # if d = 0, z = 1
# # if d = 1/2, 0.5 = 2
# # if d = 2/2 = 1, 3
# # if d = 3/2 = 1.5 ,3
# # if d = 4/2 = 2, 4
# # if 

# z_data = np.array([1, 2, 3, 3, 4, 1, 2, 3, 3, 4, 4, 1, 2, 3, 3, 4, 4, 5, 1, 2, 3, 3, 4, 4, 5, 5, 1, 2, 3, 3, 4, 4, 5, 5, 6, 1, 2, 3, 3, 4, 4, 5, 5, 6, 6,  1,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7, 1,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7, 7, 1,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7, 7, 8]) # 假设 z = x + y + 2

# # 定义要拟合的函数形式 (例如: z = a*x + b*y + c)
# def func_to_fit(xy, a, c):
#     x, y = xy
#     return a * (x - y + c)

# def func_to_fit1(x, a, c):
#     return math.floor(a * (x + c))
# # 准备数据 (curve_fit需要将x, y合并为一个输入)
# xy_data = np.array([x_data, y_data])

# # 拟合数据
# params, covariance = curve_fit(func_to_fit, xy_data, z_data)
# params1, covariance = curve_fit(func_to_fit1, d_data, z_data)
# print(params)
# print(f"拟合参数: a={params[0]:.2f}, c={params[1]:.2f}")
# print(params1)
# print(f"拟合参数1: a={params1[0]:.2f}, c={params1[1]:.2f}")
# # 生成一个函数对象
# def fitted_function(x, y):
#     return params[0] * (x - y + params[1])

# # 使用拟合函数
# print(f"在 (1, 2) 处的预测值: {fitted_function(1, 2)}") # 1*1 + 2*2 + 2 = 7

def extract_context(trace1, ncam_trace):
    ncam_regex_cycles = r'[0-9]+\s+([0-9]+)\s+'  # group 1 (cycles)
    ncam_regex_pipeline = r'(\[[^\]]*])?\s+'  # group 2 (pipeline)
    ncam_regex_pc = r'[\[|<|-](0x[0-9a-f]{8,16})[\]|>|-] '  # group 3 (pc)
    ncam_regex_bits = r'0x[0-9a-f]{4,8}(\s+0x[0-9a-f]{4,8})*\s*'  # group 4 (instruction data, ignore)
    ncam_regex_lanes = r'(\[.*])*'  # group 5 (ignore)
    ncam_regex_cond = r'[A-Z ]+'
    ncam_regex_mnemonic = r'(\[?[^:]+)'  # group 6 (disassembly)
    ncam_regex = re.compile(ncam_regex_cycles + ncam_regex_pipeline + ncam_regex_pc + ncam_regex_bits + ncam_regex_lanes + ncam_regex_cond + ncam_regex_mnemonic)
    ncam_pipe_warmed_up = re.compile(r".*\(w0\) x5 \<= 0x[0]+5")

    xcam_regex_core = r'([.|/*0-9a-f ]* )?'  # group 1
    xcam_regex_cycles = r'([0-9]*)[F|\:]?'  # group 2
    xcam_regex_flags = r'[ ]{6,8}M?[ |\d|\*]*[ |\.|\:]'  # nothing
    xcam_regex_pc = r'([u]?[0-9a-f]{8,16})'  # group 3
    xcam_regex_insn = r'(\s+[-0-9a-f]{4,8})+[| ]'  # group 4 : encoding
    xcam_regex_mnemonic = r'([a-z0-9_.\-,\[\]\(\) ]*)'  # group 5 : mnemonic

    xcam_regex_debug_mnemonic = r'-debug'

    xcam_regex_debug_text = xcam_regex_core + xcam_regex_cycles + xcam_regex_flags + xcam_regex_debug_mnemonic
    xcam_regex_debug = re.compile(xcam_regex_debug_text)

    xcam_regex_full = xcam_regex_core + xcam_regex_cycles + xcam_regex_flags + xcam_regex_pc + xcam_regex_insn + xcam_regex_mnemonic
    xcam_regex = re.compile(xcam_regex_full)
    xcam_pipe_warmed_up = re.compile(r".*w[0-9]: t0 <- [0]+5")
    ncam_pcs = []
    with open(ncam_trace) as f2:
        for line1 in f2:
            trace_res = ncam_regex.match(line1)
            if trace_res:
                ncam_pc = trace_res.group(3)
                ncam_pc = ncam_pc[2:]
                ncam_pcs.append(ncam_pc)
                # print(ncam_pc)
    i = 0
    with open(trace1) as f1:
        for line1 in f1:
            trace_res = xcam_regex.match(line1)
            if trace_res:
                vcs_pc = trace_res.group(3)
                vcs_pc = vcs_pc[2:]
                # vcs_pcs.append(vcs_pc)
                if not ncam_pcs[i] in line1:
                    print(i)
                    print(ncam_pcs[i])
                    print(vcs_pc)
                    print(line1)
                    break
                i = i + 1
    # with open(trace1) as f1, open(ncam_trace) as f2:
    #     line = 0
    #     for line1, line2 in zip(f1, f2):
    #         line = line + 1
    #         trace_res = ncam_regex.match(line1)
    #         if trace_res:
    #             ncam_pc = trace_res.group(3)
    #             ncam_pc = ncam_pc[2:]
    #         else:
    #             ncam_pc = None
    #         print(ncam_pc)
    #         if ncam_pc not in line2:
    #             print(ncam_pc)
    #             print(f"{line} : {line1}, {line2}")
    #             break
extract_context(
    "/SCRATCH/jingru/calibration/calibration-tests/rmx700/test-out/RMX700/vcs/networking/ip_pktcheckb512k_lite_ccm/trace.txt",
    "/SCRATCH/jingru/calibration/calibration-tests/rmx700/test-out/RMX700/ncam/networking/ip_pktcheckb512k_lite_ccm/trace.txt"
)
