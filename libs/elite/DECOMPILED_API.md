# EliteMMO.API.dll - Complete Decompiled API Documentation

**Generated:** 2025-12-14 11:28:50
**Method:** .NET Reflection Decompilation
**Permission:** Granted by original creator

---

## AbilityType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.AbilityType
**Base Type:** Enum

### Enum Values

- `General` = 0
- `Job` = 1
- `Pet` = 2
- `Weapon` = 3
- `Trait` = 4
- `BloodPactRage` = 5
- `Corsair` = 6
- `CorsairShot` = 7
- `BloodPactWard` = 8
- `Samba` = 9
- `Waltz` = 10
- `Step` = 11
- `Florish1` = 12
- `Scholar` = 13
- `Jig` = 14
- `Flourish2` = 15
- `Monster` = 16
- `Flourish3` = 17
- `Weaponskill` = 18
- `Rune` = 19
- `Ward` = 20
- `Effusion` = 21

---

## CombatType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.CombatType
**Base Type:** Enum

### Enum Values

- `Magic` = 4096
- `Combat` = 8192

---

## CraftRank

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.CraftRank
**Base Type:** Enum

### Enum Values

- `Amateur` = 0
- `Recruit` = 1
- `Initiate` = 2
- `Novice` = 3
- `Apprentice` = 4
- `Journeyman` = 5
- `Craftsman` = 6
- `Artisan` = 7
- `Adept` = 8
- `Veteran` = 9

---

## ElementColor

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.ElementColor
**Base Type:** Enum

### Enum Values

- `Red` = 0
- `Clear` = 1
- `Green` = 2
- `Yellow` = 3
- `Purple` = 4
- `Blue` = 5
- `White` = 6
- `Black` = 7

---

## ElementType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.ElementType
**Base Type:** Enum

### Enum Values

- `Fire` = 0
- `Ice` = 1
- `Air` = 2
- `Earth` = 3
- `Thunder` = 4
- `Water` = 5
- `Light` = 6
- `Dark` = 7
- `Special` = 15
- `Unknown` = 255

---

## EliteAPI

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.EliteAPI
**Base Type:** Object

### Constructors

```csharp
EliteAPI(Int32 processId)
```

### Properties

| Property | Type | Get | Set |
|----------|------|-----|-----|
| `AuctionHouse` | `AuctionHouseTools` | âœ“ | âœ“ |
| `AutoFollow` | `AutoFollowTools` | âœ“ | âœ“ |
| `CastBar` | `CastBarTools` | âœ“ | âœ“ |
| `Chat` | `ChatTools` | âœ“ | âœ“ |
| `CraftMenu` | `CraftMenuTools` | âœ“ | âœ“ |
| `Dialog` | `DialogTools` | âœ“ | âœ“ |
| `Entity` | `EntityTools` | âœ“ | âœ“ |
| `Fish` | `FishTools` | âœ“ | âœ“ |
| `Inventory` | `InventoryTools` | âœ“ | âœ“ |
| `Menu` | `MenuTools` | âœ“ | âœ“ |
| `Party` | `PartyTools` | âœ“ | âœ“ |
| `Player` | `PlayerTools` | âœ“ | âœ“ |
| `Recast` | `RecastTools` | âœ“ | âœ“ |
| `Resources` | `ResourceTools` | âœ“ | âœ“ |
| `Target` | `TargetTools` | âœ“ | âœ“ |
| `ThirdParty` | `ThirdPartyTools` | âœ“ | âœ“ |
| `TradeMenu` | `TradeMenuTools` | âœ“ | âœ“ |
| `VanaTime` | `VanaTimeTools` | âœ“ | âœ“ |
| `Weather` | `WeatherTools` | âœ“ | âœ“ |

### Methods

#### BlockInput

```csharp
Void BlockInput(IntPtr apiObject, Boolean block)
```

#### ConsoleGetArg

```csharp
Int32 ConsoleGetArg(IntPtr apiObject, Int32 index, Byte[] lpBuffer, Int32 size)
```

#### ConsoleGetArgCount

```csharp
Int32 ConsoleGetArgCount(IntPtr apiObject)
```

#### ConsoleIsNewCommand

```csharp
Int32 ConsoleIsNewCommand(IntPtr apiObject)
```

#### CreateInstance

```csharp
IntPtr CreateInstance(Int32 processId)
```

#### CreateTextObject

```csharp
Void CreateTextObject(IntPtr apiObject, String name)
```

#### DeleteInstance

```csharp
Void DeleteInstance(IntPtr apiObject)
```

#### DeleteTextObject

```csharp
Void DeleteTextObject(IntPtr apiObject, String name)
```

#### FightFish

```csharp
Boolean FightFish(IntPtr apiObject)
```

#### FlushCommands

```csharp
Void FlushCommands(IntPtr apiObject)
```

#### GetAbilityById

```csharp
IntPtr GetAbilityById(IntPtr apiObject, UInt32 id)
```

#### GetAbilityByName

```csharp
IntPtr GetAbilityByName(IntPtr apiObject, String name, Int32 languageId)
```

#### GetAbilityByTimerId

```csharp
IntPtr GetAbilityByTimerId(IntPtr apiObject, UInt32 id)
```

#### GetAbilityId

```csharp
Int32 GetAbilityId(IntPtr apiObject, Int32 index)
```

#### GetAbilityRecast

```csharp
Int32 GetAbilityRecast(IntPtr apiObject, Int32 index)
```

#### GetAHItemCount

```csharp
Int32 GetAHItemCount(IntPtr apiObject)
```

#### GetAHItemCountLoaded

```csharp
Int32 GetAHItemCountLoaded(IntPtr apiObject)
```

#### GetAHItemIds

```csharp
Boolean GetAHItemIds(IntPtr apiObject, Byte[] lpBuffer)
```

#### GetAllianceInfo

```csharp
Boolean GetAllianceInfo(IntPtr apiObject, Byte[] lpBuffer)
```

#### GetAutoFollowFollowId

```csharp
UInt32 GetAutoFollowFollowId(IntPtr apiObject)
```

#### GetAutoFollowFollowIndex

```csharp
UInt32 GetAutoFollowFollowIndex(IntPtr apiObject)
```

#### GetAutoFollowTargetId

```csharp
UInt32 GetAutoFollowTargetId(IntPtr apiObject)
```

#### GetAutoFollowTargetIndex

```csharp
UInt32 GetAutoFollowTargetIndex(IntPtr apiObject)
```

#### GetCastBarCount

```csharp
Single GetCastBarCount(IntPtr apiObject)
```

#### GetCastBarMax

```csharp
Single GetCastBarMax(IntPtr apiObject)
```

#### GetCastBarPercent

```csharp
Single GetCastBarPercent(IntPtr apiObject)
```

#### GetChatLineCount

```csharp
Int32 GetChatLineCount(IntPtr apiObject)
```

#### GetChatLineRaw

```csharp
Boolean GetChatLineRaw(IntPtr apiObject, Int32 index, Byte[] buffer, Int32 size)
```

#### GetContainerCount

```csharp
Int32 GetContainerCount(IntPtr apiObject, Int32 containerId)
```

#### GetContainerItem

```csharp
Boolean GetContainerItem(IntPtr apiObject, Int32 containerId, Int32 itemIndex, Byte[] lpBuffer)
```

#### GetContainerMaxCount

