//
//  ImGuiCInclude.h
//  PBRendererCLibs
//
//  Created by Thomas Roughton on 25/05/16.
//  Copyright © 2016 Thomas Roughton. All rights reserved.
//

#ifndef ImGuiCInclude_h
#define ImGuiCInclude_h

typedef unsigned char bool;

typedef struct ImDrawChannel ImDrawChannel;               // Temporary storage for outputting drawing commands out of order, used by ImDrawList::ChannelsSplit()
typedef struct ImDrawList ImDrawList;                  // A single draw command list (generally one per window)
typedef struct ImFont ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
typedef struct ImFontAtlas ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF font loader
typedef struct ImFontConfig ImFontConfig;                // Configuration data when adding a font or merging fonts
typedef struct ImColor ImColor;                     // Helper functions to create a color that can be converted to either u32 or float4
typedef struct ImDrawCmd ImDrawCmd;
typedef struct ImGuiOnceUponAFrame ImGuiOnceUponAFrame;         // Simple helper for running a block of code not more than once a frame, used by IMGUI_ONCE_UPON_A_FRAME macro
typedef struct ImGuiStorage ImGuiStorage;                // Simple custom key value storage
typedef struct ImGuiStyle ImGuiStyle;                  // Runtime data for styling/colors
typedef struct ImGuiTextFilter ImGuiTextFilter;             // Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
typedef struct ImGuiTextBuffer ImGuiTextBuffer;             // Text buffer for logging/accumulating text
typedef struct ImGuiTextEditCallbackData ImGuiTextEditCallbackData;   // Shared state of ImGui::InputText() when using custom callbacks (advanced)
typedef struct ImGuiListClipper ImGuiListClipper;

// Enumerations (declared as int for compatibility and to not pollute the top of this file)
typedef unsigned int ImU32;
typedef unsigned short ImWchar;     // character for keyboard input/display
typedef void* ImTextureID;          // user data to identify a texture (this is whatever to you want it to be! read the FAQ about ImTextureID in imgui.cpp)
typedef ImU32 ImGuiID;              // unique ID used by widgets (typically hashed from a stack of string)
typedef int ImGuiCol;               // a color identifier for styling       // enum ImGuiCol_
typedef int ImGuiStyleVar;          // a variable identifier for styling    // enum ImGuiStyleVar_
typedef int ImGuiKey;               // a key identifier (ImGui-side enum)   // enum ImGuiKey_
typedef int ImGuiAlign;             // alignment                            // enum ImGuiAlign_
typedef int ImGuiColorEditMode;     // color edit mode for ColorEdit*()     // enum ImGuiColorEditMode_
typedef int ImGuiMouseCursor;       // a mouse cursor identifier            // enum ImGuiMouseCursor_
typedef int ImGuiWindowFlags;       // window flags for Begin*()            // enum ImGuiWindowFlags_
typedef int ImGuiSetCond;           // condition flags for Set*()           // enum ImGuiSetCond_
typedef int ImGuiInputTextFlags;    // flags for InputText*()               // enum ImGuiInputTextFlags_
typedef int ImGuiSelectableFlags;   // flags for Selectable()               // enum ImGuiSelectableFlags_
typedef int (*ImGuiTextEditCallback)(ImGuiTextEditCallbackData *data);

#ifndef ImDrawIdx
typedef unsigned short ImDrawIdx;
#endif


// Draw callbacks for advanced uses.
// NB- You most likely do NOT need to use draw callbacks just to create your own widget or customized UI rendering (you can poke into the draw list for that)
// Draw callback may be useful for example, if you want to render a complex 3D scene inside a UI element, change your GPU render state, etc.
// The expected behavior from your rendering loop is:
//   if (cmd.UserCallback != NULL)
//       cmd.UserCallback(parent_list, cmd);
//   else
//       RenderTriangles()
typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);

typedef struct ImVec2
{
    float x, y;
} ImVec2;

typedef struct ImVec4
{
    float x, y, z, w;
} ImVec4;

