/** 
 * @file ProtocolTest.c
 * @date 2023/7/17~
 * @author Minho Shin (smh9800@g.skku.edu)
 * @version 0.0.0.1
 * @brief Fastech 프로그램의 Protocol Test 구현을 위한 프로그램
 * @details C언어와 GTK3(라즈비안(데비안11) 호환을 위해서), GLADE(UI XML->.glade파일) 사용
 * Ethernet 부분(Ezi Servo Plus-E 모델용)만 구현, 
 * 라이브러리로 분리할 만한 기본 함수, GUI프로그램 구현 함수가 섞인 상태
 * @warning 동작 시 예외처리가 제대로 안되어있으니 정확한 절차로만 작동시킬것
 */

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

#ifndef DWORD
typedef uint32_t DWORD;
#endif
/************************************************************************************************************************************
 ********************************나중에 라이브러리로 뺄 FASTECH 라이브러리와 같은 기능의 함수*************************************************
 ************************************************************************************************************************************/
 
typedef uint8_t BYTE;
typedef struct {
    BYTE frame_type;
    void (*handler)(uint8_t data);
} HandlerMapping;

#define BUFFER_SIZE 258
#define PORT 3001 //UDP GUI

int client_socket;
struct sockaddr_in server_addr;

static BYTE header, sync_no, frame_type, data;
static BYTE buffer[BUFFER_SIZE];

char *protocol;

bool FAS_Connect(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID);
bool FAS_ConnectTCP(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID);

void FAS_Close(int iBdID);

int FAS_ServoEnable(int iBdID, bool bOnOff);
int FAS_MoveOriginSingleAxis(int iBdID);
int FAS_MoveStop(int iBdID);
int FAS_MoveVelocity(int iBdID, DWORD lVelocity, int iVelDir);

/************************************************************************************************************************************
 ********************************GUI 프로그램의 버튼 등 구성요소들에서 사용하는 callback함수*************************************************
 ************************************************************************************************************************************/
 
static void on_button_connect_clicked(GtkButton *button, gpointer user_data);
static void on_button_send_clicked(GtkButton *button, gpointer user_data);

static void on_combo_protocol_changed(GtkComboBoxText *combo_text, gpointer user_data);
static void on_combo_command_changed(GtkComboBox *combo_id, gpointer user_data);
static void on_combo_data1_changed(GtkComboBox *combo_id, gpointer user_data);

static void on_check_autosync_toggled(GtkToggleButton *togglebutton, gpointer user_data);
static void on_check_fastech_toggled(GtkToggleButton *togglebutton, gpointer user_data);

/************************************************************************************************************************************
 ******************************************************* 편의상 만든 함수 **************************************************************
 ************************************************************************************************************************************/
 
void print_buffer(uint8_t *array, size_t size);
void library_interface();

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
    
    // GTK 초기화
    gtk_init(&argc, &argv);

    // GtkBuilder 생성
    builder = gtk_builder_new();

    // XML file에서 UI불러오기
    if (!gtk_builder_add_from_file(builder, "ProtocolTest.glade", &error)) {
        g_printerr("Error loading UI file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Window 생성
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    gtk_widget_show_all(window);
    
    GtkStack *stk2 = GTK_STACK(gtk_builder_get_object(builder, "stk2"));
    
    // callback 함수 연결, user_data를 빌더로 사용함
    button = gtk_builder_get_object(builder, "button_connect");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_connect_clicked), builder);
    button = gtk_builder_get_object(builder, "button_send");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_send_clicked), builder);
    
    combo_text = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_protocol"));
    g_signal_connect(combo_text, "changed", G_CALLBACK(on_combo_protocol_changed), NULL);
    
    combo_id = GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_command"));
    g_signal_connect(combo_id, "changed", G_CALLBACK(on_combo_command_changed), stk2);
    combo_id = GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_data1"));
    g_signal_connect(combo_id, "changed", G_CALLBACK(on_combo_data1_changed), NULL);


    checkbox = gtk_builder_get_object(builder, "check_autosync");
    g_signal_connect(checkbox, "toggled", G_CALLBACK(on_check_autosync_toggled), NULL);
    checkbox = gtk_builder_get_object(builder, "check_fastech");
    g_signal_connect(checkbox, "toggled", G_CALLBACK(on_check_fastech_toggled), NULL);

    // Start the GTK main loop
    gtk_main();

    return 0;
}

/************************************************************************************************************************************
 ********************************GUI 프로그램의 버튼 등 구성요소들에서 사용하는 callback함수*************************************************
 ************************************************************************************************************************************/
 
 /**@brief Connect버튼의 callback*/
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

 /**@brief Send버튼의 callback*/