```csharp
Int32 GetContainerMaxCount(IntPtr apiObject, Int32 containerId)
```

#### GetCraftItemCount

```csharp
Byte GetCraftItemCount(IntPtr apiObject, Int32 index)
```

#### GetCraftItemId

```csharp
UInt16 GetCraftItemId(IntPtr apiObject, Int32 index)
```

#### GetCraftItemIndex

```csharp
Byte GetCraftItemIndex(IntPtr apiObject, Int32 index)
```

#### GetCurrentDay

```csharp
UInt32 GetCurrentDay(IntPtr apiObject)
```

#### GetCurrentHour

```csharp
UInt32 GetCurrentHour(IntPtr apiObject)
```

#### GetCurrentMinute

```csharp
UInt32 GetCurrentMinute(IntPtr apiObject)
```

#### GetCurrentMonth

```csharp
UInt32 GetCurrentMonth(IntPtr apiObject)
```

#### GetCurrentMoonPercent

```csharp
UInt32 GetCurrentMoonPercent(IntPtr apiObject)
```

#### GetCurrentMoonPhase

```csharp
UInt32 GetCurrentMoonPhase(IntPtr apiObject)
```

#### GetCurrentSecond

```csharp
UInt32 GetCurrentSecond(IntPtr apiObject)
```

#### GetCurrentTimestamp

```csharp
UInt32 GetCurrentTimestamp(IntPtr apiObject, StringBuilder lpBuffer, Int32 nSize)
```

#### GetCurrentWeather

```csharp
Int32 GetCurrentWeather(IntPtr apiObject)
```

#### GetCurrentWeekDay

```csharp
UInt32 GetCurrentWeekDay(IntPtr apiObject)
```

#### GetCurrentYear

```csharp
UInt32 GetCurrentYear(IntPtr apiObject)
```

#### GetDialogId

```csharp
Int32 GetDialogId(IntPtr apiObject)
```

#### GetDialogIndex

```csharp
UInt16 GetDialogIndex(IntPtr apiObject)
```

#### GetDialogOptionCount

```csharp
Int32 GetDialogOptionCount(IntPtr apiObject)
```

#### GetDialogString

```csharp
Boolean GetDialogString(IntPtr apiObject, Byte[] lpBuffer, Int32 size)
```

#### GetEntity

```csharp
Boolean GetEntity(IntPtr apiObject, Int32 index, Byte[] lpBuffer)
```

#### GetEntityData

```csharp
Boolean GetEntityData(IntPtr apiObject, Int32 index, Int32 offset, Byte[] lpBuffer, Int32 size)
```

#### GetEntityPointer

```csharp
Int32 GetEntityPointer(IntPtr apiObject, Int32 index)
```

#### GetEquippedItem

```csharp
Boolean GetEquippedItem(IntPtr apiObject, Int32 slotId, Byte[] lpBuffer)
```

#### GetFishFightTime

```csharp
Int16 GetFishFightTime(IntPtr apiObject)
```

#### GetFishId1

```csharp
Int32 GetFishId1(IntPtr apiObject)
```

#### GetFishId2

```csharp
Int32 GetFishId2(IntPtr apiObject)
```

#### GetFishId3

```csharp
Int32 GetFishId3(IntPtr apiObject)
```

#### GetFishId4

```csharp
Int32 GetFishId4(IntPtr apiObject)
```

#### GetFishMaxStamina

```csharp
Int32 GetFishMaxStamina(IntPtr apiObject)
```

#### GetFishStamina

```csharp
Int32 GetFishStamina(IntPtr apiObject)
```

#### GetItemById

```csharp
IntPtr GetItemById(IntPtr apiObject, UInt32 id)
```

#### GetItemByName

```csharp
IntPtr GetItemByName(IntPtr apiObject, String name, Int32 languageId)
```

#### GetJobPoints

```csharp
Boolean GetJobPoints(IntPtr apiObject, Int32 jobId, Byte[] buffer)
```

#### GetMenuHelpName

```csharp
Boolean GetMenuHelpName(IntPtr apiObject, Byte[] lpBuffer, Int32 size)
```

#### GetMenuHelpString

```csharp
Boolean GetMenuHelpString(IntPtr apiObject, Byte[] lpBuffer, Int32 size)
```

#### GetMenuIndex

```csharp
Int32 GetMenuIndex(IntPtr apiObject)
```

#### GetMenuItemsCount

```csharp
Int32 GetMenuItemsCount(IntPtr apiObject)
```

#### GetMenuName

```csharp
Boolean GetMenuName(IntPtr apiObject, Byte[] lpBuffer, Int32 size)
```

#### GetPartyMembers

```csharp
Boolean GetPartyMembers(IntPtr apiObject, Byte[] lpBuffer)
```

#### GetPetTP

```csharp
UInt32 GetPetTP(IntPtr apiObject)
```

#### GetPlayerIndex

```csharp
Int32 GetPlayerIndex(IntPtr apiObject)
```

#### GetPlayerInfo

```csharp
Boolean GetPlayerInfo(IntPtr apiObject, Byte[] lpBuffer)
```

#### GetPlayerLoginStatus

```csharp
Int32 GetPlayerLoginStatus(IntPtr apiObject)
```

#### GetPlayerServerId

```csharp
Int32 GetPlayerServerId(IntPtr apiObject)
```

#### GetPlayerViewMode

```csharp
Int32 GetPlayerViewMode(IntPtr apiObject)
```

#### GetPlayerZone

```csharp
Int32 GetPlayerZone(IntPtr apiObject)
```

#### GetRawVanaTime

```csharp
UInt64 GetRawVanaTime(IntPtr apiObject)
```

#### GetSelectedItemId

```csharp
UInt32 GetSelectedItemId(IntPtr apiObject)
```

#### GetSelectedItemIndex

```csharp
UInt32 GetSelectedItemIndex(IntPtr apiObject)
```

#### GetSelectedItemName

```csharp
UInt32 GetSelectedItemName(IntPtr apiObject, StringBuilder lpBuffer, Int32 nSize)
```

#### GetSetBlueMagicSpells

```csharp
Boolean GetSetBlueMagicSpells(IntPtr apiObject, Byte[] buffer)
```

#### GetShopItemCount

```csharp
UInt32 GetShopItemCount(IntPtr apiObject)
```

#### GetShopItemCountMax

```csharp
UInt32 GetShopItemCountMax(IntPtr apiObject)
```

#### GetSpellById

```csharp
IntPtr GetSpellById(IntPtr apiObject, UInt32 id)
```

#### GetSpellByName

```csharp
IntPtr GetSpellByName(IntPtr apiObject, String name, Int32 languageId)
```

#### GetSpellRecast

```csharp
Int32 GetSpellRecast(IntPtr apiObject, Int32 index)
```

#### GetString

```csharp
IntPtr GetString(IntPtr apiObject, String table, UInt32 index, Int32 languageId)
```

#### GetStringIndex

```csharp
UInt32 GetStringIndex(IntPtr apiObject, String table, String name, Int32 languageId)
```

#### GetTargetInfo

```csharp
Boolean GetTargetInfo(IntPtr apiObject, Byte[] lpBuffer)
```

#### GetTradeItemCount

```csharp
Byte GetTradeItemCount(IntPtr apiObject, Int32 index)
```

#### GetTradeItemId

