#include <furi.h>
#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/empty_screen.h>
#include <m-string.h>
#include <furi_hal_version.h>

typedef DialogMessageButton (*GeneralDialogScreen)(DialogsApp* dialogs, DialogMessage* message);

static DialogMessageButton main_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;

    const char* screen_header = "hosk made an app!\n"
                                "how neat is that?\n";
    const char* screen_text = "it's a basic copy of about, \n"
                              "and that's okay.";

    dialog_message_set_header(message, screen_header, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, screen_text, 0, 26, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_header(message, NULL, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);

    return result;
}

static DialogMessageButton other_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;

    const char* screen_text = "I can put text on another line \n"
                              "and even this one \n"
                              "i might delete this one";

    dialog_message_set_text(message, screen_text, 0, 0, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);

    return result;
}

const GeneralDialogScreen test_screens[] = {
    main_screen,
    other_screen
    };

const size_t test_screens_count = sizeof(test_screens) / sizeof(GeneralDialogScreen);

int32_t test_app(void* p) {
    DialogsApp* dialogs = furi_record_open("dialogs");
    DialogMessage* message = dialog_message_alloc();

    Gui* gui = furi_record_open("gui");
    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
    EmptyScreen* empty_screen = empty_screen_alloc();
    const uint32_t empty_screen_index = 0;

    size_t screen_index = 0;
    DialogMessageButton screen_result;

    // draw empty screen to prevent menu flickering
    view_dispatcher_add_view(
        view_dispatcher, empty_screen_index, empty_screen_get_view(empty_screen));
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_switch_to_view(view_dispatcher, empty_screen_index);

    while(1) {
        if(screen_index >= test_screens_count - 1) {
            dialog_message_set_buttons(message, "Back", NULL, NULL);
        } else {
            dialog_message_set_buttons(message, "Back", NULL, "Next");
        }

        screen_result = test_screens[screen_index](dialogs, message);

        if(screen_result == DialogMessageButtonLeft) {
            if(screen_index <= 0) {
                break;
            } else {
                screen_index--;
            }
        } else if(screen_result == DialogMessageButtonRight) {
            if(screen_index < test_screens_count) {
                screen_index++;
            }
        } else if(screen_result == DialogMessageButtonBack) {
            break;
        }
    }

    dialog_message_free(message);
    furi_record_close("dialogs");

    view_dispatcher_remove_view(view_dispatcher, empty_screen_index);
    view_dispatcher_free(view_dispatcher);
    empty_screen_free(empty_screen);
    furi_record_close("gui");

    return 0;
}
