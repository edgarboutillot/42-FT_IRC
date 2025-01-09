//
// Created by dinho on 3/27/24.
//

#ifndef INCLUDE_HPP
#define INCLUDE_HPP

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <list>
#include <map>
#include <netinet/in.h>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"
#define BLU "\e[1;34m"

#define CRLF "\r\n"
#define RPL_WELCOME(nickname) (": 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + CRLF)

// PASS MESSAGE
#define ERR_NEEDMOREPARAMS(nickname, command) (": 461 " + nickname + " " + command + " :Not enough parameters" + CRLF)
#define ERR_ALREADYREGISTRED(nickname) (": 462 " + nickname + " :You may not reregister" + CRLF)
#define ERR_PASSWDMISMATCH(nickname) (": 464 " + nickname + " :Password incorrect" + CRLF)

// NICK MESSAGE
#define RPL_NICKCHANGE(oldNickname, newNickname) (":" + oldNickname + " NICK :" + newNickname + "\r\n")
#define RPL_NICK(nickname) (": 001 " + nickname + " :Your nickname is now " + nickname + CRLF)
#define ERR_NONICKNAMEGIVEN(nickname) (": 431 " + nickname + " :No nickname given" + CRLF)
#define ERR_ERRONEUSNICKNAME(nickname) (": 432 " + nickname + " :Erroneus nickname" + CRLF)
#define ERR_NICKNAMEINUSE(nickname) (": 433 " + nickname + " :Nickname is already in use" + CRLF)
#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " :You have not registered" + CRLF)

// CHANNEL MESSAGE
#define JOIN_CHANNEL(channel, nickname) (":" + nickname + " JOIN " + channel + CRLF)
#define RPL_NOTOPIC(client, channel) (": 331 " + client + " " + channel + " :No topic is set" + CRLF)
#define RPL_TOPIC(client, channel, topic) (": 332 " + client + " " + channel + " :" + topic + CRLF)
#define RPL_NAMREPLY(channel, nickname, names) (": 353 " + nickname + " = " + channel + " : " + names + CRLF)
#define RPL_ENDOFNAMES(nickname, channel) (": 366 " + nickname + " " + channel + " :End of /NAMES list" + CRLF)

// OPER MESSAGE
#define ERR_NOOPERHOST(nickname) (": 491 " + nickname + " :No O-lines for your host" + CRLF)
#define RPL_YOUREOPER(nickname) (": 381 " + nickname + " :You are now an IRC operator" + CRLF)

// JOIN
#define ERR_CHANNELISFULL(nickname, channel) (": 471 " + nickname + " " + channel + " :Cannot join channel (+l)" + CRLF)
#define RPL_CHANNELMODEIS(client, channel, arg) (": 324 " + client + " " + channel + " " + arg + CRLF)

// PRIVMSG
#define ERR_NOSUCHNICK(nickname) (": 401 " + nickname + " :No such nick/channel" + CRLF)
#define ERR_NORECIPIENT(client) (": 411 " + client + ": No recipient given" + CRLF)
#define ERR_NOTEXTTOSEND(client, command) (": 412 " + client + ": No text to send" + CRLF)

// KICK
#define ERR_NOSUCHCHANNEL(client, channel) (": 403 " + client + " " + channel + " :No such channel" + CRLF)
#define ERR_CHANOPRIVSNEEDED(client, channel)                                                                          \
	(": 482 " + client + " " + channel + " :You're not channel operator" + CRLF)
#define ERR_USERNOTINCHANNEL(client, nickname, channel)                                                                \
	(": 441 " + client + " " + nickname + " " + channel + " :They aren't on that channel" + CRLF)
#define ERR_NOTONCHANNEL(client, channel) (" :442 " + client + " " + channel + " :You're not on that channel" + CRLF)
#define RPL_KICK(client, channel, nickname, comment)                                                                   \
	(":" + client + " KICK " + channel + " " + nickname + " :" + comment + CRLF)

// INVITE
#define RPL_INIVITING(client, nickname, channel) (": 341 " + client + " " + nickname + " " + channel + CRLF)
#define ERR_USERONCHANNEL(client, nickname, channel)                                                                   \
	(": 443 " + client + " " + nickname + " " + channel + " :is already on channel" + CRLF)

// PART
#define RPL_PART(client, channel, message) (":" + client + " PART " + channel + " :" + message + CRLF)

// mode -i (definir/supprimer le canal sur invitation)
#define ERR_INVITEONLYCHAN(client, channel) (": 473 " + client + " " + channel + " :Cannot join channel (+i)" + CRLF)

// mode -k (Définir/supprimer la clé du canal (mot de passe))
#define ERR_BADCHANNELKEY(client, channel) (": 475 " + client + " " + channel + " :Cannot join channel (+k)" + CRLF)
// mode -o (Donner/retirer le privilège de l’opérateur de canal)

// mode -l (Définir/supprimer la limite d’utilisateurs pour le canal)
#define ERR_UMODEUNKNOWNFLAG(client) (": 501 " + client + " :Unknown mode flag" + CRLF)

// TOPIC
#define RPL_TOPICWHOTIME(nickname, channel, time) (": 333 " + nickname + " " + channel + " " + nickname + " " + time + CRLF)

#endif // INCLUDE_HPP
