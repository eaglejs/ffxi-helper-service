# ffxi-helper-service

A C++ service for monitoring and interacting with Final Fantasy XI (pol.exe) processes. This service reads player data from memory, monitors properties like Tactical Points at configurable intervals, and sends HTTP requests when changes are detected.

## Features

- **Multi-Process Support**: Automatically detects and monitors multiple FFXI processes
- **Memory Reading**: Reads player data directly from game memory (player names, IDs, tactical points)
- **Property Monitoring**: Configurable monitoring intervals for different game properties
- **Change Detection**: Real-time detection and reporting of property changes
- **HTTP Integration**: Automatic HTTP POST requests to REST endpoints when tactical points change
- **HTTP Client**: Built-in HTTP client for API interactions using libcurl
- **32-bit Compatible**: Compiled for 32-bit to work with FFXI processes

## Prerequisites

- **MSYS2/MinGW**: For compilation (32-bit toolchain)
  - Install from: https://www.msys2.org/
  - Required packages: `mingw-w64-i686-gcc`, `mingw-w64-i686-gdb`
- **vcpkg**: For dependency management
  - Clone from: https://github.com/Microsoft/vcpkg
- **CMake**: For build system (optional, but recommended)
- **VS Code**: For development and debugging (optional)

## Build Instructions

### Method 1: CMake Build (Recommended)

1. **Configure the build**:
```powershell
cd "c:\Users\SL4X3\repos\ffxi-helper-service"
$env:PATH = "C:\msys64\mingw32\bin;$env:PATH"
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
```

2. **Build the project**:
```powershell
cmake --build build --config Debug
```

3. **Run the executable**:
```powershell
.\build\FFXIHelperService_d.exe
```

### Method 2: Direct g++ Build

```powershell
cd "c:\Users\SL4X3\repos\ffxi-helper-service\src"
$env:PATH = "C:\msys64\mingw32\bin;$env:PATH"
g++ -g -Wall -Wextra -m32 -I..\includes FFXIHelperService.cpp helpers\memory.cpp helpers\http.cpp Player\Player.cpp Player\TacticalPointsProperty.cpp -o FFXIHelperService.exe -lcurl
```

### Clean Build (CMake)

