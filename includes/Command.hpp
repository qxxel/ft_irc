/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:13 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/28 18:22:09 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <iostream>
# include <map>
# include <vector>
# include "Bot.hpp"
# include "defines.hpp"

class Bot;
class Channel;
class Client;
class Server;

class Command
{
	private:
		// PRIVATE VARIABLES
		std::string					_raw;
		std::string					_name;
		std::vector<std::string>	_args;
	public:
		// CONSTRUCTOR AND DESTRUCTOR
		Command(std::string raw);
		~Command();

		//SETTERS AND GETTERS
		std::string					getRaw();
		std::string					getName();
		std::vector<std::string>	getArgs();
		void						setName(std::string name);
		void						setArgs(std::vector<std::string> args);

		// STATIC MEMBER FUNCTIONS
		static std::map<std::string, std::string>	splitChannelsPasswords(std::string str1, std::string str2, char del);
		static bool			isValidString(std::string str, bool skip, bool acceptSpace);
		static int			stringToInt(std::string str);
		static std::string	intToString(int value);
		static std::string	joinStrings(const std::vector<std::string>& vec);
		static std::string	timeToString(time_t value);

		static void			executeCommand(Server &serv, Client *client, Command *cmd, int epfd);
		static void			handleHelp(Client *client, std::vector<std::string> *args);
		static void			handleJoin(Server &serv, Client *client, std::vector<std::string> *args);
		static void			handleKick(Server &serv, Client *client, std::vector<std::string> *args);
		static void			handleMode(Server &serv, Client *client, std::vector<std::string> *args);
		static void			handlePrivMsg(Server &serv, Client *client, std::vector<std::string> *args);

		// OTHERS MEMBER FUNCTIONS
		void		parse();
		std::string	clear(std::string raw);
		bool		is_available(Server &serv, std::string name);
		void		handleInvite(Server &serv, Client *client, std::vector<std::string> *args);
		void		handleNames(Server &serv, Client *client, std::vector<std::string> *args);
		void		handleNick(Server &serv, Client *client, std::vector<std::string> *args);
		void		handlePart(Server &serv, Client *client, std::vector<std::string> *args);
		void		handlePass(Server &serv, Client *client, std::vector<std::string> *args);
		void		handleQuit(Server &serv, Client *client, std::vector<std::string> *args, int epfd);
		void		handleTopic(Server &serv, Client *client, std::vector<std::string> *args);
		void		handleUser(Client *client, std::vector<std::string> *args);
		std::vector<std::string>			splitChannels(std::string str, char del);

		// EXCEPTIONS
		class splitFailed: public std::exception
		{
			public:
				const char*	what() const throw() { return ("the split failed"); }
		};
		class notUnsignedIntNumber: public std::exception
		{
			public:
				const char*	what() const throw() { return ("The input must be a integer"); }
		};
};

std::ostream &	operator<<(std::ostream &o, Command &cmd);

#endif
