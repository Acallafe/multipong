#include "Red.h"
#include <iostream>
#include <stdlib.h>     /* atoi */
#include <cstring>

#define MAX_LEN 512

Red::Red()
{
    //ctor
}

Red::~Red()
{
    //dtor
}


int Red::inicia(){
    /* Inicializamos la Red */
    if(SDLNet_Init()==-1) {
        std::cout << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
        return -2;
    }else{
        std::cout << "Red Inicializada" << std::endl;
    }

    return 0;
}

int Red::iniciaServidor(int port){
    // create a listening TCP socket on port 9999 (server)
    IPaddress ip;

    std::cout << "Creando servidor : " << std::endl;

    if(SDLNet_ResolveHost(&ip,NULL,port)==-1) {
        std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    tcpsock=SDLNet_TCP_Open(&ip);
    if(!tcpsock) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return -2;
    }

    std::cout << "Servidor creado : " << std::endl;

    return 0;
}

int Red::esperaClientes(int nclientes){
    connectedClients = 0;

    std::cout << "Esperando cliente : " << std::endl;

    //Esperar conexion de usuarios
    while(connectedClients<nclientes){
        clientes[connectedClients]=SDLNet_TCP_Accept(tcpsock);
        if(clientes[connectedClients]) {
            //Enviamos el n�mero de cliente
            std::cout << "Cliente conectado :" << connectedClients << std::endl;

            servidorEnviaNumero(&clientes[connectedClients],connectedClients+1);
            connectedClients++;
        }
    }

    return 0;
}

int Red::iniciaCliente(std::string host, int port){
    // connect to localhost at port 9999 using TCP (client)
    IPaddress ip;

    if(SDLNet_ResolveHost(&ip,host.c_str(),port)==-1) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    tcpsock=SDLNet_TCP_Open(&ip);
    if(!tcpsock) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    //Recibir numero cliente

    return clienteRecibeNumero();
}

int Red::envia(TCPsocket* cliente, char* msg){
    int len = strlen(msg)+1;
    int result=SDLNet_TCP_Send(*cliente,msg,len);
    if(result<len) {
        std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        // It may be good to disconnect sock because it is likely invalid now.
        return -1;
    }

    std::cout << "Envio Mensaje " << msg << std::endl;

    return 0;
}

int Red::recibe(TCPsocket* servidor, char * msg){
    int result;

    std::cout << "Esperando Respuesta : " << std::endl;
    result=SDLNet_TCP_Recv(*servidor,msg,MAX_LEN);
    if(result<=0) {
        // An error may have occured, but sometimes you can just ignore it
        // It may be good to disconnect sock because it is likely invalid now.
        return -1;
    }else{
        std::cout << "Mensaje recibido : " << msg << std::endl;
    }

    return 0;
}

int Red::clienteRecibeNumero(){
    if(recibe(&tcpsock,buffer)>=0){
        std::cout << "Numero cliente recibido "<< buffer << std::endl;
        //Recibimos el numero de jugador
        int playerNumber = atoi(buffer);
        return playerNumber;
    }else{
        return -1;
    }
}

int Red::servidorEnviaNumero(TCPsocket * cliente, int numero){
    sprintf(buffer,"%d",numero);

    int len = std::strlen(buffer)+1;
    int result=SDLNet_TCP_Send(*cliente,buffer,len);
    if(result<len) {
        printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        // It may be good to disconnect sock because it is likely invalid now.
        return -1;
    }

    std::cout << "Envio numero cliente " << buffer << std::endl;

    return 0;
}
