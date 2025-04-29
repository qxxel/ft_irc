/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:48:51 by mreynaud          #+#    #+#             */
/*   Updated: 2025/04/29 16:30:43 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

class Bot : public Client
{
	private:
		bool rand;

	public:
		Bot(int fd);
		~Bot();

	static void handleBot(Server &serv, Client *client, std::vector<std::string> *args);
};
