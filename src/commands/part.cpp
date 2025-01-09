//
// Created by dupuy on 17/05/24.
//

#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

void Server::PART(std::string cmd, int client_fd)
{
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	if ( splited_cmd.size() < 2 )
	{
		queueDataForSend(ERR_NEEDMOREPARAMS(std::string("*"), std::string(" PART")), client_fd);
		return;
	}
	clientPart(client_fd, cmd);
}

std::string Server::trim(const std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	if ( std::string::npos == first )
		return str;
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

void Server::clientPart(int client_fd, const std::string& cmd)
{
	Client* client = getClientByFd(client_fd);
	if ( !client->isRegistered() || !client->isLogged() )
	{
		queueDataForSend(ERR_NOTREGISTERED(std::string("*")), client_fd);
		return;
	}

	std::vector<std::string> channels;
	std::string new_cmd		   = cmd.substr(cmd.find_first_of(' ') + 1);
	std::string message		   = "";
	std::string::size_type pos = new_cmd.find_first_of(':');
	if ( pos != std::string::npos )
	{
		message = new_cmd.substr(pos + 1);
		new_cmd = new_cmd.substr(0, pos);
	}
	extractChannel(new_cmd, channels);

	for ( size_t i = 0; i < channels.size(); ++i )
	{
		channels[i]		 = trim(channels[i]);
		Channel* channel = getChannelByName(channels[i]);
		if ( channel == NULL )
		{
			queueDataForSend(ERR_NOSUCHCHANNEL(client->getNickname(), channels[i]), client_fd);
			continue;
		}
		if ( !channel->isInChanByFd(client_fd) )
		{
			queueDataForSend(ERR_NOTONCHANNEL(client->getNickname(), channels[i]), client_fd);
			continue;
		}

		std::string partMessage = RPL_PART(client->getNickname(), channel->getName(), message);
		queueDataForSend(partMessage, client_fd);

		std::map<std::string, Client*> MapClient = channel->getMapClient();
		for ( std::map<std::string, Client*>::iterator it = MapClient.begin(); it != MapClient.end(); ++it )
		{
			if ( it->second )
			{
				if ( it->second->getFd() != client_fd )
					queueDataForSend(partMessage, it->second->getFd());
			}
		}

		if ( channel->isInInviteList(client) )
			channel->removeInviteList(client->getNickname());
		if ( channel->isOperator(client) )
			channel->removeOperator(client);
		channel->removeClientTime(client);
		channel->removeClient(client);
		channel->setOlderClientAsOperator();
		client->removeChannelClient(channel->getName());

		if ( MapClient.size() == 1 )
		{
			removeChannel(channel->getName());
		}
		else
		{
			std::string namesReply = RPL_NAMREPLY(channel->getName(), client->getNickname(), channel->getAllNames());
			for ( std::map<std::string, Client*>::iterator it = MapClient.begin(); it != MapClient.end(); ++it )
			{
				if ( it->second )
				{
					if ( it->second->getFd() != client_fd )
					{
						queueDataForSend(namesReply, it->second->getFd());
						queueDataForSend(RPL_ENDOFNAMES(client->getNickname(), channel->getName()),
										 it->second->getFd());
					}
				}
			}
		}
	}
}
