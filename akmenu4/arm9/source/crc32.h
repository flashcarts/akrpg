//#pragma once

#ifdef __cplusplus
extern "C" {
#endif

unsigned int chksum_crc32 (unsigned char *block, unsigned int length);
void chksum_crc32gentab ();
unsigned long CheckSum32 (unsigned char *buf, unsigned len);

#ifdef __cplusplus
}
#endif
