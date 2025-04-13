/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:18:48 by agerbaud          #+#    #+#             */
/*   Updated: 2025/04/13 18:46:36 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <exception>
# include <iostream>
# include <map>
# include "Client.hpp"
# include "Server.hpp"

class Client;
class Server;

class Channel
{
	private:
		std::vector<Client*>	_clientsList;
		std::vector<Client*>	_opList;
		Client					*_oldestClient;
		std::string				_name;
		std::string				_topic;
		std::string				_pwd;
		int						_maxUsers;
		bool					_invOnly;
		bool					_lockTopic;

	public:
		Channel(std::string name, Client *creator);
		~Channel();

		// GETTERS AND SETTERS
		std::vector<Client*>	getClientsList() const;
		std::vector<Client*>	getOpList() const;
		Client					*getOldestClient() const;
		const std::string		&getName() const;
		const std::string		&getTopic() const;
		const std::string		&getPwd() const;
		int						getMaxUsers() const;
		bool					getInvOnly() const;
		bool					getLockTopic() const;
		void	setOldestClient(Client *oldestClient);
		void	setName(const std::string &name);
		void	setTopic(const std::string &topic);
		void	setPwd(const std::string &pwd);
		void	setMaxUsers(int maxUsers);
		void	setInvOnly(bool invOnly);
		void	setLockTopic(bool lockTopic);

		void	parseName(std::string name) const;
		Client	*findClientName(std::string name);
		void	delClientName(std::string name);
		void	delOpName(std::string name);
		bool	isOpName(std::string name);
		void	sendClients(std::string message);

		class	NameIsntValid: public std::exception
		{
			public:
				const char*	what() const throw() { return ("the name isn't valid"); }
		};

};
