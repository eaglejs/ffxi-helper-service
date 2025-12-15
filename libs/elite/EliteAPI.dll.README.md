# EliteAPI.dll Documentation

## Overview
**EliteAPI.dll** is a native C++ library that provides memory reading and manipulation capabilities for Final Fantasy XI. It serves as the core interface for interacting with the FFXI game client process, enabling real-time access to game state, chat messages, and player data.

- **Filename:** `EliteAPI.dll`
- **Architecture:** x86 (32-bit)
- **Language:** C++ (MSVC compiled)
- **Calling Convention:** `__stdcall` (WinAPI standard)
- **Dependencies:** `KERNEL32.dll`, `USER32.dll`, `ADVAPI32.dll`, `MSVCP140.dll`, `VCRUNTIME140.dll` (Visual C++ Redistributable)

## Analysis Status
This file has been analyzed using static binary analysis tools (`objdump`, `strings`).
- **Total Exports:** 160 functions
- **Compilation:** Visual Studio C++ (likely 2015/2017/2019)
- **Internals:** Contains traces of `std::map`, `std::string`, and an internal `EliteAPI::ResourceParser` class.

## Exported Functions

The DLL exports 160 functions. Most are "flat" C-style exports, though the underlying implementation is C++.

### Instance Management
| Ordinal | Function Name | Description |
|:-------:|:--------------|:------------|
| 4 | `CreateInstance` | Creates a new API instance attached to a process ID. |
| 6 | `DeleteInstance` | Destroys an API instance. |
| 106 | `Reinitialize` | Re-scans memory pointers for the instance. |

### Chat & Console
| Ordinal | Function Name | Description |
|:-------:|:--------------|:------------|
| 26 | `GetChatLineCount` | Returns total number of chat lines. |
| 27 | `GetChatLineRaw` | Returns a pointer to a specific chat line. |
| 107 | `SendString` | Sends a command or chat message to the game. |
| 9 | `FlushCommands` | Forces immediate sending of queued commands. |
| 1 | `ConsoleGetArg` | Retrieves a console argument. |
| 2 | `ConsoleGetArgCount` | Returns number of console arguments. |
| 3 | `ConsoleIsNewCommand` | Checks if a new command is present. |
| 5 | `CreateTextObject` | Creates an on-screen text overlay. |
| 7 | `DeleteTextObject` | Removes a text overlay. |

### Player & Entity
| Ordinal | Function Name | Description |
|:-------:|:--------------|:------------|
| 71 | `GetPlayerInfo` | Gets local player stats (HP, MP, TP, etc.). |
| 70 | `GetPlayerIndex` | Gets the entity index of the local player. |
| 76 | `GetPlayerZone` | Gets current zone ID. |
| 73 | `GetPlayerLoginStatus`| Gets login state (Login, Loading, In-Game). |
| 49 | `GetEntity` | Gets basic entity info (Name, ID, Index). |
| 50 | `GetEntityData` | Reads raw data from an entity. |
| 122 | `SetEntityData` | Writes raw data to an entity. |
| 51 | `GetEntityPointer` | Gets the memory address of an entity. |
| 89 | `GetTargetInfo` | Gets current target information. |
| 152 | `SetTarget` | Sets the current target by entity index. |
| 19-22 | `GetAutoFollow...` | Gets auto-follow state details. |
| 110-112 | `SetAutoFollow...` | Configures auto-follow. |

### Inventory & Items
| Ordinal | Function Name | Description |
|:-------:|:--------------|:------------|
| 28 | `GetContainerCount` | Gets item count in a container. |
| 29 | `GetContainerItem` | Gets item details from a container. |
| 52 | `GetEquippedItem` | Gets item in a specific equipment slot. |
| 60 | `GetItemById` | Looks up item data by ID. |
| 61 | `GetItemByName` | Looks up item data by name. |
| 10 | `GetAHItemCount` | Gets Auction House item count. |

