# EliteAPI.dll Documentation

## Overview
**EliteAPI.dll** is a native C/C++ library that provides memory reading and manipulation capabilities for Final Fantasy XI. It serves as the core interface for interacting with the FFXI game client process, enabling real-time access to game state, chat messages, and player data.

**Origin:** Elite MMO Network (elitemmonetwork.com)
**Architecture:** 32-bit (x86)
**Language:** Native C/C++ (unmanaged code)
**Size:** ~702 KB

**Note:** This is a binary DLL without source code. The original creator has granted permission for decompilation and documentation. For implementation details, refer to the EliteMMO.API.dll managed wrapper which has been fully decompiled (see [DECOMPILED_API.md](DECOMPILED_API.md)).

## Purpose
This DLL acts as a bridge between external applications and the FFXI game client by:
- Attaching to running FFXI process memory
- Reading game state information (player stats, inventory, etc.)
- Monitoring in-game chat messages in real-time
- Sending commands to the game client
- Providing a stable API interface despite game updates

## Core Functions

### Instance Management

#### `CreateInstance(DWORD processId)`
Creates a new Elite API instance attached to a specific FFXI process.

**Parameters:**
- `processId` (DWORD): The Windows process ID of the FFXI game client

**Returns:**
- `void*`: Pointer to the Elite API instance, or NULL on failure

**Usage:**
```cpp
DWORD ffxiProcessId = 12345;
void* eliteInstance = CreateInstance(ffxiProcessId);
if (eliteInstance) {
    // Successfully attached to process
}
```

---

#### `DeleteInstance(void* instance)`
Cleans up and releases an Elite API instance.

**Parameters:**
- `instance` (void*): The Elite API instance to destroy

**Returns:**
- `void`

**Usage:**
```cpp
DeleteInstance(eliteInstance);
eliteInstance = nullptr;
```

---

#### `Reinitialize(void* instance)`
Reinitializes the Elite API instance, refreshing memory pointers.

**Parameters:**
- `instance` (void*): The Elite API instance to reinitialize

**Returns:**
- `bool`: True if reinitialization succeeded

**Usage:**
```cpp
if (Reinitialize(eliteInstance)) {
    // Instance refreshed successfully
}
```

---

### Chat Monitoring Functions

#### `GetChatLineCount(void* instance)`
Returns the total number of chat lines currently available in the game's chat log.

**Parameters:**
- `instance` (void*): The Elite API instance

**Returns:**
- `int`: Total number of chat lines

**Usage:**
```cpp
int totalLines = GetChatLineCount(eliteInstance);
std::cout << "Total chat lines: " << totalLines << std::endl;
```

**Notes:**
- The count represents all chat lines since the game session started
- Use this to detect new messages by comparing against the last processed index
- Return value is typically in the range of 0-10000

---

#### `GetChatLineRaw(void* instance, int index)`
Retrieves a specific chat line by its index in raw format.

**Parameters:**
- `instance` (void*): The Elite API instance
- `index` (int): The zero-based index of the chat line to retrieve

**Returns:**
- `const char*`: Pointer to the raw chat line string, or NULL if invalid

**Usage:**
```cpp
int count = GetChatLineCount(eliteInstance);
for (int i = lastIndex; i < count; i++) {
    const char* line = GetChatLineRaw(eliteInstance, i);
    if (line != nullptr) {
        std::cout << "Chat line " << i << ": " << line << std::endl;
    }
}
```

**Raw Format:**
The returned string contains FFXI formatting codes:
- **0x1E**: Start of formatting sequence
- **0x1F**: Color/formatting code (followed by parameter byte)
- **0x7F**: End of formatting sequence
- Text format: `[ChatType] PlayerName: Message`

**Example Raw Line:**
```
[Say] Player1: Hello, how are you?
```

**Important:**
- The returned pointer is valid until the next call to this function
- Do not free or modify the returned string
- Copy the string immediately if you need to store it

---

### Command Functions

#### `SendString(void* instance, const char* command)`
Sends a text command or message to the game client.

**Parameters:**
- `instance` (void*): The Elite API instance
- `command` (const char*): The command string to send

**Returns:**
- `void`

**Usage:**
```cpp
// Send a chat message
SendString(eliteInstance, "/say Hello, everyone!");

// Execute a game command
SendString(eliteInstance, "/wave");

// Use macros
SendString(eliteInstance, "/macro set 1");
```

---

#### `FlushCommands(void* instance)`
Flushes pending commands to ensure they are sent to the game immediately.

**Parameters:**
- `instance` (void*): The Elite API instance

**Returns:**
- `void`

**Usage:**
```cpp
SendString(eliteInstance, "/say Test message");
FlushCommands(eliteInstance);  // Ensure message is sent immediately
```

---

## Chat Message Types

When parsing chat lines retrieved via `GetChatLineRaw()`, the following message types can be detected:

| Type | Identifier | Pattern | Description |
|------|-----------|---------|-------------|
| **Say** | `[Say]` | `: ` | Local area chat |
| **Shout** | `[Shout]` | `!!` | Zone-wide announcement |
| **Tell** | `[Tell]` | `>>` | Private message |
| **Party** | `[Party]` | `(` | Party chat |
| **Linkshell** | `[Linkshell]` | `<` | Linkshell chat |
| **Yell** | `[Yell]` | N/A | Wide area chat |
| **Emote** | `[Emote]` | N/A | Character emotes |
| **System** | N/A | N/A | Game system messages |

---

## Formatting Code Reference

FFXI chat messages contain special formatting codes that must be stripped for clean text:

