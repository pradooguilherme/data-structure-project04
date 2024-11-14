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

typedef struct RemoveKey
{
    char id_aluno[4];
    char sigla_disc[4];
} RemoveKey;

void printKey(Key *k)
{
    if (k == NULL)
    {
        printf("Estrutura Key é nula.\n");
        return;
    }

    printf("Keys:\n");
    for (int i = 0; i < 2; i++)
    {
        printf("  key[%d]: %s\n", i, k->key[i]);
    }

    printf("RRNs:\n");
    for (int i = 0; i < 2; i++)
    {
        printf("  rrn[%d]: %d\n", i, k->rrn[i]);
    }
}

RemoveKey *read_remove_file()
{
    FILE *remove_file = fopen("remove.bin", "r+b");

    RemoveKey *removeKeys = (RemoveKey *)malloc(100 * sizeof(RemoveKey));

    if (removeKeys != NULL)
    {

        fread(removeKeys, sizeof(struct RemoveKey), 100, remove_file);

        fclose(remove_file);

        return removeKeys;
    }

    printf("Falha na alocação de memória para o registro de remoção.\n");

    fclose(remove_file);

    return NULL;
}

RemoveKey *read_busca_file()
{
    FILE *remove_file = fopen("busca.bin", "r+b");

    RemoveKey *removeKeys = (RemoveKey *)malloc(100 * sizeof(RemoveKey));

    if (removeKeys != NULL)
    {

        fread(removeKeys, sizeof(struct RemoveKey), 100, remove_file);

        fclose(remove_file);

        return removeKeys;
    }

    printf("Falha na alocação de memória para o registro de busca.\n");

    fclose(remove_file);

    return NULL;
}

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

    lastInserted++;

    fseek(log_file, 0, SEEK_SET);
    fwrite(&lastInserted, sizeof(int), 1, log_file);

    fclose(log_file);

    return lastInserted;
}

int getLastRecordSearched()
{
    FILE *log_file = iniciaLogFile();

    int lastSearched;

    fseek(log_file, 8, SEEK_SET);
    fread(&lastSearched, sizeof(int), 1, log_file);

    lastSearched++;

    fseek(log_file, 8, SEEK_SET);
    fwrite(&lastSearched, sizeof(int), 1, log_file);

    fclose(log_file);

    return lastSearched;
}

int getLastRecordRemoved()
{

    FILE *log_file = iniciaLogFile();

    int lastRemoved;

    fseek(log_file, 4, SEEK_SET);
    fread(&lastRemoved, sizeof(int), 1, log_file);

    lastRemoved++;

    fseek(log_file, 4, SEEK_SET);
    fwrite(&lastRemoved, sizeof(int), 1, log_file);

    fclose(log_file);

    return lastRemoved;
}