typedef struct ImDrawCmd
{
    unsigned int    ElemCount;              // Number of indices (multiple of 3) to be rendered as triangles. Vertices are stored in the callee ImDrawList's vtx_buffer[] array, indices in idx_buffer[].
    ImVec4          ClipRect;               // Clipping rectangle (x1, y1, x2, y2)
    ImTextureID     TextureId;              // User-provided texture ID. Set by user in ImfontAtlas::SetTexID() for fonts or passed to Image*() functions. Ignore if never using images or multiple fonts atlas.
    ImDrawCallback  UserCallback;           // If != NULL, call the function instead of rendering the vertices. clip_rect and texture_id will be set normally.
    void*           UserCallbackData;       // The draw callback code can access this.
} ImDrawCmd;

ImDrawCmd ImDrawCmdMake() {
    ImDrawCmd cmd;
    cmd.ElemCount = 0; cmd.ClipRect.x = cmd.ClipRect.y = -8192.0f; cmd.ClipRect.z = cmd.ClipRect.w = +8192.0f; cmd.TextureId = NULL; cmd.UserCallback = NULL; cmd.UserCallbackData = NULL;
    return cmd;
}

// Vertex layout
#ifndef IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT
typedef struct ImDrawVert
{
    ImVec2  pos;
    ImVec2  uv;
    ImU32   col;
} ImDrawVert;
#else
// You can override the vertex format layout by defining IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT in imconfig.h
// The code expect ImVec2 pos (8 bytes), ImVec2 uv (8 bytes), ImU32 col (4 bytes), but you can re-order them or add other fields as needed to simplify integration in your engine.
// The type has to be described within the macro (you can either declare the struct or use a typedef)
IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT;
#endif

typedef struct ImVector {
    int                         Size;
    int                         Capacity;
    void*                          Data;
} ImVector;

struct ImDrawList
{
    // This is what you have to render
    ImVector     CmdBuffer;          // Commands. Typically 1 command = 1 gpu draw call.
    ImVector     IdxBuffer;          // Index buffer. Each command consume ImDrawCmd::ElemCount of those
    ImVector    VtxBuffer;          // Vertex buffer.
    
    // [Internal, used while building lists]
    const char*             _OwnerName;         // Pointer to owner window's name (if any) for debugging
    unsigned int            _VtxCurrentIdx;     // [Internal] == VtxBuffer.Size
    ImDrawVert*             _VtxWritePtr;       // [Internal] point within VtxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImDrawIdx*              _IdxWritePtr;       // [Internal] point within IdxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImVector       _ClipRectStack;     // [Internal]
    ImVector   _TextureIdStack;    // [Internal]
    ImVector       _Path;              // [Internal] current path building
    int                     _ChannelsCurrent;   // [Internal] current channel number (0)
    int                     _ChannelsCount;     // [Internal] number of active channels (1+)
    ImVector _Channels;          // [Internal] draw channels for columns API (not resized down so _ChannelsCount may be smaller than _Channels.Size)
};

// All draw data to render an ImGui frame
typedef struct ImDrawData
{
    bool            Valid;                  // Only valid after Render() is called and before the next NewFrame() is called.
    ImDrawList**    CmdLists;
    int             CmdListsCount;
    int             TotalVtxCount;          // For convenience, sum of all cmd_lists vtx_buffer.Size
    int             TotalIdxCount;          // For convenience, sum of all cmd_lists idx_buffer.Size
} ImDrawData;

typedef struct ImFontAtlas {
    // Members
    // (Access texture data via GetTexData*() calls which will setup a default font for you.)
    void*                       TexID;              // User data to refer to the texture once it has been uploaded to user's graphic systems. It ia passed back to you during rendering.
    unsigned char*              TexPixelsAlpha8;    // 1 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight
    unsigned int*               TexPixelsRGBA32;    // 4 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight * 4
    int                         TexWidth;           // Texture width calculated during Build().
    int                         TexHeight;          // Texture height calculated during Build().
    int                         TexDesiredWidth;    // Texture width desired by user before Build(). Must be a power-of-two. If have many glyphs your graphics API have texture size restrictions you may want to increase texture width to decrease height.
    ImVec2                      TexUvWhitePixel;    // Texture coordinates to a white pixel
    ImVector          Fonts;              // Hold all the fonts returned by AddFont*. Fonts[0] is the default font upon calling ImGui::NewFrame(), use ImGui::PushFont()/PopFont() to change the current font.
    
    // Private
    ImVector      ConfigData;         // Internal data

} ImFontAtlas;


