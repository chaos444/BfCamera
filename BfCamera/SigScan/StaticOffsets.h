#include "SigScan.h"

// macro for lazily adding signatures to this sig list
#define AddOffset(name, pattern, type) static DWORD64 Get_## name (){static DWORD64 name = NULL; if (name != NULL) return name; return name=(DWORD64)PatternScanner::FindPattern({pattern, type});} 

class StaticOffsets {
public:
	AddOffset(OFFSET_GAMERENDERER,  "48 8B 0D [?? ?? ?? ?? 48 85 C9 74 0E 48 8B 01 48 8D 15 ?? ?? ??", PatternType::RelativePointer )
	AddOffset(OFFSET_FIRSTTYPEINFO, "48 8B 05 [?? ?? ?? ?? 48 89 41 08 48 89 0D ?? ?? ?? ??", PatternType::RelativePointer)
	AddOffset(OFFSET_CAMERAHOOK, "49 8B D6 48 C1 E3 08 48 03 9F ?? ?? ?? ?? 48 8B CB E8 [?? ?? ?? ?? 4C 8B 4C 24 28 41 0F B6 41 07", PatternType::RelativePointer)
	AddOffset(OFFSET_GAMETIMESETTINGS, "48 89 05 [?? ?? ?? ?? C7 40 ?? ?? ?? ?? ?? 8B 43 18", PatternType::RelativePointer)
};