int getAddressofInsertion()
{

    FILE *data_file = iniciaArquivo();

    fseek(data_file, 0, SEEK_END);

    int addressOfInsertion = ftell(data_file);

    fclose(data_file);

    return addressOfInsertion;
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

void insertRegister(Register *registro, int lastRecord)
{
    FILE *data_file = iniciaArquivo();

    if (!ifOperationFlag(data_file))
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

    fseek(hash, address * sizeof(Key), SEEK_SET);
    fread(registro, sizeof(Key), 1, hash);

    return registro;
}

int avaiblePosition(Key *registro)
{

    if (registro->key[0][0] == '#')
    {
        return 0;
    }

    if (registro->key[1][0] == '#')
    {
        return 1;
    }

    return -1;
}

int lookForPosition(FILE *hash_file, char key[7], int *address)
{
    int realAddress = hash(key, 0);
    int lastAddress = realAddress;

    printf("Aviso: Endereço %d.\n", realAddress);

    Key *registro = getKeysOnAdress(hash_file, realAddress);

    int pos = avaiblePosition(registro);

    if (pos != -1)
    {
        free(registro);
        *address = realAddress;
        return pos;
    }

    free(registro);

    int i = 1;

    printf("Aviso: Houve colisão.\n");

    printf("Aviso: Tentativa %d.\n", i);

    while (1)
    {
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

        printf("Aviso: Houve colisão.\n");

        free(registro);
        i++;

        printf("Aviso: Tentativa %d.\n", i);
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

    fseek(hash_file, address * sizeof(Key), SEEK_SET);
    fwrite(keyOnAdress, sizeof(Key), 1, hash_file);

    printf("Chave %s inserida com sucesso no endereço %d.\n", key, address);

    free(keyOnAdress);
    return;
}

void removeHash(FILE *hash_file, char key[7])
{
    int realAddress = hash(key, 0);
    int lastAddress = realAddress;

    Key *registro = getKeysOnAdress(hash_file, realAddress);

    int pos = 0;
    int overflow = 1;

    while (strcmp(registro->key[pos], key) != 0)
    {

        if (pos == 1)
        {
            pos = 0;
            realAddress = hash(key, overflow);

            if (realAddress == lastAddress)
            {
                printf("Aviso: Chave %s não encontrada.\n", key);
                free(registro);
                return;
            }

            registro = getKeysOnAdress(hash_file, realAddress);
            overflow++;
        }
        else
        {
            pos++;
        }
    }

    char removido = '#';

    printf("Chave %s encontrada e removida com sucesso.\n", key);

    strncpy(registro->key[pos], &removido, 1);

    fseek(hash_file, realAddress * sizeof(Key), SEEK_SET);
    fwrite(registro, sizeof(Key), 1, hash_file);

    free(registro);
}

void print_rrn_data(int byteofset)
{
    FILE *data_file = iniciaArquivo();

    fseek(data_file, byteofset, SEEK_SET);

    int tamanho_registro = 0;
    char delimitador = '#';

    fread(&tamanho_registro, sizeof(int), 1, data_file);

    char buffer[tamanho_registro + 1];
    fread(buffer, sizeof(char), tamanho_registro - 9, data_file);

    Register registro;

    char *token = strtok(buffer, &delimitador);
    if (token != NULL)
        strncpy(registro.id_aluno, token, sizeof(registro.id_aluno) - 1);
    registro.id_aluno[3] = '\0';

    token = strtok(NULL, &delimitador);
    if (token != NULL)
        strncpy(registro.sigla_disc, token, sizeof(registro.sigla_disc) - 1);
    registro.sigla_disc[3] = '\0';

    token = strtok(NULL, &delimitador);
    if (token != NULL)
        strncpy(registro.nome_aluno, token, sizeof(registro.nome_aluno) - 1);

    token = strtok(NULL, &delimitador);
    if (token != NULL)
        strncpy(registro.nome_disc, token, sizeof(registro.nome_disc) - 1);

    fread(&registro.media, sizeof(float), 1, data_file);
    fseek(data_file, 1, SEEK_CUR);
    fread(&registro.freq, sizeof(float), 1, data_file);

    printf("\nID Aluno: %s\n", registro.id_aluno);
    printf("Sigla Disciplina: %s\n", registro.sigla_disc);
    printf("Nome do Aluno: %s\n", registro.nome_aluno);
    printf("Nome da Disciplina: %s\n", registro.nome_disc);
    printf("Média: %.2f\n", registro.media);
    printf("Frequência: %.2f\n", registro.freq);

    fclose(data_file);
}

int buscaHash(FILE *hash_file, char key[7])
{
    int realAddress = hash(key, 0);
    int lastAddress = realAddress;

    Key *registro = getKeysOnAdress(hash_file, realAddress);

    int pos = 0;
    int overflow = 1;

    while (strcmp(registro->key[pos], key) != 0)
    {
        if (pos == 1)
        {
            pos = 0;
            realAddress = hash(key, overflow);

            if (realAddress == lastAddress)
            {
                printf("Aviso: Chave %s não encontrada.\n", key);
                free(registro);
                return -1;
            }

            registro = getKeysOnAdress(hash_file, realAddress);
            overflow++;
        }
        else
        {
            pos++;
        }
    }

    printf("Chave %s encontrada, endereço %d, %d acesso(s).\n", key, realAddress, overflow);

    return registro->rrn[pos];

    free(registro);
}

int main()
{
    Register *registros = read_insert_file();
    RemoveKey *removeKeys = read_remove_file();
    RemoveKey *buscaKeys = read_busca_file();

    int resposta;

    bool flag = true;

    while (flag)
    {
        printf("\nO que deseja fazer?\n[1]Inserção\n[2]Remoção\n[3]Busca\n[4]Remoção dos Arquivos\n[5]Sair do programa\nR: ");
        scanf("%d", &resposta);

        if (resposta == 1)
        {
            FILE *hash = iniciaHash();

            char key[7];

            int i = getLastRecordInserted();
            int byteOfSet = getAddressofInsertion();

            snprintf(key, sizeof(key), "%s%s", registros[i].id_aluno, registros[i].sigla_disc);

            int ifduplicada = buscaHash(hash, key);

            if (ifduplicada == -1)
            {
                insereHash(hash, key, byteOfSet);
                insertRegister(registros, i);
            }else{

                printf("Aviso: Chave duplicada.\n");
            }

            fclose(hash);
        }

        if (resposta == 2)
        {
            FILE *hash_file = iniciaHash();

            char key[7];
            int lastRemoved = getLastRecordRemoved();

            snprintf(key, sizeof(key), "%s%s", removeKeys[lastRemoved].id_aluno, removeKeys[lastRemoved].sigla_disc);

            removeHash(hash_file, key);

            fclose(hash_file);
        }

        if (resposta == 3)
        {
            FILE *hash_file = iniciaHash();

            char key[7];
            int lastSearched = getLastRecordSearched();

            snprintf(key, sizeof(key), "%s%s", buscaKeys[lastSearched].id_aluno, buscaKeys[lastSearched].sigla_disc);

            int resultado = buscaHash(hash_file, key);

            if (resultado != -1)
            {
                print_rrn_data(resultado);
            }

            fclose(hash_file);
        }

        if (resposta == 4)
        {
            remove("dados.bin");
            remove("log_file.bin");
            remove("hash.bin");
        }

        if (resposta == 5)
        {
            flag = false;
        }
    }
}