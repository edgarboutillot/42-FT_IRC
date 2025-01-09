//
// Created by hdupuy on 5/21/24.
//

#include "client.hpp"
#include "server.hpp"

void Server::modeT ( char option, std::vector< std::string > splited_cmd, Client* client, int client_fd,
					 Channel* channel )
{
	std::string channelName = channel->getName ();
	if ( splited_cmd.size () < 3 )
	{
		queueDataForSend ( ERR_NEEDMOREPARAMS ( client->getNickname (), "MODE" ), client_fd );
		return;
	}
	if ( option == '+' )
	{
		if ( !channel->isOperator ( client ) )
		{
			queueDataForSend ( ERR_CHANOPRIVSNEEDED ( client->getNickname (), channelName ), client_fd );
			return;
		}
		if ( channel->alreadySet ( "t" ) )
		{
			std::string msg = "MODE " + channel->getName () + " +t" + " :Channel mode is already set" + CRLF;
			queueDataForSend ( msg, client->getFd () );
			return;
		}
		else
		{
			channel->setTopicProtected ( true ); // gerer topic en fonction
			std::string modeArgs = channel->addArgToModeVector ( "t" );
			queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channelName, modeArgs ), client_fd );
			sendChannel ( client, channel,
						  RPL_CHANNELMODEIS ( client->getNickname (), channelName, channel->getModeArgs () ) );
			modeChangeMsgforAll (client, channel->getName(), "", client_fd, channel, "+t");
			return;
		}
	}
	else if ( option == '-' )
	{
		if ( !channel->isOperator ( client ) )
		{
			queueDataForSend ( ERR_CHANOPRIVSNEEDED ( client->getNickname (), channelName ), client_fd );
			return;
		}
		if ( !channel->alreadySet ( "t" ) )
		{
			std::string msg = "MODE " + channel->getName () + " +t" + " :Channel mode is not set" + CRLF;
			queueDataForSend ( msg, client->getFd () );
			return;
		}
		else
		{
			channel->setTopicProtected ( false );
			std::string modeArgs = channel->removeArgToModeVector ( "t" );
			queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channelName, modeArgs ), client_fd );
			sendChannel ( client, channel,
						  RPL_CHANNELMODEIS ( client->getNickname (), channelName, channel->getModeArgs () ) );
			modeChangeMsgforAll (client, channel->getName(), "", client_fd, channel, "-t");
			return;
		}
	}
}
