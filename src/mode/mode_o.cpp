//
// Created by dupuy on 22/05/24.
//

#include "client.hpp"
#include "server.hpp"

void Server::modeChangeMsgforAll ( Client* client, std::string channelName, std::string modeArgs, int client_fd,
								   Channel* channel, std::string mode )
{
	queueDataForSend ( ":" + client->getNickname () + " MODE " + channelName + " " + mode + " " + modeArgs + CRLF,
					   client_fd );
	sendChannel ( client, channel,
				  ":" + client->getNickname () + " MODE " + channelName + " " + mode + " " + modeArgs + CRLF );
}

void Server::modeO ( char option, std::string target, std::vector< std::string > splited_cmd, Channel* channel,
					 Client* client, int client_fd )
{
	size_t cmdSize			= splited_cmd.size ();
	std::string channelName = channel->getName ();

	if ( !channel->isOperator ( client ) )
	{
		queueDataForSend ( ERR_CHANOPRIVSNEEDED ( client->getNickname (), channelName ), client_fd );
		return;
	}
	if ( option == '+' )
	{
		if ( !target.empty () )
		{
			Channel* channel = getChannelByName ( channelName );
			if ( !channel )
			{
				queueDataForSend ( ERR_NOSUCHCHANNEL ( client->getNickname (), channelName ), client_fd );
				return;
			}

			std::map< std::string, Client* > MapClientofChan = channel->getMapClient ();
			std::map< std::string, Client* >::iterator it	 = MapClientofChan.find ( target );
			if ( !channel->isInChannel ( target ) )
			{
				queueDataForSend ( ERR_USERNOTINCHANNEL ( client->getNickname (), target, channelName ), client_fd );
				return;
			}

			OperTime VectorOperator = channel->getOperator ();
			for ( OperTime::iterator itOp = VectorOperator.begin (); itOp != VectorOperator.end (); ++itOp )
			{
				if ( itOp->first->getNickname () == target )
				{
					std::string msg2 = ": 482 " + target + " " + channel->getName () +
									   " :User is already an operator on the channel" +
									   CRLF; // Marche parfaitement pour envoyer sur le channel
					send ( client_fd, msg2.c_str (), msg2.length (), 0 );
					return;
				}
			}
			channel->setChannelOperator ( it->second );
			std::string modeArgs	  = channel->addArgToModeVector ( "o" );
			std::string channelModeIs = RPL_CHANNELMODEIS ( client->getNickname (), channel->getName (), modeArgs );
			queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channelName, modeArgs ), client_fd );
			sendChannel ( client, channel, channelModeIs );
			queueDataForSend ( RPL_NAMREPLY ( channel->getName (), client->getNickname (), channel->getAllNames () ),
							   client_fd );
			queueDataForSend ( RPL_ENDOFNAMES ( client->getNickname (), channel->getName () ), client_fd );
			sendChannel ( client, channel,
						  RPL_NAMREPLY ( channel->getName (), client->getNickname (), channel->getAllNames () ) );
			queueDataForSend ( RPL_ENDOFNAMES ( client->getNickname (), channel->getName () ), client_fd );
			modeChangeMsgforAll ( client, channelName, target, client_fd, channel, "+o" );
			return;
		}
		else
		{
			queueDataForSend ( ERR_NEEDMOREPARAMS ( client->getNickname (), "mode +o" ), client_fd );
			return;
		}
	}
	else if ( option == '-' )
	{
		if ( cmdSize == 4 )
		{
			std::string channelName					 = splited_cmd[1];
			std::string target						 = splited_cmd[3];
			std::map< std::string, Client* > clients = channel->getMapClient ();
			Client* toDel							 = clients[target];

			Channel* channel = getChannelByName ( channelName );
			if ( !channel )
			{
				queueDataForSend ( ERR_NOSUCHCHANNEL ( client->getNickname (), channelName ), client_fd );
				return;
			}

			std::cout << target << std::endl;
			if ( !channel->isInChannel ( target ) )
			{
				queueDataForSend ( ERR_USERNOTINCHANNEL ( client->getNickname (), target, channelName ), client_fd );
				return;
			}

			channel->removeOperator ( toDel );
			std::string modeArgs	  = channel->removeArgToModeVector ( "o" );
			std::string channelModeIs = RPL_CHANNELMODEIS ( client->getNickname (), channel->getName (), modeArgs );
			reply ( channelModeIs, client->getFd () );
			sendChannel ( client, channel, channelModeIs );
			queueDataForSend ( RPL_NAMREPLY ( channel->getName (), client->getNickname (), channel->getAllNames () ),
							   client_fd );
			sendChannel ( client, channel,
						  RPL_NAMREPLY ( channel->getName (), client->getNickname (), channel->getAllNames () ) );
			queueDataForSend ( RPL_ENDOFNAMES ( client->getNickname (), channel->getName () ), client_fd );
			modeChangeMsgforAll ( client, channelName, target, client_fd, channel, "-o" );
			return;
		}
		else
		{
			queueDataForSend ( ERR_NEEDMOREPARAMS ( client->getNickname (), "mode -o <target>" ), client_fd );
			return;
		}
	}
}
