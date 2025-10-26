#include "Player/ChatLogProperty.h"
#include "Player/Player.h"
#include "helpers/memory.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <sstream>
#include <Windows.h>

// Helper function to convert Shift-JIS (Japanese) to UTF-8
std::string ShiftJISToUTF8(const char* shiftjis, size_t length)
{
    if (length == 0) return "";

    // Convert Shift-JIS (CP932) to wide char
    int wideSize = MultiByteToWideChar(932, 0, shiftjis, length, nullptr, 0);
    if (wideSize == 0) return std::string(shiftjis, length); // Fallback to original

    std::wstring wideStr(wideSize, 0);
    MultiByteToWideChar(932, 0, shiftjis, length, &wideStr[0], wideSize);

    // Convert wide char to UTF-8
    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), wideSize, nullptr, 0, nullptr, nullptr);
    if (utf8Size == 0) return std::string(shiftjis, length); // Fallback to original

    std::string utf8Str(utf8Size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), wideSize, &utf8Str[0], utf8Size, nullptr, nullptr);

    return utf8Str;
}

ChatLogProperty::ChatLogProperty()
{
    initializePatterns();
}

void ChatLogProperty::initializePatterns()
{
    // Match FFXI chat formats (with optional leading space and special characters)
    // Note: . matches any character (including special FFXI color codes)
    // Order matters - more specific patterns should come first

    // Party: (Name) Message or (ⁿName√) Message
    chatPatterns.push_back({std::regex("^\\s*\\([^)]+\\)"), ChatMessageType::Party});

    // Tell sent: You>> PlayerName : Message
    chatPatterns.push_back({std::regex("^\\s*You>>"), ChatMessageType::Tell});

    // Tell received: Name>> or ⁿName√>> or >>Name
    chatPatterns.push_back({std::regex("^\\s*[^>]+>>"), ChatMessageType::Tell});
    chatPatterns.push_back({std::regex("^\\s*>>[^\\s]"), ChatMessageType::Tell});

    // Linkshell 1 and 2: [1]<Name> or [2]<Name> - CHECK FIRST
    chatPatterns.push_back({std::regex("\\[1\\]<"), ChatMessageType::Linkshell1});
    chatPatterns.push_back({std::regex("\\[2\\]<"), ChatMessageType::Linkshell2});

    // Party: (Name) Message or (ⁿName√) Message
    chatPatterns.push_back({std::regex("\\([^)]+\\)"), ChatMessageType::Party});

    // Tell: Name>> or ⁿName√>> or >>Name
    chatPatterns.push_back({std::regex("[^>]+>>"), ChatMessageType::Tell});
    chatPatterns.push_back({std::regex(">>[^\\s]"), ChatMessageType::Tell});

    // Unity: {Name} - must contain at least one letter (not just numbers like {3})
    chatPatterns.push_back({std::regex("^\\s*\\{[^}]*[A-Za-z][^}]*\\}"), ChatMessageType::Unity});

    // Say: Name : Message (must have space before and after colon)
    chatPatterns.push_back({std::regex("^\\s*[A-Za-z]+'?[A-Za-z]*\\s+:\\s+"), ChatMessageType::Say});

    // Drops: You find a
    chatPatterns.push_back({std::regex("You find a"), ChatMessageType::Drops});

    // Obtained: obtains a / Obtained key item:
    chatPatterns.push_back({std::regex("obtains a"), ChatMessageType::Obtained});
    chatPatterns.push_back({std::regex("^Obtained key item:"), ChatMessageType::Obtained});

    // Trial: Name 123: (name followed by number and colon)
    chatPatterns.push_back({std::regex("^[A-Za-z]+\\s+\\d+:"), ChatMessageType::Trial});

    // Default to system for anything else
}

void ChatLogProperty::RegisterCallback(ChatCallback callback)
{
    chatCallback = callback;
}

void ChatLogProperty::UnregisterCallback()
{
    chatCallback = nullptr;
}

// Helper function to remove FFXI special characters (color codes, etc.)
std::string ChatLogProperty::CleanFFXIString(const std::string& input)
{
    std::string result;
    for (unsigned char c : input)
    {
        // Only keep printable ASCII characters (space through ~)
        if (c >= 0x20 && c <= 0x7E)
        {
            result += c;
        }
    }
    return result;
}

