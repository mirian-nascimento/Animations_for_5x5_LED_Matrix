#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"


// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7
const uint8_t colunas[4] = {5, 6, 8, 9}; // Pinos das colunas do teclado matricial
const uint8_t linhas[4] = {1, 2, 3, 4};  // Pinos das linhas do teclado matricial
const char teclado[4][4] = 
{
    {'1', '2', '3', 'A'}, 
    {'4', '5', '6', 'B'}, 
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin) {

  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

// Função para converter a posição do matriz para uma posição do vetor.
int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x); // Linha par (esquerda para direita).
    } else {
        return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

void inicializa_teclado() {
    // Configura os pinos das colunas como saída
    for (int i = 0; i < 4; i++) {
        gpio_init(colunas[i]);
        gpio_set_dir(colunas[i], GPIO_OUT);
        gpio_put(colunas[i], 1); // Colunas inicialmente desativadas
    }

    // Configura os pinos das linhas como entrada com pull-up
    for (int i = 0; i < 4; i++) {
        gpio_init(linhas[i]);
        gpio_set_dir(linhas[i], GPIO_IN);
        gpio_pull_up(linhas[i]);
    }
}

char leitura_teclado();
void Animamath();

int main() {

  inicializa_teclado();


  // Inicializa entradas e saídas.
  stdio_init_all();

  // Inicializa matriz de LEDs NeoPixel.
  npInit(LED_PIN);
  npClear();

  // Aqui, você desenha nos LEDs.

  npWrite(); // Escreve os dados nos LEDs.

  // Não faz mais nada. Loop infinito.
  while (true) {
    char tecla = leitura_teclado();

   switch (tecla) {
  case '1':
    Animamath();
    break;

  default:
    break;
}
    sleep_ms(200);
  }
}


void Animamath(){

int matriz[5][5][3] = {
      {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{71, 255, 0}, {71, 255, 0}, {71, 255, 0}, {71, 255, 0}, {71, 255, 0}},
    {{71, 255, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {71, 255, 0}},      
    {{71, 255, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {71, 255, 0}},
    {{71, 255, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {71, 255, 0}}};

     // Desenhando Sprite contido na matriz.c
  for(int linha = 0; linha < 5; linha++){
    for(int coluna = 0; coluna < 5; coluna++){
      int posicao = getIndex(linha, coluna);
      npSetLED(posicao, matriz[coluna][linha][0], matriz[coluna][linha][1], matriz[coluna][linha][2]);
    }
  }

    npWrite();
    sleep_ms(2000);
    npClear();

    int matriz2[5][5][3] = {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {71, 255, 0}, {71, 255, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {71, 255, 0}, {71, 255, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}}};

    for(int linha = 0; linha < 5; linha++){
    for(int coluna = 0; coluna < 5; coluna++){
      int posicao = getIndex(linha, coluna);
      npSetLED(posicao, matriz2[coluna][linha][0], matriz2[coluna][linha][1], matriz2[coluna][linha][2]);
    }
  }

    
    npWrite();
    sleep_ms(2000);

    npClear();
    int matriz3[5][5][3] = {{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {71, 255, 0}, {71, 255, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {0, 0, 0}}};

    for(int linha = 0; linha < 5; linha++){
    for(int coluna = 0; coluna < 5; coluna++){
      int posicao = getIndex(linha, coluna);
      npSetLED(posicao, matriz3[coluna][linha][0], matriz3[coluna][linha][1], matriz3[coluna][linha][2]);
    }
  }

    npWrite();
    sleep_ms(2000);

  npClear();
  int matriz4[5][5][3] = {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {71, 255, 0}, {71, 255, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}}};

    for(int linha = 0; linha < 5; linha++){
    for(int coluna = 0; coluna < 5; coluna++){
      int posicao = getIndex(linha, coluna);
      npSetLED(posicao, matriz4[coluna][linha][0], matriz4[coluna][linha][1], matriz4[coluna][linha][2]);
    }
  }

    npWrite();
    sleep_ms(2000);

    npClear();
    int matriz5[5][5][3] = {
      {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {71, 255, 0}, {71, 255, 0}, {71, 255, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {71, 255, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}};

    for(int linha = 0; linha < 5; linha++){
    for(int coluna = 0; coluna < 5; coluna++){
      int posicao = getIndex(linha, coluna);
      npSetLED(posicao, matriz5[coluna][linha][0], matriz5[coluna][linha][1], matriz5[coluna][linha][2]);
    }
  }    

    npWrite();
    sleep_ms(2000);
    npClear();
    npWrite();
    sleep_ms(2000);



}


char leitura_teclado() 
{
    char tecla = 'n'; // Valor padrão para quando nenhuma tecla for pressionada

    // Desliga todas as colunas
    for (int i = 0; i < 4; i++) 
    {
        gpio_put(colunas[i], 1);
    }

    for (int coluna = 0; coluna < 4; coluna++) 
    {
        // Ativa a coluna atual
        gpio_put(colunas[coluna], 0);

        for (int linha = 0; linha < 4; linha++) 
        {
            // Verifica se a linha está ativa
            if (gpio_get(linhas[linha]) == 0) 
            {
                tecla = teclado[linha][coluna]; // Mapeia a tecla pressionada

                // Aguarda a tecla ser liberada (debounce)
                while (gpio_get(linhas[linha]) == 0) 
                {
                    sleep_ms(10);
                }
                break; // Sai do laço após detectar a tecla
            }
        }

        // Desativa a coluna atual
        gpio_put(colunas[coluna], 1);

        if (tecla != 'n') // Sai do laço de colunas se uma tecla foi detectada
        {
            break;
        }
    }

    return tecla; // Retorna a tecla pressionada
}