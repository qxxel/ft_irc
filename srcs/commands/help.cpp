/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   help.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:40:46 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/07 20:33:31 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"


// DISPLAY ALL COMMANDS USAGES
void	Command::handleHelp(Client *client, std::vector<std::string> *args) // ONLY FOR NC
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle HELP failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: HELP
	if (!args || args->size() >= 2)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " HELP :Not enough parameters");
		std::cout << "handle HELP failed => wrong format" << std::endl;
		return ;
	}

	// SHOW ALL COMMANDS AND DESCRIPTIONS
	if (args->size() == 0)
	{
		Server::sendClient(client->getFd(), "Commands list:\n");
		Server::sendClient(client->getFd(), "PRIVMSG\t\t" PRIVMSG_DESC);
		Server::sendClient(client->getFd(), "JOIN\t\t" JOIN_DESC);
		Server::sendClient(client->getFd(), "PART\t\t" PART_DESC);
		Server::sendClient(client->getFd(), "QUIT\t\t" QUIT_DESC);
		Server::sendClient(client->getFd(), "KICK\t\t" KICK_DESC);
		Server::sendClient(client->getFd(), "INVITE\t\t" INV_DESC);
		Server::sendClient(client->getFd(), "TOPIC\t\t" TPC_DESC);
		Server::sendClient(client->getFd(), "NAMES\t\t" NAMES_DESC);
		Server::sendClient(client->getFd(), "MODE\t\t" MODE_DESC);
	}
	// SHOW USAGE OF THE ASKED COMMAND
	else
	{
		// IF THE CLIENT WANT HELP WITH PRIVMSG
		if (args->at(0) == "PRIVMSG")
		{
			Server::sendClient(client->getFd(), "PRIVMSG:\n");
			Server::sendClient(client->getFd(), PRIVMSG_USG);
			Server::sendClient(client->getFd(), PRIVMSG_DESC);
		}

		// IF THE CLIENT WANT HELP WITH JOIN
		else if (args->at(0) == "JOIN")
		{
			Server::sendClient(client->getFd(), "JOIN:\n");
			Server::sendClient(client->getFd(), JOIN_USG);
			Server::sendClient(client->getFd(), JOIN_DESC);
		}

		// IF THE CLIENT WANT HELP WITH PART
		else if (args->at(0) == "PART")
		{
			Server::sendClient(client->getFd(), "PART:\n");
			Server::sendClient(client->getFd(), PART_USG);
			Server::sendClient(client->getFd(), PART_DESC);
		}

		// IF THE CLIENT WANT HELP WITH QUIT
		else if (args->at(0) == "QUIT")
		{
			Server::sendClient(client->getFd(), "QUIT:\n");
			Server::sendClient(client->getFd(), QUIT_USG);
			Server::sendClient(client->getFd(), QUIT_DESC);
		}

		// IF THE CLIENT WANT HELP WITH KICK
		else if (args->at(0) == "KICK")
		{
			Server::sendClient(client->getFd(), "KICK:\n");
			Server::sendClient(client->getFd(), KICK_USG);
			Server::sendClient(client->getFd(), KICK_DESC);
		}

		// IF THE CLIENT WANT HELP WITH INVITE
		else if (args->at(0) == "INVITE")
		{
			Server::sendClient(client->getFd(), "INVITE:\n");
			Server::sendClient(client->getFd(), INV_USG);
			Server::sendClient(client->getFd(), INV_DESC);
		}

		// IF THE CLIENT WANT HELP WITH TOPIC
		else if (args->at(0) == "TOPIC")
		{
			Server::sendClient(client->getFd(), "TOPIC:\n");
			Server::sendClient(client->getFd(), TPC_USG);
			Server::sendClient(client->getFd(), TPC_DESC);
		}

		// IF THE CLIENT WANT HELP WITH NAMES
		else if (args->at(0) == "NAMES")
		{
			Server::sendClient(client->getFd(), "NAMES:\n");
			Server::sendClient(client->getFd(), NAMES_USG);
			Server::sendClient(client->getFd(), NAMES_DESC);
		}

		// IF THE CLIENT WANT HELP WITH MODE
		else if (args->at(0) == "MODE")
		{
			Server::sendClient(client->getFd(), "MODE:\n");
			Server::sendClient(client->getFd(), MODE_USG);
			Server::sendClient(client->getFd(), MODE_DESC);
		}

		// OTHERS INPUT
		else
		{
			Server::sendClient(client->getFd(), NO_CMD);
			std::cout << "handle HELP failed => command not found" << std::endl;
			return ;
		}
	}

	std::cout << "handle HELP successfuly called" << std::endl;
}
