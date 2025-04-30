/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 16:12:51 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/30 11:50:51 by agerbaud         ###   ########.fr       */
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
# define NOT_ALW "You're not allowed to enter this channel\n"
# define AUTHY_GOOD "Finished authentification !\n\n"
# define ENTER_NCK_USR "Please enter a username or a nickname: USER <username> <0> <*> or NICK <nickname>\n"
# define UKWN_CMD "Unknown command: "
# define ENTER_NICK "Enter a server nickname: NICK <nickname>\n"
# define ENTER_USER "Enter a server username: USER <username> <0> <*>\n"
# define USR_NAME "Username: "
# define NICK_NAME "Nickname: "
# define CANT_NICK "Can not change nickname !\n"
# define CANT_USER "Can not change username !\n"
# define HAS_INVALID_CHARS "This field can not contain any of these characters: '\\0', '\\r', '\\a', '\\n', 'space', ',', ';'\n"
# define IS_TAKEN "This name is unavailable !\n"
# define NEED_AUTH "You need to finish authentication to execute commands\n"

# define JOIN_USG "usage: JOIN <channel>{,<channel>} [<key>{,<key>}]\n"
# define MSG_RULE "You have to put a ':' in front of your message\n"
# define ALRDY_IN_CHNL "Already in this channel\n"
# define NOT_ALW "You're not allowed to enter this channel\n"
# define INV_CHNL_NAME "The name of the channel is invalid\n"
# define CHNL_CREATE "Channel created\n"
# define CHNL_JOIN "Channel joined\n"
# define NO_CHNL_IN "Not in a channel\n"
# define NO_PERM "You're not channel operator\n"
# define KICK_USG "Kick users one by one\n"
# define BAD_TRGT "No user found channel\n"
# define TRGT_KICK "Target kicked\n"
# define NO_CHNL "Channel don't exist\n"
# define NO_TPC "No topic is set\n"
# define TRGT_NOT_FOUND "Target not found\n"
# define NO_CHNL_ASK "You're not on that channel\n"
# define CHNL_LEFT "Channel left\n"
# define INVT_SEND "Invitation sent\n"
# define WRNG_PASS "Wrong password\n"
# define ND_PASS "Need password to join\n"
# define CHNL_FULL "Channel full\n"
# define ACTL_TPC "This is the current topic\n"
# define ALRD_OP "Already op in this channel\n"
# define ISNT_OP "Target isn't op in this channel\n"
# define ISNT_NB "Argument isn't a number\n"
# define INV_PASS_FRMT "The password is invalid\n"
# define LST_MODES "MODE flags are -+iktop one by one\n"



// # define ERR_USR "Enter a username: USER <username>\n"
// # define ERR_NCK "Enter a nickname: NICK <nickname>\n"
// # define CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#:"
// # define FIELD_SET "This field has already been set\n"
// # define LST_CMDS "1. LIST(usage: LIST): displays all available commands\n2. JOIN(usage: JOIN #<chanel_name>): if <channel_name> exists, joins it. creates it and joins it otherwise\n"

#endif