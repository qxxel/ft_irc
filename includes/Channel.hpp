/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:18:48 by agerbaud          #+#    #+#             */
/*   Updated: 2025/04/10 20:34:38 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <exception>
# include <iostream>
# include <map>
# include "Client.hpp"


class Channel
{
	private:
		std::map<Client*, bool>	_clientsList;
		Client					*_oldestClient;
		std::string				_name;
		std::string				_topic;
		std::string				_pwd;
		int						_maxUsers;
		bool					_invOnly;

	public:
		Channel(std::string name, Client *creator);
		~Channel();

		// GETTERS AND SETTERS
		std::map<Client*, bool>	getClientsList() const;
		Client					*getOldestClient() const;
		const std::string		&getName() const;
		const std::string		&getTopic() const;
		const std::string		&getPwd() const;
		int						getMaxUsers() const;
		bool					getInvOnly() const;
		void	setOldestClient(Client *oldestClient);
		void	setName(const std::string &name);
		void	setTopic(const std::string &topic);
		void	setPwd(const std::string &pwd);
		void	setMaxUsers(int maxUsers);
		void	setInvOnly(bool invOnly);

		void		parseName(std::string name) const throw();
		const bool	isValidChar(char c) const;

		class	NameIsntValid: public std::exception
		{
			public:
				const char*	what() { return ("the name isn't valid"); };
		};

};