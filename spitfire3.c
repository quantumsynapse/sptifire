#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>
#include <time.h>

// Calcula a potência com base nos parâmetros fornecidos
double calcularPotencia(int rpm, int altitude, double temperatura) {
    return rpm * 0.8 - altitude * 0.05 + temperatura * 1.2;
}

// Calcula o consumo de combustível com base na potência e RPM
double calcularConsumoCombustivel(int rpm, double potencia) {
    return potencia / rpm * 0.05;
}

// Insere dados no banco de dados e no arquivo CSV
int inserirDados(sqlite3 *db, FILE *csvFile, int rpm, int altitude, double temperatura, double potencia, double consumo) {
    char *errMsg = 0;
    char sql[256];
    sprintf(sql, "INSERT INTO Desempenho (RPM, Altitude, Temperatura, Potencia, Consumo) VALUES (%d, %d, %lf, %lf, %lf);", rpm, altitude, temperatura, potencia, consumo);

    if (sqlite3_exec(db, sql, NULL, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro SQL: %s\n", errMsg);
        sqlite3_free(errMsg);
        return 0;
    }

    fprintf(csvFile, "%d,%d,%.2f,%.2f,%.2f\n", rpm, altitude, temperatura, potencia, consumo);
    return 1;
}

int main() {
    sqlite3 *db;
    FILE *csvFile;
    char *errMsg = 0;
    const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS Desempenho (ID INTEGER PRIMARY KEY AUTOINCREMENT, RPM INT, Altitude INT, Temperatura REAL, Potencia REAL, Consumo REAL);";
    const char *csvFileName = "MotorDesempenho.csv";

    // Abre o banco de dados
    if (sqlite3_open("MotorDesempenho.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Erro ao abrir o banco de dados: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Cria a tabela se ela não existir
    if (sqlite3_exec(db, sqlCreateTable, NULL, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Erro SQL: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return 1;
    }

    // Abre o arquivo CSV
    csvFile = fopen(csvFileName, "w");
    if (csvFile == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo CSV.\n");
        sqlite3_close(db);
        return 1;
    }

    fprintf(csvFile, "RPM,Altitude,Temperatura,Potencia,Consumo\n");

    srand(time(NULL));
    printf("Simulação de Desempenho do Motor Rolls-Royce Merlin\n");

    int duracao;
    printf("Digite a duração da simulação (número de dados a serem gerados): ");
    scanf("%d", &duracao);

    for (int i = 0; i < duracao; i++) {
        int rpm = rand() % 3000 + 1000;
        int altitude = rand() % 10000;
        double temperatura = (rand() % 401 / 10.0) - 20;

        double potencia = calcularPotencia(rpm, altitude, temperatura);
        double consumo = calcularConsumoCombustivel(rpm, potencia);

        printf("Dados da Simulação %d: RPM: %d, Altitude: %d, Temperatura: %.1f, Potência: %.2f, Consumo: %.2f\n",
               i + 1, rpm, altitude, temperatura, potencia, consumo);

        if (!inserirDados(db, csvFile, rpm, altitude, temperatura, potencia, consumo)) {
            fprintf(stderr, "Erro ao inserir dados no banco de dados ou no arquivo CSV.\n");
            break;
        }

        sleep(1);
    }

    fclose(csvFile);
    sqlite3_close(db);
    printf("Simulação Encerrada, dados gravados no banco de dados e no arquivo CSV.\n");
    return 0;
}
