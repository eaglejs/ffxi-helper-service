# EliteMMO.API.dll Documentation

## ⚠️ **DECOMPILED API AVAILABLE**
**See [DECOMPILED_API.md](DECOMPILED_API.md) for the complete decompiled API documentation with all classes, methods, properties, and enums.**

## Overview
**EliteMMO.API.dll** is a .NET managed wrapper library that provides a high-level, object-oriented interface to the native EliteAPI.dll. It abstracts the low-level memory operations and provides a clean, modern API for .NET applications to interact with Final Fantasy XI.

**Origin:** Elite MMO Network (elitemmonetwork.com)
**Type:** .NET Managed Assembly
**Target Framework:** .NET Framework 4.x / .NET Standard
**Language:** C# (managed code)
**Architecture:** x86 (32-bit)

**Decompilation Status:** ✅ **Complete** - Original creator granted permission for decompilation
**Generated:** December 14, 2025
**Method:** .NET Reflection via ILSpy

## Purpose
This DLL serves as a managed wrapper around EliteAPI.dll, providing:
- **Type-safe interfaces** for FFXI game state access
- **Object-oriented design** with classes and properties
- **Automatic memory management** via .NET garbage collection
- **Event-driven architecture** for game state changes
- **Exception handling** with meaningful error messages
- **LINQ-compatible collections** for queries

## Key Differences from EliteAPI.dll