static void on_button_send_clicked(GtkButton *button, gpointer user_data){
    sync_no++;
    
    library_interface();
    
    int send_result = sendto(client_socket, buffer, buffer[1] + 2, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr)); 
    if (send_result < 0) {
        perror("sendto failed");
    }
    while(1){
        ssize_t received_bytes = recvfrom(client_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if (received_bytes < 0) {
            perror("recvfrom failed");
            continue;
        }

        // Print the received data in hexadecimal format
        printf("Server: ");
        for (ssize_t i = 0; i < received_bytes; i++) {
            printf("%02x ", (BYTE)buffer[i]);
        }
        printf("\n");
        break;
    }
    memset(&buffer, 0, sizeof(buffer));
}

 /**@brief TCP/UDP 프로토콜 선택 콤보박스의 callback*/
static void on_combo_protocol_changed(GtkComboBoxText *combo_text, gpointer user_data) {
    protocol = gtk_combo_box_text_get_active_text(combo_text);
    if (protocol != NULL) {
        g_print("Selected Protocol: %s\n", protocol);
    }
}

 /**@brief 명령어 콤보박스 combo_command의 callback*/
static void on_combo_command_changed(GtkComboBox *combo_id, gpointer user_data) {
    const gchar *selected_id = gtk_combo_box_get_active_id(combo_id);
    
    if (selected_id != NULL) {
        GtkStack *stk2 = GTK_STACK(user_data);

        // 선택한 옵션에 따라 보여지는 페이지를 변경
        if (g_strcmp0(selected_id, "0x2A") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page1");
        } 
        else if (g_strcmp0(selected_id, "0x31") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page0");
        }
        else if (g_strcmp0(selected_id, "0x33") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page0");
        }
        else if (g_strcmp0(selected_id, "0x37") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page2");
        }
    }
    
    if (selected_id  != NULL) {
        g_print("Selected Command: %s\n", selected_id);
        char* endptr;
        unsigned long int value = strtoul(selected_id, &endptr, 16);
        if (*endptr == '\0' && value <= UINT8_MAX) {
            frame_type = (uint8_t)value;
        } else {
            g_print("Invalid input: %s\n", selected_id);
        }
    } else {
        g_print("No item selected.\n");
    }
    g_print("Converted Frame: %X \n", frame_type);
}

 /**@brief 명령어 콤보박스 combo_data1의 callback*/
static void on_combo_data1_changed(GtkComboBox *combo_id, gpointer user_data) {
    const gchar *selected_id = gtk_combo_box_get_active_id(combo_id);

    if (selected_id != NULL) {
        g_print("Selected Data: %s\n", selected_id);
        char* endptr;
        unsigned long int value = strtoul(selected_id, &endptr, 16);
        if (*endptr == '\0' && value <= UINT8_MAX) {
            data = (uint8_t)value;
        } else {
            g_print("Invalid input: %s\n", selected_id);
        }
    } else {
        g_print("No item selected.\n");
    }
    g_print("Converted Data: %X \n", data);
}

 /**@brief AutoSync 체크박스의 callback*/
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

 /**@brief FASTECH 프로토콜 체크박스의 callback*/
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

static void on_button_move_velocity_clicked(GtkButton *button, gpointer user_data) {
    // Get the GtkBuilder object passed as user data
    GtkBuilder *builder = GTK_BUILDER(user_data);

    // Get the entry widgets by their IDs
    GtkEntry *entry_velocity = GTK_ENTRY(gtk_builder_get_object(builder, "entry_velocity"));
    GtkComboBoxText *combo_vel_dir = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_vel_dir"));

    // Get the entered text from the entries
    const char *velocity_text = gtk_entry_get_text(entry_velocity);
    const char *vel_dir_text = gtk_combo_box_text_get_active_id(combo_vel_dir);

    // Convert the entered text to integer values
    int velocity = atoi(velocity_text);
    int vel_dir = atoi(vel_dir_text);

    // Call FAS_MoveVelocity function with the entered parameters
    FAS_MoveVelocity(0, velocity, vel_dir);
}
/************************************************************************************************************************************
 ********************************나중에 라이브러리로 뺄 FASTECH 라이브러리와 같은 기능의 함수*************************************************
 ************************************************************************************************************************************/
 
 /**@brief UDP 연결 시 사용
  * @param BYTE sb1,sb2,sb3,sb4 IPv4주소 입력 시 각 자리
  * @param int iBdID 드라이브 ID
  * @return boolean 성공시 TRUE 실패시 FALSE*/
