#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>           // Para fabs()
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "inc/ssd1306.h"

// Configurações do joystick
#define DEFAULT_VRY 2000    // Valor médio do eixo Y
#define THRESHOLD   100     // Limiar para detectar variação

// Pinos dos LEDs (BitDogLab)
const uint LED_RED   = 13;
const uint LED_BLUE  = 12;
const uint LED_GREEN = 11;

// Pino do joystick (usaremos o eixo Y no canal ADC1, GP27)
const uint JOYSTICK_Y = 27;

// Pino do buzzer (defina conforme sua montagem; aqui usamos o GPIO 10)
const uint BUZZER_PIN = 10;

// Variáveis para controle de temperatura
float referencia_temp = 25.0;    // Temperatura de referência inicial
float temperatura_atual = 25.0;  // Temperatura simulada atual
uint16_t y_value = 0;            // Valor lido do ADC para o eixo Y

// Configurações do display OLED via I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDRESS 0x3C

// Estrutura do display
ssd1306_t ssd;

//-----------------------------------------------------------------
// Funções de Inicialização
//-----------------------------------------------------------------
void initialize_leds() {
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
}

void initialize_buzzer_pwm(uint pin) {
    // Configura o pino como PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    // Define um wrap que, junto com o divisor, determina a frequência
    // Aqui usamos wrap 1000 e divisor 125.0 para obter uma frequência aproximadamente audível
    pwm_set_wrap(slice_num, 1000);
    pwm_set_clkdiv(slice_num, 125.0f);
    // Inicialmente, desliga o som definindo duty cycle 0
    pwm_set_gpio_level(pin, 0);
    pwm_set_enabled(slice_num, true);
}

void initialize_joystick() {
    adc_init();
    adc_gpio_init(JOYSTICK_Y);  // Configura o pino do joystick para ADC
}

uint16_t ler_joystick() {
    adc_select_input(1);  // Canal 1 (GP27)
    sleep_us(2);
    return adc_read();
}

void initialize_display() {
    // Inicializa o I2C para o display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa e configura o display SSD1306
    ssd1306_initialize(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, OLED_ADDRESS, I2C_PORT);
    ssd1306_configure(&ssd);
    ssd1306_clear_display(&ssd, false);
    ssd1306_send_data_to_display(&ssd);
}

//-----------------------------------------------------------------
// Função Principal
//-----------------------------------------------------------------
int main() {
    stdio_init_all();
    // Aguarda a conexão USB para comunicação serial
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    
    initialize_leds();
    initialize_buzzer_pwm(BUZZER_PIN);
    initialize_joystick();
    initialize_display();

    // Solicita a temperatura de referência via serial
    printf("Digite a temperatura de referencia (em graus): ");
    scanf("%f", &referencia_temp);
    printf("Temperatura de referencia definida: %.2f\n", referencia_temp);

    char tempStr[20]; // Buffer para formatar strings de exibição

    while (true) {
        // Leitura do joystick
        y_value = ler_joystick();

        // Cálculo do delta a partir do movimento do joystick
        int delta = 0;
        if (y_value > (DEFAULT_VRY + THRESHOLD)) {
            // Movimento para cima: aumenta a temperatura
            delta = (y_value - DEFAULT_VRY) / 200;
        } else if (y_value < (DEFAULT_VRY - THRESHOLD)) {
            // Movimento para baixo: diminui a temperatura
            delta = -((DEFAULT_VRY - y_value) / 200);
        } else {
            delta = 0;
        }

        // Ajusta a temperatura atual conforme o delta ou gradualmente retorna à referência
        if (delta != 0) {
            temperatura_atual += delta;
        } else {
            if (temperatura_atual > referencia_temp)
                temperatura_atual -= 0.1;
            else if (temperatura_atual < referencia_temp)
                temperatura_atual += 0.1;
        }
        // Se a diferença for muito pequena, "fixa" a temperatura à referência
        if (fabs(temperatura_atual - referencia_temp) < 0.2) {
            temperatura_atual = referencia_temp;
        }

        // Controle dos LEDs com base na temperatura simulada:
        // Se a temperatura estiver aproximadamente na referência (±0.5 grau)
        if (temperatura_atual >= referencia_temp - 0.5 && temperatura_atual <= referencia_temp + 0.5) {
            gpio_put(LED_GREEN, 1);
            gpio_put(LED_RED, 0);
            gpio_put(LED_BLUE, 0);
        } else if (temperatura_atual > referencia_temp) {
            gpio_put(LED_RED, 1);
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_BLUE, 0);
        } else { // temperatura_atual < referencia_temp
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_RED, 0);
        }

        // Aciona o buzzer via PWM se a temperatura estiver fora da faixa segura
        // Faixa segura: entre (referencia - 25) e (referencia + 25)
        {
            uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
            if (temperatura_atual > referencia_temp + 25 || temperatura_atual < referencia_temp - 25) {
                // Define duty cycle de 50% (500 de 1000)
                pwm_set_gpio_level(BUZZER_PIN, 500);
            } else {
                // Desliga o buzzer (duty cycle 0)
                pwm_set_gpio_level(BUZZER_PIN, 0);
            }
        }

        // Feedback via comunicação serial
        printf("Temp Ref: %.2f | Temp Atual: %.2f | Joystick Y: %d\n",
               referencia_temp, temperatura_atual, y_value);

        // Atualiza o display OLED:
        ssd1306_clear_display(&ssd, false);
        sprintf(tempStr, "Ref: %.1f", referencia_temp);
        ssd1306_draw_string(&ssd, tempStr, 0, 0);  // Linha 0 (posição y=0)
        sprintf(tempStr, "Atual: %.1f", temperatura_atual);
        ssd1306_draw_string(&ssd, tempStr, 0, 8);  // Linha 1 (posição y=8)
        // Exibe alerta no display se estiver fora da faixa segura
        if (temperatura_atual > referencia_temp + 25 || temperatura_atual < referencia_temp - 25) {
            ssd1306_draw_string(&ssd, "ALERTA!", 0, 16);
        }
        ssd1306_send_data_to_display(&ssd);

        sleep_ms(200);  // Delay para suavizar a atualização
    }
    
    return 0;
}
