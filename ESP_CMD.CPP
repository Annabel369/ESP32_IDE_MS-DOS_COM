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

int is_transmit_empty() { 
    return inportb(COM1 + 5) & 0x20; 
}

void write_serial(char a) {
    while (is_transmit_empty() == 0);
    outportb(COM1, a);
}

void write_string(const char* str) {
    while(*str) {
        write_serial(*str);
        str++;
    }
    // Envia o ENTER (\r) no final para o ESP32 entender que o comando acabou
    write_serial('\r'); 
}

int main(int argc, char *argv[]) {
    printf("CH341 DOS COM1 Sender (16-bits)\n");
    printf("-------------------------------\n");

    if (argc < 2) {
        printf("Uso: ESP_CMD.EXE <comando>\n");
        printf("Exemplo: ESP_CMD.EXE \"TELA 1\"\n");
        return 1;
    }

    // Inicia o hardware da porta serial COM1
    init_serial();

    // Pega o comando passado por parametro
    char *comando = argv[1];
    
    printf("Enviando para o ESP32 (COM1): %s\n", comando);
    write_string(comando);
    
    printf("Comando enviado com sucesso!\n");

    return 0;
}
