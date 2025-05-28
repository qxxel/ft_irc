/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:18:48 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/28 18:16:21 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <exception>
# include <iostream>
# include <time.h>
# include <vector>

class Client;
class Server;

class Channel
{
	private:
		std::vector<Client*>	_clientsList;
		std::vector<Client*>	_opList;
		Client					*_oldestClient;
		std::string				_name;
		std::string				_pwd;
		std::string				_topic;
		std::string				_topicSetter;
		time_t					_topicSetTimestamp;
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
		const std::string		&getTopicSetter() const;
		time_t					getTopicSetTimestamp() const;
		time_t					getModeSetTimestamp() const;
		int						getMaxUsers() const;
		bool					getInvOnly() const;
		bool					getLockTopic() const;
		void	setOldestClient(Client *oldestClient);
		void	setName(const std::string &name);
		void	setTopic(const std::string &topic);
		void	setPwd(const std::string &pwd);
		void	setTopicSetter(const std::string &topicSetter);
		void	setTopicSetTimestamp(time_t topicSetTimestamp);
		void	setModeSetTimestamp(time_t modeSetTimestamp);
		void	setMaxUsers(int maxUsers);
		void	setInvOnly(bool invOnly);
		void	setLockTopic(bool lockTopic);


		void		delClient(int fdClient);
		void		delOpClient(int fdClient);
		bool		isOpClient(int fdClient);
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

#endif
