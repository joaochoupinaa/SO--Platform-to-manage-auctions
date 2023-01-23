#include"header.h"
#include"users_lib.h"
#include <pthread.h>

backend info;


void comandos(){
    printf("\n========== Comando ========== \n");
    printf("help - Lista de Comandos \n");
    printf("users - Lista de Utilizadores \n");
    printf("list - Lista de Itens a Venda \n");
    printf("kick - Banir um cliente \n");
    printf("prom - Lista de Promotores \n");
    printf("reprom - Atualizar promotores \n");
    printf("cancel - Cancelar um Promotor \n");
    printf("close - Encerrar um Programa \n");
}

void adicionaUsers(user aux){
    printf("\nNovo utilizador: %s\n", aux.username);
    strcpy(info.listaUsers[info.numUsers].password,aux.password);
    strcpy(info.listaUsers[info.numUsers].username,aux.username);
    info.listaUsers[info.numUsers].saldo=aux.saldo;
    strcpy(info.listaUsers[info.numUsers].pipe_name,aux.pipe_name);
    info.numUsers++;
}

void adicionaItem(item auxItem){
    printf("\nNew Item : [%d] %s", auxItem.IDitem, auxItem.name);
    strcpy(info.items[info.numItens].category, auxItem.category);
    info.items[info.numItens].current_value = auxItem.current_value;
    info.items[info.numItens].duration = auxItem.duration;
    info.items[info.numItens].IDitem = auxItem.IDitem;
    strcpy(info.items[info.numItens].name, auxItem.name);
    strcpy(info.items[info.numItens].user_buyer, auxItem.user_buyer);
    strcpy(info.items[info.numItens].user_sell, auxItem.user_sell);
    info.items[info.numItens].value = auxItem.value;
}


void apagarUser(user auxUser){
    for(int i = 0; i<info.numUsers; i++){
        if(strcmp(info.listaUsers[i].username, auxUser.username) == 0){
            printf("User : %s foi encerrado\n", info.listaUsers[i].username);
            for(int j=i+1; j<info.numUsers; i++){
                strcpy(info.listaUsers[j-1].password, info.listaUsers[j].password);
                strcpy(info.listaUsers[j-1].pipe_name, info.listaUsers[j].pipe_name);
                info.listaUsers[j-1].saldo = info.listaUsers[j].saldo;
                strcpy(info.listaUsers[j-1].username, info.listaUsers[j].username);
            }
            info.numUsers--;
            return;
        }
    }
}

void apagarItems(item auxItem){
    for(int i = 0; i<info.numItens; i++){
        if(info.items[i].IDitem == auxItem.IDitem){
            for(int j=i+1; j<info.numItens; i++){
                strcpy(info.items[j-1].category, info.items[j].category);
                info.items[j-1].current_value = info.items[j].current_value;
                info.items[j-1].duration = info.items[j].duration;
                info.items[j-1].IDitem = info.items[j].IDitem;
                strcpy(info.items[j-1].name, info.items[j].name);
                strcpy(info.items[j-1].user_buyer, info.items[j].user_buyer);
                strcpy(info.items[j-1].user_sell, info.items[j].user_sell);
                info.items[j-1].value = info.items[j].value;
            }
            info.numItens--;
            return;
        }
    }
}

void apagarPromocao(promocao auxPromocao){
    for(int i = 0; i<info.numPromocao; i++){
        if(info.promocoes[i].descont == auxPromocao.descont && (strcmp(info.promocoes[i].category, auxPromocao.category) == 0)){
            for(int j=i+1; j<info.numPromocao; i++){
                strcpy(info.promocoes[j-1].category, info.promocoes[j].category);
                info.promocoes[j-1].descont = info.promocoes[j].descont;
                info.promocoes[j-1].duration = info.promocoes[j].duration;
            }
            info.numPromocao--;
            return;
        }
    }
}

void apagarItemsTerminados(){
    for(int i = 0; i<info.numItens; i++){
        if(info.items[i].duration == 0){
            for(int j=i+1; j<info.numItens; j++){
                strcpy(info.items[j-1].category, info.items[j].category);
                info.items[j-1].current_value = info.items[j].current_value;
                info.items[j-1].duration = info.items[j].duration;
                info.items[j-1].IDitem = info.items[j].IDitem;
                strcpy(info.items[j-1].name, info.items[j].name);
                strcpy(info.items[j-1].user_buyer, info.items[j].user_buyer);
                strcpy(info.items[j-1].user_sell, info.items[j].user_sell);
                info.items[j-1].value = info.items[j].value;
                info.items[j-1].new = true;
            }
            info.numItens--;
            return;
        }
    }
}

