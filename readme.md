# Projeto: Controle de LEDs e Display com Joystick no RP2040

## Enunciado
Com o objetivo de consolidar os conceitos sobre o uso de conversores analógico-digitais (ADC) no RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab, este projeto propõe a implementação de um sistema interativo que utiliza um joystick para controlar LEDs RGB e um display SSD1306.

## Objetivos
- Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.
- Utilizar PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick.
- Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
- Aplicar o protocolo de comunicação I2C na integração com o display.

## Descrição do Projeto
O joystick fornecerá valores analógicos correspondentes aos eixos X e Y, que serão utilizados para:

### Controle da Intensidade dos LEDs RGB
- **LED Azul:** O brilho será ajustado conforme o valor do eixo Y. Quando o joystick estiver na posição central (valor 2048), o LED permanecerá apagado. Movendo para cima (valores menores) ou para baixo (valores maiores), o LED aumentará seu brilho, atingindo intensidade máxima nos extremos (0 e 4095).
- **LED Vermelho:** Seguirá o mesmo princípio, mas com base no eixo X. Quando o joystick estiver na posição central (valor 2048), o LED estará apagado. Movendo para a esquerda (valores menores) ou para a direita (valores maiores), o LED aumentará de brilho.
- **Controle via PWM:** A variação da intensidade luminosa será controlada por PWM para transições suaves.

### Interação com o Display SSD1306
- O display SSD1306 exibirá um quadrado de **8x8 pixels**, inicialmente centralizado, que se moverá proporcionalmente aos valores capturados pelo joystick.
- **Mudança de borda do display:** O botão do joystick alternará entre diferentes estilos de borda a cada novo acionamento.

### Funcionalidades dos Botões
- **Botão do Joystick:**
  - Alternar o estado do LED Verde a cada acionamento.
  - Modificar a borda do display.
- **Botão A:**
  - Ativar ou desativar os LEDs PWM a cada acionamento.

## Componentes Utilizados
Os seguintes componentes, já interconectados à placa BitDogLab, foram utilizados:
- **LED RGB** conectado às GPIOs (11, 12 e 13).
- **Botão do Joystick** conectado à GPIO 22.
- **Joystick** conectado aos GPIOs 26 e 27 (ADC).
- **Botão A** conectado à GPIO 5.
- **Display SSD1306** conectado via I2C (GPIO 14 e GPIO 15).

## Requisitos do Projeto
1. **Uso de interrupções:** Todas as funcionalidades relacionadas aos botões foram implementadas utilizando rotinas de interrupção (IRQ).
2. **Debouncing:** O tratamento de bouncing dos botões foi realizado via software.
3. **Utilização do Display 128x64:** A implementação gráfica demonstra o uso do protocolo I2C e os princípios de funcionamento do display.
4. **Código organizado e comentado:** O código está estruturado e bem documentado para facilitar o entendimento.

## Como Executar
1. Clone este repositório:
   ```sh
   git clone https://github.com/seu-usuario/Projeto_BitDogLab.git
2. Compile o código e carregue na placa BitDogLab.
3. Interaja com o joystick e os botões para testar as funcionalidades.

## Demonstração

**🎥[Clique para assistir o vídeo](https://youtu.be/BKWQ9z1pGeY)**

## Considerações Finais
Este projeto foi uma excelente oportunidade para consolidar conhecimentos em:

- Programação de microcontroladores
- Manipulação de hardware
- Utilização do periférico ADC
- Comunicação via I2C