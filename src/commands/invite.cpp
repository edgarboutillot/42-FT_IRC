#include "include.hpp"
#include "server.hpp"

void Server::INVITE(std::vector<std::string> splited_cmd, int client_fd)
{
	Client* client = getClientByFd(client_fd);
	if ( !client->isRegistered() || !client->isLogged() )
	{
		queueDataForSend(ERR_NOTREGISTERED(std::string("*")), client_fd);
		return;
	}
	if ( splited_cmd.size() < 3 )
	{
		queueDataForSend(ERR_NEEDMOREPARAMS(std::string("*"), std::string("INVITE")), client_fd);
		return;
	}
	else if ( splited_cmd.size() > 3 )
	{
		std::cout << "Error: too much params, usage is INVITE <nickname> <channel>" << std::endl;
		return;
	}
	else
	{
		std::string channelName	 = splited_cmd[2];
		std::string userToInvite = splited_cmd[1];

		Client* client = getClientByFd(client_fd);
		if ( !client->isRegistered() || !client->isLogged() )
		{
			queueDataForSend(ERR_NOTREGISTERED(std::string("*")), client_fd);
			return;
		}
		invite(client, channelName, userToInvite);
	}
}

void Server::invite(Client* client, std::string channelName, std::string userToInvite)
{
	std::map<std::string, Channel*>::iterator itChan = _MapChannels.find(channelName);
	if ( itChan == _MapChannels.end() )
	{
		queueDataForSend(ERR_NOSUCHCHANNEL(client->getNickname(), channelName), client->getFd());
		return;
	}
	Channel* channel								  = getChannelByName(channelName);
	std::map<std::string, Client*> clientOfChan		  = channel->getMapClient();
	std::map<std::string, Client*>::iterator itClient = clientOfChan.find(client->getNickname());
	if ( itClient == clientOfChan.end() )
	{
		queueDataForSend(ERR_NOTONCHANNEL(client->getNickname(), channelName), client->getFd());
		return;
	}

	std::map<int, Client*>::iterator it = _clients.begin();
	for ( ; it != _clients.end(); ++it )
		if ( it->second->getNickname() == userToInvite )
			break;
	if ( channel->isInChannel(userToInvite) )
	{
		queueDataForSend(ERR_USERONCHANNEL(client->getNickname(), userToInvite, channelName), client->getFd());
		return;
	}
	if ( !channel->isOperator(client) )
	{
		queueDataForSend(ERR_CHANOPRIVSNEEDED(client->getNickname(), channelName), client->getFd());
		return;
	}
	else
	{
		for ( std::map<int, Client*>::iterator itClient = _clients.begin(); itClient != _clients.end(); ++itClient )
		{
			if ( itClient->second->getNickname() == userToInvite )
			{
				Client* userInvited		  = itClient->second;
				std::string inviteMessage = client->getNickname() + " invite you in channel : " + channelName + CRLF;
				send(userInvited->getFd(), inviteMessage.c_str(), inviteMessage.length(), 0);
				queueDataForSend(RPL_INIVITING(client->getNickname(), userInvited->getNickname(), channelName),
								 client->getFd());
				channel->addClientToInviteList(userInvited);
				return;
			}
		}
	}
}