If you need to start fresh:
```powershell
cd "c:\Users\SL4X3\repos\ffxi-helper-service"
Remove-Item -Recurse -Force build
mkdir build
$env:PATH = "C:\msys64\mingw32\bin;$env:PATH"
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

## VS Code Integration

### Build Tasks

- **CMake Build**: Use `Ctrl+Shift+P` → "Tasks: Run Task" → "CMake: Build"
- **Direct Build**: Use `Ctrl+Shift+P` → "Tasks: Run Task" → "C/C++: g++.exe build active file"

### Debugging

Two debug configurations are available:
- **"Debug FFXIHelperService"**: Uses direct g++ build
- **"Debug FFXIHelperService (CMake)"**: Uses CMake build

Press `F5` to start debugging with the default configuration.

## Project Structure

```
ffxi-helper-service/
├── src/                          # Source files
│   ├── FFXIHelperService.cpp     # Main application
│   ├── helpers/
│   │   ├── memory.cpp            # Memory reading utilities
│   │   └── http.cpp              # HTTP client implementation
│   └── Player/
│       ├── Player.cpp            # Player management & static properties
│       └── TacticalPointsProperty.cpp  # Tactical points monitoring
├── includes/                     # Header files
│   ├── helpers/
│   │   ├── memory.h              # Memory reading utilities
│   │   └── http.h                # HTTP client header
│   └── Player/
│       ├── Player.h              # Player class definition
│       └── TacticalPointsProperty.h    # Tactical points property header
├── build/                        # CMake build output
├── .vscode/                      # VS Code configuration
├── CMakeLists.txt               # CMake configuration
├── vcpkg.json                   # vcpkg dependencies
└── README.md
```

## Usage

1. **Start FFXI**: Launch one or more Final Fantasy XI clients
2. **Run the service**: Execute the built executable
3. **Monitor output**: The service will automatically detect FFXI processes and display player information
4. **Real-time monitoring**: Watch for property changes (like Tactical Points) in real-time
5. **HTTP Integration**: The service automatically sends HTTP POST requests to a REST endpoint when tactical points change

### Sample Output

```
Starting FFXI Helper Service...
Found 5 pol.exe processes
Successfully initialized process 17304
Successfully initialized process 10396
Updated player name for process 4024: Rubydrops
Updated player name for process 10396: Surakusa
Updated player name for process 17304: Piplup
[...]
--- Process ID: 17304 ---
Player Name: Piplup (Static)
Player ID: 325554 (Static)
Tactical Points: 1760 (Updates every 100ms)
[...]
Player [Piplup] (PID: 17304) - TP changed from 1760 to 1764
Successfully sent TP update for Piplup (TP: 1764)
```

### HTTP Endpoint Integration

The service automatically sends HTTP POST requests to `/set_tp` when tactical points change:

```json
{
  "playerName": "Piplup",
  "playerId": 325554,
  "tp": 1764
}
```

## Development

### Adding New Properties

1. Create a new property class inheriting from `PlayerProperty`
2. Implement required virtual methods:
   - `refresh()`: Read and update the property value
   - `hasChanged()`: Check if the property has changed
   - `reportChange()`: Report changes to external systems
   - `getPropertyName()`: Return the property name
   - `displayValue()`: Display the current value
3. Register the property in the `Player` class constructor

### Static vs Dynamic Properties

The `Player` class handles two types of properties:

- **Static Properties**: Read once at initialization (player name, player ID)
  - Stored directly in the `Player` class
  - Accessed via `getPlayerName()` and `getPlayerId()`

- **Dynamic Properties**: Monitored continuously (tactical points)
  - Implemented as `PlayerProperty` subclasses
  - Registered with configurable monitoring intervals

### HTTP Client Usage

The project includes a built-in HTTP client for API interactions:

```cpp
#include "helpers/http.h"

// GET request
HttpClient client;
HttpResponse response = client.get("https://api.example.com/data");
if (response.success) {
    std::cout << "Response: " << response.body << std::endl;
}

// POST request with JSON
std::string jsonData = R"({"playerName": "Piplup", "playerId": 325554, "tp": 1764})";
HttpResponse postResponse = client.post("https://api.example.com/set_tp", jsonData, "application/json");
```

### Memory Reading Configuration

Player data is read from `FFXiMain.dll` using specific offsets:

- **Player Name**: Base `0x000EA53C` + offset `{0x314}`
- **Player ID**: Base `0x000106BC` + offset `{0x4E0}`
- **Tactical Points**: Base `0x000EA53C` + offset `{0x318}` (monitored continuously)

## Dependencies

- **libcurl**: HTTP client functionality
- **Windows API**: Process and memory management
- **C++17**: Modern C++ features

All dependencies are managed through vcpkg and automatically configured during the CMake build process.

## Architecture

The service uses a component-based architecture:

- **Player**: Manages individual FFXI processes and static properties (name, ID)
- **PlayerProperty**: Base class for dynamically monitored properties
- **TacticalPointsProperty**: Monitors tactical points and triggers HTTP requests on changes
- **Monitoring Thread**: Handles periodic property checks and change detection
- **Memory Utilities**: Safe memory reading from external processes using DLL injection
- **HTTP Client**: External API communication with JSON support

### Data Flow

1. Service starts and discovers all `pol.exe` processes
2. For each process, reads static properties (name, ID) once from `FFXiMain.dll`
3. Registers dynamic properties (tactical points) for continuous monitoring
4. Monitoring thread checks properties at configured intervals (100ms default)
5. When tactical points change, HTTP POST request is sent with player data
6. Process continues until service is stopped

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test with actual FFXI processes
5. Submit a pull request

## License

[Add your license information here]

## Windows Service Installation

You can install the FFXIHelperService as a Windows service to run automatically at startup. Here are several methods:

### Method 1: Using NSSM (Non-Sucking Service Manager) - Recommended

1. **Download NSSM**:
   - Download from: https://nssm.cc/download
   - Extract to a folder (e.g., `C:\nssm`)

2. **Install the service**:
```powershell
# Run as Administrator
cd "C:\nssm\win64"  # or win32 for 32-bit systems
.\nssm.exe install FFXIHelperService "C:\Users\SL4X3\repos\ffxi-helper-service\build\FFXIHelperService_d.exe"
```

3. **Configure the service** (optional):
```powershell
# Set working directory
.\nssm.exe set FFXIHelperService AppDirectory "C:\Users\SL4X3\repos\ffxi-helper-service\build"