void apagarPromocaoTerminados(){
    for(int i = 0; i<info.numPromocao; i++){
        if(info.promocoes[i].duration == 0){
            for(int j=i+1; j<info.numPromocao; j++){
                strcpy(info.promocoes[j-1].category, info.promocoes[j].category);
                info.promocoes[j-1].descont = info.promocoes[j].descont;
                info.promocoes[j-1].duration = info.promocoes[j].duration;
                info.promocoes[j-1].new = true;
            }
            info.numPromocao--;
            return;
        }
    }
}

void verificaExpiracao(){
    int res, fd_cli;

    for(int i = 0; i < info.numItens; i++){
        if(info.items[i].duration == 0){
            if(strcmp(info.items[i].user_buyer, "") == 0)
                printf("<Tempo expirou> Item: %d nao foi vendido!\n", info.items[i].IDitem);
            else
                printf("<Tempo expirou> Item %d - %s foi vendido: (categoria)%s (preco atual)%d (comprador)%s \n", info.items[i].IDitem, info.items[i].name, info.items[i].category, info.items[i].current_value, info.items[i].user_buyer);
            info.items[i].new = false;  
            for (int j = 0; j < info.numUsers; j++){
                fd_cli = open(info.listaUsers[j].pipe_name, O_WRONLY);
                int value = FLAG_ITEM;
                res = write(fd_cli, &value, sizeof(int));
                res = write(fd_cli, &info.items[i], sizeof(item));
                close(fd_cli);  
            }
        }
    }
    apagarItemsTerminados();
    for(int i = 0; i < info.numPromocao; i++){
        if(info.promocoes[i].duration == 0){
            printf("<Tempo expirou> Promocao de %d na categoria %s expirou! \n", info.promocoes[i].descont, info.promocoes[i].category);
            info.promocoes[i].new = false;
            for (int j = 0; j < info.numUsers; j++){
                fd_cli = open(info.listaUsers[j].pipe_name, O_WRONLY);
                int value = FLAG_PROM;
                res = write(fd_cli, &value, sizeof(int));
                res = write(fd_cli, &info.promocoes[i], sizeof(promocao));
                close(fd_cli);  
            } 
        } 
    }
    apagarPromocaoTerminados();
}

void *increment_seconds(void *arg){
    
    while(info.FLAG_TERMINOU == 0){
        sleep(1);
        info.seconds++;
        for(int i = 0; i < info.numItens; i++){
            info.items[i].duration--;
        }
        for(int i = 0; i < info.numPromocao; i++){
            info.promocoes[i].duration--;
        }
        verificaExpiracao();
    }
    return NULL;
}

void* LaunchPromotores(void * dados){

    promotor* data = (promotor *)dados;
    int res, fd_cli;
    pthread_t t[20];
    int pl[2];
    pipe(pl);
    char promotor1[20], aux[20]="";
    promocao auxProm;
    char path[100];
    int id;


    id=fork();

    if(id == 0){
        close(1);//fechar acesso ao monitor
        dup(pl[1]);//duplicar p[1] na primeira posição do pipe
        close(pl[0]);//fechar extremidade de leitura do pipe
        close(pl[1]);//fechar extremidade de escrita do pipe


        sprintf(path,"Promotores/%s",data->name);

        execl(path, data->name, NULL);

        perror("");
        exit(1);
    }
    else
    {
        data->pid=id;
        close(pl[1]);

        while(1)
        {
            read(pl[0],&promotor1,sizeof(promotor1));

            if(strcmp(promotor1,aux)!=0)
            {
                printf("Promocao: %s\n",promotor1);
                strcpy(aux,promotor1);
                if (info.numPromocao < MAXPROMOCOES ){
                    strcpy(auxProm.category, strtok(promotor1, " "));
                    auxProm.descont = atoi(strtok(NULL, " "));
                    auxProm.duration = atoi(strtok(NULL, " "));
                    strcpy( info.promocoes[info.numPromocao].category, auxProm.category);
                    info.promocoes[info.numPromocao].descont= auxProm.descont;
                    info.promocoes[info.numPromocao].duration= auxProm.duration;
                    auxProm.new = true;

                    for (int i = 0; i < info.numUsers; i++){
                        int value = FLAG_PROM;
                        fd_cli = open(info.listaUsers[i].pipe_name, O_WRONLY);
                        res = write(fd_cli, &value, sizeof(int));
                        res = write(fd_cli, &auxProm, sizeof(auxProm));
                        close(fd_cli);  
                    }      

                    info.numPromocao++;
                };

                strcpy(promotor1,"");
            }
        }

        close(pl[0]);
    }

    pthread_exit(NULL);
}


