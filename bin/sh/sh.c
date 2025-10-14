#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>

enum {
	FLAG_NONE = 0,
	FLAG_PIPE = 1 << 0,
} flags;

static char *PS;
static char *PS1 = "$ ";
static char *PS2 = "> ";
static char *PS4 = "+ ";

struct termios termios;

struct token {
	const char *ptr;
	enum {
		TOKEN_WORD,
		TOKEN_ASSIGNMENT_WORD,
		TOKEN_NAME,
		TOKEN_NEWLINE,
		TOKEN_IO_NUMBER,
		TOKEN_IO_LOCATION,
		TOKEN_PIPE, // |
		TOKEN_AND, // &
		TOKEN_AND_IF, // &&
		TOKEN_OR_IF, // ||
		TOKEN_DSEMI, // ;;
		TOKEN_SEMI_AND, // ;&
		TOKEN_DLESS, // <<
		TOKEN_DGREAT, // >>
		TOKEN_LESSAND, // <&
		TOKEN_GREATAND, // >&
		TOKEN_LESSGREAT, //<>
		TOKEN_DLESSDASH, // <<-
		TOKEN_IF,
		TOKEN_THEN,
		TOKEN_ELSE,
		TOKEN_ELIF,
		TOKEN_FI,
		TOKEN_DO,
		TOKEN_DONE,
		TOKEN_CASE,
		TOKEN_ESAC,
		TOKEN_WHILE,
		TOKEN_UNTIL,
		TOKEN_FOR,
		TOKEN_LBRACE, // {
		TOKEN_RBRACE, // }
		TOKEN_LPAREN, // (
		TOKEN_RPAREN, // )
		TOKEN_BANG, // !
		TOKEN_IN, // in
		TOKEN_EOF
	} type;
};

struct ast {
	enum {
		AST_COMMAND, // <cmd>
		AST_SEQUENCE, // ; or \n
		AST_AND, // &&
		AST_OR, // ||
		AST_PIPE, // |
		AST_BLOCK, // { ... }
		AST_SUBSHELL // ( ... )
	} type;

	char **argv;
	int argc;

	struct ast *left;
	struct ast *right;
};

struct parser {
	struct token *tokens;
	int pos;
};

struct ast *parse_expr(struct parser *p);
void ast_print(struct ast *node, int indent);
int ast_exec(struct ast *node);

struct ast *ast_new_command(char **argv, int argc)
{
	struct ast *node = calloc(1, sizeof(struct ast));
	node->type = AST_COMMAND;
	node->argv = argv;
	node->argc = argc;
	return node;
}

struct ast *ast_new_binary(int type, struct ast *left, struct ast *right)
{
	struct ast *node = calloc(1, sizeof(struct ast));
	node->type = type;
	node->left = left;
	node->right = right;
	return node;
}

struct ast *ast_new_block(struct ast *child, int subshell)
{
	struct ast *node = calloc(1, sizeof(struct ast));
	node->type = subshell ? AST_SUBSHELL : AST_BLOCK;
	node->left = child;
	return node;
}

void ast_free(struct ast *node)
{
	if (!node)
		return;

	ast_free(node->left);
	ast_free(node->right);

	if (node->type == AST_COMMAND && node->argv) {
		for (int i = 0; i < node->argc; i++)
			free(node->argv[i]);
		free(node->argv);
	}

	free(node);
}

ssize_t tputs(const char *s);

