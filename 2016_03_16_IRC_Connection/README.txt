Problem: IRC: Making a Connection
Link: https://www.reddit.com/r/dailyprogrammer/comments/4ad23z/20160314_challenge_258_easy_irc_making_a/
Difficulty: EASY
Description:

A network socket is an endpoint of a connection across a computer network. Today, most communication between computers is based on the Internet Protocol; therefore most network sockets are Internet sockets. Internet Relay Chat (IRC) is a chat system on the Internet. It allows people from around the world to have conversations together, but it can also be used for two people to chat privately.
Freenode is an IRC network used to discuss peer-directed projects. Their servers are all accessible from the domain names chat.freenode.net and irc.freenode.net. In 2010, it became the largest free and open source software-focused IRC network. In 2013 it became the largest IRC network, encompassing more than 90,000 users and 40,000 channels and gaining almost 5,000 new users per year. We have a channel on freenode ourselves for all things /r/DailyProgrammer on freenode, which is #reddit-dailyprogrammer.
Your challenge today will be to communicate with the freenode IRC server. This will consist of opening a TCP socket to freenode and sending two protocol messages to initiate the connection. The original IRC RFC defines a message as a line of text up to 512 bytes starting with a message code, followed by one or more space separated parameters, and ending with a CRLF (\r\n). The last paramater can be prefixed by a colon to mark it as a parameter that can contain spaces, which will take up the rest of the line. An example of a colon-prefixed parameter would be the contents of a chat message, as that is something that contains spaces.
The first of the two initiation messages (NICK) defines what name people will see when you send a chat message. It will have to be unique, and you will not be able to connect if you specify a name which is currently in use or reserved. It has a single parameter <nickname> and will be sent in the following form:
NICK <nickname>
The second of the two initiation messages (USER) defines your username, user mode, server name, and real name. The username must also be unique and is usually set to be the same as the nickname. Originally, hostname was sent instead of user mode, but this was changed in a later version of the IRC protocol. For our purposes, standard mode 0 will work fine. As for server name, this will be ignored by the server and is conventionally set as an asterisk (*). The real name parameter can be whatever you want, though it is usually set to be the same value as the nickname. It does not have to be unique and may contain spaces. As such, it must be prefixed by a colon. The USER message will be sent in the following form:
USER <username> 0 * :<realname>
Input Description

You will give your program a list of lines specifying server, port, nickname, username, and realname. The first line will contain the server and the port, separated by a colon. The second through fourth lines will contain nick information.
chat.freenode.net:6667
Nickname
Username
Real Name
Output Description

Your program will open a socket to the specified server and port, and send the two required messages. For example:
NICK Nickname
USER Username 0 * :Real Name
Afterwards, it will begin to receive messages back from the server. Many messages sent from the server will be prefixed to indicate the origin of the message. This will be in the format :server or :nick[!user][@host], followed by a space. The exact contents of these initial messages are usually not important, but you must output them in some manner. The first few messages received on a successful connection will look something like this:
:wolfe.freenode.net NOTICE * :*** Looking up your hostname...
:wolfe.freenode.net NOTICE * :*** Checking Ident
:wolfe.freenode.net NOTICE * :*** Found your hostname
:wolfe.freenode.net NOTICE * :*** No Ident response
:wolfe.freenode.net 001 Nickname :Welcome to the freenode Internet Relay Chat Network Nickname
Challenge Input

The server will occasionally send PING messages to you. These have a single parameter beginning with a colon. The exact contents of that parameter will vary between servers, but is usually a unique string used to verify that your client is still connected and responsive. On freenode, it appears to be the name of the specific server you are connected to. For example:
PING :wolfe.freenode.net
Challenge Output

In response, you must send a PONG message with the parameter being the same unique string from the PING. You must continue to do this for the entire time your program is running, or it will get automatically disconnected from the server. For example:
PONG :wolfe.freenode.net
Notes

You can see the full original IRC specification at https://tools.ietf.org/html/rfc1459. Sections 2.3 and 4.1 are of particular note, as they describe the message format and the initial connection.
A Regular Expression For IRC Messages
Happy Pi Day!

——————————————————————————————————————————————————————————————————————————————————-


Problem: IRC: Responding to commands
Link: https://www.reddit.com/r/dailyprogrammer/comments/4anny5/challenge_258_intermediate_irc_responding_to/
Difficulty: INTERMEDIATE
Description:

