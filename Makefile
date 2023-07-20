SRC_DIR = src/
OBJ_DIR = bin/
INC_DIR = include/

NAME = ircserv

SRCS = $(wildcard $(SRC_DIR)*.cpp)
SRCS := $(SRCS:$(SRC_DIR)%=%)

OBJS = $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))
DEPENDS = $(OBJS:.o=.d)

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra --std=c++98 $(DEFINES)
INCLUDE = -I$(INC_DIR)
DEBUGFLAGS = -g

define generate_compile_commands
	make --always-make --dry-run \
		| grep -wE 'gcc|cc|clang|g\+\+|c\+\+' \
		| grep -w '\-c' \
		| jq -nR "[inputs|{directory:\""$(PWD)"\", command:., file: match(\"[^ ]*\\\.(cpp|c)\").string[0:]}]" \
		> $(OBJ_DIR)/compile_commands.json
endef

all: $(NAME)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	@$(call generate_compile_commands)
	@printf "\n$(BLUE)Linking...   $(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)
	@printf "$(BLUE)Done\n$(RESET)"

-include $(DEPENDS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@printf "$(BLUE)→ $(GREEN)Compiling $(subst $(SRC_DIR),,$<)\n$(RESET)"
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -MMD -MP -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@printf '$(BLUE)→ $(ORANGE)Removing object files$(RESET)\n'
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf '$(BLUE)→ $(ORANGE)Removing executable$(RESET)\n'
	@rm -f $(NAME)

re: fclean print_newline all

print_newline:
	@echo

compile_commands: $(OBJ_DIR)
	@$(call generate_compile_commands)
	@echo 'Done'

.PHONY: all clean fclean re print_newline compile_commands

# colors

ORANGE = \e[1;33m
BLUE   = \e[1;34m
GREEN  = \e[1;32m
RESET  = \e[0m
