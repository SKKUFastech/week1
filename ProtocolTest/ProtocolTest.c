#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Define the callback function prototype
bool FAS_Connect(GtkButton *button, gpointer user_data);

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;
    GObject *button;
    GError *error = NULL;

    // Initialize GTK+
    gtk_init(&argc, &argv);

    // Create a new GtkBuilder
    builder = gtk_builder_new();

    // Load the UI from XML file
    if (!gtk_builder_add_from_file(builder, "ProtocolTest.glade", &error)) {
        g_printerr("Error loading UI file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Get the main window
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    // Show the main window
    gtk_widget_show_all(window);

    // Connect the signal handler (callback function) with user_data as builder
    button = gtk_builder_get_object(builder, "button_connect");
    g_signal_connect(button, "clicked", G_CALLBACK(FAS_Connect), builder);

    // Start the GTK+ main loop
    gtk_main();

    return 0;
}

bool FAS_Connect(GtkButton *button, gpointer user_data) {
    // Get the GtkBuilder object passed as user data
    GtkBuilder *builder = GTK_BUILDER(user_data);

    // Get the entry widget by its ID
    GtkEntry *entry_ip = GTK_ENTRY(gtk_builder_get_object(builder, "entry_ip"));

    // Get the entered text from the entry
    const gchar *ip_text = gtk_entry_get_text(entry_ip);

    // Check if the IP is valid (For a simple example, let's assume it's valid if it's not empty)
    if (g_strcmp0(ip_text, "") != 0) {
        g_print("IP: %s\n", ip_text);

        // Parse and store IP address in BYTE format
        guint8 sb1, sb2, sb3, sb4;
        gchar *ip_copy = g_strdup(ip_text); // 문자열 복사
        gchar *token, *saveptr;
        token = strtok_r(ip_copy, ".", &saveptr);
        if (token != NULL) {
            sb1 = atoi(token);
            token = strtok_r(NULL, ".", &saveptr);
            if (token != NULL) {
                sb2 = atoi(token);
                token = strtok_r(NULL, ".", &saveptr);
                if (token != NULL) {
                    sb3 = atoi(token);
                    token = strtok_r(NULL, ".", &saveptr);
                    if (token != NULL) {
                        sb4 = atoi(token);
                        // Now you have the IP address components as BYTE values (sb1, sb2, sb3, sb4)
                        g_print("Parsed IP: %d.%d.%d.%d\n", sb1, sb2, sb3, sb4);
                    }
                }
            }
        }
        g_free(ip_copy); // 할당된 문자열 메모리를 해제
        return true;
    } else {
        g_print("Please enter a valid IP.\n");
        return false;
    }
}