ChatMessageType ChatLogProperty::DetermineChatType(const std::string& line)
{
    // Debug: print what we're trying to match
    std::cout << "[ChatLog] Determining type for: \"" << line << "\"" << std::endl;

    // Debug: Check if linkshell pattern exists as substring
    if (line.find("[2]<") != std::string::npos)
    {
        std::cout << "[ChatLog] Found [2]< substring!" << std::endl;
    }
    if (line.find("[1]<") != std::string::npos)
    {
        std::cout << "[ChatLog] Found [1]< substring!" << std::endl;
    }

    // Try each pattern in order
    int patternIndex = 0;
    for (const auto& pattern : chatPatterns)
    {
        try {
            if (std::regex_search(line, pattern.pattern))
            {
                std::cout << "[ChatLog] Matched pattern " << patternIndex << std::endl;
                return pattern.type;
            }
        } catch (const std::regex_error& e) {
            std::cout << "[ChatLog] Regex error at pattern " << patternIndex << ": " << e.what() << std::endl;
        }
        patternIndex++;
    }

    // Default to unknown if no pattern matches
    std::cout << "[ChatLog] No pattern matched - returning Unknown" << std::endl;
    return ChatMessageType::Unknown;
}

ChatMessage ChatLogProperty::ParseChatLine(const std::string& line)
{
    ChatMessage msg;

    if (line.empty())
    {
        return msg;
    }

    // Determine chat type using regex patterns
    msg.type = DetermineChatType(line);
    msg.timestamp = std::time(nullptr);

    // Extract sender and message based on type
    std::string trimmedLine = line;

    // Remove leading/trailing whitespace
    size_t start = trimmedLine.find_first_not_of(" \t\r\n");
    size_t end = trimmedLine.find_last_not_of(" \t\r\n");
    if (start != std::string::npos && end != std::string::npos)
    {
        trimmedLine = trimmedLine.substr(start, end - start + 1);
    }

    // Store the original raw content (after trimming)
    msg.rawContent = trimmedLine;

    switch (msg.type)
    {
        case ChatMessageType::Party:
        {
            // Format: (ⁿSenderName√) Message or (SenderName) Message
            size_t openParen = trimmedLine.find('(');
            size_t closeParen = trimmedLine.find(')');
            if (openParen != std::string::npos && closeParen != std::string::npos)
            {
                std::string rawSender = trimmedLine.substr(openParen + 1, closeParen - openParen - 1);
                msg.sender = CleanFFXIString(rawSender);
                msg.message = trimmedLine.substr(closeParen + 1);
                // Trim message
                size_t msgStart = msg.message.find_first_not_of(" \t");
                if (msgStart != std::string::npos)
                {
                    msg.message = msg.message.substr(msgStart);
                }
            }
            break;
        }

        case ChatMessageType::Tell:
        {
            // Format: ⁿSenderName√>> Message or SenderName>> Message or You>> RecipientName : Message or >>RecipientName : Message
            size_t arrowPos = trimmedLine.find(">>");
            if (arrowPos != std::string::npos)
            {
                if (arrowPos == 0)
                {
                    // Sent tell format: >>RecipientName : Message
                    std::cout << "[ChatLog] Parsing sent tell (>> at start)" << std::endl;
                    std::string afterArrow = trimmedLine.substr(2); // Skip the >>
                    std::cout << "[ChatLog] After arrow: '" << afterArrow << "'" << std::endl;
                    size_t colonPos = afterArrow.find(':');
                    std::cout << "[ChatLog] Colon position: " << colonPos << std::endl;
                    if (colonPos != std::string::npos)
                    {
                        std::string recipient = afterArrow.substr(0, colonPos);
                        std::cout << "[ChatLog] Recipient before trim: '" << recipient << "'" << std::endl;
                        // Trim recipient name
                        size_t nameStart = recipient.find_first_not_of(" \t");
                        if (nameStart != std::string::npos)
                        {
                            recipient = recipient.substr(nameStart);
                            size_t nameEnd = recipient.find_last_not_of(" \t");
                            if (nameEnd != std::string::npos)
                            {
                                recipient = recipient.substr(0, nameEnd + 1);
                            }
                        }
                        std::cout << "[ChatLog] Recipient after trim: '" << recipient << "'" << std::endl;
                        msg.sender = "You >> " + CleanFFXIString(recipient);
                        msg.message = afterArrow.substr(colonPos + 1);
                        std::cout << "[ChatLog] Message before trim: '" << msg.message << "'" << std::endl;
                        // Trim message
                        size_t msgStart = msg.message.find_first_not_of(" \t");
                        if (msgStart != std::string::npos)
                        {
                            msg.message = msg.message.substr(msgStart);
                        }
                        std::cout << "[ChatLog] Message after trim: '" << msg.message << "'" << std::endl;
                        std::cout << "[ChatLog] Sender: '" << msg.sender << "'" << std::endl;
                    }
                }
                else
                {
                    std::string rawSender = trimmedLine.substr(0, arrowPos);

                    // Check if this is a sent tell (You>>)
                    if (rawSender == "You")
                    {
                        // Sent tell: You>> RecipientName : Message
                        std::string afterArrow = trimmedLine.substr(arrowPos + 2);
                        size_t colonPos = afterArrow.find(':');
                        if (colonPos != std::string::npos)
                        {
                            std::string recipient = afterArrow.substr(0, colonPos);
                            // Trim recipient name
                            size_t nameStart = recipient.find_first_not_of(" \t");
                            if (nameStart != std::string::npos)
                            {
                                recipient = recipient.substr(nameStart);
                                size_t nameEnd = recipient.find_last_not_of(" \t");
                                if (nameEnd != std::string::npos)
                                {
                                    recipient = recipient.substr(0, nameEnd + 1);
                                }
                            }
                            msg.sender = "You >> " + CleanFFXIString(recipient);
                            msg.message = afterArrow.substr(colonPos + 1);
                            // Trim message
                            size_t msgStart = msg.message.find_first_not_of(" \t");
                            if (msgStart != std::string::npos)
                            {
                                msg.message = msg.message.substr(msgStart);
                            }
                        }
                    }
                    else
                    {
                        // Received tell
                        msg.sender = CleanFFXIString(rawSender);
                        msg.message = trimmedLine.substr(arrowPos + 2);
                        // Trim message
                        size_t msgStart = msg.message.find_first_not_of(" \t");
                        if (msgStart != std::string::npos)
                        {
                            msg.message = msg.message.substr(msgStart);
                        }
                    }
                }
            }
            break;
        }
        case ChatMessageType::Linkshell1:
        case ChatMessageType::Linkshell2:
        {
            // Format: [1]<ⁿSenderName√> Message or [2]<ⁿSenderName√> Message
            size_t openBracket = trimmedLine.find('<');
            size_t closeBracket = trimmedLine.find('>');
            if (openBracket != std::string::npos && closeBracket != std::string::npos)
            {
                std::string rawSender = trimmedLine.substr(openBracket + 1, closeBracket - openBracket - 1);
                msg.sender = CleanFFXIString(rawSender);
                msg.message = trimmedLine.substr(closeBracket + 1);
                // Trim message
                size_t msgStart = msg.message.find_first_not_of(" \t");
                if (msgStart != std::string::npos)
                {
                    msg.message = msg.message.substr(msgStart);
                }
            }
            break;
        }

        case ChatMessageType::Say:
        {
            // Format: SenderName : Message (space colon space)
            size_t colonPos = trimmedLine.find(" : ");
            if (colonPos != std::string::npos)
            {
                std::string rawSender = trimmedLine.substr(0, colonPos);
                msg.sender = CleanFFXIString(rawSender);
                msg.message = trimmedLine.substr(colonPos + 3); // +3 to skip " : "
            }
            break;
        }

        case ChatMessageType::Shout:
        {
            // Format: [SenderName] : Message
            size_t openBracket = trimmedLine.find('[');
            size_t closeBracket = trimmedLine.find(']');
            if (openBracket != std::string::npos && closeBracket != std::string::npos)
            {
                std::string rawSender = trimmedLine.substr(openBracket + 1, closeBracket - openBracket - 1);
                msg.sender = CleanFFXIString(rawSender);
                size_t colonPos = trimmedLine.find(':', closeBracket);
                if (colonPos != std::string::npos)
                {
                    msg.message = trimmedLine.substr(colonPos + 1);
                    // Trim message
                    size_t msgStart = msg.message.find_first_not_of(" \t");
                    if (msgStart != std::string::npos)
                    {
                        msg.message = msg.message.substr(msgStart);
                    }
                }
            }
            break;
        }

        case ChatMessageType::Yell:
        {
            // Format: SenderName[Zone]: Message
            size_t openBracket = trimmedLine.find('[');
            if (openBracket != std::string::npos)
            {
                std::string rawSender = trimmedLine.substr(0, openBracket);
                msg.sender = CleanFFXIString(rawSender);
                size_t colonPos = trimmedLine.find(':', openBracket);
                if (colonPos != std::string::npos)
                {
                    msg.message = trimmedLine.substr(colonPos + 1);
                    // Trim message
                    size_t msgStart = msg.message.find_first_not_of(" \t");
                    if (msgStart != std::string::npos)
                    {
                        msg.message = msg.message.substr(msgStart);
                    }
                }
            }
            break;
        }

        case ChatMessageType::Unity:
        {
            // Format: {ⁿSenderName√} Message or {SenderName} Message
            size_t openBrace = trimmedLine.find('{');
            size_t closeBrace = trimmedLine.find('}');
            if (openBrace != std::string::npos && closeBrace != std::string::npos)
            {
                std::string rawSender = trimmedLine.substr(openBrace + 1, closeBrace - openBrace - 1);
                msg.sender = CleanFFXIString(rawSender);
                msg.message = trimmedLine.substr(closeBrace + 1);
                // Trim message
                size_t msgStart = msg.message.find_first_not_of(" \t");
                if (msgStart != std::string::npos)
                {
                    msg.message = msg.message.substr(msgStart);
                }
            }
            break;
        }

        case ChatMessageType::Drops:
        case ChatMessageType::Obtained:
        case ChatMessageType::Trial:
        case ChatMessageType::System:
        default:
        {
            // System messages - entire line is the message
            msg.sender = "System";
            msg.message = trimmedLine;
            break;
        }
    }

    return msg;
}