void Promotores(){

    int pd[MAXPROMOTERS];
    char prom[20];
    int error,i=0;
    pthread_t promotorThread[MAXPROMOTERS];

    FILE *filedesc;
    filedesc = fopen("promoter.txt","r");

    while (fscanf(filedesc,"%s %d",prom,&error) == 1 && i < MAXPROMOTERS ){
        strcpy(info.listaProm[i].name,prom);
        info.numProm++;

        if(pthread_create(&promotorThread[i],NULL,&LaunchPromotores,&info.listaProm[i]) != 0) return;
        i++;
    }

    i=0;
    while (i<MAXPROMOTERS){
        pd[i] = info.listaProm[i].pid;
        ++i;
    }
}

void CancelPromotor (char* nome_prom){
    int i=0;

    do {
        if(strcmp(info.listaProm[i].name,nome_prom) ==0 )
        {

            union sigval val;
            sigqueue(info.listaProm[i].pid, SIGUSR1 , val);

            strcpy(info.listaProm[i].name,"--");
        }
        ++i;
    } while (i<10);
}


int atualizaSaldo(char nome[], int saldoNovo) {
    int nlidos, saldo;
    if(getUserBalance(nome) == -1){
        return 0;

    }
    for (int i = 0; i < info.numUsers; i++){
        if(strcmp(info.listaUsers[i].username, nome) == 0){
            updateUserBalance(nome, saldoNovo);
        }
    }
    return 1;
}

int le_itens() {    
    FILE *file;
    file = fopen(fileFI, "rt");
    if(file == NULL){
        printf("\nNao foi possivel abrir o ficheiro dos itens\n");
        return info.numItens;
    }
    while(info.numItens < MAXITEMS && fscanf(file, "%d %s %s %d %d %d %s %s\n", &info.items[info.numItens].IDitem, info.items[info.numItens].name, info.items[info.numItens].category, &info.items[info.numItens].current_value, &info.items[info.numItens].value, &info.items[info.numItens].duration, info.items[info.numItens].user_sell, info.items[info.numItens].user_buyer)!=EOF){
        info.id++;
        info.items[info.numItens].new = true;
        printf("\nNew Item : [%d] %s", info.items[info.numItens].IDitem, info.items[info.numItens].name);
        info.numItens++;
    }
    fclose(file);
    return info.numItens;
}

void escreve_itens(){
    FILE *file;
    file = fopen(fileFI, "wt");
    if(file == NULL){
        printf("\nNao foi possivel abrir o ficheiro dos itens\n");
        return ;
    }
    for(int i=0; i< info.numItens; i++){
        fprintf(file, "%d %s %s %d %d %d %s %s\n", info.items[i].IDitem, info.items[i].name, info.items[i].category, info.items[i].current_value, info.items[i].value, info.items[i].duration, info.items[i].user_sell, info.items[i].user_buyer);
    }
    fclose(file);
    return ;
}

void escreve_promotores(){
    FILE *file;
    file = fopen(fileFP, "wt");
    if(file == NULL){
        printf("\nNao foi possivel abrir o ficheiro dos itens\n");
        return ;
    }
    for(int i=0; i< info.numProm; i++){
        fprintf(file, "%s\n", info.listaProm[i].name);
    }
    fclose(file);
    return ;
}


