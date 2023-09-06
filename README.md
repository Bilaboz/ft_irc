![lint](https://github.com/Bilaboz/ft_irc/actions/workflows/cpp-linter.yml/badge.svg)

# ft_irc

A simple IRC server written in C++98

# ⚠️ Important note

This server has been developed with [irssi](https://irssi.org/) as reference IRC client<br />
It is not guaranteed that this server will work with other IRC clients

## Compiling

```sh
git clone https://github.com/Bilaboz/ft_irc.git
cd ft_irc
make
```

## Usage

```sh
./ft_irc <port> [password]
```
Example: `./ft_irc 6667 securepassword`<br>
Note: the password is optional


## List of currently supported commands

<details>
<summary>Supported commands</summary>
<br>
WHO<br>
DIE<br>
PING<br>
QUIT<br>
JOIN<br>
NICK<br>
PASS<br>
PART<br>
KICK<br>
MODE (+-itkol)<br>
USER<br>
TOPIC<br>
WHOIS<br>
INVITE<br>
NOTICE<br>
PRIVMSG<br>
</details>


## 🤝 Contributing

Contributions, issues and feature requests are welcome!<br>Feel free to check [issues page](https://github.com/Bilaboz/ft_irc/issues)


## Show your support

Give a ⭐️ if this project helped you!


## Authors

- [@Bilaboz](https://www.github.com/Bilaboz)
- [@nthimoni](https://github.com/nthimoni)
- [@leobesnard](https://github.com/leobesnard)


