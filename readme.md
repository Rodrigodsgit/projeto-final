<table align="center">
  <tr>
    <td><img src="image.png" width="100"></td>
    <td align="center">
      <b>Embarca Tech - CEPEDI</b><br>
      Capacitação Inicial em Sistemas Embarcados<br>
      U4 - Microcontroladores<br>
      <b>Discente - Rodrigo Damasceno Sampaio (Grupo 9)</b>
    </td>
    <td><img src="image2.png" width="100"></td>
  </tr>
</table>

## 🌟 Projeto: Ar Condicionado Inteligente Simulado
Este projeto integra a utilização de um display OLED SSD1306, LEDs, um joystick e um buzzer na placa BitDogLab (RP2040) para simular o controle de temperatura ambiente. O sistema permite:

- Definir via comunicação serial uma temperatura de referência.
- Simular variações de temperatura utilizando o joystick (eixo Y).
- Acionar LEDs para indicar se a temperatura está dentro ou fora da faixa ideal:
  - LED verde: temperatura dentro da faixa ideal (±0.5º em relação à referência).
  - LED vermelho: temperatura acima da referência.
  - LED azul: temperatura abaixo da referência.
- Emitir um alerta sonoro (via buzzer com PWM) quando a temperatura se desvia mais de 25º da referência.
- Exibir em tempo real a temperatura de referência e a temperatura atual no display OLED, juntamente com a mensagem "ALERTA!" quando necessário.

## 🎯 Objetivos

✅ Compreender o funcionamento do ADC e a leitura de valores analógicos no RP2040.  
✅ Utilizar PWM para controle de intensidade de som no buzzer (buzzer passivo).  
✅ Integrar um display OLED SSD1306 via I2C para exibição de informações dinâmicas.  
✅ Desenvolver uma lógica de controle para simular variações de temperatura e acionar alertas visuais e sonoros.

## 📚 Descrição do Projeto
O projeto utiliza a placa BitDogLab (RP2040) para integrar múltiplos componentes:

**Temperatura Simulada e Controle:**  
- O usuário define a temperatura de referência via comunicação serial.  
- O movimento do joystick (eixo Y) simula variações na temperatura.  
- A temperatura atual é ajustada conforme a entrada do joystick e, quando neutro, retorna gradualmente à referência.  
- Se a variação for menor que 0.2º, o sistema "fixa" a temperatura na referência para estabilidade.

**LEDs Indicadores:**  
- **LED Verde:** Acende quando a temperatura está dentro de ±0.5º da referência.  
- **LED Vermelho:** Acende quando a temperatura está acima da referência.  
- **LED Azul:** Acende quando a temperatura está abaixo da referência.

**Buzzer:**  
- Utilizado para emitir um alerta sonoro através de PWM.  
- Se a temperatura se desvia mais de 25º da referência (acima ou abaixo), o buzzer é acionado até que o valor volte para a faixa segura.

**Display OLED (SSD1306):**  
- Exibe a temperatura de referência e a temperatura atual atualizadas em tempo real.  
- Mostra a mensagem "ALERTA!" quando a temperatura estiver fora da faixa segura.

| 🛠 Componentes e Conexões              | GPIO        |
|----------------------------------------|-------------|
| 🔴 LED Vermelho                        | 13          |
| 🔵 LED Azul                            | 12          |
| 🟢 LED Verde                           | 11          |
| 🎛 Joystick (Eixo Y - ADC)             | 27          |
| 🔊 Buzzer (PWM)                        | 10          |
| 🖥 Display OLED (SDA)                   | 14          |
| 🖥 Display OLED (SCL)                   | 15          |

## 🚀 Como Rodar o Projeto

### 📌 **1️⃣ Clonar o Repositório**
```bash
git clone https://github.com/Rodrigodsgit/projeto-final.git
cd semaforo
```

### 📌 **2️⃣ Configurar o Ambiente**
Instale a extensão da Raspberry Pi Pico Project no VsCode  

### 📌 3️⃣ Compilar o Código
Através da extensão, use o acesso rapido e clique em "Compile Project"

### 📌 4️⃣ Rode o Código
Através da extensão, use o acesso rapido e clique em "Run Project (USB)"

## 🚀 Video Demonstração


[🎥 Assista ao vídeo](https://youtu.be/3eyLokqe-nU)
