# 💣 Arduino Bomb Defusal Game (KTANE Inspired)

Este projeto é um dispositivo físico inspirado no jogo *Keep Talking and Nobody Explodes*, desenvolvido com Arduino.

O objetivo é simples: desarmar todos os módulos antes que o tempo acabe.

---

## 🎮 Como funciona

O dispositivo possui **3 módulos independentes**, cada um com sua própria lógica:

- 🔌 Fios
- 🎛️ Chaves giratórias (potenciômetros)
- 🔀 Interruptores (switches)

Ao pressionar o botão verde, o sistema é iniciado e o tempo começa a correr.

Cada erro reduz o tempo restante, aumentando a pressão sobre os jogadores.

---

## 🧠 Mecânica do jogo

Os jogadores devem interpretar informações exibidas em um display LCD e seguir regras presentes em um manual externo.

O jogo exige:
- Comunicação
- Lógica
- Atenção aos detalhes
- Trabalho em equipe

---

## 🧩 Módulos

### 🔌 Módulo 1 — Fios
- 5 fios com cores diferentes
- Devem ser cortados na ordem correta
- A ordem é baseada em regras e condições do manual

---

### 🎛️ Módulo 2 — Chaves giratórias
- 3 controles:
  - Intensidade do azul
  - Intensidade do vermelho
  - Velocidade do LED
- O jogador deve ajustar corretamente os valores e confirmar

---

### 🔀 Módulo 3 — Interruptores
- 4 switches
- Devem ser colocados em sequências específicas
- Não há penalidade, apenas validação contínua

---

## ⚙️ Tecnologias utilizadas

- Arduino Uno
- LCD I2C
- Fita NeoPixel (WS2812)
- Potenciômetros
- Switches
- Botões
- LEDs PWM

---

## 💡 Funcionalidades

- Sistema de estados (FSM)
- Timer com penalidades
- Feedback visual com LEDs
- Interface via display LCD
- Lógica modular inspirada em puzzles reais

---

## 🛠️ Estrutura do código

O código foi organizado em:

- Controle de estados do jogo
- Leitura de entradas (botões, fios, switches)
- Validação de módulos
- Controle de LEDs e efeitos visuais
- Sistema de tempo

---

## 📸 Demonstração

fotos

---

## 🚀 Possíveis melhorias

- Adicionar buzzer para efeitos sonoros
- Mais módulos (teclado, memória, código Morse, etc.)
- Interface web ou app complementar
- Sistema de dificuldades

---

## 👨‍💻 Autor

Projeto desenvolvido por **Fernando Milani Venerando**

---

## 📜 Licença

Uso educacional e pessoal.
