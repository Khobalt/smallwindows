#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>

// UI Layout constants
extern const int TOOLBAR_HEIGHT;
extern const int STATUSBAR_HEIGHT;
extern const int MENUBAR_HEIGHT;
extern const int COLOR_PICKER_WIDTH;

// Menu IDs
#define IDM_FILE_NEW        1001
#define IDM_FILE_OPEN       1002
#define IDM_FILE_SAVE       1003
#define IDM_FILE_SAVEAS     1004
#define IDM_FILE_EXIT       1005
#define IDM_EDIT_UNDO       1006
#define IDM_EDIT_REDO       1007
#define IDM_EDIT_CLEAR      1008
#define IDM_VIEW_ZOOM_IN    1009
#define IDM_VIEW_ZOOM_OUT   1010
#define IDM_VIEW_ZOOM_FIT   1011
#define IDM_VIEW_GRID       1012
#define IDM_VIEW_THEME      1013
#define IDM_TOOLS_BRUSH     1014
#define IDM_TOOLS_ERASER    1015
#define IDM_TOOLS_RECT      1016
#define IDM_TOOLS_CIRCLE    1017
#define IDM_TOOLS_LINE      1018
#define IDM_HELP_ABOUT      1019

// Color palette
extern COLORREF colorPalette[];

#endif // CONFIG_H