| Code | Hex | Purpose |
|------|-----|---------|
| Start Marker | `0x1E` | Begins formatting sequence |
| Format Code | `0x1F` | Indicates color/style (followed by parameter) |
| End Marker | `0x7F` | Ends formatting sequence |
| Control Chars | `< 0x20` | Non-printable control characters |

**Example Parsing:**
```cpp
std::string RemoveFormattingCodes(const std::string& text) {
    std::string result;
    for (size_t i = 0; i < text.length(); i++) {
        unsigned char c = static_cast<unsigned char>(text[i]);

        if (c == 0x1E || c == 0x1F || c == 0x7F) {
            if (c == 0x1F && i + 1 < text.length()) {
                i++; // Skip parameter byte
            }
            continue;
        }

        if (c >= 0x20 || c == '\n' || c == '\r' || c == '\t') {
            result += text[i];
        }
    }
    return result;
}
```

---

## Integration Example

### Complete Chat Monitoring Implementation

```cpp
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>

// Function pointer types
typedef void* (__cdecl *CreateInstanceFunc)(DWORD);
typedef void (__cdecl *DeleteInstanceFunc)(void*);
typedef int (__cdecl *GetChatLineCountFunc)(void*);
typedef const char* (__cdecl *GetChatLineRawFunc)(void*, int);

int main() {
    // Load the DLL
    HMODULE dll = LoadLibraryA("EliteAPI.dll");
    if (!dll) {
        std::cerr << "Failed to load EliteAPI.dll" << std::endl;
        return 1;
    }

    // Get function pointers
    auto CreateInstance = (CreateInstanceFunc)GetProcAddress(dll, "CreateInstance");
    auto DeleteInstance = (DeleteInstanceFunc)GetProcAddress(dll, "DeleteInstance");
    auto GetChatLineCount = (GetChatLineCountFunc)GetProcAddress(dll, "GetChatLineCount");
    auto GetChatLineRaw = (GetChatLineRawFunc)GetProcAddress(dll, "GetChatLineRaw");

    if (!CreateInstance || !DeleteInstance || !GetChatLineCount || !GetChatLineRaw) {
        std::cerr << "Failed to load required functions" << std::endl;
        FreeLibrary(dll);
        return 1;
    }

    // Attach to FFXI process (replace with actual process ID)
    DWORD processId = 12345;
    void* instance = CreateInstance(processId);
    if (!instance) {
        std::cerr << "Failed to create Elite API instance" << std::endl;
        FreeLibrary(dll);
        return 1;
    }

    // Monitor chat
    int lastProcessedLine = GetChatLineCount(instance);
    std::cout << "Starting from line: " << lastProcessedLine << std::endl;

    while (true) {
        int currentCount = GetChatLineCount(instance);

        // Process new messages
        while (lastProcessedLine < currentCount) {
            const char* line = GetChatLineRaw(instance, lastProcessedLine);
            if (line) {
                std::cout << "New message: " << line << std::endl;
            }
            lastProcessedLine++;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Cleanup
    DeleteInstance(instance);
    FreeLibrary(dll);
    return 0;
}
```

---

## Technical Details

### Calling Convention
All exported functions use the `__cdecl` calling convention (C default).

### Thread Safety
- **NOT thread-safe**: Each instance should be accessed from a single thread
- Use mutexes if accessing from multiple threads
- Chat monitoring is safe in a dedicated worker thread

### Memory Management
- The DLL manages internal memory for chat strings
- Do not free pointers returned by `GetChatLineRaw()`
- Copy strings immediately if you need to store them

### Process Attachment
- Requires the FFXI process to be running
- Process ID can be obtained via Windows API (`EnumProcesses`, `CreateToolhelp32Snapshot`)
- The DLL reads process memory, requiring appropriate permissions

---

## Error Handling

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| `CreateInstance()` returns NULL | Invalid process ID or process not found | Verify FFXI is running and process ID is correct |
| Chat line count is 0 | No chat messages yet or API not initialized | Wait for game to fully load |
| `GetChatLineRaw()` returns NULL | Invalid index or line unavailable | Check index is within valid range |
| Suspicious chat line count (>10000) | Memory read error | Reinitialize instance |

### Best Practices

1. **Validate Return Values**: Always check if pointers are NULL before use
2. **Sanity Check Data**: Verify line counts and string lengths are reasonable
3. **Handle Exceptions**: Wrap memory operations in try-catch blocks
4. **Graceful Degradation**: Continue operation even if some functions fail
5. **Periodic Reinitialization**: Call `Reinitialize()` if data seems stale

---

## Dependencies

- **Windows API**: Uses Win32 API for process attachment
- **FFXI Client**: Must be running with compatible version
- **32-bit Runtime**: Requires 32-bit application or WoW64 on 64-bit systems

---

## Version Compatibility

- Works with multiple FFXI client versions
- Uses memory pattern scanning to adapt to updates
- May require updates when major FFXI patches are released

---

## Security Considerations

- **Requires elevated privileges** on some systems
- **Reads process memory** - may be flagged by anti-cheat systems
- **Use responsibly** and in accordance with game terms of service
- **Not for automation** - designed for information reading, not botting

---

## Related Files

- [EliteMMO.API.dll.README.md](EliteMMO.API.dll.README.md) - .NET wrapper around this DLL
- [EliteAPI.h](../../includes/Player/EliteAPI.h) - C++ wrapper class header
- [EliteAPI.cpp](../../src/Player/EliteAPI.cpp) - C++ wrapper implementation

---

## Support & Resources

- **Elite MMO Network**: [elitemmonetwork.com](http://elitemmonetwork.com)
- **Documentation**: See project integration files in repository
- **Community**: FFXI development community forums

---

*This documentation is based on reverse-engineering and integration work. Function signatures and behavior may vary with different versions.*
