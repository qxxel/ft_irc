/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:38:57 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/08 16:23:35 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"

// SET THE NICKNAME OF THE CLIENT
void	Command::handleNick(Server &serv, Client *client, std::vector<std::string> *args)
{
	// IF CLIENT HAS NOT GIVEN THE PASSWORD
	if (!client->getPwd())
		return (void)Server::sendClient(client->getFd(), ":localhost 904 * :" ENTER_PWD);

	// IF CLIENT HAS ALREADY SET A NICKNAME
	else if (client->getNick().compare(""))
	{
		if (client->getUser().empty())
			Server::sendClient(client->getFd(), ":localhost 905 * :" CANT_NICK);
		else
			Server::sendClient(client->getFd(), ":localhost 905 * :" CANT_NICK);
		return ;
	}

	// IF BAD ARGUMENTS NUMBER
	else if (args->size() != 1)
		return (void)Server::sendClient(client->getFd(), ":localhost 461 * :" ENTER_NICK);

	else
	{
		// IF GOOD NICKNAME
		if (isValidString(args->at(0), false))
		{
			// IF AVAILABLE USERNAME
			if (is_available(serv, args->at(0)))
			{
				client->setNick(args->at(0));
				Server::sendClient(client->getFd(), NICK_NAME + client->getNick() + "\n");
			}

			// IF NICKNAME IS TAKEN
			else
				return (Server::sendClient(client->getFd(), ":localhost 908 * :" IS_TAKEN ENTER_NICK));
		}
		// IF BAD NICKNAME
		else
			Server::sendClient(client->getFd(), ":localhost 900 * :" HAS_INVALID_CHARS);
	}

	// SETS AUTH TO TRUE IF AUTH FINISHED
	if (!client->getNick().empty() && !client->getUser().empty() && client->getPwd())
		client->setAuth(true);

	// TELLS THE CLIENTS TO SET A USERNAME
	else if (!client->getUser().compare(""))
		return (Server::sendClient(client->getFd(), ENTER_USER));
}
