//Inicialização das bibliotecas e diretivas

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include <string.h>
#include <time.h>

// Definição de macros e variáveis globais
#define buzzerA 21
#define buzzerB 10
#define LED_AZUL 12
#define LED_VERMELHO 13
#define LED_VERDE 11
#define BTN_JOYSTICK 22
#define BTN_A 5
#define BTN_B 6
#define VRY 26
#define VRX 26
#define WRAP 2000
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
struct render_area *frame_area_ptr; //Variável global para conseguir manipular o display oled em qualquer função
bool alarme_ativo = false;
static volatile int lastTimeJoy = 0;//Variável para debouncing do botão do joystick
static volatile int lastTimeA = 0; //Variável para debouncing do botão A
typedef enum {
    MODO_QUADRADO,
    MODO_TIMER
} modo_t;

static uint64_t last_tick_us = 0;
volatile modo_t modo_atual = MODO_QUADRADO;
static volatile uint tempo_em_segundos = 0;
volatile bool timer_regressivo_ativo = false;


//Função de inicialização dos periféricos usados no projeto
struct render_area init(){
    //Inicializando os buzzers para controlar via PWM
    gpio_set_function(buzzerA, GPIO_FUNC_PWM); // Configura o buzzerA para PWM
    gpio_set_function(buzzerB, GPIO_FUNC_PWM); // Configura o buzzerA para PWM
    uint slice_buzzer = pwm_gpio_to_slice_num(buzzerA);
    pwm_set_clkdiv(slice_buzzer, 4.0); // Divisor de clock razoável para áudio
    pwm_set_wrap(slice_buzzer, 5000);  // Valor inicial de wrap (frequência base)
    pwm_set_gpio_level(buzzerA, 0);    // Inicialmente desligado
    pwm_set_gpio_level(buzzerB, 0);    // Inicialmente desligado
    pwm_set_enabled(slice_buzzer, true);

    //inicialização básica dos GPIOS
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(BTN_JOYSTICK);
    gpio_set_dir(BTN_JOYSTICK, GPIO_IN);
    gpio_pull_up(BTN_JOYSTICK);
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);
    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);
    // Inicializa o conversor analógico-digital (ADC)
    adc_init();
    // Configura os pinos GPIO 26 e 27 como entradas de ADC (alta impedância, sem resistores pull-up)
    adc_gpio_init(VRY);
    adc_gpio_init(VRX);

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000); 
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    return frame_area;
}

//Função responsável por imprimir no display oled o quadrado 8x8
void draw_joystick_square(uint x_raw, uint y_raw) {

    //Cálculo de posição do quadrado em relação ao display
    uint x_pos = (x_raw * (128 - 8)) / 39;
    uint y_pos = 56 - ((y_raw * (64 - 8)) / 39);
    // Garantir que o valor fique dentro do intervalo válido
    if(x_pos >= 120) x_pos = 118;
    if(y_pos > 56) y_pos = 56;

    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    ssd1306_send_buffer(ssd, ssd1306_buffer_length);

    restart:
    ssd1306_draw_square(ssd, x_pos, y_pos,8); //Desenha um quadrado de 8x8 pixels
    render_on_display(ssd, frame_area_ptr); //Renderiza tudo no display
    
}

//Função responsável por imprimir no display oled o timer
void draw_timer_display(uint segundos) {
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    uint minutos = segundos / 60;
    uint segs = segundos % 60;
    char tempo_str[6];
    sprintf(tempo_str, "%02u:%02u", minutos, segs);

    ssd1306_draw_text(ssd, 32, 2, tempo_str); // Desenha o texto no meio da tela
    render_on_display(ssd, frame_area_ptr);
}

//rotina de interrupção dos botões
void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t currentTime = to_us_since_boot(get_absolute_time());
    //tratamento debouncing
    if(gpio == BTN_JOYSTICK && currentTime - lastTimeJoy > 200000){
        lastTimeJoy = currentTime;
        if(!timer_regressivo_ativo) modo_atual = (modo_atual == MODO_QUADRADO) ? MODO_TIMER : MODO_QUADRADO;
        if(modo_atual == MODO_TIMER) draw_timer_display(tempo_em_segundos);
    }else if(gpio == BTN_A && currentTime - lastTimeA > 200000){
        lastTimeA = currentTime;
        if (!timer_regressivo_ativo && tempo_em_segundos > 0){
            timer_regressivo_ativo = true;
            ledRgb(0, 1, 0);
        } 
        last_tick_us = to_us_since_boot(get_absolute_time());
    }
}

