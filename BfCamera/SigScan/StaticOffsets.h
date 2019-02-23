#include "SigScan.h"

// macro for lazily adding signatures to this sig list
#define AddOffset(name, pattern, type) static DWORD64 Get_## name (){static DWORD64 name = NULL; if (name != NULL) return name; return name=(DWORD64)PatternScanner::FindPattern({pattern, type});} 

class StaticOffsets {
public:
	AddOffset(OFFSET_GAMERENDERER,  "48 8B 0D [?? ?? ?? ?? 48 85 C9 74 0E 48 8B 01 48 8D 15 ?? ?? ??", PatternType::RelativePointer )
	AddOffset(OFFSET_FIRSTTYPEINFO, "48 8B 05 [?? ?? ?? ?? 48 89 41 08 48 89 0D ?? ?? ?? ??", PatternType::RelativePointer)
};
