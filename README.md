# Monitoramento Inteligente de Nível de Água

**Projeto Final de CBTIAAP - Aplicação de IoT na Automação de Ambientes e Processos**

### Elaborado por:

- Cesar Beda Candido da Silva - CB302704X
- Cauã Barros da Costa - CB3025179
- Eduardo Barbosa Rodrigues - CB302637X
- Vinicius Pontes Oliva - CB3025764
---

## 📝 Descrição do Projeto

Este projeto tem como objetivo desenvolver um sistema de Internet das Coisas (IoT) para monitoramento em tempo real do nível de água em um reservatório, utilizando o microcontrolador **ESP32** e a plataforma **ThingSpeak** para visualização de dados na nuvem. O sistema é capaz de ler o nível de água, calcular o volume e a porcentagem de preenchimento, e exibir essas informações localmente em um display OLED, além de sinalizar o status do nível através de LEDs. Os dados são periodicamente enviados para um canal público no ThingSpeak, permitindo o acompanhamento remoto e a análise histórica.

## 🔗 Links
- [**Projeto no Wokwi**](https://wokwi.com/projects/435576484318505985)
- [**Canal no ThingSpeak**](https://thingspeak.mathworks.com/channels/3003027)
- [**Video Demonstração**](#)

## 💡 Contexto do Problema e Relevância

No contexto de residências ou jardins, o monitoramento eficiente do nível de água em reservatórios é crucial para evitar desperdícios, planejar o reabastecimento e garantir a disponibilidade do recurso. Muitas vezes, o controle é feito manualmente, o que pode ser ineficiente e propenso a esquecimentos, resultando em surpresas desagradáveis como a falta de água ou o transbordamento.

Este sistema IoT proposto busca resolver esse problema oferecendo uma solução automatizada para:

- **Otimização do uso da água:** Permite que os usuários saibam exatamente o volume disponível, auxiliando em decisões sobre consumo e reabastecimento.
- **Prevenção de Escassez:** Alerta sobre níveis baixos, evitando interrupções no fornecimento.
- **Controle de Nível:** Informa o status do nível (baixo, ok, cheio, acima do limite ideal), permitindo ações proativas.
- **Monitoramento Remoto:** A visualização dos dados na nuvem via ThingSpeak oferece conveniência e acessibilidade de qualquer lugar.

## Ilustração
![Imagen Funcionamento sensor de nivel ultrassonico](https://github.com/user-attachments/assets/044cc1be-ce8c-4b39-b1ff-42dd208278c1)

## 💻 Softwares Utilizados

- **Simulador:** Wokwi Simulator
- **Plataforma Cloud IoT:** ThingSpeak (com biblioteca oficial `ThingSpeak.h`)
- **Ambiente de Desenvolvimento:** Arduino IDE (compatível com ESP32 no Wokwi)
- **Controle de Versão:** Git e GitHub

## 🛠️ Componentes de Hardware Utilizados

- **Microcontrolador:** ESP32
- **Sensor de Nível:** Sensor Ultrassônico HC-SR04 (simulado)
- **Display:** OLED SSD1306 128x64 I2C
- **Indicadores Visuais:** LEDs (Amarelo, Verde, Azul, Vermelho)

## 📚 Bibliotecas Utilizadas

As seguintes bibliotecas são necessárias para a compilação e funcionamento do projeto:

- `NewPing` (para o sensor ultrassônico)
- `ThingSpeak` (para comunicação com a plataforma ThingSpeak)
- `Adafruit GFX Library` (biblioteca gráfica para o OLED)
- `Adafruit SSD1306` (driver para o display OLED)

## Esquema do Circuito Completo (Wokwi)
![Imagem Circuito Completo](https://github.com/user-attachments/assets/a85b4ab2-f17f-4e72-96ed-8ac895ac5273)

## 🔌 Conexões do Circuito (Pinagem)

Para replicar o circuito utilizando o ESP32 e os componentes listados, siga as seguintes conexões (os números dos pinos são baseados no esquema do Wokwi e podem variar ligeiramente em outras placas ESP32, mas os rótulos SDA/SCL/Trigger/Echo/GPIO são universais):

* **Sensor Ultrassônico HC-SR04:**
    * `VCC` ao `5V` do ESP32 (ou `VIN`).
    * `GND` ao `GND` do ESP32.
    * `Trig` ao pino `GPIO 23` do ESP32.
    * `Echo` ao pino `GPIO 19` do ESP32.

* **Display OLED SSD1306 (I2C):**
    * `VCC` ao `3.3V` (ou `3V3`) do ESP32.
    * `GND` ao `GND` do ESP32.
    * `SCL` (Clock) ao pino `GPIO 22` do ESP32.
    * `SDA` (Data) ao pino `GPIO 21` do ESP32.

* **LEDs Indicadores de Nível:**
    * Cada LED deve ser conectado a um pino digital do ESP32 através de um resistor (tipicamente 220 Ohm ou 330 Ohm para proteger o LED).
    * `LED Amarelo` ao pino `GPIO 13` do ESP32.
    * `LED Verde` ao pino `GPIO 12` do ESP32.
    * `LED Azul` ao pino `GPIO 14` do ESP32.
    * `LED Vermelho` ao pino `GPIO 27` do ESP32.
    *(Lembre-se de conectar o outro lado de cada LED ao GND através do resistor)*

## 🚀 Como o Sistema Funciona

O coração do sistema é o microcontrolador ESP32, que realiza as seguintes ações:

1.  **Leitura do Nível:** Um sensor ultrassônico HC-SR04 é posicionado na parte superior do reservatório e mede a distância da superfície da água até o sensor.
2.  **Cálculo de Volume e Porcentagem:** Com base na distância lida e nas dimensões pré-definidas do tanque (altura e raio), o ESP32 calcula o volume atual de água em litros e a porcentagem de preenchimento do reservatório.
    - **Observação:** O sistema foi configurado para considerar um "limite ideal" de 1000 litros, e indica quando o volume excede esse limite, mesmo que haja capacidade física adicional no tanque.
3.  **Exibição Local (OLED):** O display OLED exibe em tempo real a distância lida, o nível de água em centímetros e porcentagem, o volume atual em litros e um status textual (Ex: "Nível Baixo", "Nível OK", "Acima do Limite").
4.  **Sinalização por LEDs:** Quatro LEDs coloridos fornecem uma indicação visual rápida do status do nível de água:
    - **Amarelo:** Nível Baixo (0% a 35%)
    - **Verde:** Nível OK (>35% a 70%)
    - **Azul:** Cheio (>70% a 100%)
    - **Vermelho:** Acima do Limite (>100%) ou Indefinido (sensor fora de alcance) - (Nota: Para "Indefinido", todos os LEDs ficam apagados)
5.  **Envio de Dados para a Nuvem:** Periodicamente (a cada 15 segundos), os dados de volume e porcentagem são enviados para um canal ThingSpeak.
6.  **Visualização Remota:** Os dados enviados são visualizados em tempo real através de gráficos interativos no canal ThingSpeak, acessível publicamente pela web.

## ⚙️ Detalhes da Implementação e Personalização
 Esta seção explora as principais configurações e variáveis do código que podem ser ajustadas para adaptar o projeto a diferentes cenários e reservatórios.

### 2.1. Configuração do Tanque
 
 As definições iniciais no código (`sketch.ino`) estabelecem as características do reservatório de água. Embora o projeto seja configurado por padrão para um **tanque cilíndrico de 1000 litros**, essas variáveis podem ser facilmente alteradas:
 
 ```cpp
 #define TANK_HEIGHT_CM       76.0 // Altura física do tanque até o limite de 1000L
 #define TANK_DIAMETER_CM     129.44
 #define TANK_RADIUS_CM       (TANK_DIAMETER_CM / 2.0)
 ```
 * **`TANK_HEIGHT_CM`**: Define a altura **física** do tanque que corresponde ao volume total desejado (neste caso, 1000L).
 * **`TANK_DIAMETER_CM` / `TANK_RADIUS_CM`**: Definem o diâmetro e o raio do tanque. Para tanques cilíndricos, o volume é calculado usando a fórmula $V = \pi \times r^2 \times h$.
 * **Alterando para Outros Formatos**: Para um reservatório retangular (ex: uma caixa d'água), seria necessário modificar a lógica de cálculo do volume no código, utilizando as dimensões de comprimento, largura e altura apropriadas ($V = comprimento \\times largura \\times altura$). As variáveis `TANK_HEIGHT_CM`, `TANK_DIAMETER_CM` e `TANK_RADIUS_CM` precisariam ser adaptadas ou substituídas por `TANK_LENGTH_CM`, `TANK_WIDTH_CM` e `TANK_HEIGHT_CM`.
 
### 2.2. Offset do Sensor
 
 A variável `SENSOR_MOUNT_OFFSET_CM` é crucial para calibrar corretamente a leitura do nível de água:
 
 ```cpp
 #define SENSOR_MOUNT_OFFSET_CM 10.0 // Se o sensor lê 10cm, significa que o tanque tem 1000L
 ```
 * Este valor representa a distância do sensor ultrassônico até a **superfície da água quando o tanque está com o volume máximo definido (1000 litros)**.
 * **Não** é necessariamente a distância da borda do tanque ou do topo físico. Por exemplo, se o sensor está a 10cm acima do ponto onde o tanque atinge 1000L, então, quando o sensor lê 10cm, o tanque está em sua capacidade máxima.
 * **Ajuste**: Esta medida deve ser ajustada precisamente de acordo com a instalação física do seu sensor e o ponto que você considera como 100% de volume.
 
### 2.3. Integração com ThingSpeak
 
 O projeto se integra à plataforma ThingSpeak para visualização remota dos dados. As credenciais e o intervalo de envio podem ser configurados:
 
 ```cpp
 unsigned long myChannelNumber = SEU_CHANNEL_ID;     // >>> SUBSTITUA PELO SEU CHANNEL ID <<<
 const char* myWriteAPIKey = "SUA_WRITE_API_KEY"; // >>> SUBSTITUA PELA SUA WRITE API KEY <<<
 #define PING_INTERVAL 15000 // 15 segundos entre as leituras
 ```
 * **`myChannelNumber` (Channel ID)**: O ID único do seu canal no ThingSpeak, obtido nas configurações do canal.
 * **`myWriteAPIKey` (Write API Key)**: A chave de API que permite ao ESP32 enviar dados para o seu canal ThingSpeak. **É crucial manter esta chave confidencial e nunca a expor em locais públicos** (fora do código do seu microcontrolador).
 * **`PING_INTERVAL`**: Define o intervalo de tempo (em milissegundos) entre cada envio de dados para o ThingSpeak. O ThingSpeak na versão gratuita geralmente impõe um **intervalo mínimo de 15 segundos (15000 ms)** entre as escritas para o mesmo canal.
 * **Ajuste**: O `PING_INTERVAL` pode ser ajustado para um valor maior que 15000 ms se você precisar de atualizações menos frequentes ou para economizar energia."

## ⚙️ Instruções para Replicar o Projeto

Para testar ou modificar este projeto:

1.  **Acesse o Projeto no Wokwi:**

    - Clique no link do projeto Wokwi abaixo para abrir a simulação diretamente no seu navegador.
    - **Link Wokwi:** `[https://www.wokwi.com/projects/SEU_LINK_DO_WOKWI]` (https://wokwi.com/projects/435576484318505985)

2.  **Configuração do ThingSpeak:**

    - Crie uma conta gratuita em [https://thingspeak.com/](https://thingspeak.com/).
    - Crie um novo canal com dois "Fields": `Field 1` (para Volume) e `Field 2` (para Porcentagem).
    - Na aba `API Keys` do seu canal, copie sua `Write API Key` e o `Channel ID`.
    - No código [sketch.ino](./sketch.ino) do projeto Wokwi, substitua `SEU_CHANNEL_ID` e `SUA_WRITE_API_KEY` pelas suas credenciais.
    - **Link ThingSpeak (Canal Público):** `[https://thingspeak.com/channels/SEU_CHANNEL_ID_DO_THINGSPEAK]` (https://thingspeak.mathworks.com/channels/3003027)

3.  **Configuração dos Gráficos no ThingSpeak:**

    - Na aba `Private View` ou `Public View` do seu canal ThingSpeak, configure os gráficos para Field 1 e Field 2 (conforme as instruções fornecidas anteriormente sobre títulos, eixos e limites de valores).

4.  **Executar a Simulação:**
    - No Wokwi, clique no botão "Start Simulation". O ESP32 irá se conectar ao Wi-Fi simulado, ler o sensor e enviar os dados para o ThingSpeak.

# Simulações

## - Simulação (Nível de Água Baixo)
![Imagem Simulação Nível de água baixo.](https://github.com/user-attachments/assets/a3bf3788-8443-44fc-889d-1cca898a4ebe)

## - Simulação (Nível de Água Médio)
![Imagem Simulação Nível de água médio.](https://github.com/user-attachments/assets/567b978b-2a5e-49fa-a8ec-a299c7acac79)

## - Simulação (Nível Água Alto)
![Imagem Simulação Nível de água alto.](https://github.com/user-attachments/assets/26d1cba4-32d9-4d72-ad98-5c8e252c5f35)

## - Simulação (Nível Água Excessivo)
![Imagem Simulação Nível de água excessivo.](https://github.com/user-attachments/assets/7b0e248e-11a4-4a01-b65a-d63740765688)

## - Simulação (Nível Água Indefinido)
![Imagem Simulação Nível Indefinido.](https://github.com/user-attachments/assets/04e45df7-a0b5-477e-9007-851b7b6114ab)

## 📈 Gráficos do ThingSpeak

Aqui estão exemplos dos gráficos gerados na plataforma ThingSpeak, demonstrando a visualização em tempo real do volume e porcentagem de água no reservatório.

![Imagem Gráficos ThingSpeak.](https://github.com/user-attachments/assets/b0dde27d-3eaa-4f63-b139-22198d46eb44)

*Nota: A imagem acima é ilustrativa dos tipos de gráficos gerados e pode não corresponder diretamente aos dados das simulações apresentadas anteriormente.*

# 🎬 Vídeos Demonstrativos (Youtube)

- [**Execução Completa do Projeto (Simulação):**](https://SEU_LINK_DO_VIDEO_AQUI) Demonstração do funcionamento do projeto no Wokwi e a integração com a plataforma ThingSpeak.
- [**Vídeo Complementar (Projeto Real):**](https://SEU_LINK_DO_VIDEO_COMPLEMENTAR_AQUI) Exemplo da aplicação e funcionamento em um projeto real utilizando o sensor HC-SR04.