static char *token_string[] = { [TOKEN_WORD] = "word",
				[TOKEN_ASSIGNMENT_WORD] = "assignment_word",
				[TOKEN_NAME] = "name",
				[TOKEN_NEWLINE] = "newline",
				[TOKEN_IO_NUMBER] = "io_number",
				[TOKEN_IO_LOCATION] = "io_location",
				[TOKEN_PIPE] = "pipe",
				[TOKEN_AND_IF] = "and_if",
				[TOKEN_OR_IF] = "or_if",
				[TOKEN_DSEMI] = "dsemi",
				[TOKEN_SEMI_AND] = "semi_and",
				[TOKEN_DLESS] = "dless",
				[TOKEN_DGREAT] = "dgreat",
				[TOKEN_LESSAND] = "lessand",
				[TOKEN_GREATAND] = "greatand",
				[TOKEN_LESSGREAT] = "lessgreat",
				[TOKEN_DLESSDASH] = "dlessdash",
				[TOKEN_IF] = "if",
				[TOKEN_THEN] = "then",
				[TOKEN_ELSE] = "else",
				[TOKEN_ELIF] = "elif",
				[TOKEN_FI] = "fi",
				[TOKEN_DO] = "do",
				[TOKEN_DONE] = "done",
				[TOKEN_CASE] = "case",
				[TOKEN_ESAC] = "esac",
				[TOKEN_WHILE] = "while",
				[TOKEN_UNTIL] = "until",
				[TOKEN_FOR] = "for",
				[TOKEN_LBRACE] = "lbrace",
				[TOKEN_RBRACE] = "rbrace",
				[TOKEN_LPAREN] = "lparen",
				[TOKEN_RPAREN] = "rparen",
				[TOKEN_BANG] = "bang",
				[TOKEN_IN] = "in",
				[TOKEN_EOF] = "eof" };

void error(const char *msg)
{
	tputs(msg);
	exit(1);
}

ssize_t tputc(char c)
{
	return write(STDERR_FILENO, &c, 1);
}

ssize_t tputs(const char *s)
{
	return write(STDERR_FILENO, s, strlen(s));
}

ssize_t uputs(const char *s)
{
	if ((flags & FLAG_PIPE) == 0)
		return tputs(s);
	return 0;
}

ssize_t uputc(char c)
{
	if ((flags & FLAG_PIPE) == 0)
		return tputc(c);
	return 0;
}

ssize_t readline(char *prompt, char *buf, size_t bufsz)
{
	char c;
	int cursor;
	ssize_t len;

	if (prompt != NULL)
		uputs(prompt);

	cursor = len = 0;

	while (1) {
		ssize_t n = read(STDIN_FILENO, &c, 1);

		if (n == 0) {
			if (len == 0)
				return -1;
			break;
		}

		if (n < 0)
			return -1;

		if (c == '\r' || c == '\n') {
			uputc('\n');
			break;
		}

		if (c == 0x03) { // Ctrl-C
			uputs("^C\n");
			cursor = len = 0;
			if (prompt != NULL)
				uputs(prompt);
			continue;
		}

		if (c == 0x04) { // Ctrl-D
			if (len == 0) {
				uputs("exit\n");
				return -1;
			}
			continue;
		}

		if (c == 0x7f || c == 0x08) { // Backspace
			if (cursor > 0) {
				uputs("\b \b");
				cursor--;
				len--;
				// Shift characters after the cursor to the left
				for (int i = cursor; i < len; i++) {
					buf[i - 1] = buf[i];
				}
				if (len > 0)
					buf[len] = '\0';
			}
			continue;
		}

		if (c == 0x15) { // Ctrl-U
			while (cursor > 0) {
				uputs("\b \b");
				cursor--;
				len--;
			}
			len = 0;
			continue;
		}

		if (c == 0x17) { // Ctrl-W
			while (len > 0 && buf[len - 1] == ' ') {
				uputs("\b \b");
				cursor--;
				len--;
			}
			while (len > 0 && buf[len - 1] != ' ') {
				uputs("\b \b");
				cursor--;
				len--;
			}
			continue;
		}

		if (c == 0x01) { // Ctrl-A
			while (cursor > 0) {
				tputs("\033[D");
				cursor--;
			}
			continue;
		}

		if (c == 0x05) { // Ctrl-E
			while (cursor < len) {
				tputs("\033[C");
				cursor++;
			}
			continue;
		}

		if (c == 0x0c) { // Ctrl-L
			if ((flags & FLAG_PIPE) == 0) {
				tputs("\033[H\033[J");
				if (prompt != NULL)
					uputs(prompt);
				uputs(buf);
				for (int i = cursor; i < len; i++) {
					tputs("\033[D");
				}
			}
			continue;
		}

		if (c == '\033') {
			char seq[2];
			if (read(STDIN_FILENO, &seq[0], 1) == 0)
				continue;

			if (read(STDIN_FILENO, &seq[1], 1) == 0)
				continue;

			if (seq[0] == '[') {
				if (seq[1] == 'D') { // left
					if (cursor > 0) {
						tputs("\033[D");
						cursor--;
					}
					continue;
				} else if (seq[1] == 'C') { // right
					if (cursor < len) {
						tputs("\033[C");
						cursor++;
					}
					continue;
				}
			}
		}

		if (c == '#') {
			char c = 1;

			while (c && c != '\n') {
				ssize_t n = read(STDIN_FILENO, &c, 1);

				if (n == 0)
					break;

				if (n < 0)
					return -1;
			}

			continue;
		}

		if (len < bufsz - 1) {
			for (int i = len; i > cursor; i--) {
				buf[i] = buf[i - 1];
			}

			uputc(c);
			buf[cursor++] = c;
			len++;

			for (int i = cursor; i < len; i++) {
				tputc(buf[i]);
			}

			for (int i = cursor; i < len; i++) {
				tputs("\033[D");
			}
		}
	}

	if (len < bufsz)
		buf[len] = '\0';
	else
		error("sh: input too long\n");

	return len;
}

