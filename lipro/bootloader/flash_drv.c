#include <string.h>
#include <stdlib.h>
#include <stdint.h>

extern const uint8_t flash_start[];
extern const uint8_t flash_end[];

int flash_read(void* buffer, int32_t offset, int32_t size)
{
        int32_t flash_size = flash_end - flash_start;
        int bytes_to_read = _min(size, flash_size - offset);
        memcpy(buffer, flash_start + offset, bytes_to_read);
        return bytes_to_read;                      
}
