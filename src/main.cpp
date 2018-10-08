#include <ncurses.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include <sys/ioctl.h>
int h, w, sz;
WINDOW *boardWin,*info;
void startup();
void resizeHandler(int sig);
struct Board {
    struct piece{
        enum shape { NONE, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING };
        shape type;
        bool color;
    };
    bool side;
    piece places[64];
    Board(){
        piece *lego = &places[0]; 
        *lego++ = piece{piece::ROOK,true};
        *lego++ = piece{piece::KNIGHT,true};
        *lego++ = piece{piece::BISHOP,true};
        *lego++ = piece{piece::KING,true};
        *lego++ = piece{piece::QUEEN,true};
        *lego++ = piece{piece::BISHOP,true};
        *lego++ = piece{piece::KNIGHT,true};
        *lego++ = piece{piece::ROOK,true};
        for(int i = 0; i < 8; ++i,++lego){
            *lego = piece{piece::PAWN,true};
        }
        for(int i = 0; i < 32; ++i,++lego){
            *lego = piece{piece::NONE,true};
        }
        for(int i = 0; i < 8; ++i,++lego){
            *lego = piece{piece::PAWN,false};
        }
        *lego++ = piece{piece::ROOK,false};
        *lego++ = piece{piece::KNIGHT,false};
        *lego++ = piece{piece::BISHOP,false};
        *lego++ = piece{piece::KING,false};
        *lego++ = piece{piece::QUEEN,false};
        *lego++ = piece{piece::BISHOP,false};
        *lego++ = piece{piece::KNIGHT,false};
        *lego++ = piece{piece::ROOK,false};
    }
};
Board board;
void draw_board(){
    int color = board.side,dex = board.side?64:0,inc = board.side?-1:1;
    wattron(boardWin,COLOR_PAIR(color+1));
    for(int sqh = 0; sqh < 8; ++sqh){
        for(int j = 0; j < sz/2; ++j){
            for(int sqw = 0; sqw < 8; ++sqw){
                for(int i = 0; i < sz; ++i){
                    if(i == sz/2 && j == sz/4){
                        char ch;
                        switch(board.places[sqh*8 + sqw].type){
                            case Board::piece::NONE:
                                ch = ' ';
                                break;
                            case Board::piece::PAWN:
                                ch = 'p';
                                break;
                            case Board::piece::ROOK:
                                ch = 'R';
                                break;
                            case Board::piece::KNIGHT:
                                ch = 'N';
                                break;
                            case Board::piece::BISHOP:
                                ch = 'B';
                                break;
                            case Board::piece::QUEEN:
                                ch = 'Q';
                                break;
                            case Board::piece::KING:
                                ch = 'K';
                                break;
                        }
                        waddch(boardWin, ch);
                    }
                    else waddch(boardWin, ' ');
                }
                color = !color;
                wattron(boardWin,COLOR_PAIR(color+1));
            }
        }
        color = !color;
        wattron(boardWin,COLOR_PAIR(color+1));
    }
}
void startup(){
    initscr();
	raw();
	keypad(stdscr, TRUE);
    curs_set(0);
    noecho();
    refresh();
    getmaxyx(stdscr, h, w);  /* get the new screen size */
    if(w < h/2){
        sz = 3*w/4;
    }
    else{
        sz = 3*h/2;
    }
    sz /= 8;
    sz+=sz%2;
    boardWin = newwin(4*sz,8*sz,0,(w-8*sz)/2);
    info  = newwin(h-4*sz,8*sz,4*sz,(w-8*sz)/2);
    start_color();
    init_color(0, 0, 0, 0);
    init_color(1, 1000, 1000, 1000);
    init_color(2, 529/2, 808/2, 922/2);
    init_color(3, 200, 200, 200);
    init_color(4, 800, 800, 800);
    init_pair(1,1,0);//black background
    init_pair(2,0,1);//white background
    init_pair(3,0,2);//blueish background
    init_pair(4,1,3);//dark gray square white text
    init_pair(5,0,4);//light gray square black text;
    bkgd(COLOR_PAIR(3));
    refresh();
    draw_board();
    wbkgd(info,COLOR_PAIR(3));
    wattron(info,COLOR_PAIR(3));
    
    waddstr(info,"F1: quit\nF2: connect to server");
    wrefresh(boardWin);
    wrefresh(info);
}
void handle_winch(int sig)
{
    endwin();
    refresh();
    clear();
    startup();
}

int main (int argc, char **argv)
{
    startup();

    
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_winch;
    sigaction(SIGWINCH, &sa, NULL);

    int ch,w2,h2;
    while (1) {
        getmaxyx(stdscr, h2, w2);
        if ((ch = getch()) == ERR) {
        }
        else {
            switch(ch) {
                case KEY_F(1):
                    endwin();
                    return 0;
                    break;
            }
        }
    }

    //usleep(1000000);
    endwin();
    return 0;  // make sure your main returns int
}