//Função responsável por emitir o alarme nos buzzers
void alarme_thread() {
    uint slice_buzzer = pwm_gpio_to_slice_num(buzzerA);
    while (true) {
        if (alarme_ativo) {
            // Alarme tocando
            pwm_set_wrap(slice_buzzer, 5000);
            pwm_set_gpio_level(buzzerA, 2500);
            pwm_set_gpio_level(buzzerB, 2500);
            sleep_ms(150);
            pwm_set_wrap(slice_buzzer, 3000);
            pwm_set_gpio_level(buzzerA, 1500);
            pwm_set_gpio_level(buzzerB, 1500);
            sleep_ms(150);
        } else {
            // Alarme desligado
            pwm_set_gpio_level(buzzerA, 0);
            pwm_set_gpio_level(buzzerB, 0);
            sleep_ms(50); // Espera um pouco para não ocupar 100% da CPU
        }
    }
}

//Rotina de contagem regressiva do alarme
void contagem_regressiva(){
    if (timer_regressivo_ativo) {
        
        uint64_t now = to_us_since_boot(get_absolute_time());
    
        if (now - last_tick_us >= 1000000) { // 1 segundo
            last_tick_us = now;
    
            if (tempo_em_segundos > 0) {
                tempo_em_segundos--;
                draw_timer_display(tempo_em_segundos);
            } else {
                timer_regressivo_ativo = false;
                alarme_ativo = true;
                ledRgb(1, 0 ,0);
            }
        }
    }
    
}

//Função para definir estado dos leds rgb
void ledRgb(int r, int g, int b){
    gpio_put(LED_VERMELHO, r);
    gpio_put(LED_VERDE, g);
    gpio_put(LED_AZUL, b);
}

int main() {
    // Inicializa as interfaces de entrada e saída padrão (UART)
    stdio_init_all();
    //Inicializa os periféricos e tem como retorno o frame area
    struct render_area frame_area = init();
    frame_area_ptr = &frame_area;
    multicore_launch_core1(alarme_thread); //Execução de tarefas multicore para tocar a melodia do alarme sem interromper o programa principal	
    //Ativa as interrupções com callback para os botões A e Joystick
    gpio_set_irq_enabled_with_callback(BTN_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    //Armazenam as posições anteriores dos potenciometros para detectar se houve mudança
    uint xAnt, yAnt;
    ledRgb(0, 0, 1);
    // Inicia o loop infinito para leitura e exibição dos valores do joystick
    while (1) {
        adc_select_input(1); // eixo X
        uint adc_x_raw = adc_read();
        
        adc_select_input(0); // eixo Y
        uint adc_y_raw = adc_read();
    
        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1; 
        uint bar_x_pos = adc_x_raw * bar_width / adc_max;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max;
    
        if (modo_atual == MODO_QUADRADO) {
            if(bar_x_pos != xAnt || bar_y_pos != yAnt) draw_joystick_square(bar_x_pos, bar_y_pos);
        } else if (modo_atual == MODO_TIMER && !timer_regressivo_ativo) {
            // Se o joystick for movido para cima ou para baixo
            if (bar_y_pos > 20 && bar_y_pos < 30 && tempo_em_segundos < 5999) { // Limite: 99 minutos 59 segundos
                tempo_em_segundos += 1;
                draw_timer_display(tempo_em_segundos);
            } else if (bar_y_pos < 16  && bar_y_pos > 8 && tempo_em_segundos > 0) {
                tempo_em_segundos -= 1;
                draw_timer_display(tempo_em_segundos);
            } else if (bar_y_pos > 30 && tempo_em_segundos < 5999) {
                tempo_em_segundos += 2;
                draw_timer_display(tempo_em_segundos);
            }else if (bar_y_pos < 8 && tempo_em_segundos > 0) {
                tempo_em_segundos -= 2;
                draw_timer_display(tempo_em_segundos);
            }
            sleep_ms(140);
        }
    
        xAnt = bar_x_pos;
        yAnt = bar_y_pos;
        printf("%d\n", bar_y_pos);
        contagem_regressiva();
        sleep_ms(10);
    }
    
}