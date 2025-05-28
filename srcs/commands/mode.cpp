/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:40:36 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/28 18:41:27 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// VIEW OR SET A MODE IN A CHANNEL
void	Command::handleMode(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle MODE failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: MODE <channel> [<+/-modes>] [<arguments>]
	if (!args || args->size() < 1)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
		std::cout << "handle MODE failed => invalid format" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + args->at(0) + " :No such channel\n");
		std::cout << "handle TOPIC failed => channel don't exist" << std::endl;
		return ;
	}

	// SEND ACTIVE MODES
	if (args->size() == 1)
	{
		std::string	modes = " +";
		std::string	modesArgs;

		if (channel->getInvOnly())
			modes += "i";
		if (channel->getLockTopic())
			modes += "t";
		if (!channel->getPwd().empty())
			modes += "k";
		if (channel->getMaxUsers())
		{
			modes += "l";
			modesArgs += " " + intToString(channel->getMaxUsers());
		}
		if (modes == " +")
			modes = "";

		Server::sendClient(client->getFd(), ":localhost 324 " + client->getNick() + " " + channel->getName() + modes + modesArgs + "\n");
		Server::sendClient(client->getFd(), ":localhost 329 " + client->getNick() + " " + channel->getName() + " " + timeToString(channel->getModeSetTimestamp()) + "\n");
		std::cout << "handle ask MODE successfuly called" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (!client->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), ":localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
		std::cout << "handle TOPIC failed => client isn't in the channel asked" << std::endl;
		return ;
	}

	// CHECK IF CLIENT IS OP
	if (!channel->isOpClient(client->getFd()))
	{
		Server::sendClient(client->getFd(), ":localhost 482 " + client->getNick() + " " + channel->getName() + " :You're not channel operator\n");
		std::cout << "handle modify MODE failed => client isn't moderator" << std::endl;
		return ;
	}


	// ADD INVITE ONLY MODE
	if (args->at(1) == "+i")
	{
		// USAGE: MODE <channel> +i
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE +i failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF INVITE ONLY ISN'T SET
		if (!channel->getInvOnly())
		{
			channel->setInvOnly(true);

			// ADD CHANNEL IN CURRENT CLIENTS' JOINABLE CHANNELS
			for (std::vector<Client*>::iterator it = channel->getClientsList().begin(); it != channel->getClientsList().end(); it++)
				(*it)->getJoinableChannels().push_back(channel);
		}

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " +i\n");
	}
	// SUPP INVITE ONLY MODE
	else if (args->at(1) == "-i")
	{
		// USAGE: MODE <channel> -i
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE -i failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF INVITE ONLY IS SET
		if (channel->getInvOnly())
		{
			channel->setInvOnly(false);

			// ERASE CHANNEL IN ALL CLIENTS' JOINABLE CHANNELS
			for (std::vector<Client*>::iterator it1 = serv.getClients().begin(); it1 != serv.getClients().end(); it1++)
			{
				for (std::vector<Channel*>::iterator it2 = (*it1)->getJoinableChannels().begin(); it2 != (*it1)->getJoinableChannels().end(); it2++)
				{
					if ((*it2) == channel)
					{
						(*it1)->getJoinableChannels().erase(it2);
						break ;
					}
				}
			}
		}

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " -i\n");
	}

	// ADD LOCKED TOPIC MODE
	else if (args->at(1) == "+t")
	{
		// USAGE: MODE <channel> +t
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE +t failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF TOPIC ISN'T LOCK
		if (!channel->getLockTopic())
			channel->setLockTopic(true);

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " +t\n");
	}
	// SUPP LOCKED TOPIC MODE
	else if (args->at(1) == "-t")
	{
		// USAGE: MODE <channel> -t
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE -t failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF TOPIC IS LOCK
		if (channel->getLockTopic())
			channel->setLockTopic(false);

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " -t\n");
	}

	// ADD PASSWORD MODE
	else if (args->at(1) == "+k")
	{
		// USAGE: MODE <channel> +k <new_password>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE +k failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF ALL CHARACTERS ARE VALID
		if (!isValidString(args->at(2), true, false))
		{
			Server::sendClient(client->getFd(), ":localhost 900 " + client->getNick() + " " + channel->getName() + " :invalid characters in password\n");
			std::cout << "handle modify MODE +k failed => invalid password format" << std::endl;
			return ;
		}

		// CHECK IF PASSWORD ISN'T THE SAME
		if (channel->getPwd() != args->at(2))
			channel->setPwd(args->at(2));

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " +k " + args->at(2) + "\n");
	}
	// SUPP PASSWORD MODE
	else if (args->at(1) == "-k")
	{
		// USAGE: MODE <channel> -k <actual_password>
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE -k failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF PASSWORD IS SET
		if (channel->getPwd() != "")
			channel->setPwd("");

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " -k\n");
	}

	// SET OP TARGET
	else if (args->at(1) == "+o")
	{
		// USAGE: MODE <channel> +o <target_username>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE +o failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO FIND TARGET IN CHANNEL
		Client	*target = channel->findClientName(args->at(2));
		if (!target)
		{
			Server::sendClient(client->getFd(), ":localhost 441 " + client->getNick() + " " + args->at(1) + " " + channel->getName() + " :They aren't on that channel\n");
			std::cout << "handle modify MODE +o failed => inexistant target" << std::endl;
			return ;
		}

		// CHECK IF TARGET ISN'T OP
		if (!channel->isOpClient(target->getFd()))
			channel->getOpList().push_back(target);

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " +o " + target->getNick() + "\n");
	}
	// UNSET OP TARGET
	else if (args->at(1) == "-o")
	{
		// USAGE: MODE <channel> -o <target_username>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE -o failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO FIND TARGET IN CHANNEL
		Client	*target = channel->findClientName(args->at(2));
		if (!target)
		{
			Server::sendClient(client->getFd(), ":localhost 441 " + client->getNick() + " " + args->at(1) + " " + channel->getName() + " :They aren't on that channel\n");
			std::cout << "handle modify MODE -o failed => inexistant target" << std::endl;
			return ;
		}

		// CHECK IF TARGET IS OP
		if (channel->isOpClient(target->getFd()))
			channel->delOpClient(target->getFd());

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " -o " + target->getNick() + "\n");
	}

	// ADD USER LIMIT MODE
	else if (args->at(1) == "+l")
	{
		// USAGE: MODE <channel> +l <new_user_limit>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE +l failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO SET THE NEW LIMIT
		try
		{
			channel->setMaxUsers(stringToInt(args->at(2)));
			channel->setModeSetTimestamp(time(NULL));
			channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " +l " + args->at(2) + "\n");
		}
		// IF THE INPUT ISN'T AN INT > 0
		catch (notUnsignedIntNumber &e)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Invalid value\n");
			std::cout << "handle modify MODE +l failed => " << e.what() << std::endl;
			return ;
		}
	}
	// SUPP USER LIMIT MODE
	else if (args->at(1) == "-l")
	{
		// USAGE: MODE <channel> -l
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\n");
			std::cout << "handle modify MODE -l failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF THERE IS NO LIMIT
		if (channel->getMaxUsers() != -1)
			channel->setMaxUsers(-1);

		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE " + channel->getName() + " -l\n");
	}

	// OTHERS USAGES
	else
	{
		Server::sendClient(client->getFd(), ":localhost 472 " + client->getNick() + " " + args->at(1) + " :is unknown mode char to me, only ones are +/-itkol\n");
		std::cout << "handle modify MODE failed => not an existant flag" << std::endl;
		return ;
	}

	std::cout << "handle modify MODE " << args->at(1) << " successfuly called" << std::endl;
}
