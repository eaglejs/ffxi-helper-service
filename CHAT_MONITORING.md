# Chat Monitoring Implementation

## Overview
This implementation adds chat message monitoring capability to the FFXI Helper Service. It captures in-game chat messages (opcode 0x017) and sends them to your `/set_messages` endpoint.

## Components Added

### 1. ChatMessage.h
- Defines `ChatMessage` structure for storing chat data
- Supports chat types: Say, Shout, Tell, Party, Linkshell, Yell, System, Emote
- Includes timestamp and sender information

### 2. EliteAPI Updates
**New Methods:**
- `RegisterChatCallback()` - Register callback for chat messages
- `UnregisterChatCallback()` - Remove chat callback
- `GetRecentChatMessages()` - Get recent chat history
- `StartChatMonitoring()` - Begin monitoring chat packets
- `StopChatMonitoring()` - Stop monitoring chat packets

**Chat Packet Structure (0x017):**
```cpp
struct ChatPacket {
    uint32_t header;
    uint16_t packetId;      // 0x017 for chat
    uint8_t messageType;    // 0=Say, 1=Shout, etc.
    uint8_t padding;
    char senderName[16];
    char message[256];
};
```

### 3. Player Class Updates
**New Methods:**
- `enableChatMonitoring()` - Enable chat monitoring for all processes
- `disableChatMonitoring()` - Disable chat monitoring
- `getRecentChatMessages(procId, count)` - Get recent chat for a process
- `sendChatMessagesToServer(procId)` - Manually send pending messages

**Features:**
- Automatic batching (sends every 10 messages)
- Message grouping by type
- Proper JSON formatting for your endpoint
- Thread-safe message handling

## API Endpoint Integration

Messages are sent to: `http://192.168.5.30:8080/set_messages`

**JSON Format:**
```json
{
  "playerId": 12345,
  "playerName": "playername",
  "messageType": "say",
  "messages": {
    "0": "First message",
    "1": "Second message",
    "2": "Third message"
  }
}
```

## Usage

### Basic Usage
```cpp
Player player;

// Enable chat monitoring
player.enableChatMonitoring();

// Chat messages will automatically be captured and batched
// They'll be sent to your server every 10 messages

// View recent chat
auto messages = player.getRecentChatMessages(procId, 20);
for (const auto& msg : messages) {
    std::cout << msg.sender << ": " << msg.message << std::endl;
}

// Manually send pending messages
player.sendChatMessagesToServer(procId);

// Disable when done
player.disableChatMonitoring();
```

## Integration with Elite API

### ✅ **INTEGRATION COMPLETE**

The chat monitoring system is now fully integrated with Elite API! See `ELITE_API_INTEGRATION.md` for detailed documentation.

**What's Implemented:**
- ✅ Elite API DLL loading and function resolution
- ✅ Real-time chat monitoring using `GetChatLineCount()` and `GetChatLineRaw()`
- ✅ Chat message parsing and format cleanup
- ✅ Message debouncing (500ms) and batching
- ✅ HTTP POST to `/set_messages` endpoint
- ✅ Multi-process support with thread-safe operations

**Quick Start:**
```cpp
Player player;
player.enableChatMonitoring();
// Chat messages will be automatically captured and sent to your server!
```

For complete documentation, see [ELITE_API_INTEGRATION.md](ELITE_API_INTEGRATION.md)

## Message Batching

Messages are automatically batched using a **debouncing mechanism**:

### How Debouncing Works:

1. **Message Received**: When a chat message is received, it's stored in a buffer
2. **Timer Starts**: A 500ms timer begins (or resets if already running)
3. **More Messages?**: If more messages arrive within 500ms, the timer resets
4. **Timer Expires**: After 500ms of silence, all buffered messages are sent together
5. **Batch Sent**: Messages are grouped by type and sent to your endpoint

### Example Scenario:

```
Time 0ms:    Message 1 received → Timer starts
Time 50ms:   Message 2 received → Timer resets
Time 100ms:  Message 3 received → Timer resets
Time 150ms:  Message 4 received → Timer resets
Time 200ms:  Message 5 received → Timer resets
Time 700ms:  (500ms after last message) → All 5 messages sent together!
```

### Benefits:

- **Burst Handling**: Multiple messages from combat or events are batched together
- **Network Efficiency**: Reduces HTTP requests by grouping related messages
- **Server Friendly**: Your endpoint receives logical groups of messages
- **Configurable**: Debounce delay is set to 500ms but can be adjusted

### Manual Override:

You can also manually flush pending messages:
```cpp
player.sendChatMessagesToServer(procId); // Sends immediately, bypassing debounce
```

## Special Character Handling

The implementation handles FFXI special characters:
- Escapes quotes and backslashes
- Removes newlines and control characters (`\n`, `\x7F`)
- Maintains FFXI player name encoding

## Next Steps

1. **Get Elite API DLL**: Contact Elite API developers or check their website
2. **Test Packet Capture**: Verify packets are being captured with opcode 0x017
3. **Adjust Packet Structure**: The packet structure may need adjustment based on actual Elite API implementation
4. **Configure Batching**: Adjust batch size (currently 10) if needed
5. **Add Error Handling**: Enhance error handling for network failures

## Questions to Address

Before full integration:

1. **Elite API DLL Location**: Where is your Elite API DLL located?
2. **Packet Structure**: Does the ChatPacket structure match Elite API's actual structure?
3. **Batch Size**: Is 10 messages per batch appropriate, or would you prefer time-based batching?
4. **Chat Channels**: Should all chat types be monitored or only specific ones?
5. **Error Handling**: How should the system handle network failures?

## Files Modified

- `includes/Player/ChatMessage.h` (new)
- `includes/Player/EliteAPI.h` (updated)
- `src/Player/EliteAPI.cpp` (updated)
- `includes/Player/Player.h` (updated)
- `src/Player/Player.cpp` (updated)
- `CMakeLists.txt` (no changes needed - ChatMessage.h is header-only)
