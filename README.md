# Monitoramento Inteligente com IoT
**ESP32 + MQTT + Node-RED + MySQL + OpenWeather**

---

## Descrição do Projeto

Sistema de monitoramento em tempo real de temperatura, umidade e distância utilizando um ESP32 com sensores DHT11 e HC-SR04. Os dados são transmitidos via protocolo MQTT para o broker público HiveMQ, processados pelo Node-RED, exibidos em um dashboard interativo, armazenados em banco de dados MySQL e enriquecidos com dados climáticos externos via API OpenWeather.

---

## Arquitetura da Solução

```
[DHT11]  ─┐
           ├──► [ESP32] ──► MQTT (HiveMQ) ──► [Node-RED] ──► Dashboard (ui)
[HC-SR04] ─┘                                       │
                                                    ├──► MySQL (leituras)
                                         [OpenWeather API] ──► Dashboard (ui)
```

### Fluxo de Dados

1. **Coleta**: ESP32 lê os sensores a cada 3 segundos
2. **Transmissão**: Publica os valores via MQTT nos tópicos `fiap/esp32/*`
3. **Processamento**: Node-RED recebe, valida e processa os dados
4. **Visualização**: Dashboard exibe gauges e gráficos em tempo real
5. **Persistência**: Cada leitura é salva na tabela `leituras` no MySQL
6. **API Externa**: A cada 10 minutos, busca o clima atual via OpenWeather

---

## Tópicos MQTT

| Tópico                     | Sensor   | Unidade |
|---------------------------|----------|---------|
| `fiap/esp32/temperatura`  | DHT11    | °C      |
| `fiap/esp32/umidade`      | DHT11    | %       |
| `fiap/esp32/distancia`    | HC-SR04  | cm      |

---

## Esquema de Ligação

### HC-SR04 (Ultrassônico)

| Pino Sensor | GPIO ESP32 |
|-------------|------------|
| VCC         | VIN / 5V   |
| GND         | GND        |
| TRIG        | GPIO 12    |
| ECHO        | GPIO 13    |

### DHT11 (Temperatura e Umidade)

| Pino Sensor | GPIO ESP32 |
|-------------|------------|
| VCC         | 3.3V       |
| GND         | GND        |
| DATA        | GPIO 4     |

---

## Tecnologias Utilizadas

| Camada         | Tecnologia                          |
|---------------|-------------------------------------|
| Hardware       | ESP32, DHT11, HC-SR04               |
| Firmware       | Arduino (C++)                       |
| Protocolo IoT  | MQTT v3.1.1                         |
| Broker         | HiveMQ (broker.hivemq.com:1883)    |
| Processamento  | Node-RED                            |
| Dashboard      | node-red-dashboard                  |
| Banco de Dados | MySQL 8.x + node-red-node-mysql     |
| API Externa    | OpenWeather API (weather v2.5)      |

---

## Instruções de Execução

### 1. Pré-requisitos

- Arduino IDE com suporte ao ESP32
- Node.js 18+ e Node-RED instalados
- MySQL Server rodando localmente
- Conta gratuita na [OpenWeather](https://openweathermap.org/api)

### 2. Configurar o ESP32

1. Abra `esp32_monitoramento.ino` no Arduino IDE
2. Instale as bibliotecas via Library Manager:
   - `PubSubClient` (Nick O'Leary)
   - `DHT sensor library` (Adafruit)
   - `Adafruit Unified Sensor` (Adafruit)
3. Edite as credenciais no início do arquivo:
   ```cpp
   const char* ssid     = "SEU_WIFI";
   const char* password = "SUA_SENHA";
   ```
4. Faça o upload para o ESP32

### 3. Configurar o Banco de Dados

```bash
mysql -u root -p < database.sql
```

Ou execute o conteúdo do arquivo `database.sql` no MySQL Workbench / phpMyAdmin.

### 4. Configurar o Node-RED

**Instalar dependências:**
```bash
cd ~/.node-red
npm install node-red-dashboard node-red-node-mysql
```

**Importar o fluxo:**
1. Abra o Node-RED (`http://localhost:1880`)
2. Menu (≡) → Import → Cole o conteúdo de `flows.json` → Import
3. Configure o nó `MySQLdatabase` com seu usuário/senha do MySQL
4. No nó `Montar URL OpenWeather`, substitua `SUA_API_KEY_AQUI` pela sua chave da API
5. Clique em **Deploy**

### 5. Acessar o Dashboard

Abra no navegador:
```
http://localhost:1880/ui
```

---

## Estrutura do Repositório

```
/
├── esp32_monitoramento.ino   # Firmware do ESP32
├── flows.json                # Fluxo Node-RED (importar)
├── database.sql              # Script do banco de dados
└── README.md                 # Este arquivo
```

---

## Critérios Atendidos

- [x] ESP32 com 2 sensores físicos (DHT11 + HC-SR04)
- [x] Leitura periódica (a cada 3 segundos)
- [x] Protocolo MQTT com tópicos organizados
- [x] Broker HiveMQ público
- [x] Node-RED recebendo dados do MQTT
- [x] Processamento com filtro/validação e alertas por temperatura
- [x] Dashboard em tempo real (gauges + gráficos de linha)
- [x] Armazenamento em banco de dados MySQL
- [x] Integração com API externa (OpenWeather)
- [x] Dados externos exibidos no dashboard
