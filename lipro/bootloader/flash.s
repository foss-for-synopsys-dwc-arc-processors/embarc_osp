.section .rodata

.global flash_start
.global flash_end

flash_start:
        .incbin "flash.bin"
flash_end:
