/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:48:51 by mreynaud          #+#    #+#             */
/*   Updated: 2025/05/26 17:23:48 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

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

#endif