In the last challenge we initiated a connection to an IRC server. This time we are going to utilise that connection by responding to user input. On an IRC server you can communicate with other users either directly, or in a group chatroom known as a channel. Channel names are distinguished from users by a prefixed character (# on freenode) in the name.
After connecting to an IRC server you will receive some informational text from the server known as the Message Of The Day, or MOTD. The server will buffer any messages (particularly attempts to join channels) sent before it has finished. The end of the MOTD is marked by the message RPL_ENDOFMOTD which is defined as the number 376. You don't necessarily have to wait for the end of the MOTD before joining, but I've found it usually works better if you do.
:wolfe.freenode.net 376 GeekBot :End of /MOTD command.
To join a channel you must use the JOIN message. It takes a single parameter, which is a comma separated list of one or more channels.
JOIN #reddit-dailyprogrammer,#botters-test
Once you have sent this message, you will receive one or more JOIN message(s) back from the server for every channel you were successfully able to join. The message you receive back will be prefixed with yourself as the origin.
:GeekBot!G33kDude@192-168-1-42.isp.com JOIN #reddit-dailyprogrammer
:GeekBot!G33kDude@192-168-1-42.isp.com JOIN #botters-test
After you've been joined to the channel, you can send text to the channel using the PRIVMSG message. It takes two parameters, the first being the the comma separated list of users or channels to send the text to, and the second being the colon prefixed message text.
PRIVMSG #reddit-dailyprogrammer :Hello World!
In addition to being able to send messages, you can receive messages that have been sent to the channel by other users. You should listen for a phrase prefixed with your name, then respond to that chat message. For example, you might see the following chat message.
:GeekDude!G33kDude@192-168-1-42.isp.com PRIVMSG #ahkscript :GeekBot: random 20
Your code would parse this message, and see the chatted contents were GeekBot: random 20. In response, your program might do something like generate a random number, and chat it back.
PRIVMSG #ahkscript :GeekDude: 4 // chosen by fair 20 sided dice roll // guaranteed to be random
Input Description

In addition to the input from last time's challenge, there will also be two line specifying a channel to join, and a message to chat upon joining.
chat.freenode.net:6667
Nickname
Username
Real Name
#reddit-dailyprogrammer,#rdp,#botters-test
Hello World!
Output Description

In addition to the last challenge's output, you must also pick and respond to one or more chat commands. These commands must take at least one parameter, and the return value should be chatted back to the same channel prefixed with the nick of the person who invoked the command.
The following code block has the prefix > for outgoing messages, and < for incoming messages.
>NICK Nickname
>USER Username 0 * :Real Name
<:wolfe.freenode.net NOTICE * :*** Looking up your hostname...
<:wolfe.freenode.net NOTICE * :*** Checking Ident
<:wolfe.freenode.net NOTICE * :*** Found your hostname
<:wolfe.freenode.net NOTICE * :*** No Ident response
<:wolfe.freenode.net 001 Nickname :Welcome to the freenode Internet Relay Chat Network Nickname
--- A bit later ---
<:wolfe.freenode.net 376 MyRC_Bot :End of /MOTD command.
>JOIN #reddit-dailyprogrammer,#rdp,#botters-test
<:GeekBot!G33kDude@192-168-1-42.isp.com JOIN #reddit-dailyprogrammer
>PRIVMSG #reddit-dailyprogrammer :Hello World!
<:GeekBot!G33kDude@192-168-1-42.isp.com JOIN #rdp
>PRIVMSG #rdp :Hello World!
<:GeekBot!G33kDude@192-168-1-42.isp.com JOIN #botters-test
>PRIVMSG #botters-test :Hello World!
--- Wait for chat ---
<:GeekDude!G33kDude@192-168-1-42.isp.com PRIVMSG #reddit-dailyprogrammer :GeekBot: sum 12 8 7 3 5
>PRIVMSG #reddit-dailyprogrammer :GeekDude: The sum is 35
Also, don't forget to return any incoming PING messages!
Challenge Input

Your bot should handle commands sent to it directly as well as through normal channels. When you receive such a message, the channel parameter of PRIVMSG is set to your own nickname.
:GeekDude!G33kDude@192-168-1-42.isp.com PRIVMSG GeekBot :GeekBot: mult 6 9
Challenge Output

You will have to recognize that the message has been sent directly to you, so you can send your own reply directly back. If you tried to send to the same destination as the original message (as you would with a regular channel message), you would end up sending the chat to yourself.
PRIVMSG GeekDude :GeekDude: 42
Bonus

When communicating with the bot directly via private message, nickname prefixes for calling commands and for return values should be optional. For example, the following should work:
<:GeekDude!G33kDude@192-168-1-42.isp.com PRIVMSG GeekBot :GeekBot: div 1 9801
>PRIVMSG GeekDude :GeekDude: 0.00010203...
<:GeekDude!G33kDude@192-168-1-42.isp.com PRIVMSG GeekBot :div 1 9801
>PRIVMSG GeekDude :0.00010203...
Notes

Be careful not to allow your bot to generate any newlines in response to a command. For example, if your bot did hex to ascii conversion (GeekBot: hex2ascii 0D0A) someone could potentially cause the bot to send a new protocol message, which could do all sorts of nasty things. This includes sending the QUIT message which would disconnect the bot, or making it spam people potentially getting it banned. If your bot is registered to an account, someone could use this technique to delete the account, or reset the password.
To verify your code is joining channels and chatting correctly, I suggest joining the channel(s) in advance using an IRC client, such as the web based http://webchat.freenode.net/.
You can see the full original IRC specification at https://tools.ietf.org/html/rfc1459. Sections 2.3 and 4.1 are of particular note, as they describe the message format and the initial connection.
A Regular Expression For IRC Messages
I get the distinct feeling I've missed something, so if you see anything off let me know.