```csharp
UInt16 GetTradeItemId(IntPtr apiObject, Int32 index)
```

#### GetTradeItemIndex

```csharp
Byte GetTradeItemIndex(IntPtr apiObject, Int32 index)
```

#### HasAbility

```csharp
Boolean HasAbility(IntPtr apiObject, UInt32 id)
```

#### HasFishOnLine

```csharp
Boolean HasFishOnLine(IntPtr apiObject)
```

#### HasKeyItem

```csharp
Boolean HasKeyItem(IntPtr apiObject, UInt32 id)
```

#### HasPetCommand

```csharp
Boolean HasPetCommand(IntPtr apiObject, UInt32 id)
```

#### HasSpell

```csharp
Boolean HasSpell(IntPtr apiObject, UInt32 id)
```

#### HasTrait

```csharp
Boolean HasTrait(IntPtr apiObject, UInt32 id)
```

#### HasWeaponSkill

```csharp
Boolean HasWeaponSkill(IntPtr apiObject, UInt32 id)
```

#### IsAHDoneLoadingItems

```csharp
Boolean IsAHDoneLoadingItems(IntPtr apiObject)
```

#### IsAutoFollowing

```csharp
Boolean IsAutoFollowing(IntPtr apiObject)
```

#### IsCrafting

```csharp
Boolean IsCrafting(IntPtr apiObject)
```

#### IsCraftMenuOpen

```csharp
Boolean IsCraftMenuOpen(IntPtr apiObject)
```

#### IsMenuOpen

```csharp
Boolean IsMenuOpen(IntPtr apiObject)
```

#### IsTradeMenuOpen

```csharp
Boolean IsTradeMenuOpen(IntPtr apiObject)
```

#### Reinitialize

```csharp
Boolean Reinitialize(IntPtr apiObject, Int32 processId)
```

#### Reinitialize

```csharp
Boolean Reinitialize(Int32 processId)
```

#### SendString

```csharp
Void SendString(IntPtr apiObject, String str)
```

#### SetActionTimer1

```csharp
Boolean SetActionTimer1(IntPtr apiObject, Int32 index, UInt16 timer)
```

#### SetActionTimer2

```csharp
Boolean SetActionTimer2(IntPtr apiObject, Int32 index, UInt16 timer)
```

#### SetAutoFollow

```csharp
Boolean SetAutoFollow(IntPtr apiObject, Boolean follow)
```

#### SetAutoFollowCoords

```csharp
Boolean SetAutoFollowCoords(IntPtr apiObject, Single fX, Single fY, Single fZ)
```

#### SetAutoFollowInfo

```csharp
Boolean SetAutoFollowInfo(IntPtr apiObject, UInt32 tIndex, UInt32 tId, UInt32 fIndex, UInt32 fId)
```

#### SetBazaarPrice

```csharp
Boolean SetBazaarPrice(IntPtr apiObject, Int32 price)
```

#### SetBGBorderSize

```csharp
Void SetBGBorderSize(IntPtr apiObject, String name, Single size)
```

#### SetBGColor

```csharp
Void SetBGColor(IntPtr apiObject, String name, Byte a, Byte r, Byte g, Byte b)
```

#### SetBGVisibility

```csharp
Void SetBGVisibility(IntPtr apiObject, String name, Boolean visible)
```

#### SetBody

```csharp
Boolean SetBody(IntPtr apiObject, Int32 index, UInt16 bodyId)
```

#### SetBold

```csharp
Void SetBold(IntPtr apiObject, String name, Boolean bold)
```

#### SetColor

```csharp
Void SetColor(IntPtr apiObject, String name, Byte a, Byte r, Byte g, Byte b)
```

#### SetCostumeId

```csharp
Boolean SetCostumeId(IntPtr apiObject, Int32 index, UInt16 costumeId)
```

#### SetCraftItem

```csharp
Boolean SetCraftItem(IntPtr apiObject, Int32 index, UInt16 itemId, Byte itemIndex, Byte itemCount)
```

#### SetEntityData

```csharp
Boolean SetEntityData(IntPtr apiObject, Int32 index, Int32 offset, Byte[] lpBuffer, Int32 size)
```

#### SetFace

```csharp
Boolean SetFace(IntPtr apiObject, Int32 index, UInt16 faceId)
```

#### SetFeet

```csharp
Boolean SetFeet(IntPtr apiObject, Int32 index, UInt16 feetId)
```

#### SetFishFightTime

```csharp
Boolean SetFishFightTime(IntPtr apiObject, Int16 time)
```

#### SetFishingTimer

```csharp
Boolean SetFishingTimer(IntPtr apiObject, Int32 index, UInt16 timer)
```

#### SetFishStamina

```csharp
Boolean SetFishStamina(IntPtr apiObject, Int32 stamina)
```

#### SetFont

```csharp
Void SetFont(IntPtr apiObject, String name, String font, Int32 height)
```

#### SetHands

```csharp
Boolean SetHands(IntPtr apiObject, Int32 index, UInt16 handsId)
```

#### SetHead

```csharp
Boolean SetHead(IntPtr apiObject, Int32 index, UInt16 headId)
```

#### SetHPosition

```csharp
Boolean SetHPosition(IntPtr apiObject, Int32 index, Single pos)
```

#### SetItalic

```csharp
Void SetItalic(IntPtr apiObject, String name, Boolean italic)
```

#### SetKey

```csharp
Void SetKey(IntPtr apiObject, Byte key, Boolean down)
```

#### SetLegs

```csharp
Boolean SetLegs(IntPtr apiObject, Int32 index, UInt16 legsId)
```

#### SetLocation

```csharp
Void SetLocation(IntPtr apiObject, String name, Single x, Single y)
```

#### SetMain

```csharp
Boolean SetMain(IntPtr apiObject, Int32 index, UInt16 mainId)
```

#### SetMenuIndex

```csharp
Boolean SetMenuIndex(IntPtr apiObject, Int32 index)
```

#### SetModelSize

```csharp
Boolean SetModelSize(IntPtr apiObject, Int32 index, Single size)
```

#### SetPlayerViewMode

```csharp
Boolean SetPlayerViewMode(IntPtr apiObject, Int32 viewMode)
```

#### SetRace

```csharp
Boolean SetRace(IntPtr apiObject, Int32 index, Byte raceId)
```

#### SetRanged

```csharp
Boolean SetRanged(IntPtr apiObject, Int32 index, UInt16 rangedId)
```

#### SetRenderFlag00

```csharp
Boolean SetRenderFlag00(IntPtr apiObject, Int32 index, UInt32 flag)
```

#### SetRenderFlag01

```csharp
Boolean SetRenderFlag01(IntPtr apiObject, Int32 index, UInt32 flag)
```

#### SetRenderFlag02

```csharp
Boolean SetRenderFlag02(IntPtr apiObject, Int32 index, UInt32 flag)
```

#### SetRenderFlag03

```csharp
Boolean SetRenderFlag03(IntPtr apiObject, Int32 index, UInt32 flag)
```

#### SetRenderFlag04

```csharp
Boolean SetRenderFlag04(IntPtr apiObject, Int32 index, UInt32 flag)
```

#### SetRightJustified

```csharp
Void SetRightJustified(IntPtr apiObject, String name, Boolean rightjustified)
```

#### SetShopItemCount

```csharp
Void SetShopItemCount(IntPtr apiObject, Byte value)
```