# Set startup type to automatic
.\nssm.exe set FFXIHelperService Start SERVICE_AUTO_START

# Set service description
.\nssm.exe set FFXIHelperService Description "FFXI Helper Service - Monitors FFXI processes and sends TP updates"
```

4. **Start the service**:
```powershell
net start FFXIHelperService
# or
.\nssm.exe start FFXIHelperService
```

5. **Remove the service** (if needed):
```powershell
.\nssm.exe stop FFXIHelperService
.\nssm.exe remove FFXIHelperService confirm
```

### Method 2: Using SC (Service Control) Command

```powershell
# Run as Administrator
sc create FFXIHelperService binPath= "C:\Users\SL4X3\repos\ffxi-helper-service\build\FFXIHelperService_d.exe" start= auto DisplayName= "FFXI Helper Service"

# Start the service
sc start FFXIHelperService

# Stop the service
sc stop FFXIHelperService

# Delete the service
sc delete FFXIHelperService
```

### Method 3: Using PowerShell (Windows 10/11)

```powershell
# Run as Administrator
New-Service -Name "FFXIHelperService" -BinaryPathName "C:\Users\SL4X3\repos\ffxi-helper-service\build\FFXIHelperService_d.exe" -StartupType Automatic -DisplayName "FFXI Helper Service" -Description "Monitors FFXI processes and sends TP updates"

# Start the service
Start-Service -Name "FFXIHelperService"

# Stop the service
Stop-Service -Name "FFXIHelperService"

# Remove the service
Remove-Service -Name "FFXIHelperService"
```

### Method 4: Convert to Native Windows Service (Advanced)

For a production environment, you might want to modify the code to be a proper Windows service:

1. **Add Windows Service Framework**: Implement `ServiceMain()` and `ServiceCtrlHandler()` functions
2. **Handle Service Control Messages**: Start, stop, pause, continue
3. **Use Service Control Manager (SCM)**: Register with Windows SCM

This requires code modifications but provides better integration with Windows service management.

### Service Management

Once installed as a service, you can manage it through:

- **Services.msc**: Windows Services management console
- **Task Manager**: Services tab
- **PowerShell**: `Get-Service`, `Start-Service`, `Stop-Service`, `Restart-Service`
- **Command Line**: `net start`, `net stop`, `sc query`

### Important Notes

- **Run as Administrator**: Service installation requires administrator privileges
- **File Paths**: Use absolute paths for the executable and working directory
- **Dependencies**: Ensure all required DLLs (libcurl, etc.) are in the same directory as the executable
- **Logging**: Consider redirecting output to log files when running as a service
- **User Context**: Services run in a different security context, which may affect process access permissions

### Troubleshooting

If the service fails to start:

1. **Check Event Viewer**: Look for service-related errors in Windows Event Log
2. **Test Manually**: Run the executable manually first to ensure it works
3. **Dependencies**: Verify all required DLLs are present
4. **Permissions**: Ensure the service account has necessary permissions to access FFXI processes
5. **Working Directory**: Set the correct working directory for the service