// Flags for ImGui::Begin()
enum ImGuiWindowFlags_
{
    // Default: 0
    ImGuiWindowFlags_NoTitleBar             = 1 << 0,   // Disable title-bar
    ImGuiWindowFlags_NoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
    ImGuiWindowFlags_NoMove                 = 1 << 2,   // Disable user moving the window
    ImGuiWindowFlags_NoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programatically)
    ImGuiWindowFlags_NoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel
    ImGuiWindowFlags_NoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it
    ImGuiWindowFlags_AlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
    ImGuiWindowFlags_ShowBorders            = 1 << 7,   // Show borders around windows and items
    ImGuiWindowFlags_NoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
    ImGuiWindowFlags_NoInputs               = 1 << 9,   // Disable catching mouse or keyboard inputs
    ImGuiWindowFlags_MenuBar                = 1 << 10,  // Has a menu-bar
    ImGuiWindowFlags_HorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
    ImGuiWindowFlags_NoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
    ImGuiWindowFlags_NoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
    ImGuiWindowFlags_AlwaysVerticalScrollbar= 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    ImGuiWindowFlags_AlwaysHorizontalScrollbar=1<< 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
    // [Internal]
    ImGuiWindowFlags_ChildWindow            = 1 << 20,  // Don't use! For internal use by BeginChild()
    ImGuiWindowFlags_ChildWindowAutoFitX    = 1 << 21,  // Don't use! For internal use by BeginChild()
    ImGuiWindowFlags_ChildWindowAutoFitY    = 1 << 22,  // Don't use! For internal use by BeginChild()
    ImGuiWindowFlags_ComboBox               = 1 << 23,  // Don't use! For internal use by ComboBox()
    ImGuiWindowFlags_Tooltip                = 1 << 24,  // Don't use! For internal use by BeginTooltip()
    ImGuiWindowFlags_Popup                  = 1 << 25,  // Don't use! For internal use by BeginPopup()
    ImGuiWindowFlags_Modal                  = 1 << 26,  // Don't use! For internal use by BeginPopupModal()
    ImGuiWindowFlags_ChildMenu              = 1 << 27   // Don't use! For internal use by BeginMenu()
};

// Flags for ImGui::InputText()
enum ImGuiInputTextFlags_
{
    // Default: 0
    ImGuiInputTextFlags_CharsDecimal        = 1 << 0,   // Allow 0123456789.+-*/
    ImGuiInputTextFlags_CharsHexadecimal    = 1 << 1,   // Allow 0123456789ABCDEFabcdef
    ImGuiInputTextFlags_CharsUppercase      = 1 << 2,   // Turn a..z into A..Z
    ImGuiInputTextFlags_CharsNoBlank        = 1 << 3,   // Filter out spaces, tabs
    ImGuiInputTextFlags_AutoSelectAll       = 1 << 4,   // Select entire text when first taking mouse focus
    ImGuiInputTextFlags_EnterReturnsTrue    = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to when the value was modified)
    ImGuiInputTextFlags_CallbackCompletion  = 1 << 6,   // Call user function on pressing TAB (for completion handling)
    ImGuiInputTextFlags_CallbackHistory     = 1 << 7,   // Call user function on pressing Up/Down arrows (for history handling)
    ImGuiInputTextFlags_CallbackAlways      = 1 << 8,   // Call user function every time. User code may query cursor position, modify text buffer.
    ImGuiInputTextFlags_CallbackCharFilter  = 1 << 9,   // Call user function to filter character. Modify data->EventChar to replace/filter input, or return 1 to discard character.
    ImGuiInputTextFlags_AllowTabInput       = 1 << 10,  // Pressing TAB input a '\t' character into the text field
    ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, allow exiting edition by pressing Enter. Ctrl+Enter to add new line (by default adds new lines with Enter).
    ImGuiInputTextFlags_NoHorizontalScroll  = 1 << 12,  // Disable following the cursor horizontally
    ImGuiInputTextFlags_AlwaysInsertMode    = 1 << 13,  // Insert mode
    ImGuiInputTextFlags_ReadOnly            = 1 << 14,  // Read-only mode
    ImGuiInputTextFlags_Password            = 1 << 15,  // Password mode, display all characters as '*'
    // [Internal]
    ImGuiInputTextFlags_Multiline           = 1 << 20   // For internal use by InputTextMultiline()
};