### Abilities, Spells & Recasts
| Ordinal | Function Name | Description |
|:-------:|:--------------|:------------|
| 13 | `GetAbilityById` | Looks up ability data. |
| 84 | `GetSpellById` | Looks up spell data. |
| 17 | `GetAbilityRecast` | Gets time remaining on ability cooldown. |
| 86 | `GetSpellRecast` | Gets time remaining on spell cooldown. |
| 93 | `HasAbility` | Checks if player has an ability. |
| 97 | `HasSpell` | Checks if player has a spell. |

### Environment & Time
| Ordinal | Function Name | Description |
|:-------:|:--------------|:------------|
| 42 | `GetCurrentWeather` | Gets current weather ID. |
| 34-40 | `GetCurrent...` | Gets Vana'diel time (Year, Month, Day, etc.). |
| 39 | `GetCurrentMoonPhase`| Gets moon phase. |

### Fishing
| Ordinal | Function Name | Description |
|:-------:|:--------------|:------------|
| 53 | `GetFishFightTime` | Gets current fight duration. |
| 54-57 | `GetFishId1-4` | Gets IDs related to fishing. |
| 59 | `GetFishStamina` | Gets current fish stamina. |
| 94 | `HasFishOnLine` | Checks if a fish is hooked. |
| 8 | `FightFish` | Automates fishing fight logic. |

### Complete Export List
<details>
<summary>Click to expand full list (160 exports)</summary>

```
[0] ?BlockInput@ThirdParty@EliteAPI@@YGXPAX_N@Z
[1] ConsoleGetArg
[2] ConsoleGetArgCount
[3] ConsoleIsNewCommand
[4] CreateInstance
[5] CreateTextObject
[6] DeleteInstance
[7] DeleteTextObject
[8] FightFish
[9] FlushCommands
[10] GetAHItemCount
[11] GetAHItemCountLoaded
[12] GetAHItemIds
[13] GetAbilityById
[14] GetAbilityByName
[15] GetAbilityByTimerId
[16] GetAbilityId
[17] GetAbilityRecast
[18] GetAllianceInfo
[19] GetAutoFollowFollowId
[20] GetAutoFollowFollowIndex
[21] GetAutoFollowTargetId
[22] GetAutoFollowTargetIndex
[23] GetCastBarCount
[24] GetCastBarMax
[25] GetCastBarPercent
[26] GetChatLineCount
[27] GetChatLineRaw
[28] GetContainerCount
[29] GetContainerItem
[30] GetContainerMaxCount
[31] GetCraftItemCount
[32] GetCraftItemId
[33] GetCraftItemIndex
[34] GetCurrentDay
[35] GetCurrentHour
[36] GetCurrentMinute
[37] GetCurrentMonth
[38] GetCurrentMoonPercent
[39] GetCurrentMoonPhase
[40] GetCurrentSecond
[41] GetCurrentTimestamp
[42] GetCurrentWeather
[43] GetCurrentWeekDay
[44] GetCurrentYear
[45] GetDialogId
[46] GetDialogIndex
[47] GetDialogOptionCount
[48] GetDialogString
[49] GetEntity
[50] GetEntityData
[51] GetEntityPointer
[52] GetEquippedItem
[53] GetFishFightTime
[54] GetFishId1
[55] GetFishId2
[56] GetFishId3
[57] GetFishId4
[58] GetFishMaxStamina
[59] GetFishStamina
[60] GetItemById
[61] GetItemByName
[62] GetJobPoints
[63] GetMenuHelpName
[64] GetMenuHelpString
[65] GetMenuIndex
[66] GetMenuItemsCount
[67] GetMenuName
[68] GetPartyMembers
[69] GetPetTP
[70] GetPlayerIndex
[71] GetPlayerInfo
[72] GetPlayerJobLevels
[73] GetPlayerLoginStatus
[74] GetPlayerServerId
[75] GetPlayerViewMode
[76] GetPlayerZone
[77] GetRawVanaTime
[78] GetSelectedItemId
[79] GetSelectedItemIndex
[80] GetSelectedItemName
[81] GetSetBlueMagicSpells
[82] GetShopItemCount
[83] GetShopItemCountMax
[84] GetSpellById
[85] GetSpellByName
[86] GetSpellRecast
[87] GetString
[88] GetStringIndex
[89] GetTargetInfo
[90] GetTradeItemCount
[91] GetTradeItemId
[92] GetTradeItemIndex
[93] HasAbility
[94] HasFishOnLine
[95] HasKeyItem
[96] HasPetCommand
[97] HasSpell
[98] HasTrait
[99] HasWeaponSkill
[100] IsAHDoneLoadingItems
[101] IsAutoFollowing
[102] IsCraftMenuOpen
[103] IsCrafting
[104] IsMenuOpen
[105] IsTradeMenuOpen
[106] Reinitialize
[107] SendString
[108] SetActionTimer1
[109] SetActionTimer2
[110] SetAutoFollow
[111] SetAutoFollowCoords
[112] SetAutoFollowInfo
[113] SetBGBorderSize
[114] SetBGColor
[115] SetBGVisibility
[116] SetBazaarPrice
[117] SetBody
[118] SetBold
[119] SetColor
[120] SetCostumeId
[121] SetCraftItem
[122] SetEntityData
[123] SetFace
[124] SetFeet
[125] SetFishFightTime
[126] SetFishStamina
[127] SetFishingTimer
[128] SetFont
[129] SetHPosition
[130] SetHands
[131] SetHead
[132] SetItalic
[133] SetKey
[134] SetLegs
[135] SetLocation
[136] SetMain
[137] SetMenuIndex
[138] SetModelSize
[139] SetPlayerViewMode
[140] SetRace
[141] SetRanged
[142] SetRenderFlag00
[143] SetRenderFlag01
[144] SetRenderFlag02
[145] SetRenderFlag03
[146] SetRenderFlag04
[147] SetRightJustified
[148] SetShopItemCount
[149] SetSpeed
[150] SetStatus
[151] SetSub
[152] SetTarget
[153] SetText
[154] SetTradeItem
[155] SetVisibility
[156] SetXPosition
[157] SetYPosition
[158] SetZPosition
[159] BlockInput (Mangled alias: ?BlockInput@ThirdParty@EliteAPI@@YGXPAX_N@Z)
```
</details>