| Feature | EliteAPI.dll (Native) | EliteMMO.API.dll (Managed) |
|---------|----------------------|---------------------------|
| **Language** | C/C++ | C# (.NET) |
| **Memory** | Manual management | Garbage collected |
| **Interface** | Function pointers | Classes & objects |
| **Type Safety** | Weak (void*, char*) | Strong (.NET types) |
| **Ease of Use** | Low-level, complex | High-level, simple |
| **Performance** | Faster (direct access) | Slightly slower (interop layer) |
| **Target Apps** | C/C++ applications | .NET applications (C#, VB.NET, F#) |

---

## Architecture

```
┌─────────────────────────────────────┐
│   .NET Application (C#/VB/F#)       │
│   - EliteMMO.API.EliteAPI class     │
└──────────────┬──────────────────────┘
               │ .NET Interop Layer
┌──────────────▼──────────────────────┐
│   EliteMMO.API.dll (Managed)        │
│   - P/Invoke declarations           │
│   - Wrapper classes                 │
│   - Event handlers                  │
└──────────────┬──────────────────────┘
               │ P/Invoke (DllImport)
┌──────────────▼──────────────────────┐
│   EliteAPI.dll (Native)             │
│   - Memory reading functions        │
│   - Direct process access           │
└─────────────────────────────────────┘
```

---

## Core Namespace

```csharp
namespace EliteMMO.API
{
    public class EliteAPI;      // Main API class
    public class Player;        // Player information
    public class Target;        // Target information
    public class Party;         // Party information
    public class Chat;          // Chat monitoring
    public class Inventory;     // Inventory access
    public class Entity;        // Entity/NPC data
    // ... and more
}
```

---

## Main Classes

### EliteAPI Class
The primary entry point for accessing game data.

```csharp
public class EliteAPI : IDisposable
{
    // Constructor
    public EliteAPI(int processId);

    // Properties
    public Player Player { get; }
    public Target Target { get; }
    public Party Party { get; }
    public Chat Chat { get; }
    public Inventory Inventory { get; }

    // Methods
    public void ThirdParty.SendString(string command);
    public void Dispose();
}
```

**Usage Example:**
```csharp
using EliteMMO.API;

// Connect to FFXI process
int processId = 12345;  // Get from Process.GetProcessesByName("pol")
using (var api = new EliteAPI(processId))
{
    // Access player information
    var playerName = api.Player.Name;
    var playerHP = api.Player.HP;

    Console.WriteLine($"{playerName} - HP: {playerHP}");
}
```

---

### Player Class
Provides access to player character information.

```csharp
public class Player
{
    // Properties - Basic Info
    public string Name { get; }
    public int HP { get; }
    public int HPMax { get; }
    public int MP { get; }
    public int MPMax { get; }
    public int TP { get; }
    public byte HPPercent { get; }
    public byte MPPercent { get; }

    // Properties - Stats
    public short Strength { get; }
    public short Dexterity { get; }
    public short Vitality { get; }
    public short Agility { get; }
    public short Intelligence { get; }
    public short Mind { get; }
    public short Charisma { get; }

    // Properties - Status
    public int Status { get; }  // Bit flags for status effects
    public byte Zone { get; }
    public float PosX { get; }
    public float PosY { get; }
    public float PosZ { get; }
    public float Heading { get; }

    // Properties - Level & Job
    public byte MainJob { get; }
    public byte MainJobLevel { get; }
    public byte SubJob { get; }
    public byte SubJobLevel { get; }
}
```

**Usage Example:**
```csharp
var player = api.Player;

Console.WriteLine($"Name: {player.Name}");
Console.WriteLine($"HP: {player.HP}/{player.HPMax} ({player.HPPercent}%)");
Console.WriteLine($"MP: {player.MP}/{player.MPMax} ({player.MPPercent}%)");
Console.WriteLine($"TP: {player.TP}");
Console.WriteLine($"STR: {player.Strength}");
Console.WriteLine($"Position: ({player.PosX}, {player.PosY}, {player.PosZ})");
Console.WriteLine($"Job: {player.MainJob}/{player.SubJob} Lv{player.MainJobLevel}/{player.SubJobLevel}");
```

---

### Chat Class
Provides access to in-game chat messages with event-driven architecture.

```csharp
public class Chat
{
    // Events
    public event EventHandler<ChatEventArgs> MessageReceived;

    // Methods
    public List<ChatLine> GetRecentMessages(int count);
    public void Clear();

    // Inner Classes
    public class ChatLine
    {
        public ChatMode Mode { get; }
        public string Sender { get; }
        public string Message { get; }
        public DateTime Timestamp { get; }
    }

    public enum ChatMode
    {
        Say = 0,
        Shout = 1,
        Tell = 3,
        Party = 4,
        Linkshell = 5,
        Yell = 26,
        System = 127
    }
}
```

**Usage Example:**
```csharp
// Subscribe to chat events
api.Chat.MessageReceived += (sender, e) =>
{
    var line = e.ChatLine;
    Console.WriteLine($"[{line.Mode}] {line.Sender}: {line.Message}");
};

// Get recent messages
var recent = api.Chat.GetRecentMessages(20);
foreach (var msg in recent)
{
    Console.WriteLine($"{msg.Timestamp}: {msg.Message}");
}
```

---

### Target Class
Provides information about the current target.

```csharp
public class Target
{
    // Properties
    public string Name { get; }
    public int HP { get; }
    public int HPMax { get; }
    public byte HPPercent { get; }
    public float Distance { get; }
    public int ID { get; }
    public int Index { get; }
    public bool IsValid { get; }
    public EntityType Type { get; }

    public enum EntityType
    {
        Player = 0,
        NPC = 1,
        Monster = 2,
        Object = 3
    }
}
```

**Usage Example:**
```csharp
var target = api.Target;

if (target.IsValid)
{
    Console.WriteLine($"Target: {target.Name}");
    Console.WriteLine($"HP: {target.HP}/{target.HPMax} ({target.HPPercent}%)");
    Console.WriteLine($"Distance: {target.Distance:F2} yalms");
    Console.WriteLine($"Type: {target.Type}");
}
else
{
    Console.WriteLine("No target selected");
}
```

---

### Party Class
Provides access to party member information.

```csharp
public class Party
{
    // Properties
    public int MemberCount { get; }
    public PartyMember[] Members { get; }

    // Methods
    public PartyMember GetMember(int index);

    // Inner Classes
    public class PartyMember
    {
        public string Name { get; }
        public int HP { get; }
        public int HPMax { get; }
        public byte HPPercent { get; }
        public int MP { get; }
        public int MPMax { get; }
        public byte MPPercent { get; }
        public int TP { get; }
        public byte Zone { get; }
        public byte MainJob { get; }
        public byte MainJobLevel { get; }
        public byte SubJob { get; }
        public byte SubJobLevel { get; }
    }
}
```

**Usage Example:**
```csharp
var party = api.Party;

Console.WriteLine($"Party members: {party.MemberCount}");

for (int i = 0; i < party.MemberCount; i++)
{
    var member = party.GetMember(i);
    Console.WriteLine($"{member.Name} - HP: {member.HPPercent}% MP: {member.MPPercent}% TP: {member.TP}");
}
```

---

### Inventory Class
Provides access to inventory items.

```csharp
public class Inventory
{
    // Methods
    public InventoryItem[] GetInventory(InventoryType type);
    public int GetItemCount(int itemId);

    // Enums
    public enum InventoryType
    {
        Inventory = 0,
        SafeDeposit = 1,
        Storage = 2,
        Temporary = 3,
        Locker = 4,
        Satchel = 5,
        Sack = 6,
        Case = 7
    }

    // Inner Classes
    public class InventoryItem
    {
        public int ID { get; }
        public int Count { get; }
        public int Index { get; }
        public string Name { get; }
    }
}
```

**Usage Example:**
```csharp
var inventory = api.Inventory;

// Get all items in main inventory
var items = inventory.GetInventory(Inventory.InventoryType.Inventory);

foreach (var item in items)
{
    Console.WriteLine($"{item.Name} x{item.Count}");
}

// Check specific item count
int phoenixDownCount = inventory.GetItemCount(4148);  // Phoenix Down ID
Console.WriteLine($"Phoenix Downs: {phoenixDownCount}");
```

---

## Integration with Native DLL

### P/Invoke Declarations
EliteMMO.API.dll uses P/Invoke to call native EliteAPI.dll functions:

```csharp
internal static class NativeMethods
{
    [DllImport("EliteAPI.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr CreateInstance(int processId);

    [DllImport("EliteAPI.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void DeleteInstance(IntPtr instance);

    [DllImport("EliteAPI.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int GetChatLineCount(IntPtr instance);

    [DllImport("EliteAPI.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr GetChatLineRaw(IntPtr instance, int index);

    // ... more functions
}
```

### Memory Structure Marshaling
The DLL handles marshaling between native and managed types:

```csharp
[StructLayout(LayoutKind.Sequential)]
internal struct NativePlayerStats
{
    public short Strength;
    public short Dexterity;
    public short Vitality;
    public short Agility;
    public short Intelligence;
    public short Mind;
    public short Charisma;
}
```

---

## Usage in C# Projects

### Installation
1. Copy both DLLs to your project's output directory:
   - `EliteMMO.API.dll`
   - `EliteAPI.dll` (native dependency)

2. Add reference to `EliteMMO.API.dll` in your C# project

3. Ensure build target is **x86** (32-bit) to match native DLL

### Example Application

```csharp
using System;
using System.Diagnostics;
using System.Linq;
using EliteMMO.API;

class Program
{
    static void Main(string[] args)
    {
        // Find FFXI process
        var polProcess = Process.GetProcessesByName("pol").FirstOrDefault();
        if (polProcess == null)
        {
            Console.WriteLine("FFXI not running");
            return;
        }

        // Create API instance
        using (var api = new EliteAPI(polProcess.Id))
        {
            Console.WriteLine($"Connected to {api.Player.Name}");

            // Subscribe to chat
            api.Chat.MessageReceived += (s, e) =>
            {
                var msg = e.ChatLine;
                if (msg.Mode == Chat.ChatMode.Party)
                {
                    Console.WriteLine($"[PARTY] {msg.Sender}: {msg.Message}");
                }
            };

            // Main loop
            while (true)
            {
                // Update stats display every 5 seconds
                Console.Clear();
                Console.WriteLine($"Player: {api.Player.Name}");
                Console.WriteLine($"HP: {api.Player.HP}/{api.Player.HPMax}");
                Console.WriteLine($"MP: {api.Player.MP}/{api.Player.MPMax}");
                Console.WriteLine($"TP: {api.Player.TP}");

                if (api.Target.IsValid)
                {
                    Console.WriteLine($"\nTarget: {api.Target.Name}");
                    Console.WriteLine($"HP: {api.Target.HPPercent}%");
                }

                System.Threading.Thread.Sleep(5000);
            }
        }
    }
}
```

---

## Advanced Features

### Asynchronous Monitoring
```csharp
using System.Threading.Tasks;

public async Task MonitorPlayerHealthAsync(EliteAPI api)
{
    while (true)
    {
        await Task.Delay(100);  // 100ms polling

        if (api.Player.HPPercent < 30)
        {
            Console.WriteLine("WARNING: Low HP!");
            // Trigger alert or action
        }
    }
}
```

### LINQ Queries
```csharp
// Find party members with low MP
var lowMpMembers = api.Party.Members
    .Where(m => m.MPPercent < 20)
    .Select(m => m.Name);

foreach (var name in lowMpMembers)
{
    Console.WriteLine($"{name} needs MP!");
}
```

---

## Error Handling

### Common Exceptions
```csharp
try
{
    using (var api = new EliteAPI(processId))
    {
        var name = api.Player.Name;
    }
}
catch (InvalidOperationException ex)
{
    // Process not found or already terminated
    Console.WriteLine($"Cannot connect: {ex.Message}");
}
catch (DllNotFoundException ex)
{
    // EliteAPI.dll not found
    Console.WriteLine($"Missing DLL: {ex.Message}");
}
catch (AccessViolationException ex)
{
    // Memory read error
    Console.WriteLine($"Memory access error: {ex.Message}");
}
```

---

## Performance Considerations

- **Polling Frequency**: Don't query properties too frequently (100-500ms is reasonable)
- **Event Subscriptions**: Prefer events over polling when possible
- **Dispose Pattern**: Always dispose EliteAPI instances to free resources
- **Thread Safety**: Not thread-safe by default, use locking if accessing from multiple threads

---

## Comparison: When to Use Each DLL

| Use Case | Recommended DLL |
|----------|----------------|
| C++ application | **EliteAPI.dll** (native) |
| C# / .NET application | **EliteMMO.API.dll** (managed) |
| Maximum performance | **EliteAPI.dll** |
| Ease of development | **EliteMMO.API.dll** |
| Type safety needed | **EliteMMO.API.dll** |
| Low-level control | **EliteAPI.dll** |

---

## Dependencies

### Runtime Requirements
- **.NET Framework 4.6.1** or higher (or .NET Core 3.1+ / .NET 5+)
- **EliteAPI.dll** (must be in same directory or system PATH)
- **Windows OS** (for native interop)
- **FFXI Client** (must be running)

### Build Requirements
- Visual Studio 2017 or higher
- .NET SDK matching target framework
- Platform target set to **x86** (32-bit)

---

## Related Files

- [EliteAPI.dll.README.md](EliteAPI.dll.README.md) - Native DLL documentation
- [EliteAPI.h](../../includes/Player/EliteAPI.h) - C++ wrapper class
- [EliteAPI.cpp](../../src/Player/EliteAPI.cpp) - C++ implementation

---

## Best Practices

1. **Always use `using` statement** for proper disposal
2. **Check IsValid properties** before accessing entity data
3. **Handle null references** for Name and other string properties
4. **Implement retry logic** for transient connection issues
5. **Subscribe to events** rather than polling when possible
6. **Cache frequently accessed data** to reduce memory reads
7. **Test with multiple game versions** to ensure compatibility

---

## Support & Resources

- **Elite MMO Network**: [elitemmonetwork.com](http://elitemmonetwork.com)
- **API Documentation**: See official Elite API documentation
- **Community Forums**: FFXI development community
- **GitHub**: Check for community wrappers and examples

---

*This documentation is based on analysis and common usage patterns. Actual implementation details may vary. Always refer to official Elite API documentation when available.*
