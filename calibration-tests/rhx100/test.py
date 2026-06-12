# import json
# board_configs_file = "test.json"
# with open(board_configs_file, "r") as f:
#     board_configs = json.load(f)
# board_configs[0]["arcint_properties"].append("ras_pipemon_trace=0")
# print(board_configs)
import re
ncam_reg = re.compile(r'([0-9]+)\s+([0-9]+)\s+')
i = 0
with open("/SCRATCH/jingru/calibration/calibration-tests/rhx100/trc/nsim.rgbcmy01.out1.log") as f1, open("/SCRATCH/jingru/calibration/calibration-tests/rhx100/trc/nsim.rgbcmy01.out.log") as f2:
    for line1, line2 in zip(f1, f2):
        i = i + 1
        if "ERROR:[ISAOPT]" in line1:
            continue

        line11 = line1.split()
        line22 = line2.split()
        if (not line11):
            continue
        line10 = line11[0]
        line20 = line22[0]

        # if ("0x1000e820" in line1) or ("0x1000e820" in line2):
        if (line10 != line20):
            print(f"{i}- {line1}")
            print(f"{i}- {line2}")
            print("......................")
            # break