## Integration Details

### Header Definitions
To use these functions in C++, you should define function pointers with the `__stdcall` convention.

**Example Header:**
```cpp
#pragma once
#include <windows.h>

// Calling convention is stdcall
#define ELITE_API_CALL __stdcall

extern "C" {
    typedef void* (ELITE_API_CALL *tCreateInstance)(DWORD processId);
    typedef void  (ELITE_API_CALL *tDeleteInstance)(void* instance);
    typedef int   (ELITE_API_CALL *tGetChatLineCount)(void* instance);
    typedef const char* (ELITE_API_CALL *tGetChatLineRaw)(void* instance, int index);
}
```

### Loading the DLL
```cpp
HMODULE hDll = LoadLibrary("EliteAPI.dll");
if (hDll) {
    auto CreateInstance = (tCreateInstance)GetProcAddress(hDll, "CreateInstance");
    // ... load other functions
}
```

## Internal Notes
- **ResourceParser**: The DLL contains a `EliteAPI::ResourceParser` class, likely responsible for parsing game data files (DATs) for item/spell names.
- **ThirdParty**: The `BlockInput` function is namespaced under `EliteAPI::ThirdParty`, suggesting it might have been intended for external tool integration or protection.
- **String Handling**: The DLL makes heavy use of `std::string`. Pointers returned by functions like `GetChatLineRaw` point to internal buffers. **Do not free them.**
- **Thread Safety**: Not guaranteed. Accessing the same instance from multiple threads simultaneously is unsafe.