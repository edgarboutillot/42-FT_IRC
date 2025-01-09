.PHONY: all clean fclean re

NAME = ircserv
CC = c++
CFLAGS = -std=c++98 -Wall -Wextra -Werror -g

SRC_DIR = ./src
OBJ_DIR = .obj
INCLUDE = -I./include

SRC = 	$(SRC_DIR)/main.cpp \
		$(SRC_DIR)/server.cpp \
		$(SRC_DIR)/client.cpp \
		$(SRC_DIR)/parsing.cpp \
		$(SRC_DIR)/commands/invite.cpp \
		$(SRC_DIR)/commands/join.cpp \
		$(SRC_DIR)/commands/kick.cpp \
		$(SRC_DIR)/commands/nick.cpp \
		$(SRC_DIR)/commands/part.cpp \
		$(SRC_DIR)/commands/pass.cpp \
		$(SRC_DIR)/commands/privmsg.cpp \
		$(SRC_DIR)/commands/quit.cpp \
		$(SRC_DIR)/commands/user.cpp \
		$(SRC_DIR)/commands/topic.cpp \
		$(SRC_DIR)/commands/mode.cpp \
		$(SRC_DIR)/mode/mode_l.cpp \
		$(SRC_DIR)/mode/mode_k.cpp \
		$(SRC_DIR)/mode/mode_i.cpp \
		$(SRC_DIR)/mode/mode_t.cpp \
		$(SRC_DIR)/mode/mode_o.cpp \
		$(SRC_DIR)/channel.cpp \


OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

$(OBJ_DIR):
	mkdir -p $@%