# 🎮 Meteor Dodger

## 📖 Sobre o Projeto

Este projeto implementa um jogo simples onde o jogador controla uma nave (LED azul) através de botões, tentando desviar de meteoros (LEDs vermelhos) que caem. O jogo utiliza uma matriz de LED para exibição da nave e meteoros, um display OLED para mostrar um quadrado controlado por joystick, LEDs RGB para indicar o status de vida do jogador e um buzzer que emite som quando há colisão com meteoros.

## 🚀 Funcionalidades

- 🎮 **Controle da nave** através dos botões A e B
- 🕹️ **Joystick** para controlar um quadrado no display OLED
- 💫 **Sistema de meteoros** caindo aleatoriamente
- ❤️ **Sistema de vidas** com feedback visual através de LED RGB
- 🔔 **Feedback sonoro** através de buzzer
- ⏱️ **Contagem de tempo** de sobrevivência

## 🛠 Tecnologias Utilizadas

- **Microcontrolador**: Raspberry Pi Pico W
- **Display**: OLED SSD1306 (I2C)
- **Componentes**:
  - Matriz de LED WS2812B 5x5
  - LED RGB
  - Joystick analógico
  - 2 Botões
  - Buzzer
- **Protocolos de Comunicação**:
  - I2C (display OLED)
  - PIO (matriz de LED)
- **Linguagem de Programação**:
  - C (utilizando o SDK do Raspberry Pi Pico)

## 🔧 Estrutura do Projeto

```
📂 tarefa1_revisao_embarcatech
│── 📂 lib            # Bibliotecas auxiliares
│   ├── ssd1306.c    # Controle do display OLED
│   ├── ws2812b.c    # Controle da matriz de LED
│   └── rectangle.c  # Lógica do quadrado controlado
│── main.c           # Código principal do jogo
│── CMakeLists.txt   # Configuração do build
└── README.md        # Documentação
```

## 🏗 Instalação e Configuração

### 📥 Pré-requisitos

- Raspberry Pi Pico W
- Display OLED SSD1306
- Matriz de LED WS2812B 5x5
- LED RGB, 2 Botões, Joystick e Buzzer
- Cabo micro USB
- **Software**:
  - Visual Studio Code + Extensão Raspberry Pi Pico
  - CMake
  - SDK do Raspberry Pi Pico

### ⚙️ Passos para Configuração

1. **Clone este repositório**:

```bash
git clone https://github.com/matheusssilva991/tarefa1_revisao_embarcatech
```

2. **Compile o projeto**:

```bash
mkdir build && cd build
cmake -G Ninja ..
ninja
```

3. **Transfira o arquivo UF2** gerado para o Raspberry Pi Pico.

## 🎮 Como Jogar

- Use os **Botões A e B** para mover a nave (LED azul) horizontalmente
- Desvie dos meteoros (LEDs vermelhos) que caem
- Use o **Joystick** para mover o quadrado no display OLED
- O LED RGB indica suas vidas restantes:
  - Verde: 3 vidas
  - Azul: 2 vidas
  - Vermelho piscando: 1 vida
  - Vermelho fixo: Game Over

## Demonstração

A seguir, um vídeo demonstrando o funcionamento do projeto:

[![Vídeo de demonstração](https://drive.google.com/file/d/1-qZ8TtoOaYcHsxvbr2bGhT3PL7CxGoyy/view?usp=sharing)](https://drive.google.com/file/d/1-qZ8TtoOaYcHsxvbr2bGhT3PL7CxGoyy/view?usp=sharing)

## 📜 Licença

Este projeto é distribuído sob a licença MIT. Consulte o arquivo `LICENSE` para mais informações.

## 🤝 Contribuição

Sinta-se à vontade para contribuir! Caso tenha sugestões ou melhorias, abra uma issue ou faça um pull request.

---
✉️ Para dúvidas ou sugestões, entre em contato! 🚀

## 🤝 Equipe

Membros da equipe de desenvolvimento do projeto:
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/matheusssilva991">
        <img src="https://github.com/matheusssilva991.png" width="100px;" alt="Foto de Matheus Santos Silva no GitHub"/><br>
        <b>Matheus Santos Silva (matheusssilva991)</b>
        <p>Desenvolvedor Back-end - NestJS</p>
      </a>
    </td>
  <tr>
</table>
