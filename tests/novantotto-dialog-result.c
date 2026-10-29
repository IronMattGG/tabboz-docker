#include <assert.h>
#include <stdio.h>
#include "handle.h"

// Use the real DialogBox/EndDialog lifecycle with a deterministic event source.
BOOL PostMessage(HWND hwnd, WORD message, WORD wparam, LONG lparam)
{
    return TRUE;
}

BOOL GetMessage(LPMSG message, HWND hwnd, UINT min, UINT max)
{
    message->hwnd = hwnd;
    message->message = WM_COMMAND;
    message->wParam = 42;
    return TRUE;
}

LRESULT DispatchMessage(const MSG *message)
{
    RESOURCE *dialog = GetHandle(message->hwnd, HANDLE_WINDOW);
    return dialog->lpfnWndProc(message->hwnd, message->message, message->wParam, 0);
}

BOOL DestroyWindow(HWND hwnd)
{
    ReleaseHandle(GetHandle(hwnd, HANDLE_WINDOW));
    return TRUE;
}

static INT_PTR finish_dialog(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    return EndDialog(hwnd, wparam);
}

int main(void)
{
    EM_ASM({ globalThis.dialogBox = async () => {}; });
    for (int i = 0; i < 50; i++)
        assert(DialogBox(NULL, (LPCSTR)1, NULL, finish_dialog) == 42);
    puts("OK: DialogBox preserves its result after the window handle is released");
    free(global_table->entries);
    free(global_table);
    return 0;
}
