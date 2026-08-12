# ESP32_IDE_MS-DOS_COM

<img width="1024" height="1024" alt="Gemini_Generated_Image_y7w2yuy7w2yuy7w2" src="https://github.com/user-attachments/assets/1796cfe5-b9d3-4e6c-a08c-59285922b2b2" />


PROJETO: DOS ESP32 IDE

<img width="109" height="125" alt="Captura de tela de 2026-08-12 15-19-27" src="https://github.com/user-attachments/assets/7838005a-e9c1-403d-b005-c2a7f8133001" />

--------------------------------------------------
Objetivo:
Criar uma IDE nativa para o MS-DOS que permite escrever codigos,
abrir um monitor serial e enviar os comandos pela porta COM (USB/Serial)
para um ESP32 rodando MicroPython ou NodeMCU.

Arquitetura:
1. INTERFACE: Usaremos a biblioteca Turbo Vision do Borland C++ 3.1 
   para desenhar janelas, menus, caixas de texto e suportar mouse.
2. COMUNICACAO: O acesso as portas (0x3F8 = COM1, 0x2F8 = COM2)
   sera feito diretamente nos registradores via inportb/outportb,
   aproveitando a base que ja existia no arquivo 'dos_ser.cpp'.
3. LINGUAGEM DO ESP32: O codigo enviado pelo DOS sera texto puro
   contendo instrucoes em Python (MicroPython). O ESP32 recebe o
   texto na porta serial e executa na mesma hora! Nao e necessario
   compilar no MS-DOS.

Arquivos Previstos:
- IDE.CPP    : O programa principal usando Turbo Vision.
- SERIAL.H   : Cabecalho com as funcoes de inicializacao da porta COM.
- SERIAL.CPP : Implementacao do envio e recebimento de bytes/strings.

Proximos Passos:
- Compilar o IDE.CPP no Borland C++ 3.1.
- Juntar a janela de editor de texto (TEditor).
- Fazer a leitura da porta serial rodar sem travar a interface (idle loop).
