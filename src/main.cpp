#include <ncurses.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include <sys/ioctl.h>
#include <vector>
int h, w, sz;
WINDOW *boardWin,*info;
void startup();
void resizeHandler(int sig);
void add_wchar(int c);
typedef std::pair<char,char> pos;
typedef std::vector<pos> posList;
pos click1(-1,-1);
posList highlight;
struct Board {
    struct piece{
        enum shape { NONE = 0, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING };
        shape type;
        bool color;
        char getChar() {
            switch(type) {
                case NONE:
                    return ' ';
                    break;
                case PAWN:
                    return 'p';
                    break;
                case ROOK:
                    return 'R';
                    break;
                case KNIGHT:
                    return 'N';
                    break;
                case BISHOP:
                    return 'B';
                    break;
                case QUEEN:
                    return 'Q';
                    break;
                case KING:
                    return 'K';
                    break;
            }
        }
    };
    bool side;
    piece places[64];
    Board(){
        piece *lego = &places[0]; 
        *lego++ = piece{piece::ROOK,false};
        *lego++ = piece{piece::KNIGHT,false};
        *lego++ = piece{piece::BISHOP,false};
        *lego++ = piece{piece::KING,false};
        *lego++ = piece{piece::QUEEN,false};
        *lego++ = piece{piece::BISHOP,false};
        *lego++ = piece{piece::KNIGHT,false};
        *lego++ = piece{piece::ROOK,false};
        for(int i = 0; i < 8; ++i,++lego){
            *lego = piece{piece::PAWN,false};
        }
        for(int i = 0; i < 32; ++i,++lego){
            *lego = piece{piece::NONE,false};
        }
        for(int i = 0; i < 8; ++i,++lego){
            *lego = piece{piece::PAWN,true};
        }
        *lego++ = piece{piece::ROOK,true};
        *lego++ = piece{piece::KNIGHT,true};
        *lego++ = piece{piece::BISHOP,true};
        *lego++ = piece{piece::KING,true};
        *lego++ = piece{piece::QUEEN,true};
        *lego++ = piece{piece::BISHOP,true};
        *lego++ = piece{piece::KNIGHT,true};
        *lego++ = piece{piece::ROOK,true};
    }
    int map(pos p){
        return map(p.first,p.second);
    }
    int map(int x, int y){
        return (side?0:63)+(y*8 + x)*(side?1:-1);
    }
    pos imap(int i){
        if(side)
            return pos(i%8,i/8);
        return pos((63-i)%8,(63-i)/8);
        
    }
    void moves(pos &p, posList &l){
        l.clear();
        int x = p.first, y = p.second;
        piece &given = places[map(x,y)];
        switch(given.type){
            case piece::NONE:
                break;
            case piece::PAWN:
                if(given.color){
                    if(places[map(x,y)-8].type == piece::NONE){
                        l.push_back(imap(map(x,y)-8));
                        if(47 < map(x,y) && map(x,y) < 56 && places[map(x,y)-16].type == piece::NONE) {
                            l.push_back(imap(map(x,y)-16));
                        }
                    }
                }
                else {
                    if(places[map(x,y)+8].type == piece::NONE){
                        l.push_back(imap(map(x,y)+8));
                        if(7 < map(x,y) && map(x,y) < 16 && places[map(x,y)+16].type == piece::NONE) {
                            l.push_back(imap(map(x,y)+16));
                        }
                    }
                }
                break;
            case piece::ROOK:
                break;
            case piece::KNIGHT:
                break;
            case piece::BISHOP:
                break;
            case piece::QUEEN:
                break;
            case piece::KING:
                break;
        }
        //(*l).push_back;
    }
};
Board board;
void draw_board(){
    wmove(boardWin,0,0);
    int color = 1,dex = board.side?63:0,inc = board.side?-1:1;
    wattron(boardWin,COLOR_PAIR(color+1));
    cchar_t t;
    bool in;
    for(int sqh = 0; sqh < 8; ++sqh){
        for(int j = 0; j < sz/2; ++j){
            for(int sqw = 0; sqw < 8; ++sqw){
                for(int i = 0; i < sz; ++i){
                    in = false;
                    for(int k = 0; k < highlight.size(); ++k){
                        if(highlight.at(k).first == sqw && highlight.at(k).second == sqh){
                            in = true;
                            break;
                        }
                    }
                    if (in)
                        wattron(boardWin,COLOR_PAIR(6));  
                    else if (i == 1 && ((j != 0 && j != sz/2 - 1) || sz/2 < 3) && board.places[dex+(sqh*8 + sqw)*inc].type != Board::piece::NONE)
                        wattron(boardWin,board.places[dex+(sqh*8 + sqw)*inc].color?COLOR_PAIR(5):COLOR_PAIR(4));
                    else if (i == 0 || i == sz-1)
                        if(sqw == click1.first && sqh == click1.second && board.places[dex+(sqh*8 + sqw)*inc].type != Board::piece::NONE)
                            wattron(boardWin,COLOR_PAIR(7));    
                        else
                            wattron(boardWin,COLOR_PAIR(color+1)); 
                    if(i == sz/2 && j == sz/4 && board.places[dex+(sqh*8 + sqw)*inc].type != Board::piece::NONE) 
                        waddch(boardWin, board.places[dex+(sqh*8 + sqw)*inc].getChar());
                    else 
                        waddch(boardWin, ' ');
                }
                color = !color;
                //wattron(boardWin,COLOR_PAIR(color+1));
            }
        }
        color = !color;
        //wattron(boardWin,COLOR_PAIR(color+1));
    }
    wrefresh(boardWin);
}
void startup(){
    initscr();
	raw();
	keypad(stdscr, TRUE);
    curs_set(0);
    noecho();
    refresh();
    getmaxyx(stdscr, h, w);  /* get the new screen size */
    if(w > h/2){
        sz = 3*w/8;
    }
    else{
        sz = 3*h/4;
    }
    sz /= 8;
    sz+=sz%2;
    boardWin = newwin(4*sz,8*sz,0,(w-8*sz)/2);
    info  = newwin(h-4*sz,8*sz,4*sz,(w-8*sz)/2);
    mousemask(BUTTON1_CLICKED, NULL);
    mouseinterval(0);
    start_color();
    init_color(0, 12, 141, 302);
    init_color(1, 965, 500, 149);
    init_color(2, 0, 0, 0);
    init_color(3, 750, 750, 750);
    init_color(4, 800, 200, 200);
    init_color(5, 200, 800, 200);
    init_pair(1,0,0);//black background
    init_pair(2,1,1);//white background
    init_pair(3,3,2);//blueish background
    init_pair(4,2,3);//dark gray square white text
    init_pair(5,3,2);//light gray square black text;
    init_pair(6,4,4);
    init_pair(7,5,5);
    bkgd(COLOR_PAIR(3));
    refresh();
    draw_board();
    wbkgd(info,COLOR_PAIR(3));
    wattron(info,COLOR_PAIR(3));
    
    waddstr(info,"F1: quit\nF2: connect to server");
    wrefresh(info);
}
void handle_winch(int sig)
{
    endwin();
    refresh();
    clear();
    startup();
}
/*
typedef struct {
    attr_t  attr;
    wchar_t chars[CCHARW_MAX];
} cchar_t;
*/

