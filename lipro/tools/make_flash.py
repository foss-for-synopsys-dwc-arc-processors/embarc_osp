#!/usr/bin/python3
libnames = ['os', 'sys', 'json', 'cffi', 'subprocess']
for libname in libnames:       
    try:
        lib = __import__(libname)
    except:
        import pip
        if hasattr(pip, 'main'):
            pip.main(['install', libname, '--user'])
        else:
            import pip._internal
            pip._internal.main(['install', libname, '--user'])
        lib = __import__(libname)
    globals()[libname] = lib
                    
os.system('gcc -E common/manifest.h | grep -v \# > manifest.preprop')
ffi=cffi.FFI()
with open('manifest.preprop') as f:
    ffi.cdef(f.read())


for line in open('common/lipro_memmap.h'):
    tokens = line.split()
    if len(tokens) > 2 and tokens[0]=='#define':
        statement='{0} = {1}'.format(tokens[1], ' '.join(tokens[2:]))
        exec(statement)
        
        
man = ffi.new('manifest_t[1]')
module_count = 0
flash_offset = ffi.sizeof(man)
binary_data=bytearray()
num_cores=4

with open(sys.argv[1]) as f:
    config = json.loads(f.read())

for module in config:
    os.system('elf2bin ' + module['src'] + ' module.bin')
    with open('module.bin', 'rb') as f:
        bindata = f.read()
    print ("module " + module['module_name'] + ": " + str(len(bindata)) + " bytes")
    man[0].modules[module_count].flash_address = flash_offset
    man[0].modules[module_count].size_in_bytes = len(bindata)
    man[0].modules[module_count].module_id = int(module['module_id'])
    print(module['module_name'].upper()+'_CODE_START' + " = " + str(eval(module['module_name'].upper()+'_CODE_START')))
    man[0].modules[module_count].memory_address = eval(module['module_name'].upper()+'_CODE_START')
    if man[0].modules[module_count].module_id < num_cores:
        hl_addr = subprocess.check_output("nmac " + module['src'] + " | grep __HOSTLINK__", shell=True).decode("utf-8").split()[0]
        with open(module['module_name']+'.arg', 'w') as f:
            f.write("-prop=__HOSTLINK__=0x" + hl_addr +"\n")
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
