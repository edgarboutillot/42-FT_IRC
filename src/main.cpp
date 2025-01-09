/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbacquet <cbacquet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 15:17:04 by cbacquet          #+#    #+#             */
/*   Updated: 2024/05/30 17:55:02 by cbacquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "server.hpp"

int main(int argc, char** argv)
{
	if ( argc != 3 )
	{
		std::cerr << "the program needs 2 arguments. usage should be: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	char *endPtr;
	errno = 0;
	unsigned long port = std::strtol(argv[1], &endPtr, 10);
	if ((errno == ERANGE && (port == _SC_ULONG_MAX)) || (endPtr == argv[1]) || (*endPtr != '\0'))
	{
		std::cerr << "usage should be: ./ircserv <port> <password> and <port> should only take numbers" << std::endl;
		return 1;
	}
	if (port == 0 || port > 65535)
	{
		std::cerr << "port number should be between 1 and 65535" << std::endl;
		return 1;
	}
	try
	{
		Server serv(port, std::string(argv[2]));
		serv.serverInit();
	}
	catch ( const std::exception& e )
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
