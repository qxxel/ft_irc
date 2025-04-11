/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:03:22 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/11 13:11:46 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <map>
# include "Channel.hpp"

class Channel;

class Client
{
private:
	int			_fd;
	bool		_auth;
	bool		_pwd;
	std::string	_user;
	std::string	_nick;
	Channel		*_currentChannel;

public:
	Client(int fd);
	~Client();

	//SETTERS AND GETTERS
	void	setFd(int fd);
	void	setUser(std::string user);
	void	setNick(std::string nick);
	void	setAuth(bool auth);
	void	setPwd(bool pwd);
	void	setCurrentChannel(Channel *channel);

	bool		getAuth();
	bool		getPwd();
	int			getFd();
	std::string	getUser();
	std::string	getNick();
	Channel		*getCurrentChannel();
};

std::ostream &	operator<<(std::ostream &o, Client &client);

#endif