void ChatLogProperty::refresh(const PlayerProcessInfo& processInfo)
{
    if (!processInfo.hProcess || processInfo.dllBase == 0)
    {
        return;
    }

    DWORD procId = processInfo.procId;

    // Step 1: Get pointer address at FFXiMain.dll + 0x00128AD4
    uintptr_t pointerAddress = processInfo.dllBase + CHAT_LOG_BASE;

    // Step 2: Read the pointer value at that address
    uintptr_t chatPointer = 0;
    if (!ReadProcessMemory(processInfo.hProcess, (LPCVOID)pointerAddress,
        &chatPointer, sizeof(chatPointer), nullptr))
    {
        // Failed to read pointer - not an error, just skip
        return;
    }

    // Step 3: Read from base pointer (offset 0x00) - this contains the complete message
    uintptr_t chatAddress = chatPointer + CHAT_LOG_OFFSET;

    // Read chat buffer from memory
    char chatBuffer[CHAT_BUFFER_SIZE];
    memset(chatBuffer, 0, CHAT_BUFFER_SIZE);

    if (!ReadProcessMemory(processInfo.hProcess, (LPCVOID)chatAddress,
        chatBuffer, CHAT_BUFFER_SIZE, nullptr))
    {
        // Failed to read - not an error, just skip
        return;
    }

    // Find the length of the string (up to first null terminator)
    // Note: FFXI multi-line messages may be truncated at line breaks
    size_t bufferLength = 0;
    for (size_t i = 0; i < CHAT_BUFFER_SIZE; i++)
    {
        if (chatBuffer[i] == '\0')
            break;
        bufferLength++;
    }

    if (bufferLength == 0)
    {
        // Empty buffer, nothing to process
        return;
    }

    // Remove FFXI control characters (0x01-0x1F, 0x7F-0x9F) before conversion
    // These are formatting/color codes that interfere with text encoding
    std::string cleanedBuffer;
    cleanedBuffer.reserve(bufferLength);

    for (size_t i = 0; i < bufferLength; i++)
    {
        unsigned char c = (unsigned char)chatBuffer[i];

        // Keep printable ASCII (0x20-0x7E) and high bytes for Japanese text (0xA0+)
        // Remove control characters (0x01-0x1F) and extended control chars (0x7F-0x9F)
        if ((c >= 0x20 && c <= 0x7E) || c >= 0xA0)
        {
            cleanedBuffer += c;
        }
    }

    // The text appears to already be in ASCII/UTF-8, not Shift-JIS
    // Just use the cleaned buffer directly
    std::string currentContent = cleanedBuffer;

    // Trim whitespace
    size_t start = currentContent.find_first_not_of(" \t\r\n");
    size_t end = currentContent.find_last_not_of(" \t\r\n");
    if (start != std::string::npos && end != std::string::npos)
    {
        currentContent = currentContent.substr(start, end - start + 1);
    }

    // Remove FFXI in-game timestamp prefix (e.g., "j[4:32:53pm] ")
    // Pattern: starts with 'j' or other char, followed by [timestamp]
    if (currentContent.length() > 0 && currentContent[0] == 'j' && currentContent.find('[') == 1)
    {
        size_t closeBracket = currentContent.find(']');
        if (closeBracket != std::string::npos)
        {
            // Check if this looks like a timestamp by looking for colons
            std::string potentialTimestamp = currentContent.substr(2, closeBracket - 2);
            if (potentialTimestamp.find(':') != std::string::npos)
            {
                // Remove the timestamp prefix including the space after ]
                size_t removeUntil = closeBracket + 1;
                if (removeUntil < currentContent.length() && currentContent[removeUntil] == ' ')
                {
                    removeUntil++;
                }
                currentContent = currentContent.substr(removeUntil);
            }
        }
    }

    // Remove duplicate starting character pattern (like "yYou" -> "You")
    // FFXI sometimes prefixes messages with a duplicate of the first letter
    if (currentContent.length() >= 2)
    {
        char first = currentContent[0];
        char second = currentContent[1];
        // Check if first two chars are same letter but different case, or both lowercase
        if (first == second || (std::tolower(first) == std::tolower(second) && std::isupper(second)))
        {
            // Remove the duplicate first character
            currentContent = currentContent.substr(1);
        }
    }

    // Remove FFXI end-of-line markers (digit suffixes like .1, .2, etc. at the end)
    // These appear to be FFXI's way of marking line endings
    // We only remove the digit, keeping the period since it's legitimate punctuation
    if (currentContent.length() >= 2)
    {
        size_t len = currentContent.length();
        // Check if ends with .<digit>
        if (len >= 2 && currentContent[len - 2] == '.' &&
            currentContent[len - 1] >= '0' && currentContent[len - 1] <= '9')
        {
            // Remove only the digit, keep the period
            currentContent = currentContent.substr(0, len - 1);
            // Trim any trailing whitespace that might be left
            end = currentContent.find_last_not_of(" \t\r\n");
            if (end != std::string::npos)
            {
                currentContent = currentContent.substr(0, end + 1);
            }
        }
    }

    // Check if content has changed
    if (lastChatContent.find(procId) != lastChatContent.end())
    {
        if (lastChatContent[procId] == currentContent)
        {
            // No change
            return;
        }
    }

    // Content changed - this is a new message!
    if (!currentContent.empty())
    {
        std::cout << "[ChatLog] New message detected for process " << procId << std::endl;
        std::cout << "[ChatLog] Raw content: " << currentContent << std::endl;

        // Parse the message
        ChatMessage msg = ParseChatLine(currentContent);

        if (!msg.message.empty())
        {
            std::cout << "[ChatLog] Parsed - Sender: " << msg.sender
                      << ", Type: " << msg.getMessageTypeString()
                      << ", Message: " << msg.message << std::endl;

            // Trigger callback
            if (chatCallback)
            {
                chatCallback(procId, msg);
            }
        }
    }

    // Update last content
    lastChatContent[procId] = currentContent;
}
