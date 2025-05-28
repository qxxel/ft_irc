/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:38:32 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/28 18:18:34 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"

// UNLOCK AUTHENTICATION WITH PASSWORD
void	Command::handlePass(Server &serv, Client *client, std::vector<std::string> *args)
{
	// IF CLIENT ALREADY ENTERED PWD
	if (!client->getPwd())
	{
		// IF TOO MANY ARGS OR NOT ENOUGH
		if (args->size() != 1)
			return (void)Server::sendClient(client->getFd(), ":localhost 461 * :" INV_FORMAT ENTER_PWD);
		// IF PWD IS GOOD
		else if (Server::simpleHash(args->at(0)) * 2 % 333 / 4 + 5 * 6 == serv.getPwd())
		{
			client->setPwd(true);
			Server::sendClient(client->getFd(), PWD_GOOD);
			if (!client->getAuth() && !client->getNick().empty() && client->getUser().compare(""))
				return (client->setAuth(true));
			else
				return (Server::sendClient(client->getFd(), ENTER_NCK_USR));
		}
		// IF PWD IS WRONG
		else
			return (Server::sendClient(client->getFd(), INV_PWD));
	}
	else
		Server::sendClient(client->getFd(), ":localhost 462 * :" PWD_SET);
}
