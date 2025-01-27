#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

//arquivo .pio
#include "pio_matrix.pio.h"

//número de LEDs
#define NUM_PIXELS 25

// 
#define MATRIZ_PIN 10

// Pino do Buzzer
#define BUZZER_PIN 21

// Número de Frames
#define NUMFRAMES 10

// Mapeamento do teclado matricial
const uint8_t colunas[4] = {5, 4, 3, 2}; // Pinos das colunas
const uint8_t linhas[4] = {9, 8, 7, 6};  // Pinos das linhas
const char teclado[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}};

double frames_r[NUMFRAMES][25] = {
    {1.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 0.0, 0.0, 
     0.0, 1.0, 1.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 0.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 0.0},

    {1.0, 1.0, 0.0, 0.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0, 
     0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 0.0, 0.0},

    {0.0, 1.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 1.0, 0.0, 
     0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0},

    {0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 1.0, 1.0, 
     0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 1.0, 1.0, 0.0},

    {0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 1.0, 1.0},

    {0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 0.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0},

    {0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0, 
     0.0, 1.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 0.0},

    {0.0, 1.0, 0.0, 1.0, 0.0,
     1.0, 0.0, 1.0, 0.0, 1.0, 
     0.0, 1.0, 0.0, 1.0, 0.0,
     1.0, 0.0, 1.0, 0.0, 1.0,
     0.0, 1.0, 0.0, 1.0, 0.0},

    {1.0, 0.0, 1.0, 0.0, 1.0,
     0.0, 1.0, 0.0, 1.0, 0.0, 
     1.0, 0.0, 1.0, 0.0, 1.0,
     0.0, 1.0, 0.0, 1.0, 0.0,
     1.0, 0.0, 1.0, 0.0, 1.0},

    {1.0, 0.0, 1.0, 0.0, 1.0,
     0.0, 0.0, 1.0, 0.0, 0.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0,
     1.0, 0.0, 1.0, 0.0, 1.0}
};

double frames_g[NUMFRAMES][25] = {
    {1.0, 1.0, 0.0, 0.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0, 
     0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 0.0, 0.0},

    {0.0, 1.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 1.0, 0.0, 
     0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0},

    {0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 1.0, 1.0, 
     0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 1.0, 1.0, 0.0},

    {0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 1.0, 1.0},

    {0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 0.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0},

    {0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 0.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0},

    {1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0, 
     1.0, 1.0, 0.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    {0.0, 1.0, 1.0, 1.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 1.0, 1.0, 1.0, 0.0},

    {0.0, 1.0, 0.0, 1.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 1.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 1.0, 0.0, 1.0, 0.0},

    {1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0, 
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0}
};

double frames_b[NUMFRAMES][25] = {
    {1.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 0.0, 0.0, 
     0.0, 1.0, 1.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 0.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 0.0},

    {1.0, 1.0, 0.0, 0.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0, 
     0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 0.0, 0.0},

    {0.0, 1.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 1.0, 0.0, 
     0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 1.0, 1.0, 0.0, 0.0},

    {0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 1.0, 1.0, 
     0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 1.0, 1.0, 0.0},

    {0.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 1.0, 1.0},

    {0.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 0.0, 
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0},

    {0.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 1.0, 1.0, 
     0.0, 0.0, 1.0, 0.0, 0.0,
     1.0, 1.0, 0.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 0.0},

    {0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 1.0, 0.0, 1.0, 0.0, 
     1.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 1.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0},

    {1.0, 1.0, 1.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 0.0, 1.0, 
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 1.0, 1.0, 1.0},

    {0.0, 1.0, 0.0, 1.0, 0.0,
     0.0, 1.0, 0.0, 1.0, 0.0, 
     0.0, 1.0, 1.0, 1.0, 0.0,
     0.0, 1.0, 0.0, 1.0, 0.0,
     0.0, 1.0, 0.0, 1.0, 0.0}
};

// Declaração de funções

void animacao(PIO pio, uint sm);
void ligar_azul(PIO pio, uint sm);
void ligar_vermelho(PIO pio, uint sm);
void ligar_verde(PIO pio, uint sm);
void ligar_branco(PIO pio, uint sm);
void desligar_leds(PIO pio, uint sm);

void tocar_som(uint32_t frequencia, uint32_t duracao);

void inicializarTeclado();
char verificarPinosAtivos();
void selecionarFuncao(char caractere, PIO pio, uint sm);

// Definição da intensidade de cores do led
uint32_t matriz_rgb(double r, double g, double b) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função principal
int main()
{
    PIO pio = pio0; 
    bool set_clock;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;

    // Configurar o buzzer como saída
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    //Por a frequência de clock para 128 MHz.
    set_clock = set_sys_clock_khz(128000, false);

    // Inicializa todos os códigos stdio padrão que estão ligados ao binário.
    stdio_init_all();

    inicializarTeclado();

    printf("Inicilizando a transmissão PIO\n");
    if (set_clock) printf("Clock definido em %ld\n", clock_get_hz(clk_sys));

    // Configurações da PIO

    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, MATRIZ_PIN);

    while (true) {
        char c = '\0';
        c = verificarPinosAtivos();
        selecionarFuncao(c, pio, sm);
        sleep_ms(300);
    }
    
}

