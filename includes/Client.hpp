/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:03:22 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/17 12:49:50 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <map>
# include <vector>
# include "Channel.hpp"

class Channel;

class Client
{
private:
	int						_fd;
	bool					_auth;
	bool					_pwd;
	std::string				_user;
	std::string				_nick;
	std::vector<Channel*>	_currentsChannels;
	std::vector<Channel*>	_joinableChannels;

public:
	Client(int fd);
	~Client();

	void	delJoinableChannel(Channel *channel);
	void	delCurrentChannel(Channel *channel);
	bool	isJoinableChannel(Channel *channel);
	bool	isCurrentChannel(std::string name);

	//SETTERS AND GETTERS
	void	setFd(int fd);
	void	setUser(std::string user);
	void	setNick(std::string nick);
	void	setAuth(bool auth);
	void	setPwd(bool pwd);

	bool					getAuth();
	bool					getPwd();
	int						getFd();
	std::string				getUser();
	std::string				getNick();
	std::vector<Channel*>	&getCurrentsChannels();
	std::vector<Channel*>	&getJoinableChannels();
};

std::ostream &	operator<<(std::ostream &o, Client &client);

#endif