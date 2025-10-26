# Elite API Integration - Complete

## Overview
The FFXI Helper Service now fully integrates with Elite API for real-time chat monitoring. The system uses Elite API's `GetChatLineCount()` and `GetChatLineRaw()` functions to monitor in-game chat messages.

## Elite API Functions Used

The following Elite API functions are loaded and used:

- **CreateInstance()** - Creates an Elite API instance for interacting with the game
- **DeleteInstance(instance)** - Cleans up an Elite API instance
- **GetChatLineCount(instance)** - Returns the total number of chat lines
- **GetChatLineRaw(instance, index)** - Retrieves a specific chat line by index
- **Reinitialize(instance)** - Reinitializes the API instance (optional)
- **FlushCommands(instance)** - Flushes pending commands (optional)
- **SendString(instance, text)** - Sends a command to the game (optional)

## How It Works

### 1. Initialization
When a Player instance is created, the Elite API is initialized:
```cpp
Player player;
// Elite API is automatically loaded and initialized per process
```

The system:
1. Loads `EliteAPI.dll` from `libs/elite/` directory
2. Resolves function pointers using `GetProcAddress()`
3. Creates an Elite API instance using `CreateInstance()`
4. Verifies that required functions are available

### 2. Chat Monitoring
Chat monitoring runs in a dedicated thread that:

1. **Polls Chat Lines**: Continuously checks `GetChatLineCount()`
2. **Detects New Messages**: Compares current count with last processed index
3. **Retrieves Raw Lines**: Calls `GetChatLineRaw()` for each new line
4. **Parses Format**: Extracts sender, message type, and content
5. **Removes Formatting**: Strips FFXI formatting codes (0x1E, 0x1F, 0x7F)
6. **Triggers Callback**: Passes parsed `ChatMessage` to registered callback

### 3. Message Debouncing & Batching
The Player class implements debouncing on top of Elite API:

1. **Message Received**: Elite API chat callback triggers
2. **Debounce Timer**: 500ms timer starts/resets on each new message
3. **Timer Expires**: After 500ms of silence, batch is sent
4. **HTTP POST**: Messages are grouped by type and sent to endpoint

## Usage

### Enable Chat Monitoring
```cpp
#include "Player/Player.h"

Player player;

// Enable chat monitoring for all processes
player.enableChatMonitoring();

// Chat messages will now be:
// 1. Captured from Elite API
// 2. Debounced (batched after 500ms silence)
// 3. Sent to http://192.168.5.30:8080/set_messages
```

### Manual Message Retrieval
```cpp
// Get recent chat messages for a specific process
std::vector<ChatMessage> messages = player.getRecentChatMessages(processId, 20);

for (const auto& msg : messages) {
    std::cout << msg.sender << " (" << msg.getMessageTypeString()
              << "): " << msg.message << std::endl;
}
```

### Manual Batch Send (Override Debounce)
```cpp
// Force immediate send of pending messages (bypasses debounce)
player.sendChatMessagesToServer(processId);
```

## Chat Message Types

The system recognizes the following chat types:

| Type | Description | Detection Pattern |
|------|-------------|------------------|
| **Say** | Local chat | `[Say]`, `: ` |
| **Shout** | Zone-wide | `[Shout]`, `!!` |
| **Tell** | Private message | `[Tell]`, `>>` |
| **Party** | Party chat | `[Party]`, `(` |
| **Linkshell** | LS chat | `[Linkshell]`, `<` |
| **Yell** | Wide area | `[Yell]` |
| **Emote** | Actions | `[Emote]` |
| **System** | Game messages | (default) |

## Message Format Sent to Endpoint

```json
{
  "playerId": 12345,
  "playerName": "Playername",
  "messageType": "say",
  "messages": {
    "0": "First message",
    "1": "Second message",
    "2": "Third message"
  }
}
```

**Endpoint**: `http://192.168.5.30:8080/set_messages`
**Method**: POST
**Content-Type**: application/json

## Files Modified

### Core Implementation
- **src/Player/EliteAPI.cpp** - Complete Elite API integration
- **includes/Player/EliteAPI.h** - Elite API interface
- **includes/Player/ChatMessage.h** - Chat message structure

