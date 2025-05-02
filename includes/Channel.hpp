/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:18:48 by agerbaud          #+#    #+#             */
/*   Updated: 2025/04/30 14:02:11 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <map>
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
		time_t					_modeSetTimestamp;
		int						_maxUsers;
		bool					_invOnly;
		bool					_lockTopic;

	public:
		Channel(std::string name, Client *creator);
		~Channel();

		// GETTERS AND SETTERS
		std::vector<Client*>	&getClientsList();
		std::vector<Client*>	&getOpList();
		const std::string		&getName() const;
		const std::string		&getTopic() const;
		const std::string		&getPwd() const;
		time_t					getModeSetTimestamp() const;
		int						getMaxUsers() const;
		bool					getInvOnly() const;
		bool					getLockTopic() const;
		void	setOldestClient(Client *oldestClient);
		void	setName(const std::string &name);
		void	setTopic(const std::string &topic);
		void	setPwd(const std::string &pwd);
		void	setModeSetTimestamp(time_t modeSetTimestamp);
		void	setMaxUsers(int maxUsers);
		void	setInvOnly(bool invOnly);
		void	setLockTopic(bool lockTopic);


		void		parseName(std::string name) const;
		void		delClientName(std::string name);
		void		delOpName(std::string name);
		bool		isOpName(std::string name);
		void		sendClients(std::string exceptionName, std::string message);
		Client		*getOldestClient();
		Client		*findClientName(std::string name);
		std::string	listClients();

		class	NameIsntValid: public std::exception
		{
			public:
				const char*	what() const throw() { return ("the name isn't valid"); }
		};

};