int endswith(const char *str, const char *suffix)
{
	if (!str || !suffix)
		return 0;
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix > lenstr)
		return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int is_operator_char(char c)
{
	return c == '|' || c == '&';
}

int is_redir_char(char c)
{
	return c == '<' || c == '>';
}

int tokenize(const char *input, struct token *tokens, int max_tokens)
{
	int pos = 0, t = 0;
	size_t len = strlen(input);

	while (pos < (int)len && t < max_tokens) {
		while (pos < (int)len &&
		       (input[pos] == ' ' || input[pos] == '\t'))
			pos++;
		if (pos >= (int)len)
			break;

		if (input[pos] == '#') {
			while (pos < (int)len && input[pos] != '\n')
				pos++;
			continue;
		}

		if (input[pos] == '\\' && pos + 1 < (int)len &&
		    input[pos + 1] == '\n') {
			pos += 2;
			continue;
		}

		if (input[pos] == '(') {
			tokens[t++].type = TOKEN_LPAREN;
			pos++;
			continue;
		}
		if (input[pos] == ')') {
			tokens[t++].type = TOKEN_RPAREN;
			pos++;
			continue;
		}
		if (input[pos] == '{') {
			tokens[t++].type = TOKEN_LBRACE;
			pos++;
			continue;
		}
		if (input[pos] == '}') {
			tokens[t++].type = TOKEN_RBRACE;
			pos++;
			continue;
		}

		if (input[pos] == ';') {
			if (pos + 1 < (int)len && input[pos + 1] == ';') {
				tokens[t++].type = TOKEN_DSEMI;
				pos += 2;
			} else {
				tokens[t++].type = TOKEN_NEWLINE;
				pos++;
			}
			continue;
		}

		if (is_operator_char(input[pos])) {
			if (pos + 1 < (int)len) {
				if (input[pos] == '|' &&
				    input[pos + 1] == '|') {
					tokens[t++].type = TOKEN_OR_IF;
					pos += 2;
					continue;
				}
				if (input[pos] == '&' &&
				    input[pos + 1] == '&') {
					tokens[t++].type = TOKEN_AND_IF;
					pos += 2;
					continue;
				}
			}
			if (input[pos] == '|') {
				tokens[t++].type = TOKEN_PIPE;
				pos++;
				continue;
			}
			if (input[pos] == '&') {
				tokens[t++].type = TOKEN_AND;
				pos++;
				continue;
			}
		}

		// redirekcje
		if (is_redir_char(input[pos])) {
			if (pos + 1 < (int)len && input[pos] == '<' &&
			    input[pos + 1] == '<') {
				if (pos + 2 < (int)len &&
				    input[pos + 2] == '-') {
					tokens[t++].type = TOKEN_DLESSDASH;
					pos += 3;
					continue;
				}
				tokens[t++].type = TOKEN_DLESS;
				pos += 2;
				continue;
			}
			if (pos + 1 < (int)len && input[pos] == '>' &&
			    input[pos + 1] == '>') {
				tokens[t++].type = TOKEN_DGREAT;
				pos += 2;
				continue;
			}
			if (input[pos] == '<') {
				tokens[t++].type = TOKEN_LESSAND;
				pos++;
				continue;
			}
			if (input[pos] == '>') {
				tokens[t++].type = TOKEN_GREATAND;
				pos++;
				continue;
			}
		}

		if (input[pos] == '$' && pos + 1 < (int)len) {
			if (input[pos + 1] == '(') {
				tokens[t++].type = TOKEN_LPAREN;
				pos += 2;
				continue;
			}
			if (input[pos + 1] == '(' && pos + 2 < (int)len &&
			    input[pos + 2] == '(') {
				tokens[t++].type = TOKEN_LPAREN;
				pos += 3;
				continue;
			}
		}

		if (input[pos] == '\'' || input[pos] == '"') {
			char quote = input[pos++];
			int start = pos;
			while (pos < (int)len && input[pos] != quote)
				pos++;
			tokens[t].type = TOKEN_WORD;
			tokens[t].ptr = strndup(input + start, pos - start);
			t++;
			if (pos < (int)len)
				pos++;
			continue;
		}

		int start = pos;
		while (pos < (int)len && !isspace(input[pos]) &&
		       !is_operator_char(input[pos]) &&
		       !is_redir_char(input[pos]) && input[pos] != '(' &&
		       input[pos] != ')' && input[pos] != '{' &&
		       input[pos] != '}' && input[pos] != ';' &&
		       input[pos] != '#')
			pos++;

		if (pos - start > 0) {
			char *word = strndup(input + start, pos - start);
			char *eq = strchr(word, '=');
			if (eq != NULL && eq != word) {
				tokens[t].type = TOKEN_ASSIGNMENT_WORD;
			} else {
				tokens[t].type = TOKEN_WORD;
			}
			tokens[t].ptr = word;
			t++;
		}
	}

	tokens[t].ptr = NULL;
	tokens[t].type = TOKEN_EOF;
	return t;
}

