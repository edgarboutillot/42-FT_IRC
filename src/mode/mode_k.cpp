//
// Created by hdupuy on 5/21/24.
//

#include "client.hpp"
#include "server.hpp"

void Server::modeK ( char option, std::vector< std::string >& splited_cmd, Client* client, int client_fd,
					 Channel* channel )
{
	std::string channelName = channel->getName ();
	if ( !channel->isOperator ( client ) )
	{
		queueDataForSend ( ERR_CHANOPRIVSNEEDED ( client->getNickname (), channelName ), client_fd );
		return;
	}
	if ( option == '+' )
	{
		if ( splited_cmd.size () < 4 )
		{
			queueDataForSend ( ERR_NEEDMOREPARAMS ( client->getNickname (), "MODE" ), client_fd );
			return;
		}
		if ( channel->alreadySet ( "k" ) )
		{
			std::string msg = "MODE " + channel->getName () + " +k" + " :Channel mode is already set" + CRLF;
			queueDataForSend ( msg, client->getFd () );
			return;
		}
		std::string password = splited_cmd[3];
		channel->setPassword ( password, true );
		std::string modeArgs = channel->addArgToModeVector ( "k" );
		queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channelName, modeArgs ), client_fd );
		sendChannel ( client, channel, RPL_CHANNELMODEIS ( client->getNickname (), channelName, modeArgs ) );
		modeChangeMsgforAll (client, channel->getName(), password, client_fd, channel, "+k");
		return;
	}
	else if ( option == '-' )
	{
		if ( !channel->alreadySet ( "k" ) )
		{
			std::string msg = "MODE " + channel->getName () + " +k" + " :Channel mode is not set" + CRLF;
			queueDataForSend ( msg, client->getFd () );
			return;
		}
		if ( splited_cmd.size () < 3 )
		{
			queueDataForSend ( ERR_NEEDMOREPARAMS ( client->getNickname (), "MODE" ), client_fd );
			return;
		}
		else if ( splited_cmd.size () > 3 )
		{
			std::string replyMessage = "Error: usage is 'mode <channel> -k \r\n";
			send ( client_fd, replyMessage.c_str (), replyMessage.length (), 0 );
			return;
		}
		std::string password = "";
		channel->setPassword ( password, false );
		std::string modeArgs = channel->removeArgToModeVector ( "k" );
		queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channelName, modeArgs ), client_fd );
		sendChannel ( client, channel, RPL_CHANNELMODEIS ( client->getNickname (), channelName, modeArgs ) );
		modeChangeMsgforAll (client, channel->getName(), password, client_fd, channel, "-k");
		return;
	}
}