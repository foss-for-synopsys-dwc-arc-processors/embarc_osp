# import re

# xcam_regex_core = r'([.|/*0-9a-f ]* )?'
# xcam_regex_cycles = r'([0-9]*)[F|\:]?'
# xcam_regex_flags = r'.{8}[ .:]'
# xcam_regex_pc = r'([0-9a-f]{8,16})'
# xcam_regex_insn = r'(\s+[-0-9a-f]{4,8})+[| ]'
# xcam_regex_mnemonic = r'([a-z0-9_.,\[\] ]*)'
# xcam_regex_debug_mnemonic = r'-debug'
# xcam_regex_debug_text = xcam_regex_core + xcam_regex_cycles + xcam_regex_flags + xcam_regex_debug_mnemonic
# xcam_regex_debug = re.compile(xcam_regex_debug_text)
# xcam_regex_full = xcam_regex_core + xcam_regex_cycles + xcam_regex_flags + xcam_regex_pc + xcam_regex_insn + xcam_regex_mnemonic
# xcam_regex = re.compile(xcam_regex_full)

# trace = "/SCRATCH/jingru/calibration/calibration-tests/rhx100/test-out/RHX100/vcs/alue2Mem.s/trace.txt"
# prev_cycles = 0
# with open(trace ) as f:
#     for line in f:
#         # if "nop" in line:
#         l = xcam_regex.match(line)
#         try:
#             cycle_count = float(l.group(2))
#         except:
#             cycle_count = prev_cycles
#         if (cycle_count - prev_cycles) > 5:
#             print(line)
#         prev_cycles = cycle_count
#             # print(l)
#             # print(l.group(2))
import sys
import logging
import os
import re
import collections
import json
import csv
def get_csv_raw_data(path, type="ncam"):
    raw_data = collections.OrderedDict()
    configs = dict()
    capture_benchmark_end = False
    with open(path, newline='') as f:
        reader = csv.reader(f)
        csv_list = list(reader)
        if type == "fpga":
            header = csv_list[1]
            pct_data = csv_list[2:]
            header[0] = "benchmark"
            header[1] = "iall"
            header[2] = "crun"
            header = [k.replace(" ", "") for k in header]
        else:
            header = csv_list[0]
            pct_data = csv_list[1:]
        for d in pct_data:
            if d and not all(v == '' for v in d):
                if not capture_benchmark_end:
                    raw_data[d[0]] = dict(zip(header, d))
                else:
                    if configs.get(type, None) is None:
                        configs[type] = {d[0]: d[1]}
                    else:
                        configs[type][d[0]] = d[1]
            else:
                capture_benchmark_end = True
    print(json.dumps(raw_data, indent=4))
    print(json.dumps(configs, indent=4))
    print(header)
    return header, raw_data, configs

get_csv_raw_data("/SCRATCH/jingru/calibration/calibration-tests/rhx100/test-out/RHX100/test.csv")