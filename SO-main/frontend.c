#include"header.h"

int fd_serv, fd_cli;

void comandos(){
    printf("========== Comando ========== \n");
    printf("sell - Colocar um item a leilao\n");
    printf("list - Lista de todos os itens \n");
    printf(" licat - Lista de Itens por categoria \n");
    printf(" lisel - Lista de Itens por vendedor \n");
    printf(" lival - Lista de Itens por valor \n");
    printf(" litime - Lista de Itens por prazo \n");
    printf("time - Obter a hora atual \n");
    printf("buy - Licitar um item \n");
    printf("cash - Consultar saldo \n");
    printf("add - Carregar saldo \n");
    printf("exit - Sair \n");
}

int contaWords(char str[]){
    int i = 0, nword = 1;

    while(str[i] != '\0'){
        if(str[i] == ' '){
            nword++;
        }
        i++;
    }
    return nword;
}

void trataSig(int i){
    printf("\nA Encerrar...\n");
    exit(EXIT_FAILURE);
}

int login(user utilizador){
    int res;
    int saldo;
    fd_serv = open(PIPE_FRONT_BACK, O_WRONLY);
    int value = FLAG_NEW_USER;
    res = write(fd_serv, &value, sizeof(int));
    res = write(fd_serv, &utilizador, sizeof(utilizador));
    
    res = read(fd_cli, &saldo, sizeof(saldo));
    if(saldo != -1){
        return saldo;
    }
    return -1;
}

