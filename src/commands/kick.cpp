
#include "include.hpp"
#include "server.hpp"

void Server::kickUserFromChannel(Channel* channel, Client* client, const std::string& userToKick,
								 const std::string& kickMsg)
{
	std::map<std::string, Client*> MapClient	= channel->getMapClient();
	std::map<std::string, Client*>::iterator it = MapClient.find(userToKick);
	Client* toDel								= MapClient[userToKick];

	if ( channel->isOperator(toDel) )
		channel->removeOperator(toDel);
	if ( channel->isInInviteList(toDel) )
		channel->removeInviteList(toDel->getNickname());
	channel->removeClient(it->second);
	channel->removeClientTime ( it->second );

	std::string kickMessage = RPL_KICK(client->getNickname(), channel->getName(), userToKick, kickMsg);
	queueDataForSend(kickMessage, it->second->getFd());
	queueDataForSend(kickMessage, client->getFd());
	sendChannel(client, channel, kickMessage);

	std::string namesReply = RPL_NAMREPLY(channel->getName(), client->getNickname(), channel->getAllNames());
	queueDataForSend(namesReply, client->getFd());
	sendChannel(client, channel, namesReply);
	queueDataForSend(RPL_ENDOFNAMES(client->getNickname(), channel->getName()), client->getFd());
	sendChannel(client, channel, RPL_ENDOFNAMES(client->getNickname(), channel->getName()));
}

void Server::KICK(std::string& cmd, int client_fd)
{
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	Client* client						 = getClientByFd(client_fd);
	if ( !client->isRegistered() || !client->isLogged() )
	{
		queueDataForSend(ERR_NOTREGISTERED(std::string("*")), client_fd);
		return;
	}
	if ( splited_cmd.size() < 3 )
	{
		queueDataForSend(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK"), client_fd);
		return;
	}
	else
		clientKick(client_fd, splited_cmd[2], splited_cmd[1], splited_cmd);
}

void Server::clientKick(int client_fd, std::string userToKick, std::string channelName,
						std::vector<std::string> splited_cmd)
{
	Channel* channel = getChannelByName(channelName);
	Client* client	 = getClientByFd(client_fd);

	if ( !channel->isOperator(client) )
	{
		queueDataForSend(ERR_CHANOPRIVSNEEDED(client->getUsername(), channelName), client_fd);
		return;
	}
	std::map<std::string, Channel*>::iterator itChan = _MapChannels.find(channelName);
	if ( itChan == _MapChannels.end() )
	{
		queueDataForSend(ERR_NOSUCHCHANNEL(client->getUsername(), channelName), client_fd);
		return;
	}
	if ( !channel->isInChannel(userToKick) )
	{
		queueDataForSend(ERR_USERNOTINCHANNEL(client->getUsername(), client->getNickname(), channelName), client_fd);
		return;
	}
	if ( splited_cmd.size() >= 3 )
	{
		std::string kickMsg;
		if ( splited_cmd.size() > 3 )
		{
			for ( size_t i = 3; i < splited_cmd.size(); ++i )
			{
				if ( i > 3 )
					kickMsg += " ";
				kickMsg += splited_cmd[i];
			}
		}
		kickUserFromChannel(channel, client, userToKick, kickMsg);
		return;
	}
}
