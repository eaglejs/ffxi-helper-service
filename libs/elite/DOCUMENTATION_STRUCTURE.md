# Elite API Documentation Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                    START HERE: README.md                        │
│                  (Quick Start & Navigation)                     │
└────────────────────────────┬────────────────────────────────────┘
                             │
                ┌────────────┴────────────┐
                │                         │
                ▼                         ▼
┌───────────────────────────┐  ┌──────────────────────────────┐
│  DECOMPILED_API.md ⭐     │  │  DECOMPILATION_SUMMARY.md    │
│  (Primary Reference)       │  │  (Overview & Status)         │
│  • 32 Types                │  │  • What was done             │
│  • 180+ Methods            │  │  • How it was done           │
│  • Complete Signatures     │  │  • Next steps                │
│  • All Enums               │  │                              │
└───────────────────────────┘  └──────────────────────────────┘
                │
    ┌───────────┼───────────┐
    │           │           │
    ▼           ▼           ▼
┌─────────┐ ┌─────────┐ ┌────────────────┐
│EliteAPI │ │EliteMMO │ │ Your C++ Code  │
│.dll     │→│.API.dll │→│ (Integration)  │
│(Native) │ │(Managed)│ │                │
└─────────┘ └─────────┘ └────────────────┘
    │           │
    ▼           ▼
┌──────────────────────────────────┐
│  Supporting Documentation        │
├──────────────────────────────────┤
│ • EliteAPI.dll.README.md         │
│   (Native library overview)      │
│                                  │
│ • EliteMMO.API.dll.README.md     │
│   (Managed wrapper overview)     │
│                                  │
│ • extract_detailed_api.ps1       │
│   (Decompilation script)         │
└──────────────────────────────────┘
```

## Documentation Flow

### For Quick Reference
1. **Start:** [README.md](README.md)
2. **Lookup:** [DECOMPILED_API.md](DECOMPILED_API.md)
3. **Implement:** Use in your code

### For Deep Understanding
1. **Overview:** [EliteMMO.API.dll.README.md](EliteMMO.API.dll.README.md)
2. **Architecture:** [EliteAPI.dll.README.md](EliteAPI.dll.README.md)
3. **Details:** [DECOMPILED_API.md](DECOMPILED_API.md)
4. **Process:** [DECOMPILATION_SUMMARY.md](DECOMPILATION_SUMMARY.md)

### For Maintenance
1. **Script:** `extract_detailed_api.ps1`
2. **Run:** Execute to regenerate documentation
3. **Verify:** Check output in DECOMPILED_API.md

## Key Relationships

```
EliteAPI.dll (C++)
    ↓ (P/Invoke)
EliteMMO.API.dll (C#/.NET)
    ↓ (Decompiled via .NET Reflection)
DECOMPILED_API.md (Documentation)
    ↓ (Reference for)
Your Application Code (C++/C#)
```

## File Sizes & Content

| File | Size | Lines | Purpose |
|------|------|-------|---------|
| **DECOMPILED_API.md** | 43 KB | 2,013 | 🎯 Primary API reference |
| **README.md** | 6 KB | ~200 | 📚 Documentation index |
| **DECOMPILATION_SUMMARY.md** | 8 KB | ~350 | 📊 Process overview |
| **EliteMMO.API.dll.README.md** | 17 KB | ~600 | 📖 Managed wrapper guide |
| **EliteAPI.dll.README.md** | 12 KB | ~400 | 📖 Native library guide |

## Quick Access by Need

### "I want to..."

#### ...understand what methods are available
→ [DECOMPILED_API.md](DECOMPILED_API.md) - See all 180+ methods

#### ...get started quickly
→ [README.md](README.md) - Quick start examples

#### ...understand the architecture
→ [EliteMMO.API.dll.README.md](EliteMMO.API.dll.README.md) - Architecture diagrams

#### ...see enum values
→ [DECOMPILED_API.md](DECOMPILED_API.md) - All 31 enums documented

#### ...know how this was created
→ [DECOMPILATION_SUMMARY.md](DECOMPILATION_SUMMARY.md) - Full process

#### ...update the documentation
→ Run `extract_detailed_api.ps1` script

#### ...integrate with C++
→ [EliteAPI.dll.README.md](EliteAPI.dll.README.md) + Your includes/

#### ...integrate with C#
→ [DECOMPILED_API.md](DECOMPILED_API.md) + Reference EliteMMO.API.dll

---

## Visual Hierarchy

```
📁 libs/elite/
│
├── 📘 README.md ........................... START HERE
│   └── Links to all other docs
│
├── ⭐ DECOMPILED_API.md .................. MAIN REFERENCE
│   ├── EliteAPI class (180+ methods)
│   ├── 31 enums (jobs, items, status, etc.)
│   └── Complete signatures
│
├── 📊 DECOMPILATION_SUMMARY.md ........... OVERVIEW
│   ├── What was accomplished
│   ├── Statistics
│   └── Next steps
│
├── 📖 EliteMMO.API.dll.README.md ......... MANAGED DLL
│   ├── .NET wrapper details
│   ├── Architecture diagrams
│   └── Usage patterns
│
├── 📖 EliteAPI.dll.README.md ............. NATIVE DLL
│   ├── C/C++ library details
│   ├── Core functions
│   └── Memory management
│
├── 🔧 extract_detailed_api.ps1 ........... AUTOMATION
│   └── Regenerate DECOMPILED_API.md
│
├── 📦 EliteAPI.dll ....................... BINARY (C++)
└── 📦 EliteMMO.API.dll ................... BINARY (.NET)
```

---

## Documentation Quality Indicators

✅ **Complete** - All public API surface documented
✅ **Accurate** - Generated from DLL metadata
✅ **Current** - December 14, 2025
✅ **Authorized** - Creator permission granted
✅ **Reproducible** - Automated script available
✅ **Searchable** - Markdown format
✅ **Comprehensive** - 85+ KB of documentation

---

*This structure provides multiple entry points depending on your needs and experience level.*
