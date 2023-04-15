#include <nds.h>


#ifdef __cplusplus
extern "C" {
#endif

bool sddInitSD();

void sddReadBlocks( u32 addr, u32 blockCount, void * buffer );

void sddWriteBlocks( u32 addr, u32 blockCount, const void * buffer );


#ifdef __cplusplus
}
#endif