int main(int argc, char *argv[]){
    char comando[50], arg[50];
    user utilizador;
    int numItens = 0,numProm = 0, res, valido = 0;
    item auxItem, items[MAXITEMS];
    promocao auxProm, promocoes[MAXPROMOCOES];
    fd_set fontes;
    struct timeval t;
    
    
    if(access(PIPE_FRONT_BACK, F_OK) != 0){
        printf("[ERROR] Nao existe backend em execucao\n");
        return 0;
    }

    if(argc != 3){
        printf("Insira username e password\n");
        return 0;
    }
    strcpy(utilizador.username, argv[1]);
    strcpy(utilizador.password, argv[2]);

    sprintf(utilizador.pipe_name, PIPE_BACK_FRONT, getpid());
    if(mkfifo(utilizador.pipe_name, 0600) == -1){
        printf("[ERROR] Ao criar o pipe\n");
        return 0;
    }
    fd_cli = open(utilizador.pipe_name,O_RDWR);

    utilizador.saldo = login(utilizador);
    if(utilizador.saldo == -1){
        printf("User Recusado!\n");
        return 0;
    }
    printf("\n\n *** Bem Vindo %s ***\n\n", utilizador.username);

    if (getenv("HEARTBEAT") != NULL)
        nmaxalive = atoi(getenv("HEARTBEAT"));

    if(signal(SIGUSR1, trataSig) == SIG_ERR){
        printf("\n<ERRO> Nao foi possivel configurar o sinal SIGUSR1\n");
        return 0;
    }

    comandos();
    do{
        fflush(stdout);
        printf("Introduza um comando: ");
        fflush(stdout);

        FD_ZERO(&fontes);
        FD_SET(0, &fontes);
        FD_SET(fd_cli, &fontes);
        t.tv_sec =60;
        t.tv_usec=0;
        res = select(fd_cli + 1, &fontes, NULL, NULL, &t);

        if(res > 0 && FD_ISSET(0, &fontes)){        //VEIO DO TECLADO
            valido = 0;
            fgets(comando,50,stdin);
            comando[strlen(comando) - 1] = '\0';
            if(strncmp(comando, "sell", 4) == 0){
                if(contaWords(comando) == 6){
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    strcpy(auxItem.name, arg);
                    strcpy(arg, strtok(NULL, " "));
                    strcpy(auxItem.category, arg);
                    strcpy(arg, strtok(NULL, " "));
                    auxItem.value = atoi(arg);
                    strcpy(arg, strtok(NULL, " "));
                    auxItem.current_value = atoi(arg);
                    strcpy(arg, strtok(NULL, " "));
                    auxItem.duration = atoi(arg);
                    strcpy(auxItem.user_buyer, "");
                    strcpy(auxItem.user_sell, utilizador.username);
                    printf("\n*Item colocado a leilao\n");
                    
                    int value = FLAG_NEW_ITEM;
                    res = write(fd_serv, &value, sizeof(int));
                    res = write(fd_serv, &auxItem, sizeof(auxItem));
                }
                else{
                    printf("E necessario definir o nome, categoria, preco_base, preco_compre_ja, duracao \n"); // sell <name> <category> <value> <current_value> <duration>
                }
            }
            if(strcmp(comando, "list\0") == 0){
                if( numItens == 0)
                    printf("Nao existem itens!\n");
                for(int i = 0; i < numItens ; i++){
                    printf("Item %d - %s: (categoria)%s (preco atual)%d (preco base)%d (vendedor)%s \n", items[i].IDitem, items[i].name, items[i].category, items[i].current_value, items[i].value, items[i].user_sell);
                }
            }
            if(strncmp(comando, "licat", 5) == 0){
                if(strcmp(comando, "licat\0") != 0){
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    for(int i = 0; i < numItens ; i++){
                        if(strcmp(items[i].category, arg) == 0){
                            printf("Item %d - %s: (preco atual)%d (preco base)%d \n", items[i].IDitem, items[i].name, items[i].current_value, items[i].value);
                            valido = 1;
                        }
                    }
                    if(valido == 0)
                        printf("Nao existem itens na categoria %s", arg);
                }
                else{
                    printf("E necessario definir a categoria \n");
                }
            }
            if(strncmp(comando, "lisel", 5) == 0){
                if(strcmp(comando, "lisel\0") != 0){
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    for(int i = 0; i < numItens ; i++){
                        if(strcmp(items[i].user_sell, arg) == 0){
                            printf("Item %d - %s: (categoria)%s (preco atual)%d (preco base)%d \n", items[i].IDitem, items[i].name, items[i].category, items[i].current_value, items[i].value);
                            valido = 1;
                        }
                    }
                    if(valido == 0)
                        printf("Nao existem itens vendidos por %s", arg);
                }
                else{
                    printf("E necessario definir o vendedor \n"); 
                }
            }
            if(strncmp(comando, "lival", 5) == 0){
                if(strcmp(comando, "lival\0") != 0){
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    for(int i = 0; i < numItens ; i++){
                        if(items[i].value <= atoi(arg)){
                            printf("Item %d - %s: (categoria)%s (preco atual)%d (preco base)%d \n", items[i].IDitem, items[i].name, items[i].category, items[i].current_value, items[i].value);
                            valido = 1;
                        }
                    }
                    if(valido == 0)
                        printf("Nao existem itens ate %d€", atoi(arg));
                }
                else{
                    printf("E necessario definir o valor \n"); 
                }
            }
            if(strncmp(comando, "litime", 6) == 0){
                if(strcmp(comando, "litime\0") != 0){
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    for(int i = 0; i < numItens ; i++){
                        if(items[i].duration <= atoi(arg)){
                            printf("Item %d - %s: (categoria)%s (preco atual)%d (preco base)%d \n", items[i].IDitem, items[i].name, items[i].category, items[i].current_value, items[i].value);
                            valido = 1;
                        }
                    }
                    if(valido == 0)
                        printf("Nao existem itens ate %d segundos", atoi(arg));
                }
                else{
                    printf("E necessario definir um prazo(em segundos) \n"); // litime <hora-em-segundos>
                }
            }
            if(strcmp(comando, "time\0") == 0){
                int value = FLAG_TIME;
                write(fd_serv, &value, sizeof(int));
                write(fd_serv, &utilizador, sizeof(utilizador));
                int segundos;
                res = read(fd_cli, &segundos, sizeof(segundos));
                printf("Hora: %d \n", segundos); 
            }
            if(strncmp(comando, "buy", 3) == 0){
                if(contaWords(comando)==3){
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    int id = atoi(arg);
                    strcpy(arg, strtok(NULL, " "));
                    int valor = atoi(arg);
                    if(utilizador.saldo > valor){
                        for(int i = 0; i<numItens; i++){
                            if(items[i].IDitem == id){
                                if(items[i].current_value < valor){
                                    printf("Licitou valor para item %d \n", id);
                                    items[i].current_value = valor;
                                    strcpy(items[i].user_buyer, utilizador.username);
                                    int value = FLAG_LICITACAO;
                                    write(fd_serv, &value, sizeof(int));
                                    write(fd_serv, &items[i], sizeof(items[i]));
                                }
                                else{
                                    printf("Impossivel licitar porque o valor atual e de %d! \n", items[i].current_value);
                                }
                            }
                        }
                    }
                    else
                        printf("Impossivel licitar!\n");
                }
                else{
                    printf("E necessario definir o id e o valor \n"); // buy <IDitem> <value>
                }
            }
            if(strcmp(comando, "cash\0") == 0){
                printf("Saldo: %d \n", utilizador.saldo);
            }
            if(strncmp(comando, "add", 3) == 0){
                if(strcmp(comando, "add\0") != 0){
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    utilizador.saldo = utilizador.saldo + atoi(arg);
                    printf("\n*Saldo atual: %d€ \n", utilizador.saldo);
                    int value = FLAG_CARREGAMENTO;
                    write(fd_serv, &value, sizeof(int));
                    write(fd_serv, &utilizador, sizeof(utilizador));
                }
                else{
                    printf("E necessario definir o valor que deseja carregar\n"); // add <value>
                }
            }
            if(strcmp(comando, "exit\0") == 0){
                printf("\n*A sair...\n"); //exit
                int value = FLAG_EXIT_USER;
                res = write(fd_serv, &value, sizeof(int));
                res = write(fd_serv, &utilizador, sizeof(utilizador));
            }
        }else if(res > 0 && FD_ISSET(fd_cli, &fontes)){        //VEIO DO PIPE DO SERVIDOR
            int flag;
            res = read(fd_cli, &flag, sizeof(int));
            if(flag == FLAG_PROM){
                res = read(fd_cli, &auxProm, sizeof(auxProm));
                if(auxProm.new == true){
                    if(numProm < MAXPROMOCOES){
                        strcpy(promocoes[numProm].category, auxProm.category);
                        promocoes[numProm].descont = auxProm.descont;
                        promocoes[numProm].duration = auxProm.duration;
                        printf("\nPromocao de %d na categoria %s iniciada\n", promocoes[numProm].descont, promocoes[numProm].category);
                        numProm++;
                    }
                }else{
                    for(int i = 0; i < numProm; i++){
                        if(promocoes[i].descont == auxProm.descont && (strcmp(promocoes[i].category, auxProm.category) == 0)){
                            printf("\nPromocao de %d na categoria %s terminou\n", promocoes[i].descont, promocoes[i].category);
                            for(int j=i+1; j<numProm; j++){
                                strcpy(promocoes[j-1].category, promocoes[j].category);
                                promocoes[j-1].descont = promocoes[j].descont;
                                promocoes[j-1].duration = promocoes[j].duration;
                            }
                            numProm--;
                            break;
                        }
                    }
                }
            }else if(flag == FLAG_ITEM){
                res = read(fd_cli, &auxItem, sizeof(auxItem));
                int existe=0;
                if(auxItem.new == true){
                    for(int i=0; i<numItens; i++){
                        if(items[i].IDitem == auxItem.IDitem){
                            printf("\n %s licitou %d€ para item %d", auxItem.user_buyer, auxItem.current_value, auxItem.IDitem);
                            strcpy(items[i].user_buyer, auxItem.user_buyer);
                            items[i].current_value = auxItem.current_value;
                            existe=1;
                        }
                    }
                    if(existe == 0 && numItens < MAXITEMS){
                        strcpy(items[numItens].category, auxItem.category);
                        items[numItens].current_value = auxItem.current_value;
                        items[numItens].duration = auxItem.duration;
                        items[numItens].IDitem = auxItem.IDitem;
                        strcpy(items[numItens].name, auxItem.name);
                        strcpy(items[numItens].user_buyer, auxItem.user_buyer);
                        strcpy(items[numItens].user_sell, auxItem.user_sell);
                        items[numItens].value = auxItem.value;
                        printf("Item %d - %s: (categoria)%s (preco atual)%d (preco base)%d foi adicionado\n", items[numItens].IDitem, items[numItens].name, items[numItens].category, items[numItens].current_value, items[numItens].value);     
                        numItens++;
                    }
                }else{
                    for(int i = 0; i < numItens; i++){
                        if(items[i].IDitem == auxItem.IDitem){
                            printf("Item %d - %s: (categoria)%s (preco atual)%d foi removido", auxItem.IDitem, auxItem.name, auxItem.category, auxItem.current_value);
                            if(auxItem.duration == 0 && (strcmp(auxItem.user_buyer,"") == 0)) 
                                printf(" - nao vendido\n");  
                            else if(auxItem.duration == 0 && (strcmp(auxItem.user_buyer,"") != 0)){
                                printf(" - vendido a %s\n", auxItem.user_buyer);   
                                if(strcmp(auxItem.user_buyer, utilizador.username) == 0){
                                    utilizador.saldo = utilizador.saldo-auxItem.current_value;
                                    printf("\nSaldo atual: %d\n", utilizador.saldo);
                                }
                            }
                            for(int j=i+1; j<numItens; i++){
                                strcpy(items[j-1].category, items[j].category);
                                items[j-1].current_value = items[j].current_value;
                                items[j-1].duration = items[j].duration;
                                items[j-1].IDitem = items[j].IDitem;
                                strcpy(items[j-1].name, items[j].name);
                                strcpy(items[j-1].user_buyer, items[j].user_buyer);
                                strcpy(items[j-1].user_sell, items[j].user_sell);
                                items[j-1].value = items[j].value;
                            }
                            numItens--;
                            break;
                        }
                    }
                }
            }

        }

    }while(strcmp(comando, "exit"));
    remove(utilizador.pipe_name);
    close(fd_cli);
    unlink(utilizador.pipe_name);
    close(fd_serv);
}