#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include <arpa/inet.h>

// 나중에 라이브러리로 분리할 FASTECH 관련 함수, 변수, 자료형

typedef uint8_t BYTE; //메뉴얼상에서 BYTE라는 자료형을 쓰는데 일단 사용자 정의로 만들어둠 unsigned char와 unit8_t모두 같은 역할인데 둘중 하나로가야 중간중간 형변환 관련해서 이슈가 안생길듯함

#define BUFFER_SIZE 258
#define PORT 3001 //UDP GUI

int client_socket;
BYTE header, sync_no, frame_type;
BYTE buffer[BUFFER_SIZE];

char *protocol;

bool FAS_Connect(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID);
bool FAS_ConnectTCP(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID);

void FAS_Close(int iBdID);

// Define the callback function prototype
static void on_button_connect_clicked(GtkButton *button, gpointer user_data);

static void on_combo_protocol_changed(GtkComboBoxText *combo_text, gpointer user_data);
static void on_combo_command_changed(GtkComboBox *combo_id, gpointer user_data);
static void on_combo_data1_changed(GtkComboBox *combo_id, gpointer user_data);

static void on_check_autosync_toggled(GtkToggleButton *togglebutton, gpointer user_data);
static void on_check_fastech_toggled(GtkToggleButton *togglebutton, gpointer user_data);

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;
    GObject *button;
    GtkComboBoxText *combo_text;
    GtkComboBox* combo_id;
    GObject *checkbox;
    GError *error = NULL;
    
    srand(time(NULL));

    header = 0xAA;
    sync_no = (BYTE)(rand() % 256);
    
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
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_connect_clicked), builder);
    
    combo_text = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_protocol"));
    g_signal_connect(combo_text, "changed", G_CALLBACK(on_combo_protocol_changed), NULL);
    
    combo_id = GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_command"));
    g_signal_connect(combo_id, "changed", G_CALLBACK(on_combo_command_changed), NULL);
    combo_id = GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_data1"));
    g_signal_connect(combo_id, "changed", G_CALLBACK(on_combo_command_changed), NULL);


    checkbox = gtk_builder_get_object(builder, "check_autosync");
    g_signal_connect(checkbox, "toggled", G_CALLBACK(on_check_autosync_toggled), NULL);
    checkbox = gtk_builder_get_object(builder, "check_fastech");
    g_signal_connect(checkbox, "toggled", G_CALLBACK(on_check_fastech_toggled), NULL);

    // Start the GTK+ main loop
    gtk_main();

    return 0;
}

/************************************************************************************************************************************
 ********************************GUI 프로그램의 버튼 등 구성요소들에서 사용하는 callback함수*************************************************
 ************************************************************************************************************************************/
 
 // 연결 버튼 입력
static void on_button_connect_clicked(GtkButton *button, gpointer user_data) {
    BYTE sb1, sb2, sb3, sb4;
    // Get the GtkBuilder object passed as user data
    GtkBuilder *builder = GTK_BUILDER(user_data);

    // Get the entry widget by its ID
    GtkEntry *entry_ip = GTK_ENTRY(gtk_builder_get_object(builder, "entry_ip"));

    // Get the entered text from the entry
    const char *ip_text = gtk_entry_get_text(entry_ip);

    // Check if the IP is valid (For a simple example, let's assume it's valid if it's not empty)
    if (g_strcmp0(ip_text, "") != 0) {
        g_print("IP: %s\n", ip_text);

        // Parse and store IP address in BYTE format
        char *ip_copy = g_strdup(ip_text); // 문자열 복사
        char *token, *saveptr;

        for (int i = 0; i < 4; i++) {
            token = strtok_r(i == 0 ? ip_copy : NULL, ".", &saveptr);
            if (token == NULL) {
                g_print("Invalid IP format: %s\n", ip_text);
                g_free(ip_copy);
                return;
            }
            BYTE byte_val = atoi(token);
            switch (i) {
                case 0:
                    sb1 = byte_val;
                    break;
                case 1:
                    sb2 = byte_val;
                    break;
                case 2:
                    sb3 = byte_val;
                    break;
                case 3:
                    sb4 = byte_val;
                    break;
            }
        }
        g_print("Parsed IP: %d.%d.%d.%d\n", sb1, sb2, sb3, sb4);
        g_free(ip_copy);
    }
    else {
            g_print("Please enter a valid IP.\n");
    }
        
    if(strcmp(protocol, "TCP") == 0){
        FAS_ConnectTCP(sb1, sb2, sb3, sb4, 0);
        g_print("Selected Protocol: %s\n", protocol);
    }
    else if(strcmp(protocol, "UDP") == 0){
        FAS_Connect(sb1, sb2, sb3, sb4, 0);
        g_print("Selected Protocol: %s\n", protocol);
    }
    else if(protocol != NULL){
        g_print("Select Protocol\n");
    }
}

