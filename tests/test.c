func int putch(int ch) {
    int ser_out = 0x101, ser_ready = 0x100;
    *ser_out = ch;
    *ser_ready = 0;
    return;
}
