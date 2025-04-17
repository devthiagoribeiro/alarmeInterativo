# Alarme Interativo

Projeto desenvolvido com o objetivo de revisar e consolidar os conhecimentos adquiridos sobre o microcontrolador RP2040 e os principais recursos disponíveis na placa de desenvolvimento BitDogLab.

## 📝 Descrição

O **Alarme Interativo** é um sistema embarcado que combina diversos periféricos para criar uma aplicação funcional e interativa. O objetivo é permitir a configuração de um temporizador, que ao expirar, ativa um alarme com alerta sonoro e visual, sendo possível desativá-lo mediante a repetição de uma sequência de comandos exibida ao usuário.

O projeto visa consolidar conhecimentos sobre:
- Leitura analógica com joystick (ADC)
- Controle de botões físicos com debounce
- Exibição gráfica no display OLED SSD1306 (I2C)
- Controle de matriz de LEDs via PIO
- Sinais sonoros com buzzers (PWM)
- Interrupções e comunicação UART

## 🚀 Funcionalidades

- Modo **Quadrado**: movimenta um quadrado 8x8 no display OLED conforme a movimentação do joystick.
- Modo **Timer**: permite configurar um tempo (MM:SS) usando joystick ou via UART.
- Alarme sonoro e visual ao término do tempo.
- Geração e exibição de uma sequência aleatória de comandos com feedback na matriz de LEDs.
- Desativação do alarme somente com repetição correta da sequência.
- Controle de estados com LEDs RGB para feedback.
- Uso de interrupções para alternância de modos e controle de ações.

## 🔧 Como utilizar o projeto

### ✅ Requisitos

- Raspberry Pi Pico W
- Placa BitDogLab (ou equivalente com os periféricos)
- VS Code com extensão **Pico C SDK** configurada
- Drivers para comunicação serial com a Pico (UART)

### 📦 Instalação e Compilação

1. Clone o repositório:
   ```bash
   git clone https://github.com/devthiagoribeiro/alarmeInterativo
   cd alarme-interativo

2. Compile o projeto com o CMake (assumindo que o SDK esteja corretamente configurado):
   ```bash
    mkdir build
    cd build
    cmake ..
    make

3. Faça o upload do binário .uf2 gerado para a Pico.

    ### ▶️ Execução
    
    - Acompanhe os logs e mensagens pelo terminal serial (UART).

    - Use o botão do joystick para alternar entre modos.

    - No modo Timer, configure o tempo com o joystick ou pressione o botão B para entrada via UART.

    - Pressione o botão A para iniciar a contagem regressiva.

    - Após o alarme, repita a sequência exibida na matriz de LEDs para desativá-lo.

# 🎥 Vídeo Demonstrativo (link)[🔗 Inserir link aqui para o vídeo no YouTube ou Google Drive]