#### SetSpeed

```csharp
Boolean SetSpeed(IntPtr apiObject, Int32 index, Single speed)
```

#### SetStatus

```csharp
Boolean SetStatus(IntPtr apiObject, Int32 index, UInt32 status)
```

#### SetSub

```csharp
Boolean SetSub(IntPtr apiObject, Int32 index, UInt16 subId)
```

#### SetTarget

```csharp
Boolean SetTarget(IntPtr apiObject, Int32 index)
```

#### SetText

```csharp
Void SetText(IntPtr apiObject, String name, String text)
```

#### SetTradeItem

```csharp
Boolean SetTradeItem(IntPtr apiObject, Int32 index, UInt16 itemId, Byte itemIndex, Byte itemCount)
```

#### SetVisibility

```csharp
Void SetVisibility(IntPtr apiObject, String name, Boolean visible)
```

#### SetXPosition

```csharp
Boolean SetXPosition(IntPtr apiObject, Int32 index, Single pos)
```

#### SetYPosition

```csharp
Boolean SetYPosition(IntPtr apiObject, Int32 index, Single pos)
```

#### SetZPosition

```csharp
Boolean SetZPosition(IntPtr apiObject, Int32 index, Single pos)
```

---

## EntityHairType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.EntityHairType
**Base Type:** Enum

### Enum Values

- `Hair1A` = 0
- `Hair1B` = 1
- `Hair2A` = 2
- `Hair2B` = 3
- `Hair3A` = 4
- `Hair3B` = 5
- `Hair4A` = 6
- `Hair4B` = 7
- `Hair5A` = 8
- `Hair5B` = 9
- `Hair6A` = 10
- `Hair6B` = 11
- `Hair7A` = 12
- `Hair7B` = 13
- `Hair8A` = 14
- `Hair8B` = 15
- `Fomar` = 29
- `Mannequin` = 30

---

## EntityRace

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.EntityRace
**Base Type:** Enum

### Enum Values

- `Invalid` = 0
- `HumeMale` = 1
- `HumeFemale` = 2
- `ElvaanMale` = 3
- `ElvaanFemale` = 4
- `TarutaruMale` = 5
- `TarutaruFemale` = 6
- `Mithra` = 7
- `Galka` = 8
- `MithraChild` = 29
- `HumeChildFemale` = 30
- `HumeChildMale` = 31
- `GoldChocobo` = 32
- `BlackChocobo` = 33
- `BlueChocobo` = 34
- `RedChocobo` = 35
- `GreenChocobo` = 36

---

## EntitySpawnFlags

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.EntitySpawnFlags
**Base Type:** Enum

### Enum Values

- `Player` = 1
- `Npc` = 2
- `PartyMember` = 4
- `AllianceMember` = 8
- `Monster` = 16
- `Object` = 32
- `LocalPlayer` = 512

---

## EntityStatus

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.EntityStatus
**Base Type:** Enum

### Enum Values

- `Idle` = 0
- `Engaged` = 1
- `Dead` = 2
- `DeadEngaged` = 3
- `Event` = 4
- `Chocobo` = 5
- `_OldFishingIdle1` = 6
- `Kneel1` = 7
- `DoorOpened` = 8
- `DoorClosed` = 9
- `Maintenance01` = 28
- `Maintenance02` = 31
- `Healing` = 33
- `_OldFishingBite` = 38
- `_OldFishingCatch` = 39
- `_OldFishingRodBreak` = 40
- `_OldFishingLineBreak` = 41
- `_OldFishingCatchMonster` = 42
- `FishingNoCatch` = 43
- `Synthing` = 44
- `Sitting` = 47
- `Kneel2` = 48
- `_OldFishingIdle2` = 50
- `_OldFishingRodCenter` = 51
- `_OldFishingRodRight` = 52
- `_OldFishingRodLeft` = 53
- `FishingIdle` = 56
- `FishingBite` = 57
- `FishingCatch` = 58
- `FishingRodBreak` = 59
- `FishingLineBreak` = 60
- `FishingCatchMonster` = 61
- `FishingCancel` = 62
- `SitChair` = 63
- `Unconscious` = 64
- `UnknownEffect` = 72

---

## EntityTypes

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.EntityTypes
**Base Type:** Enum

### Enum Values

- `Player` = 0
- `Npc1` = 1
- `Npc2` = 2
- `Npc3` = 3
- `Elevator` = 4
- `Airship` = 5

---

## EquipmentSlot

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.EquipmentSlot
**Base Type:** Enum

### Enum Values

- `Main` = 0
- `Sub` = 1
- `Range` = 2
- `Ammo` = 3
- `Head` = 4
- `Body` = 5
- `Hands` = 6
- `Legs` = 7
- `Feet` = 8
- `Neck` = 9
- `Waist` = 10
- `Ear1` = 11
- `Ear2` = 12
- `Ring1` = 13
- `Ring2` = 14
- `Back` = 15

---

## EquipmentSlotMask

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.EquipmentSlotMask
**Base Type:** Enum

### Enum Values

- `None` = 0
- `Main` = 1
- `Sub` = 2
- `Range` = 4
- `Ammo` = 8
- `Head` = 16
- `Body` = 32
- `Hands` = 64
- `Legs` = 128
- `Feet` = 256
- `Neck` = 512
- `Waist` = 1024
- `LEar` = 2048
- `REar` = 4096
- `Ears` = 6144
- `LRing` = 8192
- `RRing` = 16384
- `Rings` = 24576
- `Back` = 32768
- `All` = 65535

---

## ItemFlagsMask

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.ItemFlagsMask
**Base Type:** Enum

### Enum Values

- `None` = 0
- `WallHanging` = 1
- `Flag1` = 2
- `Flag2` = 4
- `Flag3` = 8
- `DeliveryInner` = 16
- `Inscribable` = 32
- `NoAuction` = 64
- `Scroll` = 128
- `Linkshell` = 256
- `CanUse` = 512
- `CanTradeNpc` = 1024
- `CanEquip` = 2048
- `NoSale` = 4096
- `NoDelivery` = 8192
- `NoTrade` = 16384
- `Exclusive` = 24640
- `Rare` = 32768
- `Nothing` = 61760

---

## ItemType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.ItemType
**Base Type:** Enum

### Enum Values

- `None` = 0
- `Item` = 1
- `QuestItem` = 2
- `Fish` = 3
- `Weapon` = 4
- `Armor` = 5
- `Linkshell` = 6
- `UsableItem` = 7
- `Crystal` = 8
- `Currency` = 9
- `Furnishing` = 10
- `Plant` = 11
- `Flowerpot` = 12
- `PuppetItem` = 13
- `Mannequin` = 14
- `Book` = 15
- `RacingForm` = 16
- `BettingSlip` = 17
- `SoulPlate` = 18
- `Reflector` = 19
- `Logs` = 20
- `LotteryTicket` = 21
- `TabulaM` = 22
- `TabulaR` = 23
- `Voucher` = 24
- `Rune` = 25
- `Evolith` = 26
- `StorageSlip` = 27
- `Type1` = 28

---

## JobMask

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.JobMask
**Base Type:** Enum

### Enum Values