### Player Integration
- **src/Player/Player.cpp** - Debouncing and HTTP batch sending
- **includes/Player/Player.h** - Chat monitoring interface

### Build System
- **CMakeLists.txt** - Automatic DLL copying to build directory

## DLL Location

The Elite API DLL must be at: `libs/elite/EliteAPI.dll`

During build, it's automatically copied to: `build/EliteAPI.dll`

At runtime, the system searches in order:
1. `EliteAPI.dll` (current directory)
2. `libs/elite/EliteAPI.dll`
3. `./libs/elite/EliteAPI.dll`
4. `../libs/elite/EliteAPI.dll`
5. `build/EliteAPI.dll`

## Logging

Chat activity is logged to console:

```
[EliteAPI] Initializing Elite API for process 12345
[EliteAPI] Successfully loaded DLL from: libs/elite/EliteAPI.dll
[EliteAPI] Successfully initialized Elite API instance
[EliteAPI] Started chat monitoring for process 12345
[EliteAPI] Current chat line count: 42
[EliteAPI] Chat monitoring thread started
[Chat] PlayerName (Say): Hello, world!
[Chat] AnotherPlayer (Party): Ready to go?
```

## Performance Considerations

- **Polling Interval**: 100ms between chat line checks
- **Message Buffer**: Keeps last 100 messages in memory
- **Thread Safety**: All chat operations are mutex-protected
- **Debounce Delay**: 500ms (configurable in Player.cpp)

## Troubleshooting

### DLL Not Found
```
[EliteAPI] Failed to load Elite API DLL from any location
```
**Solution**: Ensure `EliteAPI.dll` exists at `libs/elite/EliteAPI.dll`

### Function Loading Failed
```
[EliteAPI] Failed to load required Elite API functions
```
**Solution**: Verify Elite API DLL version is compatible

### No Chat Messages
- Ensure FFXI is running
- Check that Elite API instance is created successfully
- Verify chat monitoring is started with `enableChatMonitoring()`

### Messages Not Sent to Server
- Check endpoint is accessible: `http://192.168.5.30:8080/set_messages`
- Verify curl library is linked properly
- Check firewall/network settings

## Next Steps

### Potential Enhancements
1. **Configurable Debounce**: Make 500ms delay adjustable
2. **Chat Filtering**: Filter by message type before sending
3. **Retry Logic**: Retry failed HTTP requests
4. **Rate Limiting**: Prevent overwhelming the endpoint
5. **Message Queue**: Persistent queue for offline buffering

### Additional Elite API Functions
The DLL exports many more functions that could be useful:
- `GetPlayerInfo()` - Get detailed player information
- `GetEntity()` - Get entity data (NPCs, players, etc.)
- `GetInventoryItem()` - Access inventory
- `SendString()` - Send commands to game
- And 150+ more functions...

## Testing

### Build & Run
```bash
# Configure CMake
cmake -B build -G "MinGW Makefiles"

# Build
cmake --build build --config Debug

# Run (requires FFXI to be running)
./build/FFXIHelperService_d.exe
```

### Expected Output
```
[EliteAPI] Initializing Elite API for process 12345
[EliteAPI] Successfully loaded DLL from: libs/elite/EliteAPI.dll
[EliteAPI] Successfully initialized Elite API instance
[EliteAPI] Started chat monitoring for process 12345
[Chat] You (Say): Testing chat monitoring
[Player] Debounce timer started for process 12345
[Player] Sending 1 chat messages for process 12345
[HTTP] Sending batch to http://192.168.5.30:8080/set_messages
```

## Summary

✅ **Elite API DLL Integration** - Complete
✅ **Real-time Chat Monitoring** - Using GetChatLineCount/GetChatLineRaw
✅ **Message Parsing** - Extracts sender, type, and content
✅ **Format Cleanup** - Removes FFXI formatting codes
✅ **Debouncing System** - 500ms batching
✅ **HTTP Batch Sending** - JSON POST to endpoint
✅ **Multi-process Support** - Handles multiple game instances
✅ **Thread-safe Operations** - Mutex-protected message handling

The chat monitoring system is now fully operational and ready to capture, process, and forward in-game chat messages to your Node.js server!