bool FAS_Connect(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID){
    char SERVER_IP[16]; //최대 길이 가정 "xxx.xxx.xxx.xxx\0" 
    snprintf(SERVER_IP, sizeof(SERVER_IP), "%u.%u.%u.%u", sb1, sb2, sb3, sb4);
    
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

 /**@brief TCP 연결 시 사용
  * @param BYTE sb1,sb2,sb3,sb4 IPv4주소 입력 시 각 자리
  * @param int iBdID 드라이브 ID
  * @return boolean 성공시 TRUE 실패시 FALSE*/
bool FAS_ConnectTCP(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID){
    char SERVER_IP[16]; //최대 길이 가정 "xxx.xxx.xxx.xxx\0" 
    snprintf(SERVER_IP, sizeof(SERVER_IP), "%u.%u.%u.%u", sb1, sb2, sb3, sb4);
    
    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    else{
        g_print("Socket created\n");
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    else{
        g_print("Connection Success\n");
    }
}

 /**@brief 연결 해제 시 사용
  * @param int iBdID 드라이브 ID */
void FAS_Close(int iBdID){
    close(client_socket);
}

 /**@brief Servo의 상태를 ON/OFF
  * @param int iBdID 드라이브 ID 
  * @param bool bOnOff Enable/Disable
  * @return 명령이 수행된 정보*/
int FAS_ServoEnable(int iBdID, bool bOnOff){
    buffer[0] = header; buffer[1] = 0x04; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type; buffer[5] = data;
}

 /**@brief Servo를 천천히 멈추는 기능
  * @param int iBdID 드라이브 ID
  * @return 명령이 수행된 정보*/
int FAS_MoveStop(int iBdID){
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type; buffer[5] = 0x00;
}

 /**@brief 시스템의 원점을 찾는 기능?
  * @param int iBdID 드라이브 ID
  * @return 명령이 수행된 정보*/
int FAS_MoveOriginSingleAxis(int iBdID){
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type; buffer[5] = 0x00;
}

/**@brief Jog 운전 시작을 요청
  * @param int iBdID 드라이브 ID
  * @param DWORD lVelocity 이동 시 속도 값 (pps)
  * @param int iVelDir 이동할 방향 (0:-Jog, 1:+Jog)
  * @return 명령이 수행된 정보*/
int FAS_MoveVelocity(int iBdID, DWORD lVelocity, int iVelDir) {
    buffer[0] = header; buffer[1] = 0x37; buffer[2] = sync_no; buffer[3] = (lVelocity >> 24) & 0xFF; // lVelocity를 4바이트로 나누어 저장
    buffer[4] = (lVelocity >> 16) & 0xFF; buffer[5] = (lVelocity >> 8) & 0xFF; buffer[6] = lVelocity & 0xFF; buffer[7] = iVelDir;
}
/************************************************************************************************************************************
 ******************************************************* 편의상 만든 함수 **************************************************************
 ************************************************************************************************************************************/
 
 /**@brief 명령전달에 쓰는 버퍼 내용을 터미널에 일단 보여주는 함수*/
 void print_buffer(uint8_t *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", array[i]);
    }
    printf("\n");
}

 /**@brief 함수들을 찾아가게하는 인터페이스 용도 함수*/
void library_interface(){
    switch(frame_type)
    {
        case 0x2A:
            FAS_ServoEnable(0, 0);
            break;
        case 0x31:
            FAS_MoveStop(0);
            break;
        case 0x33:
            FAS_MoveOriginSingleAxis(0);
            break;
        case 0x37:
        {
            // 사용자로부터 속도와 방향 값을 입력받기 위해 GtkEntry와 GtkComboBoxText를 사용
            GtkEntry *entry_velocity = GTK_ENTRY(gtk_builder_get_object(builder, "entry_velocity"));
            GtkComboBoxText *combo_vel_dir = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_vel_dir"));

            // Get the entered text from the entry and combo box
            const char *velocity_text = gtk_entry_get_text(entry_velocity);
            const char *vel_dir_text = gtk_combo_box_text_get_active_id(combo_vel_dir);

            // Convert the entered text to integer values
            int velocity = atoi(velocity_text);
            int vel_dir = atoi(vel_dir_text);

            // Call FAS_MoveVelocity function with the entered parameters
            FAS_MoveVelocity(0, velocity, vel_dir);
        }
        break;
    }
    size_t data_size = buffer[1] + 2;
    print_buffer(buffer, data_size);
}