- `None` = 0
- `WAR` = 2
- `MNK` = 4
- `WHM` = 8
- `BLM` = 16
- `RDM` = 32
- `THF` = 64
- `PLD` = 128
- `DRK` = 256
- `BST` = 512
- `BRD` = 1024
- `RNG` = 2048
- `SAM` = 4096
- `NIN` = 8192
- `DRG` = 16384
- `SMN` = 32768
- `BLU` = 65536
- `COR` = 131072
- `PUP` = 262144
- `DNC` = 524288
- `SCH` = 1048576
- `GEO` = 2097152
- `RUN` = 4194304
- `AllJobs` = 8388606
- `MON` = 8388608
- `JOB24` = 16777216
- `JOB25` = 33554432
- `JOB26` = 67108864
- `JOB27` = 134217728
- `JOB28` = 268435456
- `JOB29` = 536870912
- `JOB30` = 1073741824
- `JOB31` = 2147483648

---

## JobType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.JobType
**Base Type:** Enum

### Enum Values

- `None` = 0
- `Warrior` = 1
- `Monk` = 2
- `WhiteMage` = 3
- `BlackMage` = 4
- `RedMage` = 5
- `Thief` = 6
- `Paladin` = 7
- `DarkKnight` = 8
- `BeastMaster` = 9
- `Bard` = 10
- `Ranger` = 11
- `Samurai` = 12
- `Ninja` = 13
- `Dragoon` = 14
- `Summon` = 15
- `BlueMage` = 16
- `Corsair` = 17
- `PuppetMaster` = 18
- `Dancer` = 19
- `Scholar` = 20
- `Geomancer` = 21
- `RuneFencer` = 22

---

## Keys

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.Keys
**Base Type:** Enum

### Enum Values

- `ESCAPE` = 1
- `_1` = 2
- `_2` = 3
- `_3` = 4
- `_4` = 5
- `_5` = 6
- `_6` = 7
- `_7` = 8
- `_8` = 9
- `_9` = 10
- `_0` = 11
- `MINUS` = 12
- `EQUALS` = 13
- `BACK` = 14
- `TAB` = 15
- `Q` = 16
- `W` = 17
- `E` = 18
- `R` = 19
- `T` = 20
- `Y` = 21
- `U` = 22
- `I` = 23
- `O` = 24
- `P` = 25
- `LBRACKET` = 26
- `RBRACKET` = 27
- `RETURN` = 28
- `LCONTROL` = 29
- `A` = 30
- `S` = 31
- `D` = 32
- `F` = 33
- `G` = 34
- `H` = 35
- `J` = 36
- `K` = 37
- `L` = 38
- `SEMICOLON` = 39
- `APOSTROPHE` = 40
- `GRAVE` = 41
- `LSHIFT` = 42
- `BACKSLASH` = 43
- `Z` = 44
- `X` = 45
- `C` = 46
- `V` = 47
- `B` = 48
- `N` = 49
- `M` = 50
- `COMMA` = 51
- `PERIOD` = 52
- `SLASH` = 53
- `RSHIFT` = 54
- `MULTIPLY` = 55
- `LMENU` = 56
- `SPACE` = 57
- `CAPITAL` = 58
- `F1` = 59
- `F2` = 60
- `F3` = 61
- `F4` = 62
- `F5` = 63
- `F6` = 64
- `F7` = 65
- `F8` = 66
- `F9` = 67
- `F10` = 68
- `NUMLOCK` = 69
- `SCROLL` = 70
- `NUMPAD7` = 71
- `NUMPAD8` = 72
- `NUMPAD9` = 73
- `SUBTRACT` = 74
- `NUMPAD4` = 75
- `NUMPAD5` = 76
- `NUMPAD6` = 77
- `ADD` = 78
- `NUMPAD1` = 79
- `NUMPAD2` = 80
- `NUMPAD3` = 81
- `NUMPAD0` = 82
- `DECIMAL` = 83
- `OEM_102` = 86
- `F11` = 87
- `F12` = 88
- `F13` = 100
- `F14` = 101
- `F15` = 102
- `KANA` = 112
- `ABNT_C1` = 115
- `CONVERT` = 121
- `NOCONVERT` = 123
- `YEN` = 125
- `ABNT_C2` = 126
- `NUMPADEQUALS` = 141
- `PREVTRACK` = 144
- `AT` = 145
- `COLON` = 146
- `UNDERLINE` = 147
- `KANJI` = 148
- `STOP` = 149
- `AX` = 150
- `UNLABELED` = 151
- `NEXTTRACK` = 153
- `NUMPADENTER` = 156
- `RCONTROL` = 157
- `MUTE` = 160
- `CALCULATOR` = 161
- `PLAYPAUSE` = 162
- `MEDIASTOP` = 164
- `VOLUMEDOWN` = 174
- `VOLUMEUP` = 176
- `WEBHOME` = 178
- `NUMPADCOMMA` = 179
- `DIVIDE` = 181
- `SYSRQ` = 183
- `RMENU` = 184
- `PAUSE` = 197
- `HOME` = 199
- `UP` = 200
- `PRIOR` = 201
- `LEFT` = 203
- `RIGHT` = 205
- `END` = 207
- `DOWN` = 208
- `NEXT` = 209
- `INSERT` = 210
- `DELETE` = 211
- `LWIN` = 219
- `RWIN` = 220
- `APPS` = 221
- `POWER` = 222
- `SLEEP` = 223
- `WAKE` = 227
- `WEBSEARCH` = 229
- `WEBFAVORITES` = 230
- `WEBREFRESH` = 231
- `WEBSTOP` = 232
- `WEBFORWARD` = 233
- `WEBBACK` = 234
- `MYCOMPUTER` = 235
- `MAIL` = 236
- `MEDIASELECT` = 237

---

## LoginStatus

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.LoginStatus
**Base Type:** Enum

### Enum Values

- `LoginScreen` = 0
- `Loading` = 1
- `LoggedIn` = 2

---

## MagicType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.MagicType
**Base Type:** Enum

### Enum Values

- `None` = 0
- `WhiteMagic` = 1
- `BlackMagic` = 2
- `Summon` = 3
- `Ninjutsu` = 4
- `Song` = 5
- `BlueMagic` = 6
- `Geomancy` = 7
- `Trust` = 8

---

## MoonPhase

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.MoonPhase
**Base Type:** Enum

### Enum Values

- `New` = 0
- `WaxingCrescent` = 1
- `WaxingCrescent2` = 2
- `FirstQuarter` = 3
- `WaxingGibbous` = 4
- `WaxingGibbous2` = 5
- `Full` = 6
- `WaningGibbous` = 7
- `WaningGibbous2` = 8
- `LastQuarter` = 9
- `WaningCrescent` = 10
- `WaningCrescent2` = 11
- `Unknown` = 12

---

## Nations

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.Nations
**Base Type:** Enum

### Enum Values

- `SandOria` = 0
- `Bastok` = 1
- `Windurst` = 2

---

## PuppetSlot

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.PuppetSlot
**Base Type:** Enum

### Enum Values

- `None` = 0
- `Head` = 1
- `Body` = 2
- `Attachment` = 3

---

## RaceMask

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.RaceMask
**Base Type:** Enum

### Enum Values

- `None` = 0
- `HumeMale` = 2
- `HumeFemale` = 4
- `Hume` = 6
- `ElvaanMale` = 8
- `ElvaanFemale` = 16
- `Elvaan` = 24
- `TarutaruMale` = 32
- `TarutaruFemale` = 64
- `Tarutaru` = 96
- `Mithra` = 128
- `Female` = 212
- `Galka` = 256
- `Male` = 298
- `All` = 510

