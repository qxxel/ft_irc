/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:39:04 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/28 18:20:22 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"

// SET THE USERNAME OF THE CLIENT
void	Command::handleUser(Client *client, std::vector<std::string> *args)
{
	// IF CLIENT HAS NOT GIVEN THE PASSWORD
	if (!client->getPwd())
		return (Server::sendClient(client->getFd(), ":localhost 904 * :" ENTER_PWD));

	// IF BAD ARGUMENTS
	else if (args->size() < 3 || args->size() > 4 || args->at(1).compare("0") || args->at(2).compare("*"))
		return (Server::sendClient(client->getFd(), ":localhost 461 * :" ENTER_USER));

	// IF CLIENT HAS ALREADY SET A USERNAME
	else if (!client->getUser().empty())
		return (Server::sendClient(client->getFd(), ":localhost 905 * :" CANT_USER USR_NAME + client->getUser() + "\n"));

	else
	{
		// IF GOOD USERNAME
		if (isValidString(args->at(0), false))
		{
			client->setUser(args->at(0));
			Server::sendClient(client->getFd(), USR_NAME + client->getUser() + "\n");
		}
		// IF BAD USERNAME
		else
			return (Server::sendClient(client->getFd(), ":localhost 900 * :" HAS_INVALID_CHARS));
	}

	// IF DONE AUTHENTIFIED
	if (!client->getNick().empty() && !client->getUser().empty() && client->getPwd())
		client->setAuth(true);

	// IF NOT DONE AUTHENTIFIED
	else if (client->getNick().empty())
		return (Server::sendClient(client->getFd(), ENTER_NICK));
}
