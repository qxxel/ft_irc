/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:39:54 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/13 16:11:05 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// LEAVE SERVER
void	Command::handleQuit(Server &serv, Client *client, std::vector<std::string> *args, int epfd)
{
	// USAGE: QUIT [:<message>]
	if (!args || args->size() > 1)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " QUIT :Not enough parameters\n");
		std::cout << "handle QUIT failed => wrong format" << std::endl;
		return ;
	}

	// SET MESSAGE TO SEND
	std::string	message = ":Client quit";
	if (args->size() == 1 && args->at(0).size() > 1)
	{
		if (isValidString(args->at(0), true))
			message = args->at(0);
	}

	// SEND MESSAGE TO EVERY CURRENT CHANNEL
	for (std::vector<Channel*>::iterator it = client->getCurrentsChannels().begin(); it != client->getCurrentsChannels().end(); ++it)
			(*it)->sendClients(client->getNick(), ":" + client->getNick() + "!" + client->getUser() + "@localhost QUIT " + message + "\n");

	serv.disconnectClient(client->getFd(), epfd);
	std::cout << "handle QUIT successfuly called" << std::endl;
}
