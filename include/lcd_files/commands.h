#ifndef __COMMANDS_H__
#define __COMMANDS_H__

struct commands_t {
    const char *cmd;
    void      (*fn)(int argc, char *argv[]);
};

void command_shell(void);

void append(int argc, char *argv[]);
void cat(int argc, char *argv[]);
void cd(int argc, char *argv[]);
void date(int argc, char *argv[]);
void dino(int argc, char *argv[]);
void input(int argc, char *argv[]);
void lcd_init(int argc, char *argv[]);
void ls(int argc, char *argv[]);
void mkdir(int argc, char *argv[]);
void mount(int argc, char *argv[]);
void pwd(int argc, char *argv[]);
void rm(int argc, char *argv[]);
void shout(int argc, char *argv[]);
void clear(int argc, char *argv[]);
void drawline(int argc, char *argv[]);
void drawrect(int argc, char *argv[]);
void drawfillrect(int argc, char *argv[]);
void add(int argc, char *argv[]);
void mul(int argc, char *argv[]);

void exec(int argc, char *argv[]);
void parse_command(char *c);
void command_shell(void);

#endif /* __COMMANDS_H_ */