// Flags for ImGui::Selectable()
enum ImGuiSelectableFlags_
{
    // Default: 0
    ImGuiSelectableFlags_DontClosePopups    = 1 << 0,   // Clicking this don't close parent popup window
    ImGuiSelectableFlags_SpanAllColumns     = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
    ImGuiSelectableFlags_AllowDoubleClick   = 1 << 2    // Generate press events on double clicks too
};

// User fill ImGuiIO.KeyMap[] array with indices into the ImGuiIO.KeysDown[512] array
enum ImGuiKey_
{
    ImGuiKey_Tab,       // for tabbing through fields
    ImGuiKey_LeftArrow, // for text edit
    ImGuiKey_RightArrow,// for text edit
    ImGuiKey_UpArrow,   // for text edit
    ImGuiKey_DownArrow, // for text edit
    ImGuiKey_PageUp,
    ImGuiKey_PageDown,
    ImGuiKey_Home,      // for text edit
    ImGuiKey_End,       // for text edit
    ImGuiKey_Delete,    // for text edit
    ImGuiKey_Backspace, // for text edit
    ImGuiKey_Enter,     // for text edit
    ImGuiKey_Escape,    // for text edit
    ImGuiKey_A,         // for text edit CTRL+A: select all
    ImGuiKey_C,         // for text edit CTRL+C: copy
    ImGuiKey_V,         // for text edit CTRL+V: paste
    ImGuiKey_X,         // for text edit CTRL+X: cut
    ImGuiKey_Y,         // for text edit CTRL+Y: redo
    ImGuiKey_Z,         // for text edit CTRL+Z: undo
    ImGuiKey_COUNT
};

// Enumeration for PushStyleColor() / PopStyleColor()
enum ImGuiCol_
{
    ImGuiCol_Text,
    ImGuiCol_TextDisabled,
    ImGuiCol_WindowBg,              // Background of normal windows
    ImGuiCol_ChildWindowBg,         // Background of child windows
    ImGuiCol_PopupBg,               // Background of popups, menus, tooltips windows
    ImGuiCol_Border,
    ImGuiCol_BorderShadow,
    ImGuiCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
    ImGuiCol_FrameBgHovered,
    ImGuiCol_FrameBgActive,
    ImGuiCol_TitleBg,
    ImGuiCol_TitleBgCollapsed,
    ImGuiCol_TitleBgActive,
    ImGuiCol_MenuBarBg,
    ImGuiCol_ScrollbarBg,
    ImGuiCol_ScrollbarGrab,
    ImGuiCol_ScrollbarGrabHovered,
    ImGuiCol_ScrollbarGrabActive,
    ImGuiCol_ComboBg,
    ImGuiCol_CheckMark,
    ImGuiCol_SliderGrab,
    ImGuiCol_SliderGrabActive,
    ImGuiCol_Button,
    ImGuiCol_ButtonHovered,
    ImGuiCol_ButtonActive,
    ImGuiCol_Header,
    ImGuiCol_HeaderHovered,
    ImGuiCol_HeaderActive,
    ImGuiCol_Column,
    ImGuiCol_ColumnHovered,
    ImGuiCol_ColumnActive,
    ImGuiCol_ResizeGrip,
    ImGuiCol_ResizeGripHovered,
    ImGuiCol_ResizeGripActive,
    ImGuiCol_CloseButton,
    ImGuiCol_CloseButtonHovered,
    ImGuiCol_CloseButtonActive,
    ImGuiCol_PlotLines,
    ImGuiCol_PlotLinesHovered,
    ImGuiCol_PlotHistogram,
    ImGuiCol_PlotHistogramHovered,
    ImGuiCol_TextSelectedBg,
    ImGuiCol_ModalWindowDarkening,  // darken entire screen when a modal window is active
    ImGuiCol_COUNT
};

