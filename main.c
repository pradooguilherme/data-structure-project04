#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TAM_HASH 13

typedef struct Register
{
    char id_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float freq;

} Register;

typedef struct Key
{
    char key[2][7];
    int rrn[2];
} Key;

Register *read_insert_file()
{
    FILE *arquivo = fopen("insere.bin", "r+b");

    Register *registros = (Register *)malloc(100 * sizeof(Register));

    if (registros != NULL)
    {
        fread(registros, sizeof(struct Register), 100, arquivo);

        fclose(arquivo);
        return registros;
    }

    printf("Falha na alocação de memória para o registro de inserção\n");

    fclose(arquivo);
    return NULL;
}

FILE *iniciaLogFile()
{
    FILE *log_file = fopen("log_file.bin", "r+b");

    if (log_file == NULL)
    {

        log_file = fopen("log_file.bin", "w+b");

        if (log_file != NULL)
        {
            int index = -1;

            fwrite(&index, sizeof(int), 1, log_file);
            fwrite(&index, sizeof(int), 1, log_file);

            fseek(log_file, 0, SEEK_SET);

            return log_file;
        }
        else
        {
            printf("Falha na abertura do arquivo log.\n");
            return NULL;
        }
    }

    return log_file;
}

FILE *iniciaArquivo()
{

    FILE *seuArquivo = fopen("dados.bin", "r+b");

    if (seuArquivo == NULL)
    {

        seuArquivo = fopen("dados.bin", "w+b");

        if (seuArquivo != NULL)
        {
            bool operacao_flag = false;

            fwrite(&operacao_flag, sizeof(bool), 1, seuArquivo);
            fseek(seuArquivo, 0, SEEK_SET);

            return seuArquivo;
        }
        else
        {
            printf("Falha na abertura do arquivo dados.\n");
            return NULL;
        }
    }
    return seuArquivo;
}

FILE *iniciaHash()
{

    FILE *hash_file = fopen("hash.bin", "r+b");

    if (hash_file == NULL)
    {
        hash_file = fopen("hash.bin", "w+b");

        if (hash_file != NULL)
        {

            char no_key = '#';

            for (int i = 0; i < TAM_HASH; i++)
            {
                fseek(hash_file, i * sizeof(Key), SEEK_SET);
                fwrite(&no_key, sizeof(char), 1, hash_file);
                fseek(hash_file, 6, SEEK_CUR);
                fwrite(&no_key, sizeof(char), 1, hash_file);
            }
        }
        else
        {
            printf("Falha na abertura do arquivo hash.\n");
            return NULL;
        }
    }

    return hash_file;
}

int getLastRecordInserted()
{

    FILE *log_file = iniciaLogFile();

    int lastInserted;

    fread(&lastInserted, sizeof(int), 1, log_file);
    fseek(log_file, 0, SEEK_SET);

    lastInserted++;

    fwrite(&lastInserted, sizeof(int), 1, log_file);

    fclose(log_file);

    return lastInserted - 1;
}

bool ifOperationFlag(FILE *data_file)
{
    bool operationFlag;

    fseek(data_file, 0, SEEK_SET);
    fread(&operationFlag, sizeof(bool), 1, data_file);

    return operationFlag;
}

void writeOperationFlag(FILE *data_file)
{

    bool operationFlag = true;

    fseek(data_file, 0, SEEK_SET);
    fwrite(&operationFlag, sizeof(bool), 1, data_file);

    return;
}

int calcula_tamanho(Register *registro)
{

    int tam = 0;

    tam += strlen(registro->id_aluno);
    tam += strlen(registro->sigla_disc);
    tam += strlen(registro->nome_aluno);
    tam += strlen(registro->nome_disc);
    tam += sizeof(registro->media);
    tam += sizeof(registro->freq);
    tam = tam + 5;

    return tam;
}

void insertRegister(Register *registro)
{
    FILE *data_file = iniciaArquivo();

    int lastRecord = 0;

    if (ifOperationFlag(data_file))
    {

        lastRecord = getLastRecordInserted();
    }
    else
    {
        writeOperationFlag(data_file);
    }

    int record_size;
    char delimitador = '#';

    record_size = calcula_tamanho(&registro[lastRecord]);
    fseek(data_file, 0, SEEK_END);

    fwrite(&record_size, sizeof(int), 1, data_file);
    fwrite(registro[lastRecord].id_aluno, sizeof(char), 3, data_file);
    fwrite(&delimitador, sizeof(char), 1, data_file);
    fwrite(registro[lastRecord].sigla_disc, sizeof(char), 3, data_file);
    fwrite(&delimitador, sizeof(char), 1, data_file);
    fwrite(registro[lastRecord].nome_aluno, strlen(registro[lastRecord].nome_aluno), 1, data_file);
    fwrite(&delimitador, sizeof(char), 1, data_file);
    fwrite(registro[lastRecord].nome_disc, strlen(registro[lastRecord].nome_disc), 1, data_file);
    fwrite(&delimitador, sizeof(char), 1, data_file);
    fwrite(&registro[lastRecord].media, sizeof(float), 1, data_file);
    fwrite(&delimitador, sizeof(char), 1, data_file);
    fwrite(&registro[lastRecord].freq, sizeof(float), 1, data_file);

    fclose(data_file);
}

int hash(char key[7], int overflow)
{
    int chave = atoi(key);
    return (chave + overflow) % TAM_HASH;
}

Key *getKeysOnAdress(FILE *hash, int address)
{

    Key *registro = (Key *)malloc(sizeof(Key));

    if (registro == NULL)
    {
        printf("Falha na alocação de memória.\n");
        return NULL;
    }

    fseek(hash, address, SEEK_SET);
    fread(registro, sizeof(Key), 1, hash);

    return registro;
}

int avaiblePosition(Key *registro)
{

    if (strcmp(registro->key[0], "#") == 0)
    {
        return 0;
    }

    if (strcmp(registro->key[1], "#") == 0)
    {
        return 1;
    }

    return -1;
}

int lookForPosition(FILE *hash_file, char key[7], int *address)
{
    int realAddress = hash(key, 0);
    int lastAddress = realAddress;

    Key *registro = getKeysOnAdress(hash_file, realAddress);

    int pos = avaiblePosition(registro);

    if (pos != -1)
    {
        free(registro);
        return pos;
    }

    free(registro);

    int i = 1;

    printf("Aviso: Houve colisão.\n");

    while (1)
    {
        printf("Aviso: Tentativa %d.\n", i);

        realAddress = hash(key, i);

        if (realAddress == lastAddress)
        {
            return -1;
        }

        registro = getKeysOnAdress(hash_file, realAddress);

        pos = avaiblePosition(registro);
        if (pos != -1)
        {
            free(registro);
            *address = realAddress;
            return pos;
        }

        free(registro);
        i++;
    }
}

void insereHash(FILE *hash_file, char key[7], int rrn)
{
    int address;
    int pos = lookForPosition(hash_file, key, &address);

    if (pos == -1)
    {
        printf("A hash está cheia.\n");
        return;
    }

    Key *keyOnAdress = getKeysOnAdress(hash_file, address);

    strcpy(keyOnAdress->key[pos], key);
    keyOnAdress->rrn[pos] = rrn;

    fseek(hash_file, address, hash_file);
    fwrite(keyOnAdress, sizeof(Key), 1, hash_file);

    printf("Chave %s inserida com sucesso no endereço %d.\n", key, address);

    free(keyOnAdress);
    return;
}

int main()
{
    FILE *hash = iniciaHash();

    fclose(hash);
}