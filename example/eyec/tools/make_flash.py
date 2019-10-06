#!/usr/bin/python3
import os
import sys
import json

try:
    import cffi
except ImportError:
    import pip
    if hasattr(pip, 'main'):
        pip.main(['install', 'cffi', '--user'])
    else:
        import pip._internal
        pip._internal.main(['install', 'cffi', '--user'])
    import cffi


os.system('gcc -E ../common/manifest.h | grep -v \# > manifest.preprop')
ffi=cffi.FFI()
with open ('manifest.preprop') as f:
    ffi.cdef(f.read())


man = ffi.new('manifest_t[1]')
module_count = 0
flash_offset = ffi.sizeof(man)
binary_data=bytearray()
    

with open(sys.argv[1]) as f:
    config = json.loads(f.read())

for module in config:
    os.system('elf2bin ' + module['src'] + '> module.bin')
    with open('module.bin', 'rb') as f:
        bindata = f.read()
    print ("module " + module['module_name'] + ": " + str(len(bindata)) + " bytes")
    man[0].modules[module_count].flash_address = flash_offset
    man[0].modules[module_count].size_in_bytes = len(bindata)
    man[0].modules[module_count].module_id = int(module['module_id'])
    man[0].modules[module_count].memory_address = int(module['target_addr'], 0)
    flash_offset += len(bindata)
    binary_data.extend(bindata)
    module_count += 1
    os.system('rm module.bin')


man[0].version = 1
man[0].num_modules = module_count
os.system('rm manifest.preprop')

with open(sys.argv[2], 'wb') as f:
    f.write(ffi.buffer(man))
    print(len(binary_data))
    f.write(binary_data)
