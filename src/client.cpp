/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbacquet <cbacquet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 15:17:15 by cbacquet          #+#    #+#             */
/*   Updated: 2024/06/04 17:51:16 by cbacquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

/**
 * @brief Default constructor for the Client class.
 *
 * Initializes the client with default values.
 */
Client::Client() : _logged(false), _operator(false), _registered(false), _fd(), _nickname(), _realname(), _username() {}

/**
 * @brief Parameterized constructor for the Client class.
 *
 * Initializes the client with a given file descriptor and default values for other attributes.
 * @param fd The file descriptor for the client.
 */
Client::Client(int fd)
	: _logged(false), _operator(false), _registered(false), _fd(fd), _nickname(), _realname(), _username()
{
	memset(&_clientEpollevent, 0, sizeof(struct epoll_event));
}

/**
 * @brief Destructor for the Client class.
 */
Client::~Client() 
{
	
}

/**
 * @brief Updates the client's status.
 *
 * Adds the client's file descriptor to the epoll instance with the specified events.
 * @param epollFd The file descriptor for the epoll instance.
 * @param events The events to be associated with the client's file descriptor.
 */
void Client::updateClientStatus(const int& epollFd, uint32_t events)
{
	_clientEpollevent.events  = events;
	_clientEpollevent.data.fd = this->_fd;
	if ( epoll_ctl(epollFd, EPOLL_CTL_ADD, _fd, &_clientEpollevent) == -1 )
	{
		std::cout << RED;
		close(_fd);
		throw std::runtime_error("Error during adding the file descriptor to the epoll instance");
	}
}

/**
 * @brief Gets the client's file descriptor.
 *
 * @return The client's file descriptor.
 */
int &Client::getFd()
{
	return _fd;
}

/**
 * @brief Checks if the client is registered.
 *
 * @return True if the client is registered, false otherwise.
 */
bool Client::isRegistered() const
{
	return _registered;
}

/**
 * @brief Checks if the client is logged in.
 *
 * @return True if the client is logged in, false otherwise.
 */
bool Client::isLogged() const
{
	return _logged;
}

/**
 * @brief Checks if the client is an operator.
 *
 * @return True if the client is an operator, false otherwise.
 */
bool Client::isOperator() const
{
	return _operator;
}

/**
 * @brief Gets the client's nickname.
 *
 * @return The client's nickname.
 */
const std::string& Client::getNickname() const
{
	return _nickname;
}

/**
 * @brief Gets the client's username.
 *
 * @return The client's username.
 */
const std::string& Client::getUsername() const
{
	return _username;
}

/**
 * @brief Gets the client's real name.
 *
 * @return The client's real name.
 */
const std::string& Client::getRealname() const
{
	return _realname;
}

/**
 * @brief Gets the channels the client is connected to.
 *
 * @return A map of the channels the client is connected to.
 */
std::map<std::string, Channel*>& Client::getChannels()
{
	return _MapChannels;
}

/**
 * @brief Sets the client's registered status.
 *
 * @param registered The registered status to be set for the client.
 */
void Client::setRegistered(bool registered)
{
	_registered = registered;
}

/**
 * @brief Sets the client's logged in status.
 *
 * @param logged The logged in status to be set for the client.
 */
void Client::setLogged(bool logged)
{
	_logged = logged;
}

/**
 * @brief Sets the client's operator status.
 *
 * @param isOperator The operator status to be set for the client.
 */
void Client::setOperator(bool isOperator)
{
	_operator = isOperator;
}

/**
 * @brief Sets the client's nickname.
 *
 * @param nickname The nickname to be set for the client.
 */
void Client::setNickname(const std::string& nickname)
{
	_nickname = nickname;
}

/**
 * @brief Sets the client's username.
 *
 * @param username The username to be set for the client.
 */
void Client::setUsername(const std::string& username)
{
	_username = username;
}

/**
 * @brief Sets the client's real name.
 *
 * @param realname The real name to be set for the client.
 */
void Client::setRealname(const std::string& realname)
{
	_realname = realname;
}

void Client::removeChannelClient(const std::string & channelName)
{
	_MapChannels.erase(channelName);
}