// Enumeration for PushStyleVar() / PopStyleVar()
// NB: the enum only refers to fields of ImGuiStyle() which makes sense to be pushed/poped in UI code. Feel free to add others.
enum ImGuiStyleVar_
{
    ImGuiStyleVar_Alpha,               // float
    ImGuiStyleVar_WindowPadding,       // ImVec2
    ImGuiStyleVar_WindowRounding,      // float
    ImGuiStyleVar_WindowMinSize,       // ImVec2
    ImGuiStyleVar_ChildWindowRounding, // float
    ImGuiStyleVar_FramePadding,        // ImVec2
    ImGuiStyleVar_FrameRounding,       // float
    ImGuiStyleVar_ItemSpacing,         // ImVec2
    ImGuiStyleVar_ItemInnerSpacing,    // ImVec2
    ImGuiStyleVar_IndentSpacing,       // float
    ImGuiStyleVar_GrabMinSize          // float
};

enum ImGuiAlign_
{
    ImGuiAlign_Left     = 1 << 0,
    ImGuiAlign_Center   = 1 << 1,
    ImGuiAlign_Right    = 1 << 2,
    ImGuiAlign_Top      = 1 << 3,
    ImGuiAlign_VCenter  = 1 << 4,
    ImGuiAlign_Default  = ImGuiAlign_Left | ImGuiAlign_Top
};

// Enumeration for ColorEditMode()
enum ImGuiColorEditMode_
{
    ImGuiColorEditMode_UserSelect = -2,
    ImGuiColorEditMode_UserSelectShowButton = -1,
    ImGuiColorEditMode_RGB = 0,
    ImGuiColorEditMode_HSV = 1,
    ImGuiColorEditMode_HEX = 2
};

// Enumeration for GetMouseCursor()
enum ImGuiMouseCursor_
{
    ImGuiMouseCursor_Arrow = 0,
    ImGuiMouseCursor_TextInput,         // When hovering over InputText, etc.
    ImGuiMouseCursor_Move,              // Unused
    ImGuiMouseCursor_ResizeNS,          // Unused
    ImGuiMouseCursor_ResizeEW,          // When hovering over a column
    ImGuiMouseCursor_ResizeNESW,        // Unused
    ImGuiMouseCursor_ResizeNWSE,        // When hovering over the bottom-right corner of a window
    ImGuiMouseCursor_Count_
};

// Condition flags for ImGui::SetWindow***(), SetNextWindow***(), SetNextTreeNode***() functions
// All those functions treat 0 as a shortcut to ImGuiSetCond_Always
enum ImGuiSetCond_
{
    ImGuiSetCond_Always        = 1 << 0, // Set the variable
    ImGuiSetCond_Once          = 1 << 1, // Only set the variable on the first call per runtime session
    ImGuiSetCond_FirstUseEver  = 1 << 2, // Only set the variable if the window doesn't exist in the .ini file
    ImGuiSetCond_Appearing     = 1 << 3  // Only set the variable if the window is appearing after being inactive (or the first time)
};

struct ImGuiStyle
{
    float       Alpha;                      // Global alpha applies to everything in ImGui
    ImVec2      WindowPadding;              // Padding within a window
    ImVec2      WindowMinSize;              // Minimum window size
    float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
    ImGuiAlign  WindowTitleAlign;           // Alignment for title bar text
    float       ChildWindowRounding;        // Radius of child window corners rounding. Set to 0.0f to have rectangular windows
    ImVec2      FramePadding;               // Padding within a framed rectangle (used by most widgets)
    float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
    ImVec2      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines
    ImVec2      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
    ImVec2      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node
    float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns
    float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar
    float       ScrollbarRounding;          // Radius of grab corners for scrollbar
    float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar
    float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    ImVec2      DisplayWindowPadding;       // Window positions are clamped to be visible within the display area by at least this amount. Only covers regular windows.
    ImVec2      DisplaySafeAreaPadding;     // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
    bool        AntiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
    bool        AntiAliasedShapes;          // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    float       CurveTessellationTol;       // Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    ImVec4      Colors[ImGuiCol_COUNT];
};

