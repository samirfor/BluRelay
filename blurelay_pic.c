/**
 * Ferramenta SanUSB: http://sanusb.site50.net/
 * Compilar com MPLABX+C18: https://drive.google.com/open?id=0B5332OAhnMe2N3czQWxVX0JVSkE&authuser=0
 * Apk BluRelay para Android: https://github.com/samirfor/BluRelay/blob/master/BluRelay.apk?raw=true
 * Vídeo: https://www.youtube.com/watch?v=X2ADGWf2mXo
 * Artigo: https://www.dropbox.com/s/3vuy43qf84ef089/Interpola%C3%A7%C3%A3o%20para%20sinaliza%C3%A7%C3%A3o%20ou%20chaveamento%20de%20cargas%20el%C3%A9tricas%20remotamente%20via%20Bluetooth%20e%20localmente%20via%20potenci%C3%B4metro.pdf?dl=0
 *
 * Este programa recebe comandos do app BluRelay via bluetooth ou via potenciômetro.
 *
 */

#include "SanUSB48.h"

#pragma interrupt interrupcao

unsigned char c, bluetooth = 0;
const char quebra_linha[] = "\r\n";
unsigned long int valor, Vresult, Vanterior = 1, i, n;

void interrupcao() {

    if (serial_interrompeu) {
        serial_interrompeu = 0;
        bluetooth = 1;
        c = le_serial() + 0x30;
    }
}

void main() {
    clock_int_48MHz();
    habilita_interrupcao(recep_serial);
    taxa_serial(19200); // depende do modem bluetooth
    habilita_canal_AD(AN0); // Habilita entradas analógicas para conversão AD.
    c = 2 + 0x30; // valor padrão de intervalo incial

    while (1) {
        if (!entrada_pin_e3) {
            Reset(); // botão reset
        }

        /**
         * Recebe os dados do potenciômetro
         */
        n = le_AD10bits(0); // Lê A0 (entrada analógica com resolução de 10 bits (ADRES))
        Vresult = ((n * 8) / 1023) + 1;

        // imprime na serial (capturar pelo celular)
        sendnum(Vresult);
        sendsw((char *) quebra_linha);

        if (bluetooth) {
            valor = (c - 0x30); // Recebe valor da serial enviada pelo app BluRelay
        }
        if (Vresult != Vanterior) {
            valor = Vresult;
            Vanterior = Vresult;
            bluetooth = 0;
        }


        /**
         * Ação realizada (altera o intervalo de interpolação graduado em 100)
         */
        inverte_saida(pin_b7);
        tempo_ms(valor * 100);

    }
}

