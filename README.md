# ft_irc
![42 Project](https://img.shields.io/badge/42-Project-black)  
![Language](https://img.shields.io/badge/c++-0E7FC0?logo=cplusplus)

---

## 📌 Summary
- [About](#about)
- [Features](#features)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Directories structure](#directories-structure)
- [Ressources](#ressources)
- [Author](#author)

---

<a id="about"></a>
## 📖 About

**ft_irc** is a team project of 42 school.  
It's a project focused on network programming and real-time communication.  
The goal is to create a fully functional **IRC (Internet Relay Chat) server** in C++98, following the **RFC 1459/2812** protocol specifications.

---

<a id="features"></a>
## ✨ Features

✅ **Custom IRC server** compliant with RFC 1459/2812  
✅ **Non-blocking I/O** using epoll for concurrent connections  
✅ **TCP socket programming** with robust error handling  
✅ **User authentication** with password protection  
✅ **Channel management** (create, join, invite, kick, topic, modes)  
✅ **Private messaging** between users  
✅ **Operator privileges** and channel moderation  
✅ **Multiple simultaneous clients** support  
✅ **Compatible** with standard IRC clients (HexChat, nc)  
✅ **Clean C++98** implementation with proper memory management

---

<a id="installation"></a>
## ⚙️ Installation

**1. Clone the repository**

```bash
git clone git@github.com:qxxel/ft_irc.git
```

**2. Access the directory**

```bash
cd ft_irc
```

---

<a id="utilisation"></a>
## 🕹️ Utilisation

**1. Compile the project**

```bash
make
```

**2. Launch the executable**

```bash
./ircserv <port> <password>
```

**3. Connect with IRC client ([HexChat](https://hexchat.github.io/)) or [nc](https://linux.die.net/man/1/nc)**

If you use Hexchat, you can `add`a Network and then `edit` it to set the IP adress with the port (ex: localhost/8080) and the password.

If you want to use `nc` it's a little bit different, you use it with the shell, but you have to send message with a precise form because it's adapt for HexChat. 
```bash
nc <IP of server> <port>
```
If you use it on your own machine, `<IP of server>` is `localhost`.

---

<a id="directories-structure"></a>
## 📂 Directories structure

```plaintext
📂 ft_irc
 ┣ 📂 includes		→ headers files (hpp)
 ┣ 📂 srcs			→ sources files (cpp)
 ┣ .gitignore
 ┣ Makefile
 ┗ README.md
```

---

<a id="ressources"></a>
## 🔗 Ressources

* [42](42.fr)
* [RFC 2812](https://www.rfc-editor.org/rfc/rfc2812)

---

<a id="author"></a>
## 👤 Author

* Axel – [GitHub](https://gitub.com/qxxel)
* 42 student - login: *agerbaud*