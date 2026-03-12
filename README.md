# Controle de Acesso com ESP32

Projeto acadêmico desenvolvido no curso de **Análise e Desenvolvimento de Sistemas**, na disciplina de **Sistemas Embarcados**.

O sistema implementa um **controle de acesso embarcado baseado em senha**, utilizando ESP32 e diversos periféricos para interação com o usuário.

---

# Visão Geral do Sistema

O sistema permite autenticação por senha através de um teclado matricial.  
Dependendo da validação da senha, o sistema controla um servo motor simulando a abertura de uma porta.

O usuário recebe feedback através de:

- Display LCD
- LED RGB
- Buzzer

---

# Arquitetura do Sistema

Keypad
↓
ESP32 (Controle de acesso)
↓
├── LCD I2C (mensagens)
├── RGB LED (status)
├── Buzzer (alerta sonoro)
└── Servo Motor (abertura)


---

# Funcionamento do Sistema

Fluxo do sistema:

Usuário digita senha
↓
ESP32 valida senha
↓
Senha correta → acesso liberado
Senha incorreta → tentativa registrada
↓
3 tentativas erradas → sistema bloqueado temporariamente


---

# Interface do Usuário

| Dispositivo | Função |
|-------------|--------|
LCD | exibe mensagens do sistema |
RGB LED | indica estado do sistema |
Buzzer | alerta sonoro |
Servo | simula abertura da porta |

---

# Componentes Utilizados

Hardware utilizado no projeto:

- ESP32
- Teclado Matricial 4x4
- Display LCD 16x2 (I2C)
- Servo Motor
- LED RGB
- Buzzer
- Resistores

---

# Simulação do Circuito

Imagem da simulação no Wokwi:

![Circuito](hardware/circuito.png)

---

# Estrutura do Projeto

Controle-Acesso-ESP32
│
├── docs
│ └── documentação do projeto
│
├── firmware
│ └── esp32
│ └── controle_acesso.ino
│
├── simulation
│ └── wokwi
│ ├── diagram.json
│ ├── sketch.ino
│ ├── libraries.txt
│ └── wokwi-project.txt
│
├── hardware
│ └── circuito.png
│
├── README.md
└── .gitignore


---

# Tecnologias Utilizadas

- ESP32
- Linguagem C/C++ (Arduino Framework)
- Wokwi Simulator
- Git
- GitHub

---

# MVP1 – Sistema Embarcado Local

Primeira versão do projeto implementa o sistema funcionando localmente com:

- autenticação por senha
- controle de acesso com servo
- interface LCD
- feedback visual (RGB)
- feedback sonoro (buzzer)
- bloqueio após múltiplas tentativas

Todo o processamento ocorre diretamente no microcontrolador.

---

# MVP2 – Integração IoT

Na segunda etapa o sistema será expandido para:

- conexão **WiFi**
- envio de eventos para **TagoIO**
- registro de acessos na nuvem
- dashboard de monitoramento

Fluxo planejado:

ESP32 → WiFi → TagoIO → Dashboard


Eventos monitorados:

- acesso liberado
- acesso negado
- sistema bloqueado

---

# Simulação

O circuito pode ser executado utilizando o simulador **Wokwi**, através dos arquivos presentes na pasta:

simulation/wokwi

🔗 [Abrir simulação no Wokwi](https://wokwi.com/projects/458231570955241473)

---

# Autor

Projeto desenvolvido por **iTech, inspirational Tech**  
Prof: **Claudio Pereira**
Curso: **Análise e Desenvolvimento de Sistemas**  
Disciplina: **Sistemas Embarcados**  
Universidade: **Faculdade Nova Roma**