// This is where your app communicate with ImGui. Access via ImGui::GetIO().
// Read 'Programmer guide' section in .cpp file for general usage.
typedef struct ImGuiIO
{
    //------------------------------------------------------------------
    // Settings (fill once)                 // Default value:
    //------------------------------------------------------------------
    
    ImVec2        DisplaySize;              // <unset>              // Display size, in pixels. For clamping windows positions.
    float         DeltaTime;                // = 1.0f/60.0f         // Time elapsed since last frame, in seconds.
    float         IniSavingRate;            // = 5.0f               // Maximum time between saving positions/sizes to .ini file, in seconds.
    const char*   IniFilename;              // = "imgui.ini"        // Path to .ini file. NULL to disable .ini saving.
    const char*   LogFilename;              // = "imgui_log.txt"    // Path to .log file (default parameter to ImGui::LogToFile when no file is specified).
    float         MouseDoubleClickTime;     // = 0.30f              // Time for a double-click, in seconds.
    float         MouseDoubleClickMaxDist;  // = 6.0f               // Distance threshold to stay in to validate a double-click, in pixels.
    float         MouseDragThreshold;       // = 6.0f               // Distance threshold before considering we are dragging
    int           KeyMap[ImGuiKey_COUNT];   // <unset>              // Map of indices into the KeysDown[512] entries array
    float         KeyRepeatDelay;           // = 0.250f             // When holding a key/button, time before it starts repeating, in seconds. (for actions where 'repeat' is active)
    float         KeyRepeatRate;            // = 0.020f             // When holding a key/button, rate at which it repeats, in seconds.
    void*         UserData;                 // = NULL               // Store your own data for retrieval by callbacks.
    
    ImFontAtlas*  Fonts;                    // <auto>               // Load and assemble one or more fonts into a single tightly packed texture. Output to Fonts array.
    float         FontGlobalScale;          // = 1.0f               // Global scale all fonts
    bool          FontAllowUserScaling;     // = false              // Allow user scaling text of individual window with CTRL+Wheel.
    ImVec2        DisplayFramebufferScale;  // = (1.0f,1.0f)        // For retina display or other situations where window coordinates are different from framebuffer coordinates. User storage only, presently not used by ImGui.
    ImVec2        DisplayVisibleMin;        // <unset> (0.0f,0.0f)  // If you use DisplaySize as a virtual space larger than your screen, set DisplayVisibleMin/Max to the visible area.
    ImVec2        DisplayVisibleMax;        // <unset> (0.0f,0.0f)  // If the values are the same, we defaults to Min=(0.0f) and Max=DisplaySize
    
    // Advanced/subtle behaviors
    bool          WordMovementUsesAltKey;   // = defined(__APPLE__) // OS X style: Text editing cursor movement using Alt instead of Ctrl
    bool          ShortcutsUseSuperKey;     // = defined(__APPLE__) // OS X style: Shortcuts using Cmd/Super instead of Ctrl
    bool          DoubleClickSelectsWord;   // = defined(__APPLE__) // OS X style: Double click selects by word instead of selecting whole text
    bool          MultiSelectUsesSuperKey;  // = defined(__APPLE__) // OS X style: Multi-selection in lists uses Cmd/Super instead of Ctrl [unused yet]
    
    //------------------------------------------------------------------
    // User Functions
    //------------------------------------------------------------------
    
    // Rendering function, will be called in Render().
    // Alternatively you can keep this to NULL and call GetDrawData() after Render() to get the same pointer.
    // See example applications if you are unsure of how to implement this.
    void        (*RenderDrawListsFn)(ImDrawData* data);
    
    // Optional: access OS clipboard
    // (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
    const char* (*GetClipboardTextFn)();
    void        (*SetClipboardTextFn)(const char* text);
    
    // Optional: override memory allocations. MemFreeFn() may be called with a NULL pointer.
    // (default to posix malloc/free)
    void*       (*MemAllocFn)(size_t sz);
    void        (*MemFreeFn)(void* ptr);
    
    // Optional: notify OS Input Method Editor of the screen position of your cursor for text input position (e.g. when using Japanese/Chinese IME in Windows)
    // (default to use native imm32 api on Windows)
    void        (*ImeSetInputScreenPosFn)(int x, int y);
    void*       ImeWindowHandle;            // (Windows) Set this to your HWND to get automatic IME cursor positioning.
    
    //------------------------------------------------------------------
    // Input - Fill before calling NewFrame()
    //------------------------------------------------------------------
    
    ImVec2      MousePos;                   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    bool        MouseDown[5];               // Mouse buttons: left, right, middle + extras. ImGui itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
    float       MouseWheel;                 // Mouse wheel: 1 unit scrolls about 5 lines text.
    bool        MouseDrawCursor;            // Request ImGui to draw a mouse cursor for you (if you are on a platform without a mouse cursor).
    bool        KeyCtrl;                    // Keyboard modifier pressed: Control
    bool        KeyShift;                   // Keyboard modifier pressed: Shift
    bool        KeyAlt;                     // Keyboard modifier pressed: Alt
    bool        KeySuper;                   // Keyboard modifier pressed: Cmd/Super/Windows
    bool        KeysDown[512];              // Keyboard keys that are pressed (in whatever storage order you naturally have access to keyboard data)
    ImWchar     InputCharacters[16+1];      // List of characters input (translated by user from keypress+keyboard state). Fill using AddInputCharacter() helper.
    
    //------------------------------------------------------------------
    // Output - Retrieve after calling NewFrame(), you can use them to discard inputs or hide them from the rest of your application
    //------------------------------------------------------------------
    
    bool        WantCaptureMouse;           // Mouse is hovering a window or widget is active (= ImGui will use your mouse input)
    bool        WantCaptureKeyboard;        // Widget is active (= ImGui will use your keyboard input)
    bool        WantTextInput;              // Some text input widget is active, which will read input characters from the InputCharacters array.
    float       Framerate;                  // Framerate estimation, in frame per second. Rolling average estimation based on IO.DeltaTime over 120 frames
    int         MetricsAllocs;              // Number of active memory allocations
    int         MetricsRenderVertices;      // Vertices output during last call to Render()
    int         MetricsRenderIndices;       // Indices output during last call to Render() = number of triangles * 3
    int         MetricsActiveWindows;       // Number of visible windows (exclude child windows)
    
    //------------------------------------------------------------------
    // [Internal] ImGui will maintain those fields for you
    //------------------------------------------------------------------
    
    ImVec2      MousePosPrev;               // Previous mouse position
    ImVec2      MouseDelta;                 // Mouse delta. Note that this is zero if either current or previous position are negative to allow mouse enabling/disabling.
    bool        MouseClicked[5];            // Mouse button went from !Down to Down
    ImVec2      MouseClickedPos[5];         // Position at time of clicking
    float       MouseClickedTime[5];        // Time of last click (used to figure out double-click)
    bool        MouseDoubleClicked[5];      // Has mouse button been double-clicked?
    bool        MouseReleased[5];           // Mouse button went from Down to !Down
    bool        MouseDownOwned[5];          // Track if button was clicked inside a window. We don't request mouse capture from the application if click started outside ImGui bounds.
    float       MouseDownDuration[5];       // Duration the mouse button has been down (0.0f == just clicked)
    float       MouseDownDurationPrev[5];   // Previous time the mouse button has been down
    float       MouseDragMaxDistanceSqr[5]; // Squared maximum distance of how much mouse has traveled from the click point
    float       KeysDownDuration[512];      // Duration the keyboard key has been down (0.0f == just pressed)
    float       KeysDownDurationPrev[512];  // Previous duration the key has been down
} ImGuiIO;

void ImDrawVert_Offsets(void **pos, void **uv, void **col) {
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    *pos = (void*)OFFSETOF(ImDrawVert, pos);
    *uv = (void*)OFFSETOF(ImDrawVert, uv);
    *col = (void*)OFFSETOF(ImDrawVert, col);
#undef OFFSETOF
}

#endif /* ImGuiCInclude_h */
