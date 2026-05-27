#include	<poll.h>
#include	<iostream>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include 	<arpa/inet.h>
#include 	<netinet/in.h>
 
#define BUFFER_SIZE 50
#define MAX_CLIENTS 10


struct Client{
	int	fd;
	std::string receive;
	std::string send;
};

int	main(void)
{
	struct sockaddr_in	addr = {};
 
	//parsing du .config server
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
	//ca ne devrait pas changer
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "socket_fd : " << socket_fd << std::endl;
	int opt = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //to reuse ip adress after client disconnection 
	bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
	listen(socket_fd, 10);
 
	// --- Nouveau : tableau de pollfd ---
	// fds[0] = le server socket (on surveille les nouvelles connexions)
	// fds[1..N] = les clients connectés
	struct pollfd fds[MAX_CLIENTS + 1] = {};
 
	fds[0].fd     = socket_fd;
	fds[0].events = POLLIN; // surveiller les connexions entrantes
 
	int nfds = 1; // nombre de fds actifs dans le tableau - 1 because server socket
 
	while (1)
	{
		std::cout << "start" << std::endl;
		// poll() attend qu'un événement arrive sur n'importe quel fd
		// -1 = pas de timeout, bloque indéfiniment
		poll(fds, nfds, -1);
		
		// --- Parcourir tous les fds actifs ---
		for (int i = 0; i < nfds; i++)
		{
			std::cout << "i : " << i << std::endl;
			//std::cout << "fds[i] : " << fds[i].fd << std::endl;

			if (fds[i].revents == 0)
			{
				std::cout << "nothing..." << std::endl;
				continue; // rien sur ce fd, on passe
			}
				
			// --- Cas 1 : nouvelle connexion sur le server socket ---
			if (fds[i].fd == socket_fd)
			{
				int client_fd = accept(socket_fd, NULL, NULL);
				std::cout << "Nouveau client : fd =" << client_fd << std::endl;
 
				// Ajouter le client au tableau
				fds[nfds].fd     = client_fd;
				fds[nfds].events = POLLIN; // on veut lire ce qu'il envoie
				nfds++;
			}
			// --- Cas 2 : données à lire sur un client ---
			else
			{
				char buffer[BUFFER_SIZE] = {};
				int readed = read(fds[i].fd, buffer, sizeof(buffer));

				if (readed == 0) // client déconnecté
				{
					std::cout << "Client déconnecté : fd=" << fds[i].fd << std::endl;
					close(fds[i].fd);
					fds[i] = fds[nfds - 1]; // remplacer par le dernier
					nfds--;
					i--; // retraiter cet index
				}
				else
				{
					//parsing buffer + controle requet + creation reponse
					char response[BUFFER_SIZE] = "HTTP/1.1 200 OK\r\n";
					std::cout << readed << " octets reçus" << std::endl;
					std::cout << buffer << std::endl;
					write(fds[i].fd, response, sizeof(response)); 
				}
			}
		}
		std::cout << "nfds : " << nfds << std::endl;
	}
	return (0);
}