int loop(int flags)
{
	char buf[4096];
	off_t offset;
	int squote = 0;
	int dquote = 0;
	int parens = 0;
	int braces = 0;
	int arith = 0;

	PS = PS1;
	offset = 0;

	while (1) {
		ssize_t n = readline(PS, buf + offset, sizeof(buf) - offset);
		if (n == -1)
			return 1;

		buf[offset + n] = '\0';

		while (n > 0 && (buf[offset + n - 1] == ' ' ||
				 buf[offset + n - 1] == '\t'))
			n--;

		if (n == 0)
			continue;

		for (int i = offset; i < offset + n; i++) {
			if (buf[i] == '\'' && !dquote) {
				squote = !squote;
				continue;
			} else if (buf[i] == '"' && !squote) {
				dquote = !dquote;
				continue;
			}

			if (squote)
				continue;

			if (buf[i] == '(') {
				if (i > 0 && buf[i - 1] == '$')
					arith++;
				else
					parens++;
			} else if (buf[i] == '{') {
				braces++;
			} else if (buf[i] == ')') {
				if (arith > 0)
					arith--;
				else if (parens > 0)
					parens--;
			} else if (buf[i] == '}') {
				if (braces > 0)
					braces--;
			}
		}

		if ((parens > 0 || braces > 0) && arith == 0 && n > 0) {
			if (buf[offset + n - 1] != ';' &&
			    buf[offset + n - 1] != '(' &&
			    buf[offset + n - 1] != '{') {
				if (offset + n < sizeof(buf) - 1) {
					buf[offset + n] = ';';
					n++;
				}
			}
		}

		if (parens > 0 || braces > 0 || arith > 0 || squote || dquote ||
		    (offset + n > 0 && buf[offset + n - 1] == '\\') ||
		    (offset + n > 0 && buf[offset + n - 1] == '|') ||
		    endswith(buf, "&&") || endswith(buf, "||")) {
			PS = PS2;
			offset += n;
			continue;
		}

		struct token tokens[512];

		for (int i = 0; i < 512; i++)
			tokens[i].ptr = NULL;

		int ntok = tokenize(buf, tokens, 512);

		struct parser parser;
		parser.tokens = tokens;
		parser.pos = 0;

		struct ast *tree = parse_expr(&parser);
		ast_exec(tree);
		ast_free(tree);

		for (int i = 0; i < ntok; i++)
			if (tokens[i].ptr)
				free((void *)tokens[i].ptr);

		PS = PS1;
		offset = 0;
		memset(buf, 0, sizeof(buf));
	}

	return 0;
}