---

## SkillType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.SkillType
**Base Type:** Enum

### Enum Values

- `HandToHand` = 1
- `Dagger` = 2
- `Sword` = 3
- `GreatSword` = 4
- `Axe` = 5
- `GreatAxe` = 6
- `Scythe` = 7
- `Polarm` = 8
- `Katana` = 9
- `GreatKatana` = 10
- `Club` = 11
- `Staff` = 12
- `Archery` = 25
- `Marksmanship` = 26
- `Throwing` = 27
- `Guard` = 28
- `Evasion` = 29
- `Shield` = 30
- `Parry` = 31
- `Divine` = 32
- `Healing` = 33
- `Enhancing` = 34
- `Enfeebling` = 35
- `Elemental` = 36
- `Dark` = 37
- `Summoning` = 38
- `Ninjitsu` = 39
- `Singing` = 40
- `String` = 41
- `Wind` = 42
- `BlueMagic` = 43
- `Fishing` = 48
- `Woodworking` = 49
- `Smithing` = 50
- `Goldsmithing` = 51
- `Clothcraft` = 52
- `Leathercraft` = 53
- `Bonecraft` = 54
- `Alchemy` = 55
- `Cooking` = 56
- `Synergy` = 57
- `ChocoboDigging` = 58

---

## StatusEffect

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.StatusEffect
**Base Type:** Enum

### Enum Values