int main (int argc, char **argv)
{
    startup();
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_winch;
    sigaction(SIGWINCH, &sa, NULL);
    MEVENT event;
    int ch,x,y;
    bool in;
    while (1) {
        if ((ch = getch()) == ERR) {
        }
        else {
            switch(ch) {
                case KEY_F(1):
                    endwin();
                    return 0;
                    break;
                case KEY_F(3):
                    board.side = !board.side;
                    draw_board();
                    break;
                case KEY_MOUSE:
                    if(getmouse(&event) == OK){
                        if(event.bstate & BUTTON1_PRESSED){
                            x = (event.x-(*boardWin)._begx)/sz;
                            y = (2*(event.y-(*boardWin)._begy))/sz;
                            if(0 <= x && x < 8 && 0 <= y && x < 8) {
                                in = false;
                                waddstr(info,("\n"+std::to_string(x)+" "+std::to_string(y)).c_str());
                                wrefresh(info);
                                for(int k = 0; k < highlight.size(); ++k){
                                    if(highlight.at(k).first == x && highlight.at(k).second == y){
                                        in = true;
                                        break;
                                    }
                                }
                                if(in) {
                                    board.places[board.map(x,y)] = board.places[board.map(click1)];
                                    board.places[board.map(click1)] = Board::piece{Board::piece::NONE,false};;
                                }
                                else {
                                    click1 = pos(x,y);
                                    board.moves(click1,highlight);
                                }
                                draw_board();
                                wrefresh(boardWin);    
                            }
                            //board.side = !board.side;
                            //draw_board();
                        }
                    }
                    break;
            }
        }
    }

    //usleep(1000000);
    endwin();
    return 0;  // make sure your main returns int
}
