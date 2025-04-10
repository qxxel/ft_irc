/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-03-26 16:12:51 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-03-26 16:12:51 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_HPP
#define DEFINES_HPP

# define MAX_EVENTS	4096
# define MAX_BODY_SIZE	5000

# define SERVER_WELCOME "Welcome to cri_ft !\n"
# define INV_FORMAT "Invalid format !\n"
# define ENTER_PWD "Enter server password: PASS <password>\n"
# define PWD_SET "Password was already given !\n"
# define PWD_GOOD "Good password !\n"
# define INV_PWD "Invalid password !\n"
# define AUTHY_GOOD "Finished authentification !\n"
# define ENTER_NCK_USR "Please enter a username or a nickname: USER <username> <*> <0> or NICK <nickname>\n"


// # define ERR_USR "Enter a username: USER <username>\n"
// # define ERR_NCK "Enter a nickname: NICK <nickname>\n"
// # define CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#:"
// # define FIELD_SET "This field has already been set\n"
// # define IS_TAKEN "Another user already has this name\n"
// # define LST_CMDS "1. LIST(usage: LIST): displays all available commands\n2. JOIN(usage: JOIN #<chanel_name>): if <channel_name> exists, joins it. creates it and joins it otherwise\n"

#endif