#include "../inc/event_loop.hpp"

int	event_loop(void)
{
	struct sockaddr_in	addr = {};
	std::vector<struct pollfd> fds;
	std::map<int, Client> clients;
 
	//parsing du .config server
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
	//ça ne devrait pas changer
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		std::cerr << "Socket() failed: " << strerror(errno) << std::endl;
		return (1);
	}
	fcntl(socket_fd, F_SETFL, O_NONBLOCK); //socket non-bloquant 
	int opt = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) //to reuse ip adress after client disconnection 
	{
		std::cerr << "Setsockopt() failed : " << strerror(errno) << std::endl;
		close(socket_fd);
		return (1);
	}
	if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "bind() failed : " << strerror(errno) << std::endl;
		close(socket_fd);
		return (1);
	}
	if (listen(socket_fd, 10) < 0)
	{
		std::cerr << "listen() failed : " << strerror(errno) << std::endl;
		close(socket_fd);
		return (1);
	}
 
	struct pollfd server_pfd = {};
	server_pfd.fd = socket_fd;
	server_pfd.events = POLLIN; // POLLIN uniquement car pour les nouvelles connexions que des entrées pas de sortie
	fds.push_back(server_pfd);
  
	while (1)
	{
		// poll() attend qu'un événement arrive sur n'importe quel fd
		// -1 = pas de timeout, bloque indéfiniment
		int ret = poll(fds.data(), fds.size(), -1);
		if (ret < 0)
		{
			std::cerr << "poll() failed : " << strerror(errno) << std::endl;
			break;
		}
		if ( ret == 0)
			continue;
		
		// --- Parcourir tous les fds actifs ---
		for (int i = 0; i < (int)fds.size(); i++)
		{
			if (!(fds[i].revents & (POLLIN | POLLOUT | POLLERR | POLLHUP)))
			{
				std::cout << "nothing..." << std::endl;
				continue; // rien sur ce fd, on passe
			}
			// --- Cas 1 : nouvelle connexion sur le server socket ---
			if (fds[i].fd == socket_fd)
			{
				std::cout << "New connexion" << std::endl;
				int client_fd = accept(socket_fd, NULL, NULL);
				if (client_fd < 0)
				{
					std::cerr << "accept() failed : " << strerror(errno) << std::endl;
					continue; // <- saute à la prochaine iteration de la boucle for
				}
				fcntl(client_fd, F_SETFL, O_NONBLOCK); // socket non-bloquant
				// Ajouter le client au tableau
				struct pollfd client_pfd = {};
				client_pfd.fd = client_fd;
				client_pfd.events = POLLIN; // on veut lire ce qu'il envoie
				fds.push_back(client_pfd);
				clients[client_fd] = Client();
				std::cout << "new client : " << client_fd << std::endl;
			}
			// --- Cas 2 : données à lire sur un client ---
			else
			{
				if (fds[i].revents & (POLLERR | POLLHUP))
				{
					std::cout << "Erreur ou déconnexion sur fd : " << fds[i].fd << std::endl;
					close(fds[i].fd);
					clients.erase(fds[i].fd);
					fds.erase(fds.begin() + i);
					i--;
					continue;
				}
				if (fds[i].revents & POLLIN)
				{
					std::cout << "Something to read" << std::endl;
					char buffer[BUFFER_SIZE] = {};
					int readed = read(fds[i].fd, buffer, BUFFER_SIZE);

					if (readed == 0) // client déconnecté
					{
						std::cout << "Client déconnecté : fd=" << fds[i].fd << std::endl;
						close(fds[i].fd);
						clients.erase(fds[i].fd);
						fds.erase(fds.begin() + i);
						i--; // retraiter cet index
					}
					else
					{
						clients[fds[i].fd].receive += std::string(buffer, readed);
						//check si c'est la fin du header
						if (clients[fds[i].fd].receive.find("\r\n\r\n") != std::string::npos)
						{
							// réponse prête -> on la bufferise et on active POLLOUT
							clients[fds[i].fd].send = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
							fds[i].events = POLLIN | POLLOUT; // demander à poll de surveiller l'écriture
						}
					}
				}
				if (fds[i].revents & POLLOUT && !clients[fds[i].fd].send.empty()) //pas if/else car on peut avoir POLLIN et POLLOUT en meme temps
				{
					std::cout << "Something to write" << std::endl;
					int sent = write(fds[i].fd, clients[fds[i].fd].send.c_str(), clients[fds[i].fd].send.size());
					if (sent > 0)
						clients[fds[i].fd].send.erase(0, sent);
					if (clients[fds[i].fd].send.empty())
						fds[i].events = POLLIN;
				}
			}
		}
	}
	return (0);
}