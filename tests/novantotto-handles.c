#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handle.h"

static LRESULT control_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    return 0;
}

static int live_handles(void)
{
    int count = 0;
    for (int i = 0; i < global_table->count; i++)
        count += global_table->entries[i].refcount != 0;
    return count;
}

int main(void)
{
    RESOURCE *main_window = AllocateHandle(HANDLE_WINDOW, NULL);
    RESOURCE *other_window = AllocateHandle(HANDLE_WINDOW, NULL);
    HWND main_id = main_window->handle;
    HWND other_id = other_window->handle;

    for (int cycle = 0; cycle < 500; cycle++)
    {
        RESOURCE *dialog = AllocateHandle(HANDLE_WINDOW, main_id);
        assert(dialog != NULL);
        HWND dialog_id = dialog->handle;
        for (int id = 1; id <= 24; id++)
        {
            RESOURCE *control = AllocateHandle(HANDLE_CONTROL, dialog_id);
            assert(control != NULL);
            control->control.nIDDlgItem = id;
            control->lpClassName = strdup("BUTTON");
            control->lpfnWndProc = control_proc;
        }
        RESOURCE *nested = AllocateHandle(HANDLE_WINDOW, dialog_id);
        assert(nested != NULL);
        HWND nested_id = nested->handle;
        RESOURCE *nested_control = AllocateHandle(HANDLE_CONTROL, nested_id);
        assert(nested_control != NULL);
        HWND nested_control_id = nested_control->handle;
        nested_control->lpClassName = strdup("BUTTON");

        ReleaseHandle(dialog);
        assert(GetHandle(dialog_id, HANDLE_ANY) == NULL);
        assert(GetHandle(nested_id, HANDLE_ANY) == NULL);
        assert(GetHandle(nested_control_id, HANDLE_ANY) == NULL);
        assert(GetDlgItem(dialog_id, 1) == NULL);
        assert(GetHandle(main_id, HANDLE_WINDOW) == main_window);
        assert(GetHandle(other_id, HANDLE_WINDOW) == other_window);
        assert(live_handles() == 2);

        // A slot previously occupied by a control must be safe to reuse as a window.
        RESOURCE *reused = AllocateHandle(HANDLE_WINDOW, main_id);
        assert(reused != NULL);
        RESOURCE *reused_control = AllocateHandle(HANDLE_WINDOW, main_id);
        assert(reused_control != NULL);
        assert(reused->lpClassName == NULL);
        assert(reused->lpfnWndProc == NULL);
        assert(reused_control->lpClassName == NULL);
        assert(reused_control->lpfnWndProc == NULL);
        ReleaseHandle(reused_control);
        ReleaseHandle(reused);
    }

    main_window->refcount++;
    ReleaseHandle(main_window);
    assert(GetHandle(main_id, HANDLE_WINDOW) == main_window);
    ReleaseHandle(main_window);
    ReleaseHandle(other_window);
    assert(live_handles() == 0);
    free(global_table->entries);
    free(global_table);
    puts("OK: 500 nested dialog cycles; controls released, slots reusable, unrelated windows intact");
    return 0;
}
