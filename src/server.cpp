/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbacquet <cbacquet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 15:17:00 by cbacquet          #+#    #+#             */
/*   Updated: 2024/06/13 16:39:41 by cbacquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

bool Signal = false;

Server::Server() : _port(), _socket(), _epollFd() {}

Server::Server(u_int64_t port, const std::string& password) : _socket()
{
	_port							  = port;
	_password						  = password;
	_passOperator					  = "admin";
	struct epoll_event servEpollEvent = {};

	_epollFd = epoll_create1(0);
	setServerSocket();

	if ( _epollFd == -1 )
	{
		std::cout << RED;
		throw std::runtime_error("Error during creation of the epoll instance");
	}

	memset(&servEpollEvent, 0, sizeof(servEpollEvent));
	servEpollEvent.events  = EPOLLIN;
	servEpollEvent.data.fd = this->_socket;

	if ( epoll_ctl(_epollFd, EPOLL_CTL_ADD, _socket, &servEpollEvent) == -1 )
	{
		std::cout << RED;
		close(_socket);
		close(_epollFd);
		throw std::runtime_error("Error during adding the file descriptor to the epoll instance");
	}
}

Server::~Server()
{
	for ( std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it )
	{
		delete it->second;
		// _MapChannels.erase(itChan);
	}
	for ( std::map<std::string, Channel*>::iterator itChan = _MapChannels.begin(); itChan != _MapChannels.end();
		  ++itChan )
	{
		delete itChan->second;
		// _MapChannels.erase(itChan);
	}
	// _MapChannels.clear();
}

void Server::setServerSocket()
{
	int option					= 1;
	struct sockaddr_in sockaddr = {};

	// Socket creation //
	_socket = socket(AF_INET, SOCK_STREAM, 0);

	if ( _socket == -1 )
	{
		std::cerr << RED;
		throw(std::runtime_error("Error during creation of the socket"));
	}

	// Set socket options //
	setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	std::cout << BLU << "-------- IRC CONNECTED --------\n" << WHI << "Creation of a socket..." << std::endl;

	if ( _password.empty() )
		std::cout << RED << "!! WARNING: NO PASSWORD SET FOR THE SERVER !!" << WHI << std::endl;

	sockaddr				 = (struct sockaddr_in) {};
	sockaddr.sin_family		 = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port		 = htons(this->_port);

	// Bind the socket to the port //
	if ( bind(_socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1 )
	{
		close(_socket);
		std::cerr << RED;
		throw(std::runtime_error("Error during bind socket to adress and port"));
	}

	// Listen to the socket //
	if ( listen(_socket, SOMAXCONN) == -1 )
	{
		close(_socket);
		std::cerr << RED;
		throw(std::runtime_error("Error during listen the socket"));
	}

	std::cout << "The server is listening on the port " << GRE << "<" << YEL << this->_port << GRE << ">" << WHI
			  << std::endl;
}

void handleSigInt(int sig);

void Server::serverInit()
{
	signal(SIGINT, handleSigInt);
	std::cout << "Waiting for incoming connections on port " << GRE << "<" << YEL << getPort() << GRE << ">\n"
			  << WHI << std::endl;
	while ( !Signal )
	{
		// std::cout << "test" << std::endl;
		struct epoll_event events[MAX_EVENT];
		int num_events = epoll_wait(_epollFd, events, MAX_EVENT, -1);

		if ( num_events == -1 )
			break;
		for ( int i = 0; i < num_events; i++ )
		{
			if ( events[i].events & EPOLLIN ) // Lecture disponible
			{
				if ( events[i].data.fd == getSocket() )
					requestConnexion(events[i].data.fd);
				else
					receiveNewData(events[i].data.fd);
			}
			if ( events[i].events & EPOLLOUT ) // Prêt à écrire
			{
				sendBufferedData(events[i].data.fd);
				if ( isBufferEmpty(events[i].data.fd) )
					removeEpollOutFlag(events[i].data.fd);
			}
		}
	}
}

void Server::sendBufferedData(int client_fd)
{
	Client* client = _clients[client_fd];
	if ( client->sendBuffer.empty() )
		return;

	ssize_t bytesSent = send(client_fd, client->sendBuffer.c_str(), client->sendBuffer.size(), MSG_DONTWAIT);
	if ( bytesSent == -1 )
	{
		std::cerr << "Error sending data to client" << std::endl;
		clientDisconnect(client_fd);
		return;
	}
	client->sendBuffer = client->sendBuffer.substr(bytesSent);
	if ( client->sendBuffer.empty() )
		removeEpollOutFlag(client_fd);
}

bool Server::isBufferEmpty(int client_fd)
{
	Client* client = _clients[client_fd];
	return client->sendBuffer.empty();
}

void Server::removeEpollOutFlag(int client_fd)
{
	Client* client = _clients[client_fd];

	struct epoll_event ev;
	ev.events  = EPOLLIN; // Ne surveiller que les événements de lecture
	ev.data.fd = client->getFd();

	if ( epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, client_fd, &ev) == -1 )
	{
		std::cerr << "Error modifying client fd in epoll" << std::endl;
		clientDisconnect(client_fd);
	}
}

void Server::requestConnexion(int clientFd)
{
	clientFd = accept(getSocket(), NULL, NULL);
	if ( clientFd == -1 )
	{
		std::cerr << "Error during accepting connexion" << std::endl;
		return;
	}
	std::cout << "New connection accepted from client [fd" << clientFd << "] " << std::endl;
	Client* newClient = new Client(clientFd);
	addClient(clientFd, newClient);
}

void Server::addClient(int key, Client* clientToAdd)
{
	// TODO call updateClientStatus with EPOLLOUT off
	// clientToAdd->updateClientStatus(this->_epollFd);
	clientToAdd->updateClientStatus(this->_epollFd, EPOLLIN);
	_clients[key] = clientToAdd;
}

void Server::receiveNewData(int client_fd)
{
	std::vector<std::string> cmd;
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(client_fd, buff, sizeof(buff) - 1,
						 MSG_DONTWAIT); // Utilisation de MSG_DONTWAIT pour une opération non bloquante

	if ( bytes == -1 )
	{
		std::cerr << "Error receiving data from client" << std::endl;
		clientDisconnect(client_fd);
		return;
	}
	else if ( bytes == 0 )
	{
		// std::cout << "EOT detected on fd" << client_fd << std::endl;
		if ( _partialCommand[client_fd].size() > 0 )
		{
			parseCommand(_partialCommand[client_fd], client_fd);
			_partialCommand[client_fd].clear();
		}
		clientJoin0(client_fd);
		clientDisconnect(client_fd);
		return;
	}
	else
	{
		buff[bytes]	   = '\0';
		Client* client = _clients[client_fd];
		client->buffer += buff;
		_partialCommand[client_fd] += buff;

		size_t pos;
		while ( (pos = client->buffer.find_first_of("\r\n")) != std::string::npos )
		{
			std::string cmd = client->buffer.substr(0, pos);
			parseCommand(cmd, client_fd);
			client->buffer.erase(0, pos + 1);
		}
	}
}

void Server::queueDataForSend(const std::string& data, int client_fd)
{
	Client* client = _clients[client_fd];
	client->sendBuffer += data;

	struct epoll_event ev;
	ev.events  = EPOLLIN | EPOLLOUT; // Surveiller à la fois la lecture et l'écriture
	ev.data.fd = client_fd;
	if ( epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, client_fd, &ev) == -1 )
	{
		std::cerr << "Error modifying client fd in epoll" << std::endl;
		clientDisconnect(client_fd);
	}
}

void Server::reply(std::string response, int fd)
{
	std::cout << "Response:\n" << response;
	if ( send(fd, response.c_str(), response.size(), 0) == -1 )
		std::cerr << "Response send() faild" << std::endl;
}

Client* Server::getClientByFd(int fd)
{
	if ( _clients.find(fd) != _clients.end() )
		return _clients[fd];
	throw std::runtime_error("Client not found");
}

void handleSigInt(int sig)
{
	static_cast<void>(sig);
	std::cout << RED << "SIGINT received. Stopping the server..." << WHI << std::endl;
	std::cout << BLU << "-------- IRC DISCONNECTED --------" << WHI << std::endl;
	Signal = true;
}

void Server::setNickUsed(const nickSet& nickUsed)
{
	_nickUsed = nickUsed;
}

const std::string& Server::getPassword() const
{
	return _password;
}

void Server::setPassword(const std::string& password)
{
	_password = password;
}

u_int64_t Server::getPort() const
{
	return _port;
}

void Server::setPort(u_int64_t port)
{
	_port = port;
}

int Server::getSocket() const
{
	return _socket;
}

void Server::setSocket(int socket)
{
	_socket = socket;
}

int Server::getEpollFd() const
{
	return _epollFd;
}

void Server::setEpollFd(int epollFd)
{
	_epollFd = epollFd;
}

void Server::addChannel(const std::string& channelName)
{
	Channel* newChannel							 = new Channel(channelName);
	std::map<std::string, Channel*>::iterator it = _MapChannels.find(channelName);
	if ( it == _MapChannels.end() )
		_MapChannels[channelName] = newChannel;
}

void Server::removeChannel(const std::string& channelName)
{
	Channel* channel = _MapChannels[channelName];
	_MapChannels.erase(channelName);
	delete channel;
}

Channel* Server::getChannelByName(const std::string& channelName)
{
	std::map<std::string, Channel*>::iterator it = _MapChannels.find(channelName);
	if ( it != _MapChannels.end() )
		return it->second;
	return NULL;
}

void Server::addUserToChannel(std::string channelName, Client* client)
{
	Channel* channel = getChannelByName(channelName);
	if ( channel != NULL )
	{
		channel->addClient(client->getNickname(), client);
		std::map<std::string, Channel*>& channelOfClient = client->getChannels();
		channelOfClient[channel->getName()]				 = channel;
	}
	else
		std::cout << " channel inexistant " << std::endl;
}