struct token *cur(struct parser *p)
{
	return &p->tokens[p->pos];
}

void next(struct parser *p)
{
	if (cur(p)->type != TOKEN_EOF)
		p->pos++;
}

struct ast *parse_command(struct parser *p)
{
	char **argv = malloc(sizeof(char *) * 128);
	int argc = 0;

	while (cur(p)->type == TOKEN_WORD ||
	       cur(p)->type == TOKEN_ASSIGNMENT_WORD) {
		argv[argc++] = strdup(cur(p)->ptr);
		next(p);
	}
	argv[argc] = NULL;
	return ast_new_command(argv, argc);
}

struct ast *parse_expr(struct parser *p);

struct ast *parse_block(struct parser *p)
{
	int subshell = cur(p)->type == TOKEN_LPAREN;
	next(p); // pomiń ( lub {

	struct ast *child = parse_expr(p);

	if (subshell && cur(p)->type != TOKEN_RPAREN) {
		printf("Syntax error: expected )\n");
		exit(1);
	}
	if (!subshell && cur(p)->type != TOKEN_RBRACE) {
		printf("Syntax error: expected }\n");
		exit(1);
	}
	next(p); // zamykający ) lub }
	return ast_new_block(child, subshell);
}

struct ast *parse_primary(struct parser *p)
{
	if (cur(p)->type == TOKEN_LBRACE || cur(p)->type == TOKEN_LPAREN) {
		int subshell = cur(p)->type == TOKEN_LPAREN;
		next(p); // pomiń ( lub {

		struct ast *child = parse_expr(p);

		if (child && child->type == AST_SEQUENCE &&
		    child->right == NULL && child->left &&
		    child->left->type == AST_COMMAND &&
		    child->left->argc == 0) {
			ast_free(child->left);
			child->left = NULL;
		}

		if (subshell && cur(p)->type != TOKEN_RPAREN) {
			printf("Syntax error: expected )\n");
			exit(1);
		}
		if (!subshell && cur(p)->type != TOKEN_RBRACE) {
			printf("Syntax error: expected }\n");
			exit(1);
		}
		next(p); // zamykający ) lub }
		return ast_new_block(child, subshell);
	} else {
		struct ast *cmd = parse_command(p);
		if (cmd->argc == 0) {
			free(cmd);
			return NULL;
		}
		return cmd;
	}
}

struct ast *parse_expr(struct parser *p)
{
	struct ast *left = parse_primary(p);
	if (!left)
		return NULL;

	while (cur(p)->type == TOKEN_AND_IF || cur(p)->type == TOKEN_OR_IF ||
	       cur(p)->type == TOKEN_PIPE || cur(p)->type == TOKEN_NEWLINE) {
		int op = AST_SEQUENCE;
		if (cur(p)->type == TOKEN_AND_IF)
			op = AST_AND;
		else if (cur(p)->type == TOKEN_OR_IF)
			op = AST_OR;
		else if (cur(p)->type == TOKEN_PIPE)
			op = AST_PIPE;

		next(p);
		struct ast *right = parse_primary(p);

		if (!right)
			break;

		left = ast_new_binary(op, left, right);
	}

	return left;
}

