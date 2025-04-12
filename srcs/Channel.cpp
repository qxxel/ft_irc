/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:18:05 by agerbaud          #+#    #+#             */
/*   Updated: 2025/04/12 17:35:05 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string name, Client *creator): _oldestClient(NULL), _maxUsers(100), _invOnly(false)
{
	this->parseName(name);
	this->_name = name;
	this->_clientsList.insert(std::make_pair(creator, true));
}

Channel::~Channel() { }

void	Channel::parseName(std::string name) const
{
	if (name.size() < 2 || 50 < name.size() || name[0] != '#')
		throw NameIsntValid();

	for (std::string::iterator it = name.begin(); it < name.end(); it++)
	{
		if (!Server::isValidChar(*it))
			throw NameIsntValid();
	}
}

Client	*Channel::findClientName(std::string name)
{
	std::map<Client*, bool>::iterator	it;

	for (it = this->_clientsList.begin(); it != this->_clientsList.end(); it++)
	{
		if ((*it).first->getUser() == name)
			return ((*it).first);
	}
	return (NULL);
}

// ---------------------------------------------CHANNEL SETTERS AND GETTERS---------------------------------------------

std::map<Client*, bool>	Channel::getClientsList() const
{
	return (this->_clientsList);
}

Client	*Channel::getOldestClient() const
{
	return (this->_oldestClient);
}

const std::string	&Channel::getName() const
{
	return (this->_name);
}

const std::string	&Channel::getTopic() const
{
	return (this->_topic);
}

const std::string	&Channel::getPwd() const
{
	return (this->_pwd);
}

int	Channel::getMaxUsers() const
{
	return (this->_maxUsers);
}

bool	Channel::getInvOnly() const
{
	return (this->_invOnly);
}

bool	Channel::getLockTopic() const
{
	return (this->_lockTopic);
}

void	Channel::setOldestClient(Client *oldestClient)
{
	this->_oldestClient = oldestClient;
}

void	Channel::setName(const std::string &name)
{
	this->_name = name;
}

void	Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

void	Channel::setPwd(const std::string &pwd)
{
	this->_pwd = pwd;
}

void	Channel::setMaxUsers(int maxUsers)
{
	this->_maxUsers = maxUsers;
}

void	Channel::setInvOnly(bool invOnly)
{
	this->_invOnly = invOnly;
}

void	Channel::setLockTopic(bool lockTopic)
{
	this->_lockTopic = lockTopic;
}