import zlib, struct

BOOTCONFIG_MAGIC       = b'#BOOTCONFIG\n'
BOOTCONFIG_MAGINC_LEN  = 12
BOOTCONFIG_ALIGN_SHIFT = 2
BOOTCONFIG_ALIGN       =   (1 << BOOTCONFIG_ALIGN_SHIFT)
BOOTCONFIG_ALIGN_MASK  = (BOOTCONFIG_ALIGN - 1)


def xbc_checksum(data: bytes) -> int:
    """Equivalent to Linux xbc_calc_checksum() — additive 32-bit sum."""
    return sum(data) & 0xffffffff

with open("initrd", "rb") as f:
    initrd = f.read()

with open("bootconfig.txt", "rb") as f:
    bootconfig_text = f.read()

pad_len = (BOOTCONFIG_ALIGN - (len(bootconfig_text) & BOOTCONFIG_ALIGN_MASK)) & BOOTCONFIG_ALIGN_MASK
bootconfig_padded = bootconfig_text + b'\0' * pad_len

size = len(bootconfig_padded)
checksum = xbc_checksum(bootconfig_padded)

final_image = (
    initrd +
    bootconfig_padded +
    struct.pack("<I", size) +
    struct.pack("<I", checksum) +
    BOOTCONFIG_MAGIC
)

with open("initrd.img", "wb") as f:
    f.write(final_image)