int ast_exec(struct ast *node)
{
	if (!node)
		return 0;

	switch (node->type) {
	case AST_COMMAND:
		if (node->argc == 0)
			return 0;

		if (strcmp(node->argv[0], "cd") == 0) {
			if (node->argc > 1)
				chdir(node->argv[1]);
			return 0;
		}

		pid_t pid = fork();
		if (pid == 0) {
			execvp(node->argv[0], node->argv);
			if (errno == ENOENT) {
				printf("sh: command not found: %s\n",
				       node->argv[0]);
			} else {
				perror("execvp");
			}
			exit(1);
		} else if (pid > 0) {
			int status;
			waitpid(pid, &status, 0);
			return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
		} else {
			perror("fork");
			return 1;
		}

	case AST_SEQUENCE:
		ast_exec(node->left);
		return ast_exec(node->right);

	case AST_AND: {
		int st = ast_exec(node->left);
		if (st == 0)
			return ast_exec(node->right);
		return st;
	}

	case AST_OR: {
		int st = ast_exec(node->left);
		if (st != 0)
			return ast_exec(node->right);
		return st;
	}

	case AST_PIPE: {
		int pipefd[2];
		if (pipe(pipefd) == -1) {
			perror("pipe");
			return 1;
		}

		pid_t pid1 = fork();
		if (pid1 == 0) {
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			exit(ast_exec(node->left));
		}

		pid_t pid2 = fork();
		if (pid2 == 0) {
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			exit(ast_exec(node->right));
		}

		close(pipefd[0]);
		close(pipefd[1]);
		int st1, st2;
		waitpid(pid1, &st1, 0);
		waitpid(pid2, &st2, 0);
		return WIFEXITED(st2) ? WEXITSTATUS(st2) : 1;
	}

	case AST_BLOCK:
		return ast_exec(node->left);

	case AST_SUBSHELL: {
		pid_t pid = fork();
		if (pid == 0) {
			int st = ast_exec(node->left);
			exit(st);
		}
		int status;
		waitpid(pid, &status, 0);
		return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
	}
	}

	return 0;
}

void ast_print(struct ast *node, int indent)
{
	if (!node)
		return;
	for (int i = 0; i < indent; i++)
		printf("  ");

	switch (node->type) {
	case AST_COMMAND:
		printf("COMMAND:");
		for (int i = 0; i < node->argc; i++)
			printf(" %s", node->argv[i]);
		printf("\n");
		break;
	case AST_SEQUENCE:
		printf("SEQUENCE\n");
		break;
	case AST_AND:
		printf("AND\n");
		break;
	case AST_OR:
		printf("OR\n");
		break;
	case AST_PIPE:
		printf("PIPE\n");
		break;
	case AST_BLOCK:
		printf("BLOCK\n");
		break;
	case AST_SUBSHELL:
		printf("SUBSHELL\n");
		break;
	}

	ast_print(node->left, indent + 1);
	ast_print(node->right, indent + 1);
}

int main(int argc, char **argv)
{
	flags = FLAG_NONE;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0) {
			int pipefd[2];

			if (pipe(pipefd) == -1) {
				write(STDERR_FILENO, "sh: pipe error\n", 15);
				exit(1);
			}

			size_t len = strlen(argv[2]);
			write(pipefd[1], argv[2], len);
			write(pipefd[1], "\n", 1);
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			flags |= FLAG_PIPE;
		} else {
			int fd = open(argv[i], 0);
			if (fd < 0) {
				tputs("sh: cannot open file: '");
				tputs(argv[i]);
				tputs("'\n");
				exit(1);
			}
			char shebang[2] = { 0 };
			if (read(fd, shebang, 2) == 2) {
				if (shebang[0] == '#' && shebang[1] == '!')
					while (1) {
						char c;
						ssize_t n = read(fd, &c, 1);
						if (n <= 0 || c == '\n')
							break;
					}
				else
					lseek(fd, 0, SEEK_SET);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
			flags |= FLAG_PIPE;
			break;
		}
	}

	if ((flags & FLAG_PIPE) == 0) {
		tcgetattr(STDIN_FILENO, &termios);
		struct termios raw = termios;
		raw.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
		raw.c_iflag &= ~(IXON | ICRNL);
		raw.c_cc[VMIN] = 1;
		raw.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &raw);
	}

	loop(flags);
	tcsetattr(STDIN_FILENO, TCSANOW, &termios);
	return 0;
}