// Inicializar o teclado matricial
void inicializarTeclado() {

    // Inicializa o conjunto de pinos que representa as linhas
    for (int i = 0; i < 4; i++) {
        gpio_init(linhas[i]);
        gpio_set_dir(linhas[i], GPIO_OUT);
        gpio_put(linhas[i], 0);
    }
    
    // Inicializa o conjunto de pinos que representa as colunas
    for (int i = 0; i < 4; i++) {
        gpio_init(colunas[i]);
        gpio_set_dir(colunas[i], GPIO_IN);
        gpio_pull_down(colunas[i]);
    }
}

// Retorna o pino ativo no teclado
char verificarPinosAtivos() {
    for (int i = 0; i < 4; i++) {
        gpio_put(linhas[i], 1);
        sleep_us(100);
      
        for (int j = 0; j < 4; j++) {
            if (gpio_get(colunas[j])) {
                gpio_put(linhas[i], 0);
                return teclado[i][j];
            }
        }
        gpio_put(linhas[i], 0);
    }
    return '\0';
}

// Seleciona qual função será executada baseada no teclado
void selecionarFuncao(char caractere, PIO pio, uint sm) { 
    switch (caractere) {
        case '1':
            animacao(pio, sm);
            break;
        case 'A':
            desligar_leds(pio, sm);
            break;
        case 'B':
            ligar_azul(pio, sm);
            break;
        case 'C':
            ligar_vermelho(pio, sm);
            break;
        case 'D':
            ligar_verde(pio, sm);
            break;
        case '#':
            ligar_branco(pio, sm);
            break;
        case '*':
            reset_usb_boot(0,0);
            break;
        default:
            break;
    }
}

// Cria animação
void animacao(PIO pio, uint sm) {
    uint32_t tons[9] = {220, 275, 330, 385, 440, 495, 550, 605, 660};

    for (int frame = 0; frame < NUMFRAMES; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            double r = frames_r[frame][i]; // Intensidade de cada pixel vermelho no frame
            double g = frames_g[frame][i]; // Intensidade de cada pixel verde no frame
            double b = frames_b[frame][i]; // Intensidade de cada pixel azul no frame

            uint32_t color = matriz_rgb(r, g, b); // Intensidade variável para o azul e verde
            pio_sm_put_blocking(pio, sm, color);
        }

        if (frame < 9)
            tocar_som(tons[frame], 100);
        else {
            for (int t = 0; t < 9; t++)
                tocar_som(tons[t], 50);
        }
        sleep_ms(500);
    }
}

// Apaga todos os LEDS
void desligar_leds(PIO pio, uint sm) {
    for (int i = 0; i < NUM_PIXELS; i++)
        pio_sm_put_blocking(pio, sm, matriz_rgb(0, 0, 0));
}

// Liga todos os LEDS na coloração azul
void ligar_azul(PIO pio, uint sm) {
    for (int i = 0; i < NUM_PIXELS; i++)
        pio_sm_put_blocking(pio, sm, matriz_rgb(0, 0, 1));
}

// Liga todos os LEDS na coloração verde
void ligar_verde(PIO pio, uint sm) {
    for (int i = 0; i < NUM_PIXELS; i++)
        pio_sm_put_blocking(pio, sm, matriz_rgb(0, 0.8, 0));
}

// Liga todos os LEDS na coloração vermelho
void ligar_vermelho(PIO pio, uint sm) {
    for (int i = 0; i < NUM_PIXELS; i++)
        pio_sm_put_blocking(pio, sm, matriz_rgb(0.5, 0, 0));
}

// Liga todos os LEDS na coloração branco
void ligar_branco(PIO pio, uint sm) {
    for (int i = 0; i < NUM_PIXELS; i++)
        pio_sm_put_blocking(pio, sm, matriz_rgb(0.2, 0.2, 0.2));
}

// Tocar som no buzzer
void tocar_som(uint32_t frequencia, uint32_t duracao) {

    // Calcula o período do sinal (em microssegundos)
    uint32_t periodo = 1000000 / frequencia;
    uint32_t meio_periodo = periodo / 2;

    // Calcula o número total de ciclos necessários
    uint32_t cycles = (frequencia * duracao) / 1000;

    // Gera a onda quadrada alternando o estado do pino
    for (uint32_t i = 0; i < cycles; i++) {
        gpio_put(BUZZER_PIN, 1); // HIGH
        sleep_us(meio_periodo); // Aguarda metade do período
        gpio_put(BUZZER_PIN, 0); // LOW
        sleep_us(meio_periodo); // Aguarda metade do período
    }
}