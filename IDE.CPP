#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#define COM1 0x3F8

void init_serial() {
    outportb(COM1 + 1, 0x00);
    outportb(COM1 + 3, 0x80);
    outportb(COM1 + 0, 0x01); // 115200 baud
    outportb(COM1 + 1, 0x00);
    outportb(COM1 + 3, 0x03); // 8N1
    outportb(COM1 + 2, 0xC7); // FIFO
    outportb(COM1 + 4, 0x0B); // DTR/RTS
}

int serial_received() { return inportb(COM1 + 5) & 1; }
char read_serial() { return inportb(COM1); }
int is_transmit_empty() { return inportb(COM1 + 5) & 0x20; }
void write_serial(char a) {
    while (is_transmit_empty() == 0);
    outportb(COM1, a);
}

// Controle do monitor
int term_x = 1, term_y = 1;
char rx_buf[100];
int rx_pos = 0;

FILE *log_file = NULL;

void scroll_monitor() {
    window(1, 15, 80, 24);
    movetext(1, 16, 80, 24, 1, 15);
    gotoxy(1, 10);
    clreol();
    window(1, 1, 80, 25);
}

void print_char_to_monitor(char c) {
    window(1, 15, 80, 24);
    textcolor(LIGHTGREEN);
    textbackground(BLACK);

    if (c == '\r') {
        term_x = 1;
    } else if (c == '\n') {
        term_x = 1;
        term_y++;
        if (term_y > 10) { scroll_monitor(); term_y = 10; }
    } else {
        gotoxy(term_x, term_y);
        putch(c);
        term_x++;
        if (term_x > 80) {
            term_x = 1;
            term_y++;
            if (term_y > 10) { scroll_monitor(); term_y = 10; }
        }
    }
    window(1, 1, 80, 25);
}

void print_string_to_monitor(char *s) {
    while(*s) {
        print_char_to_monitor(*s);
        s++;
    }
}

void process_magic_command() {
    if (strcmp(rx_buf, "*CMD:BEEP*") == 0) {
        sound(880);
        delay(300);
        nosound();
        print_string_to_monitor("\n[MAGICA] O ESP32 FEZ O PC APITAR!\n");
    } 
    else if (strcmp(rx_buf, "*CMD:DRAW*") == 0) {
        print_string_to_monitor("\n[MAGICA] ESP32 MANDOU UM DESENHO:\n");
        print_string_to_monitor("  _____________________ \n");
        print_string_to_monitor(" |  ESP32 TELA TOUCH   |\n");
        print_string_to_monitor(" | [=================] |\n");
        print_string_to_monitor(" |_____________________|\n");
    }
}

void draw_interface() {
    textbackground(BLUE);
    textcolor(WHITE);
    clrscr();

    textbackground(LIGHTGRAY);
    textcolor(BLACK);
    gotoxy(1, 1);
    cprintf(" MS-DOS ESP32 IDE Bidirecional | F9-Enviar | ESC-Sair                        ");
    
    window(1, 3, 80, 13);
    textbackground(BLUE);
    textcolor(WHITE);
    clrscr();
    cprintf("=== TESTES ===\n");
    cprintf("Digite 'TELA 1', 'TELA 2' ou 'TELA 3' para mudar o display.\n");
    cprintf("Va na Tela 2 do ESP32 e clique nos botoes magicos para ver o resultado!\n");
    
    window(1, 14, 80, 14);
    textbackground(DARKGRAY);
    textcolor(WHITE);
    clrscr();
    cprintf(" MONITOR SERIAL (COM1 115200) ");

    window(1, 15, 80, 24);
    textbackground(BLACK);
    clrscr();

    window(1, 1, 80, 25);
    gotoxy(1, 25);
    textbackground(LIGHTGRAY);
    textcolor(BLACK);
    cprintf(" Status: Aguardando COM1...                                                   ");
}

int main() {
    draw_interface();
    init_serial();

    log_file = fopen("SERIAL.LOG", "a");
    if (log_file) {
        fprintf(log_file, "\n--- NOVA SESSAO IDE (115200) ---\n");
        fflush(log_file);
    }

    int running = 1;
    while(running) {
        // Recebendo dados
        if (serial_received()) {
            char in_char = read_serial();
            print_char_to_monitor(in_char);
            
            // Grava no arquivo de log em tempo real para a IA (ou debug) ver
            if (log_file) {
                fputc(in_char, log_file);
                if (in_char == '\n') fflush(log_file);
            }
            
            if (in_char == '\r' || in_char == '\n') {
                rx_buf[rx_pos] = '\0';
                if (rx_pos > 0) process_magic_command();
                rx_pos = 0;
            } else {
                if (rx_pos < 99) {
                    rx_buf[rx_pos] = in_char;
                    rx_pos++;
                }
            }
        }

        // Enviando dados
        if (kbhit()) {
            char c = getch();
            if (c == 27) { // ESC
                running = 0;
            } else if (c == 13) { // ENTER
                write_serial('\r');
                print_char_to_monitor('\n');
            } else {
                write_serial(c);
                print_char_to_monitor(c);
            }
        }
    }

    if (log_file) {
        fprintf(log_file, "\n--- SESSAO ENCERRADA ---\n");
        fclose(log_file);
    }

    window(1, 1, 80, 25);
    textbackground(BLACK);
    textcolor(LIGHTGRAY);
    clrscr();
    return 0;
}
