# Quick Start - Elite API Chat Monitoring

## ✅ Setup Complete!

Your FFXI Helper Service is ready to monitor in-game chat using Elite API!

## What You Have

```
✅ Elite API DLL Integration
✅ Real-time Chat Monitoring
✅ Message Debouncing (500ms)
✅ Automatic HTTP Batching
✅ Multi-process Support
✅ Built & Ready to Run
```

## Files Ready

**Executable:** `build/FFXIHelperService_d.exe` (2.9 MB)
**Elite API:** `build/EliteAPI.dll` (702 KB)
**Dependencies:** curl, zlib (auto-copied)

## How to Run

### 1. Make Sure FFXI is Running
```
Launch Final Fantasy XI and log in to a character
```

### 2. Make Sure Your Node.js Server is Running
```bash
# Your server should be listening on:
http://192.168.5.30:8080/set_messages
```

### 3. Run the Helper Service
```powershell
cd C:\Users\SL4X3\repos\ffxi-helper-service
.\build\FFXIHelperService_d.exe
```

## What Happens

1. **Service Starts** → Detects running FFXI processes
2. **Elite API Loads** → Connects to game memory
3. **Chat Monitoring Begins** → Polls for new messages every 100ms
4. **Messages Captured** → Parses sender, type, content
5. **Debouncing** → Waits 500ms after last message
6. **Batch Sent** → POSTs JSON to your server

## Expected Output

```
[EliteAPI] Initializing Elite API for process 12345
[EliteAPI] Successfully loaded DLL from: libs/elite/EliteAPI.dll
[EliteAPI] Successfully initialized Elite API instance
[EliteAPI] Started chat monitoring for process 12345
[EliteAPI] Current chat line count: 42
[EliteAPI] Chat monitoring thread started
[Chat] Playername (Say): Hello, world!
[Chat] Friend (Party): Ready?
[Player] Debounce timer started for process 12345
[Player] Sending 2 chat messages for process 12345
[HTTP] POST to http://192.168.5.30:8080/set_messages
```

## Test It!

### 1. Type in FFXI Chat
```
/say Testing chat monitoring!
```

### 2. Watch Console Output
You should see the message appear in the console within ~500ms

### 3. Check Your Server
Your Node.js server should receive:
```json
{
  "playerId": 12345,
  "playerName": "YourCharName",
  "messageType": "say",
  "messages": {
    "0": "Testing chat monitoring!"
  }
}
```

## Common Commands

### Rebuild
```powershell
cmake --build build --config Debug
```

### Clean Build
```powershell
cmake --build build --target clean
cmake --build build --config Debug
```

### Check DLLs
```powershell
ls build/*.dll
```

## Chat Types Monitored

- ✅ **Say** - `/say message`
- ✅ **Shout** - `/shout message`
- ✅ **Tell** - `/tell player message`
- ✅ **Party** - `/party message`
- ✅ **Linkshell** - `/linkshell message`
- ✅ **Yell** - `/yell message`
- ✅ **Emote** - Emote actions
- ✅ **System** - Game notifications

## Troubleshooting

### "Failed to load Elite API DLL"
**Fix:** Ensure `libs/elite/EliteAPI.dll` exists
```powershell
ls libs/elite/EliteAPI.dll
```

### "No chat messages appear"
**Check:**
1. Is FFXI running?
2. Did Elite API initialize successfully?
3. Is chat monitoring started?

### "Server not receiving messages"
**Test endpoint manually:**
```bash
curl -X POST http://192.168.5.30:8080/set_messages \
  -H "Content-Type: application/json" \
  -d '{"playerId":1,"playerName":"test","messageType":"say","messages":{"0":"test"}}'
```

## Configuration

### Change Debounce Delay
Edit `src/Player/Player.cpp`, line ~XXX:
```cpp
std::chrono::milliseconds(500)  // Change 500 to your desired delay
```

### Change Server Endpoint
Edit `src/Player/Player.cpp`, look for:
```cpp
"http://192.168.5.30:8080/set_messages"
```

### Change Polling Rate
Edit `src/Player/EliteAPI.cpp`, look for:
```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(100));
```

## Documentation

📖 **[IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)** - High-level overview
📖 **[ELITE_API_INTEGRATION.md](ELITE_API_INTEGRATION.md)** - Technical details
📖 **[CHAT_MONITORING.md](CHAT_MONITORING.md)** - Chat monitoring info
📖 **[README.md](README.md)** - Project overview

## Support

If you encounter issues:
1. Check the console output for error messages
2. Review the documentation files above
3. Verify FFXI is running and Elite API loaded
4. Test your server endpoint manually

## Success! 🎉

Your chat monitoring system is:
- ✅ Built successfully
- ✅ Elite API integrated
- ✅ Ready to capture messages
- ✅ Ready to send to your server

**Just run the executable and start chatting in FFXI!**

---

_Last Updated: October 24, 2025_
