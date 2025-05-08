/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:48:51 by mreynaud          #+#    #+#             */
/*   Updated: 2025/05/08 15:50:09 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <cstring>
# include <cerrno>
# include <fcntl.h>
# include <stdlib.h>
# include <time.h>
# include "Client.hpp"

class Channel;
class Client;
class Server;

class Bot: public Client
{
	private:

	public:
		Bot();
		~Bot();

		static void handleBot(Server &serv, Channel *channel, Client *client, std::string &arg);

		void	joinChannel(Server &serv, Channel *channel);
};
