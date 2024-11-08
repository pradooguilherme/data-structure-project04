#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
    char id_aluno[4];
    char sigla_disc[4];

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

int main(){
    return 1;
}