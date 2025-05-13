/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   help.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:40:46 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/13 16:06:27 by agerbaud         ###   ########.fr       */
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
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " HELP :Not enough parameters\n");
		std::cout << "handle HELP failed => wrong format" << std::endl;
		return ;
	}

	// SHOW ALL COMMANDS AND DESCRIPTIONS
	if (args->size() == 0)
	{
		Server::sendClient(client->getFd(), ":localhost 910 " + client->getNick() + " :Available commands: PRIVMSG, JOIN, PART, QUIT, KICK, INVITE, TOPIC, NAMES, WHO, MODE\n");
		Server::sendClient(client->getFd(), ":localhost 911 " + client->getNick() + " :Use /HELP <command> for help on each.\n");
		// Server::sendClient(client->getFd(), ":localhost 919 " + client->getNick() + " :End of HELP\n");
	}

	// SHOW USAGE OF THE ASKED COMMAND
	else
	{
		// IF THE CLIENT WANT HELP WITH PRIVMSG
		if (args->at(0) == "PRIVMSG")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" PRIVMSG_USG " - " PRIVMSG_DESC);

		// IF THE CLIENT WANT HELP WITH JOIN
		else if (args->at(0) == "JOIN")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" JOIN_USG " - " JOIN_DESC);

		// IF THE CLIENT WANT HELP WITH PART
		else if (args->at(0) == "PART")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" PART_USG " - " PART_DESC);

		// IF THE CLIENT WANT HELP WITH QUIT
		else if (args->at(0) == "QUIT")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" QUIT_USG " - " QUIT_DESC);

		// IF THE CLIENT WANT HELP WITH KICK
		else if (args->at(0) == "KICK")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" KICK_USG " - " KICK_DESC);

		// IF THE CLIENT WANT HELP WITH INVITE
		else if (args->at(0) == "INVITE")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" INVITE_USG " - " INVITE_DESC);

		// IF THE CLIENT WANT HELP WITH TOPIC
		else if (args->at(0) == "TOPIC")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" TOPIC_USG " - " TOPIC_DESC);

		// IF THE CLIENT WANT HELP WITH NAMES
		else if (args->at(0) == "NAMES")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" NAMES_USG " - " NAMES_DESC);

		else if (args->at(0) == "WHO")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" WHO_USG " - " WHO_DESC);

		// IF THE CLIENT WANT HELP WITH MODE
		else if (args->at(0) == "MODE")
			Server::sendClient(client->getFd(), ":localhost 915 " + client->getNick() + " :" MODE_USG " - " MODE_DESC);

		// OTHERS INPUT
		else
		{
			Server::sendClient(client->getFd(), NO_CMD);
			std::cout << "handle HELP failed => command not found" << std::endl;
			return ;
		}
		
		// Server::sendClient(client->getFd(), ":localhost 919 " + client->getNick() + " :End of HELP\n");
	}

	std::cout << "handle HELP successfuly called" << std::endl;
}
