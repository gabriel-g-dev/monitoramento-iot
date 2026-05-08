-- ============================================================
--  Banco de Dados – Monitoramento IoT (FIAP)
--  Node-RED → MySQL
-- ============================================================

CREATE DATABASE IF NOT EXISTS iot_db
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE iot_db;

-- Tabela principal de leituras dos sensores
CREATE TABLE IF NOT EXISTS leituras (
    id         INT AUTO_INCREMENT PRIMARY KEY,
    sensor     VARCHAR(50)    NOT NULL,        -- ex: 'temperatura', 'umidade', 'distancia'
    valor      DECIMAL(10, 2) NOT NULL,        -- valor lido
    unidade    VARCHAR(10)    NOT NULL,        -- ex: 'C', '%', 'cm'
    criado_em  DATETIME       DEFAULT NOW()   -- timestamp automático
);

-- Índice para consultas por sensor e data
CREATE INDEX idx_sensor      ON leituras (sensor);
CREATE INDEX idx_criado_em   ON leituras (criado_em);

-- ──────────────────────────────────────────────────────────────
--  Exemplos de consultas úteis
-- ──────────────────────────────────────────────────────────────

-- Últimas 20 leituras de temperatura
-- SELECT * FROM leituras WHERE sensor = 'temperatura' ORDER BY criado_em DESC LIMIT 20;

-- Média por sensor na última hora
-- SELECT sensor, AVG(valor) AS media, MAX(valor) AS maximo, MIN(valor) AS minimo
-- FROM leituras
-- WHERE criado_em >= NOW() - INTERVAL 1 HOUR
-- GROUP BY sensor;

-- Total de registros por sensor
-- SELECT sensor, COUNT(*) AS total FROM leituras GROUP BY sensor;