int main(int argc, char *argv[]){
    char comando[50], arg[50], mensagem[50];
    int res, fd_cli;
    fd_set fontes;
    struct timeval t;
    backend data;
    pthread_t thread;
    
    if(access(PIPE_FRONT_BACK, F_OK) == 0){
        printf("[ERROR] Backend ja esta em execucao\n");
        return 0;
    }
    if(mkfifo(PIPE_FRONT_BACK, 0666) != 0){
        printf("[ERROR] Ao criar o pipe\n");
        return 0;
    }
    int fd_serv = open(PIPE_FRONT_BACK, O_RDWR);
    info.seconds = 0;
    info.numItens = 0;
    info.numPromocao = 0;
    info.numProm = 0;
    info.numUsers = 0;
    info.FLAG_TERMINOU = 0;
    info.id = 0;

    user auxUser;
    item auxItem;
    

    if (getenv("FPROMOTERS") != NULL)
        strcpy(fileFP, getenv("FPROMOTERS"));
    if (getenv("FUSERS") != NULL)
        strcpy(fileFU, getenv("FUSERS"));
    if (getenv("FITEMS") != NULL)
        strcpy(fileFI, getenv("FITEMS"));

    //cria thread
    pthread_create(&thread, NULL, increment_seconds, NULL);
    loadUsersFile(FUSERS);
    info.numItens = le_itens();
    Promotores();

    comandos();
    do{
        fflush(stdout);
        printf("\nIntroduza um comando: ");
        fflush(stdout);

        FD_ZERO(&fontes);
        FD_SET(0, &fontes);
        FD_SET(fd_serv, &fontes);
        t.tv_sec =60;
        t.tv_usec=0;
        res = select(fd_serv + 1, &fontes, NULL, NULL, &t);

        if(res > 0 && FD_ISSET(0, &fontes)){        //VEIO DO TECLADO
            fgets(comando,50,stdin);
            comando[strlen(comando) - 1] = '\0';
            if(strcmp(comando, "help\0") == 0){
                comandos();
            }
            if(strcmp(comando, "users\0") == 0){
                if( info.numUsers == 0)
                    printf("Nao existem users!\n");
                for(int i = 0; i < info.numUsers ; i++){
                    printf("Users: %s \n", info.listaUsers[i].username);
                }
            }
            if(strcmp(comando, "list\0") == 0){
                if( info.numItens == 0)
                    printf("Nao existem itens!\n");
                for(int i = 0; i < info.numItens ; i++){
                    printf("Item %d - %s: (categoria)%s (preco atual)%d (preco base)%d (vendedor)%s (comprador)%s \n", info.items[i].IDitem, info.items[i].name, info.items[i].category, info.items[i].current_value, info.items[i].value, info.items[i].user_sell, info.items[i].user_buyer);
                }
            }
            if(strncmp(comando, "kick", 4) == 0){
                if(strcmp(comando, "kick\0") != 0){ 
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    for(int i=0; i<info.numUsers; i++){
                        if(strcmp(info.listaUsers[i].username, arg) == 0){
                            strcpy(arg, strtok(info.listaUsers[i].pipe_name, "I"));
                            strcpy(arg, strtok(NULL, " "));
                            printf("\n User %s a terminar\n", info.listaUsers[i].username);
                            kill(atoi(arg),SIGUSR1);
                            apagarUser(info.listaUsers[i]);
                            break;
                        }
                    }
                }
                else
                    printf("E necessario definir o user a ser expulso \n");
            }
            if(strcmp(comando, "prom\0") == 0){
                if( info.numProm == 0)
                    printf("Nao existem promotores!\n");
                for(int i = 0; i < info.numProm ; i++){
                    printf("Promotor: %s \n", info.listaProm[i].name);
                }
            }
            if(strcmp(comando, "reprom\0") == 0){
                for(int i = 0; i<info.numProm;i++)
                    CancelPromotor(info.listaProm[i].name);
                Promotores();   
            }
            if(strncmp(comando, "cancel", 6) == 0){
                if(strcmp(comando, "cancel\0") != 0){ 
                    strcpy(arg, strtok(comando, " "));
                    strcpy(arg, strtok(NULL, " "));
                    CancelPromotor(arg);
                    printf("\n Promotor %s foi cancelado \n", arg);
                }
                printf("E necessario definir o promotor a ser cancelado \n");
            }
            if(strcmp(comando, "close\0") == 0){
                printf("\n A sair...\n");
                info.FLAG_TERMINOU = 1;
            }
        }else if(res > 0 && FD_ISSET(fd_serv, &fontes)){        //VEIO DO PIPE DO SERVIDOR
            int flag;
            res = read(fd_serv, &flag, sizeof(flag));
            if (flag == FLAG_NEW_USER){
                res = read(fd_serv, &auxUser, sizeof(auxUser));
                fd_cli = open(auxUser.pipe_name, O_WRONLY);
                if (isUserValid(auxUser.username, auxUser.password) == 1 && info.numUsers < MAXUSERS){
                    printf("\nNew User : %s  -  ", auxUser.username);
                    adicionaUsers(auxUser); 
                    auxUser.saldo = getUserBalance(auxUser.username);
                }else {
                    auxUser.saldo = -1;
                }
                res = write(fd_cli, &auxUser.saldo, sizeof(auxUser.saldo));
                if(info.numItens > 0){
                    for (int i = 0; i < info.numItens; i++){
                        int value = FLAG_ITEM;
                        info.items[i].new = true;
                        res = write(fd_cli, &value, sizeof(int));
                        res = write(fd_cli, &info.items[i], sizeof(item));
                    }  
                }
                close(fd_cli);  
            }else if(flag == FLAG_NEW_ITEM){
                res = read(fd_serv, &auxItem, sizeof(auxItem));
                auxItem.IDitem = info.id;
                info.id++;
                auxItem.new = true;
                adicionaItem(auxItem);
                
                for (int i = 0; i < info.numUsers; i++){
                    fd_cli = open(info.listaUsers[i].pipe_name, O_WRONLY);
                    int value = FLAG_ITEM;
                    res = write(fd_cli, &value, sizeof(int));
                    res = write(fd_cli, &auxItem, sizeof(auxItem));
                    close(fd_cli);  
                }       
                info.numItens++;
            }else if(flag == FLAG_TIME){
                res = read(fd_serv, &auxUser, sizeof(auxUser));
                for (int i = 0; i < info.numUsers; i++){
                    if(strcmp(info.listaUsers[i].username, auxUser.username) == 0){
                        fd_cli = open(info.listaUsers[i].pipe_name, O_WRONLY);
                        res = write(fd_cli, &info.seconds, sizeof(info.seconds));
                        close(fd_cli);    
                    }
                }
            }else if(flag == FLAG_LICITACAO){
                res = read(fd_serv, &auxItem, sizeof(auxItem));
                printf("\nUser %s licitou %d€ para [%d]\n", auxItem.user_buyer, auxItem.current_value, auxItem.IDitem);
                for(int i = 0; i<info.numItens; i++){
                    if(auxItem.IDitem == info.items[i].IDitem){
                        info.items[i].current_value = auxItem.current_value;
                        strcpy(info.items[i].user_buyer, auxItem.user_buyer);
                        for(int j = 0; j < info.numUsers; j++){ 
                            auxItem.new = true;
                            fd_cli = open(info.listaUsers[j].pipe_name, O_WRONLY);
                            int value = FLAG_ITEM;
                            res = write(fd_cli, &value, sizeof(int));
                            res = write(fd_cli, &auxItem, sizeof(auxItem));
                            close(fd_cli);  
                        }
                    }
                }
            }else if(flag == FLAG_CARREGAMENTO){
                res = read(fd_serv, &auxUser, sizeof(auxUser));
                for (int i = 0; i < info.numUsers; i++){
                    if(strcmp(info.listaUsers[i].username, auxUser.username) == 0){
                            info.listaUsers[i].saldo = auxUser.saldo;
                            atualizaSaldo(auxUser.username, auxUser.saldo);
                    }
                }
            }else if(flag == FLAG_EXIT_USER){
                res = read(fd_serv, &auxUser, sizeof(auxUser));
                apagarUser(auxUser);
            }            
        }

        

    }while(strcmp(comando, "close"));
    saveUsersFile(FUSERS);
    escreve_itens();
    escreve_promotores();
    for(int i = 0; i < info.numProm; i++){
        printf("\n Promotor %s foi cancelado \n", info.listaProm[i].name);
        CancelPromotor(info.listaProm[i].name);
    }
    for(int i=0; i<info.numUsers; i++){
        strcpy(arg, strtok(info.listaUsers[i].pipe_name, "I"));
        strcpy(arg, strtok(NULL, " "));
        printf("\n User %s a terminar\n", info.listaUsers[i].username);
        kill(atoi(arg),SIGUSR1);
    }
    remove(PIPE_FRONT_BACK);
    close(fd_serv);
    unlink(PIPE_FRONT_BACK);
}
