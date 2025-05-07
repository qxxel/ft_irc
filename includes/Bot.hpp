/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:48:51 by mreynaud          #+#    #+#             */
/*   Updated: 2025/05/07 20:27:32 by agerbaud         ###   ########.fr       */
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

		static void handleBot(Channel *channel, Client *client, std::string &arg);

		void	joinChannel(Channel	*channel);
		// void	kickChanel(Channel	*channel, Client *client, std::vector<std::string> *args);
};