- `KO` = 0
- `Weakness` = 1
- `Sleep` = 2
- `Poison` = 3
- `Paralysis` = 4
- `Blindness` = 5
- `Silence` = 6
- `Petrification` = 7
- `Disease` = 8
- `Curse` = 9
- `Stun` = 10
- `Bind` = 11
- `Weight` = 12
- `Slow` = 13
- `Charm1` = 14
- `Doom` = 15
- `Amnesia` = 16
- `Charm2` = 17
- `Gradual_Petrification` = 18
- `Sleep2` = 19
- `Curse2` = 20
- `Addle` = 21
- `Terror` = 28
- `Mute` = 29
- `Bane` = 30
- `Plague` = 31
- `Flee` = 32
- `Haste` = 33
- `Blaze_Spikes` = 34
- `Ice_Spikes` = 35
- `Blink` = 36
- `Stoneskin` = 37
- `Shock_Spikes` = 38
- `Aquaveil` = 39
- `Protect` = 40
- `Shell` = 41
- `Regen` = 42
- `Refresh` = 43
- `Mighty_Strikes` = 44
- `Boost` = 45
- `Hundred_Fists` = 46
- `Manafont` = 47
- `Chainspell` = 48
- `Perfect_Dodge` = 49
- `Invincible` = 50
- `Blood_Weapon` = 51
- `Soul_Voice` = 52
- `Eagle_Eye_Shot` = 53
- `Meikyo_Shisui` = 54
- `Astral_Flow` = 55
- `Berserk` = 56
- `Defender` = 57
- `Aggressor` = 58
- `Focus` = 59
- `Dodge` = 60
- `Counterstance` = 61
- `Sentinel` = 62
- `Souleater` = 63
- `Last_Resort` = 64
- `Sneak_Attack` = 65
- `Utsusemi_1_Shadow_Left` = 66
- `Third_Eye` = 67
- `Warcry` = 68
- `Invisible` = 69
- `Deodorize` = 70
- `Sneak` = 71
- `Sharpshot` = 72
- `Barrage` = 73
- `Holy_Circle` = 74
- `Arcane_Circle` = 75
- `Hide` = 76
- `Camouflage` = 77
- `Divine_Seal` = 78
- `Elemental_Seal` = 79
- `STR_Boost1` = 80
- `DEX_Boost1` = 81
- `VIT_Boost1` = 82
- `AGI_Boost1` = 83
- `INT_Boost1` = 84
- `MND_Boost1` = 85
- `CHR_Boost1` = 86
- `Trick_Attack` = 87
- `Max_HP_Boost` = 88
- `Max_MP_Boost` = 89
- `Accuracy_Boost` = 90
- `Attack_Boost` = 91
- `Evasion_Boost` = 92
- `Defense_Boost` = 93
- `Enfire` = 94
- `Enblizzard` = 95
- `Enaero` = 96
- `Enstone` = 97
- `Enthunder` = 98
- `Enwater` = 99
- `Barfire` = 100
- `Barblizzard` = 101
- `Baraero` = 102
- `Barstone` = 103
- `Barthunder` = 104
- `Barwater` = 105
- `Barsleep` = 106
- `Barpoison` = 107
- `Barparalyze` = 108
- `Barblind` = 109
- `Barsilence` = 110
- `Barpetrify` = 111
- `Barvirus` = 112
- `Reraise` = 113
- `Cover` = 114
- `Unlimited_Shot` = 115
- `Phalanx` = 116
- `Warding_Circle` = 117
- `Ancient_Circle` = 118
- `STR_Boost2` = 119
- `DEX_Boost2` = 120
- `VIT_Boost2` = 121
- `AGI_Boost2` = 122
- `INT_Boost2` = 123
- `MND_Boost2` = 124
- `CHR_Boost2` = 125
- `Spirit_Surge` = 126
- `Costume` = 127
- `Burn` = 128
- `Frost` = 129
- `Choke` = 130
- `Rasp` = 131
- `Shock` = 132
- `Drown` = 133
- `Dia` = 134
- `Bio` = 135
- `STR_Down` = 136
- `DEX_Down` = 137
- `VIT_Down` = 138
- `AGI_Down` = 139
- `INT_Down` = 140
- `MND_Down` = 141
- `CHR_Down` = 142
- `Level_Restriction` = 143
- `Max_HP_Down` = 144
- `Max_MP_Down` = 145
- `Accuracy_Down` = 146
- `Attack_Down` = 147
- `Evasion_Down` = 148
- `Defense_Down` = 149
- `Physical_Shield` = 150
- `Arrow_Shield` = 151
- `Magic_Shield` = 152
- `Damage_Spikes` = 153
- `Shining_Ruby` = 154
- `Medicine` = 155
- `Flash` = 156
- `Subjob_Restriction` = 157
- `Provoke` = 158
- `Penalty` = 159
- `Preparations` = 160
- `Sprint` = 161
- `Enchantment` = 162
- `Azure_Lore` = 163
- `Chain_Affinity` = 164
- `Burst_Affinity` = 165
- `Overdrive` = 166
- `Magic_Def_Down` = 167
- `Inhibit_TP` = 168
- `Potency` = 169
- `Regain` = 170
- `Pax` = 171
- `Intension` = 172
- `Dread_Spikes` = 173
- `Magic_Acc_Down` = 174
- `Magic_Atk_Down` = 175
- `Quickening` = 176
- `Encumbrance` = 177
- `Firestorm` = 178
- `Hailstorm` = 179
- `Windstorm` = 180
- `Sandstorm` = 181
- `Thunderstorm` = 182
- `Rainstorm` = 183
- `Aurorastorm` = 184
- `Voidstorm` = 185
- `Helix` = 186
- `Sublimation_Activated` = 187
- `Sublimation_Complete` = 188
- `Max_TP_Down` = 189
- `Magic_Atk_Boost` = 190
- `Magic_Def_Boost` = 191
- `Requiem` = 192
- `Lullaby` = 193
- `Elegy` = 194
- `Paeon` = 195
- `Ballad` = 196
- `Minne` = 197
- `Minuet` = 198
- `Madrigal` = 199
- `Prelude` = 200
- `Mambo` = 201
- `Aubade` = 202
- `Pastoral` = 203
- `Hum` = 204
- `Fantasia` = 205
- `Operetta` = 206
- `Capriccio` = 207
- `Serenade` = 208
- `Round` = 209
- `Gavotte` = 210
- `Fugue` = 211
- `Rhapsody` = 212
- `Aria` = 213
- `March` = 214
- `Etude` = 215
- `Carol` = 216
- `Threnody` = 217
- `Hymnus` = 218
- `Mazurka` = 219
- `Sirvente` = 220
- `Dirge` = 221
- `Scherzo` = 222
- `Auto_Regen` = 233
- `Auto_Refresh` = 234
- `Fishing_Imagery` = 235
- `Woodworking_Imagery` = 236
- `Smithing_Imagery` = 237
- `Goldsmithing_Imagery` = 238
- `Clothcraft_Imagery` = 239
- `Leathercraft_Imagery` = 240
- `Bonecraft_Imagery` = 241
- `Alchemy_Imagery` = 242
- `Cooking_Imagery` = 243
- `Dedication` = 249
- `Ef_Badge` = 250
- `Food` = 251
- `Chocobo` = 252
- `Signet` = 253
- `Battlefield` = 254
- `Sanction` = 256
- `Besieged` = 257
- `Illusion` = 258
- `No_Weapons_Armor` = 259
- `No_Support_Job` = 260
- `No_Job_Abilities` = 261
- `No_Magic_Casting` = 262
- `Penalty_to_Attributes` = 263
- `Pathos` = 264
- `Flurry` = 265
- `Concentration` = 266
- `Allied_Tags` = 267
- `Sigil` = 268
- `Level_Sync` = 269
- `Aftermath_lvl1` = 270
- `Aftermath_lvl2` = 271
- `Aftermath_lvl3` = 272
- `Aftermath` = 273
- `Enlight` = 274
- `Auspice` = 275
- `Confrontation` = 276
- `Enfire_2` = 277
- `Enblizzard_2` = 278
- `Enaero_2` = 279
- `Enstone_2` = 280
- `Enthunder_2` = 281
- `Enwater_2` = 282
- `Perfect_Defense` = 283
- `Egg` = 284
- `Visitant` = 285
- `Baramnesia` = 286
- `Atma` = 287
- `Endark` = 288
- `Enmity_Boost` = 289
- `Subtle_Blow_Plus` = 290
- `Enmity_Down` = 291
- `Pennant` = 292
- `Negate_Petrify` = 293
- `Negate_Terror` = 294
- `Negate_Amnesia` = 295
- `Negate_Doom` = 296
- `Negate_Poison` = 297
- `Critical_Hit_Evasion_Down` = 298
- `Overload` = 299
- `Fire_Maneuver` = 300
- `Ice_Maneuver` = 301
- `Wind_Maneuver` = 302
- `Earth_Maneuver` = 303
- `Thunder_Maneuver` = 304
- `Water_Maneuver` = 305
- `Light_Maneuver` = 306
- `Dark_Maneuver` = 307
- `DoubleUp_Chance` = 308
- `Bust` = 309
- `Fighters_Roll` = 310
- `Monks_Roll` = 311
- `Healers_Roll` = 312
- `Wizards_Roll` = 313
- `Warlocks_Roll` = 314
- `Rogues_Roll` = 315
- `Gallants_Roll` = 316
- `Chaos_Roll` = 317
- `Beast_Roll` = 318
- `Choral_Roll` = 319
- `Hunters_Roll` = 320
- `Samurai_Roll` = 321
- `Ninja_Roll` = 322
- `Drachen_Roll` = 323
- `Evokers_Roll` = 324
- `Maguss_Roll` = 325
- `Corsairs_Roll` = 326
- `Puppet_Roll` = 327
- `Dancers_Roll` = 328
- `Scholars_Roll` = 329
- `Bolters__Roll` = 330
- `Casters_Roll` = 331
- `Coursers_Roll` = 332
- `Blitzers_Roll` = 333
- `Tacticians_Roll` = 334
- `Allies_Roll` = 335
- `Misers_Roll` = 336
- `Companions_Roll` = 337
- `Avengers_Roll` = 338
- `Warriors_Charge` = 340
- `Formless_Strikes` = 341
- `Assassins_Charge` = 342
- `Feint` = 343
- `Fealty` = 344
- `Dark_Seal` = 345
- `Diabolic_Eye` = 346
- `Nightingale` = 347
- `Troubadour` = 348
- `Killer_Instinct` = 349
- `Stealth_Shot` = 350
- `Flashy_Shot` = 351
- `Sange` = 352
- `Hasso` = 353
- `Seigan` = 354
- `Convergence` = 355
- `Diffusion` = 356
- `Snake_Eye` = 357
- `Light_Arts` = 358
- `Dark_Arts` = 359
- `Penury` = 360
- `Parsimony` = 361
- `Celerity` = 362
- `Alacrity` = 363
- `Rapture` = 364
- `Ebullience` = 365
- `Accession` = 366
- `Manifestation` = 367
- `Drain_Samba` = 368
- `Aspir_Samba` = 369
- `Haste_Samba` = 370
- `Velocity_Shot` = 371
- `Building_Flourish` = 375
- `Trance` = 376
- `Tabula_Rasa` = 377
- `Drain_Daze` = 378
- `Aspir_Daze` = 379
- `Haste_Daze` = 380
- `Finishing_Move1` = 381
- `Finishing_Move2` = 382
- `Finishing_Move3` = 383
- `Finishing_Move4` = 384
- `Finishing_Move5` = 385
- `Lethargic_Daze1` = 386
- `Lethargic_Daze2` = 387
- `Lethargic_Daze3` = 388
- `Lethargic_Daze4` = 389
- `Lethargic_Daze5` = 390
- `Sluggish_Daze1` = 391
- `Sluggish_Daze2` = 392
- `Sluggish_Daze3` = 393
- `Sluggish_Daze4` = 394
- `Sluggish_Daze5` = 395
- `Weakened_Daze1` = 396
- `Weakened_Daze2` = 397
- `Weakened_Daze3` = 398
- `Weakened_Daze4` = 399
- `Weakened_Daze5` = 400
- `Addendum_White` = 401
- `Addendum_Black` = 402
- `Reprisal` = 403
- `Magic_Evasion_Down` = 404
- `Retaliation` = 405
- `Footwork` = 406
- `Klimaform` = 407
- `Sekkanoki` = 408
- `Pianissimo` = 409
- `Saber_Dance` = 410
- `Fan_Dance` = 411
- `Altruism` = 412
- `Focalization` = 413
- `Tranquility` = 414
- `Equanimity` = 415
- `Enlightenment` = 416
- `Afflatus_Solace` = 417
- `Afflatus_Misery` = 418
- `Composure` = 419
- `Yonin` = 420
- `Innin` = 421
- `Carbuncles_Favor` = 422
- `Ifrits_Favor` = 423
- `Shivas_Favor` = 424
- `Garudas_Favor` = 425
- `Titans_Favor` = 426
- `Ramuhs_Favor` = 427
- `Leviathans_Favor` = 428
- `Fenrirs_Favor` = 429
- `Diabolos_Favor` = 430
- `Avatars_Favor` = 431
- `Multi_Strikes` = 432
- `Double_Shot` = 433
- `Transcendency` = 434
- `Restraint` = 435
- `Perfect_Counter` = 436
- `Mana_Wall` = 437
- `Divine_Emblem` = 438
- `Nether_Void` = 439
- `Sengikori` = 440
- `Futae` = 441
- `Presto` = 442
- `Climactic_Flourish` = 443
- `Utsusemi_2_Shadows_Left` = 444
- `Utsusemi_3_Shadows_Left` = 445
- `Utsusemi_4_Shadows_Left` = 446
- `Multi_Shots` = 447
- `Bewildered_Daze1` = 448
- `Bewildered_Daze2` = 449
- `Bewildered_Daze3` = 450
- `Bewildered_Daze4` = 451
- `Bewildered_Daze5` = 452
- `Divine_Caress` = 453
- `Saboteur` = 454
- `Tenuto` = 455
- `Spur` = 456
- `Efflux` = 457
- `Earthen_Armor` = 458
- `Divine_Caress2` = 459
- `Blood_Rage` = 460
- `Impetus` = 461
- `Conspirator` = 462
- `Sepulcher` = 463
- `Arcane_Crest` = 464
- `Hamanoha` = 465
- `Dragon_Breaker` = 466
- `Triple_Shot` = 467
- `Striking_Flourish` = 468
- `Perpetuance` = 469
- `Immanence` = 470
- `Migawari` = 471
- `Ternary_Flourish` = 472
- `Muddled` = 473
- `Prowess` = 474
- `Voidwatcher` = 475
- `Ensphere` = 476
- `Sacrosanctity` = 477
- `Palisade` = 478
- `Scarlet_Delirium` = 479
- `Scarlet_Delirium2` = 480
- `Decoy_Shot` = 482
- `Hagakure` = 483
- `Issekigan` = 484
- `Unbridled_Learning` = 485
- `Counter_Boost` = 486
- `Endrain` = 487
- `Enaspir` = 488
- `Afterglow` = 489
- `Imminent_Strikes` = 490
- `MNK_SP2` = 491
- `Asylum` = 492
- `Subtle_Sorcery` = 493
- `Encomium` = 494
- `THF_SP2` = 495
- `Righteous_Guard` = 496
- `DRK_SP2` = 497
- `BST_SP2` = 498
- `Malinconico` = 499
- `Instinctive_Aim` = 500
- `Yaegasumi` = 501
- `Tengen` = 502
- `Rouse_Wyvern` = 503
- `Astral_Conduit` = 504
- `Unbridled_Wisdom` = 505
- `Three_To_One` = 506
- `Grand_Pas` = 507
- `Unknown` = -1