// TCP/UDP 프로토콜 선택하는 콤보박스(TEXT를 가져오는 방식)
static void on_combo_protocol_changed(GtkComboBoxText *combo_text, gpointer user_data) {
    protocol = gtk_combo_box_text_get_active_text(combo_text);
    if (protocol != NULL) {
        g_print("Selected Protocol: %s\n", protocol);
    }
}

// 커맨드를 선택하는 콤보박스(ID를 가져오는 방식)
static void on_combo_command_changed(GtkComboBox *combo_id, gpointer user_data) {
    const gchar *selected_id = gtk_combo_box_get_active_id(combo_id);

    if (selected_id  != NULL) {
        g_print("Selected Command: %s\n", selected_id);
        char* endptr;
        unsigned long int value = strtoul(selected_id, &endptr, 16);
        if (*endptr == '\0' && value <= UINT8_MAX) {
            frame_type = (uint8_t)value;
            printf("Converted Frame: %X \n", frame_type);
        } else {
            printf("Invalid input: %s\n", selected_id);
        }
    } else {
        g_print("No item selected.\n");
    }
}

// 커맨드의 데이터 선택하는 콤보박스(ID를 가져오는 방식)
static void on_combo_data1_changed(GtkComboBox *combo_id, gpointer user_data) {
    const gchar *selected_id = gtk_combo_box_get_active_id(combo_id);

    if (selected_id != NULL) {
        g_print("Selected Command: %s\n", selected_id);
        char* endptr;
        unsigned long int value = strtoul(selected_id, &endptr, 16);
        if (*endptr == '\0' && value <= UINT8_MAX) {
            buffer[5] = (uint8_t)value;
            printf("Converted Frame: %X \n", buffer[5]);
        } else {
            printf("Invalid input: %s\n", selected_id);
        }
    } else {
        g_print("No item selected.\n");
    }
}


// AutoSync 체크박스
static void on_check_autosync_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    gboolean is_checked = gtk_toggle_button_get_active(togglebutton);
    if (is_checked) {
        sync_no = (BYTE)(rand() % 256);
        g_print("Auto Sync Enabled Sync No: %X \n",sync_no);
    } else {
        sync_no = 0x00;
        g_print("Auto Sync Disabled Sync No: %X \n",sync_no);
    }
}

// FASTECH 프로토콜 체크박스
static void on_check_fastech_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    gboolean is_checked = gtk_toggle_button_get_active(togglebutton);
    if (is_checked) {
        header = 0xAA;
        g_print("FASTECH Protocol header: %X \n",header);
    } else {
        header = 0x00;
        g_print("USER Protocol header: %X \n",header);
    }
}

/************************************************************************************************************************************
 ********************************나중에 라이브러리로 뺄 FASTECH 라이브러리와 같은 기능의 함수*************************************************
 ************************************************************************************************************************************/
 
bool FAS_Connect(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID){
    char SERVER_IP[16]; //최대 길이 가정 "xxx.xxx.xxx.xxx\0" 
    snprintf(SERVER_IP, sizeof(SERVER_IP), "%u.%u.%u.%u", sb1, sb2, sb3, sb4);
    
    struct sockaddr_in server_addr;
    
    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }
    else{
        g_print("Socket created\n");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }
    else{
        g_print("Valid address\n");
    }
}
 
bool FAS_ConnectTCP(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID){
    
}

void FAS_Close(int iBdID){
    close(client_socket);
}
