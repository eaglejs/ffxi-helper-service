# Elite API Libraries - Documentation Index

## About This Documentation

This documentation was created through **authorized decompilation** of the Elite API DLL files. The original creator of these libraries granted explicit permission for decompilation and documentation, as no official documentation exists.

**Date:** December 14, 2025
**Method:** .NET Reflection (EliteMMO.API.dll) & Binary Analysis (EliteAPI.dll)
**Permission:** Granted by original creator

---

## Available Documentation

### 1. [DECOMPILED_API.md](DECOMPILED_API.md) âœ¨ **PRIMARY REFERENCE**
Complete decompiled API documentation for **EliteMMO.API.dll** (.NET managed wrapper).

**Contains:**
- All 32 public classes and enums
- Complete method signatures with parameters
- All properties with get/set indicators
- Enum values with numeric mappings
- Constructors for all classes

**Use this for:** Understanding the full API surface, implementing features, and integration work.

### 2. [EliteMMO.API.dll.README.md](EliteMMO.API.dll.README.md)
High-level overview and architecture guide for the .NET managed wrapper.

**Contains:**
- Library purpose and capabilities
- Architecture diagrams
- Comparison with native EliteAPI.dll
- Usage examples and patterns
- Integration guidance

### 3. [EliteAPI.dll.README.md](EliteAPI.dll.README.md)
Documentation for the native C/C++ core library.

**Contains:**
- Core functionality overview
- Native function descriptions
- Memory management notes
- Integration requirements

---

## Quick Reference: Key Classes

### EliteAPI (Main Entry Point)
The primary class for interacting with FFXI. All functionality is accessed through this class or its property objects.

**Constructor:**
```csharp
EliteAPI api = new EliteAPI(processId);
```

**Key Properties:**
- `Chat` - Chat monitoring and message access
- `Player` - Player stats, HP, MP, position, etc.
- `Entity` - NPC, monster, and player entity data
- `Inventory` - Item and equipment management
- `Party` - Party and alliance information
- `Target` - Current target information
- `VanaTime` - In-game time and date
- `Weather` - Current weather conditions
- `Resources` - Item, spell, and ability lookups
- `Recast` - Spell and ability recast timers

### Important Enums

#### JobType
All FFXI job types (WAR, MNK, WHM, BLM, RDM, THF, etc.)

#### EntityStatus
Entity states (Idle, Engaged, Dead, Resting, Cutscene, etc.)

#### StorageContainer
Inventory containers (Inventory, Safe, Storage, Locker, Satchel, etc.)

#### StatusEffect
All status effects and buffs

---

## File Structure

```
libs/elite/
â"œâ"€â"€ EliteAPI.dll                    # Native C/C++ library (x86)
â"œâ"€â"€ EliteMMO.API.dll                # .NET managed wrapper (x86)
â"œâ"€â"€ DECOMPILED_API.md              # âœ¨ Complete decompiled reference
â"œâ"€â"€ EliteAPI.dll.README.md         # Native library overview
â"œâ"€â"€ EliteMMO.API.dll.README.md     # Managed wrapper overview
â"œâ"€â"€ README.md                      # This file
â"œâ"€â"€ extract_detailed_api.ps1       # Decompilation script (32-bit PS)
â""â"€â"€ API_DOCUMENTATION.md           # (Old/incomplete - see DECOMPILED_API.md)
```

---

## Usage Example

### C# (.NET) Integration

```csharp
using EliteMMO.API;

// Create API instance for FFXI process
int ffxiProcessId = 12345;  // Get from Process.GetProcessesByName("pol")
EliteAPI api = new EliteAPI(ffxiProcessId);

// Get player information
var playerInfo = api.Player.GetPlayerInfo();
Console.WriteLine($"Player: {playerInfo.Name}");
Console.WriteLine($"HP: {playerInfo.HP}/{playerInfo.HPMax}");
Console.WriteLine($"Job: {playerInfo.MainJob} Lv.{playerInfo.MainJobLevel}");

// Monitor chat messages
int chatLineCount = api.Chat.GetChatLineCount();
for (int i = 0; i < chatLineCount; i++) {
    var chatEntry = api.Chat.GetChatLineRaw(i);
    // Process chat entry...
}

// Get target information
var target = api.Target.GetTargetInfo();
if (target.TargetIndex > 0) {
    Console.WriteLine($"Target: {target.Name}");
    Console.WriteLine($"HP: {target.HPPercent}%");
}
```

### C++ (Native) Integration

```cpp
#include <windows.h>

// Function pointers
typedef void* (*CreateInstanceFunc)(DWORD processId);
typedef void (*DeleteInstanceFunc)(void* instance);

// Load EliteAPI.dll
HMODULE eliteApi = LoadLibrary("EliteAPI.dll");
CreateInstanceFunc CreateInstance = (CreateInstanceFunc)GetProcAddress(eliteApi, "CreateInstance");
DeleteInstanceFunc DeleteInstance = (DeleteInstanceFunc)GetProcAddress(eliteApi, "DeleteInstance");

// Create instance
void* apiInstance = CreateInstance(processId);

// Use API...

// Cleanup
DeleteInstance(apiInstance);
FreeLibrary(eliteApi);
```

---

## Important Notes

### Architecture Requirements
- Both DLLs are **32-bit (x86)** only
- Your application must target x86 architecture
- For 64-bit apps, you need a 32-bit proxy process

### Dependencies
- **EliteMMO.API.dll** depends on **EliteAPI.dll**
- Both DLLs must be in the same directory or in the system PATH
- .NET Framework 4.0+ required for managed wrapper

### Permission and Legal
- Original creator has granted permission for decompilation
- These DLLs interact with FFXI game memory
- Use responsibly and in accordance with game terms of service

---

## Decompilation Tools Used

### For .NET DLL (EliteMMO.API.dll)
- **ILSpy 9.1** - Open source .NET decompiler
- **.NET Reflection** - Runtime type inspection
- **32-bit PowerShell** - Required for x86 assembly loading

### For Native DLL (EliteAPI.dll)
- Binary analysis tools (IDA, Ghidra) can be used
- Export table inspection via `dumpbin` or similar
- Reverse engineering for detailed implementation

---

## Next Steps

1. **Read [DECOMPILED_API.md](DECOMPILED_API.md)** for complete API reference
2. Review example implementations in `src/Player/EliteAPI.cpp`
3. Check `includes/Player/EliteAPI.h` for C++ wrapper declarations
4. Test integration with your FFXI process

---

## Questions or Issues?

- Decompilation script: `extract_detailed_api.ps1`
- Update documentation: Re-run the script after DLL updates
- Missing information: Check DECOMPILED_API.md first, then use reflection tools

**Last Updated:** December 14, 2025