---

## StorageContainer

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.StorageContainer
**Base Type:** Enum

### Enum Values

- `Inventory` = 0
- `Safe` = 1
- `Storage` = 2
- `Temporary` = 3
- `Locker` = 4
- `Satchel` = 5
- `Sack` = 6
- `Case` = 7
- `Wardrobe` = 8
- `Safe2` = 9
- `Wardrobe2` = 10
- `Wardrobe3` = 11
- `Wardrobe4` = 12

---

## TargetType

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.TargetType
**Base Type:** Enum

### Enum Values

- `None` = 0
- `Self` = 1
- `Player` = 2
- `PartyMember` = 4
- `AllianceMember` = 8
- `Npc` = 16
- `Enemy` = 32
- `Unknown` = 64
- `CorpseOnly` = 128
- `Corpse` = 157

---

## ThirdPartyTools

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.ThirdPartyTools
**Base Type:** Object

### Constructors

```csharp
ThirdPartyTools(IntPtr apiObject)
```

### Methods

#### BlockInput

```csharp
Void BlockInput(Boolean block)
```

#### ConsoleGetArg

```csharp
String ConsoleGetArg(Int32 index)
```

#### ConsoleGetArgCount

```csharp
Int32 ConsoleGetArgCount()
```

#### ConsoleIsNewCommand

```csharp
Int32 ConsoleIsNewCommand()
```

#### CreateTextObject

```csharp
Void CreateTextObject(String name)
```

#### DeleteTextObject

```csharp
Void DeleteTextObject(String name)
```

#### FlushCommands

```csharp
Void FlushCommands()
```

#### KeyDown

```csharp
Void KeyDown(Byte key)
```

#### KeyDown

```csharp
Void KeyDown(Keys key)
```

#### KeyPress

```csharp
Void KeyPress(Keys key)
```

#### KeyPress

```csharp
Void KeyPress(Byte key)
```

#### KeyUp

```csharp
Void KeyUp(Byte key)
```

#### KeyUp

```csharp
Void KeyUp(Keys key)
```

#### SendString

```csharp
Void SendString(String str)
```

#### SetBGBorderSize

```csharp
Void SetBGBorderSize(String name, Single size)
```

#### SetBGColor

```csharp
Void SetBGColor(String name, Byte a, Byte r, Byte g, Byte b)
```

#### SetBGVisibility

```csharp
Void SetBGVisibility(String name, Boolean visible)
```

#### SetBold

```csharp
Void SetBold(String name, Boolean bold)
```

#### SetColor

```csharp
Void SetColor(String name, Byte a, Byte r, Byte g, Byte b)
```

#### SetFont

```csharp
Void SetFont(String name, String font, Int32 height)
```

#### SetItalic

```csharp
Void SetItalic(String name, Boolean italic)
```

#### SetKey

```csharp
Void SetKey(Byte key, Boolean down)
```

#### SetLocation

```csharp
Void SetLocation(String name, Single x, Single y)
```

#### SetRightJustified

```csharp
Void SetRightJustified(String name, Boolean rightjustified)
```

#### SetText

```csharp
Void SetText(String name, String text)
```

#### SetVisibility

```csharp
Void SetVisibility(String name, Boolean visible)
```

---

## TreasureStatus

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.TreasureStatus
**Base Type:** Enum

### Enum Values

- `None` = 0
- `Pass` = 1
- `Lot` = 2

---

## Weather

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.Weather
**Base Type:** Enum

### Enum Values

- `Clear` = 0
- `Sunny` = 1
- `Cloudy` = 2
- `Fog` = 3
- `Fire` = 4
- `FireTwo` = 5
- `Water` = 6
- `WaterTwo` = 7
- `Earth` = 8
- `EarthTwo` = 9
- `Wind` = 10
- `WindTwo` = 11
- `Ice` = 12
- `IceTwo` = 13
- `Lightning` = 14
- `LightningTwo` = 15
- `Light` = 16
- `LightTwo` = 17
- `Dark` = 18
- `DarkTwo` = 19

---

## Weekday

**Namespace:** EliteMMO.API
**Full Name:** EliteMMO.API.Weekday
**Base Type:** Enum

### Enum Values

- `Firesday` = 0
- `Earthsday` = 1
- `Watersday` = 2
- `Windsday` = 3
- `Iceday` = 4
- `Lightningday` = 5
- `Lightsday` = 6
- `Darksday` = 7
- `Unknown` = 8

---


