#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
#include <bits/stdc++.h>

#include "board.hpp"
#include "engine.hpp"

#define MAX_DEPTH 200

auto start = std::chrono::high_resolution_clock::now();
auto end = start;

void Swapper(std::vector<U16>& moveList,std::vector<int>& moveVals,const int& i);
int eval(const Board& b,const bool& color);
int ITMinA(const Board& b, int alpha,int beta,int d,const bool& color);
int ITMaxA(const Board& b, int alpha,int beta,int d,const bool& color);
void ITAMax(const Board& b,const bool& color,std::unordered_set<U16>& moveset,int& out);
void ITAMin(const Board& b,const bool& color,std::unordered_set<U16>& moveset,int& out);
int evalHalf(const Board& b,const bool& color,int out);
std::string board_to_string(Board* b);
std::unordered_map<std::string,int> boardrepeats; 

void Swapper(std::vector<U16>& moveList,std::vector<int>& moveVals,const int& i){
    for (int j=0;j<i;j++){
        if (moveVals[j] < moveVals[i]){
            std::swap(moveVals[j],moveVals[i]);
            std::swap(moveList[j],moveList[i]);
        }
    }
}

int evalHalf(const Board& b,const bool& color,int out){
    std::unordered_set<U16> Move = b.get_legal_moves();
    int b_bishops=0,b_pawns_in=0,b_pawns_out=0,b_rooks=0,w_bishops=0,w_pawns_in=0,w_pawns_out=0,w_rooks=0;
    if (b.data.b_bishop != pos(7,7)){
        b_bishops++;
    }
    if (b.data.b_pawn_bs != pos(7,7)){
        U8 pos = b.data.b_pawn_bs;
        U8 type = b.data.board_0[pos];
        if ((type & (ROOK | BISHOP | BLACK)) == BLACK){
            if((gety(pos) != 6)&&(getx(pos) != 6)){
                b_pawns_in++;
            }
            else{
                b_pawns_out++;
            }
        }
        else if((type & (BLACK | ROOK)) == BLACK){
            b_bishops++;
        }
        else{
            b_rooks++;
        }
    }
    if (b.data.b_pawn_ws != pos(7,7)){
        U8 pos = b.data.b_pawn_ws;
        U8 type = b.data.board_0[pos];
        if ((type & (ROOK | BISHOP | BLACK)) == BLACK){
            if((gety(pos) != 6)&&(getx(pos) != 6)){
                b_pawns_in++;
            }
            else{
                b_pawns_out++;
            }
        }
        else if((type & (BLACK | ROOK)) == BLACK){
            b_bishops++;
        }
        else{
            b_rooks++;
        }
    }
    if (b.data.b_rook_bs != pos(7,7)){
        b_rooks++;
    }
    if (b.data.b_rook_ws != pos(7,7)){
        b_rooks++;
    }
    //white
    if (b.data.w_bishop != pos(7,7)){
        w_bishops++;
    }
    if (b.data.w_pawn_bs != pos(7,7)){
        U8 pos = b.data.w_pawn_bs;
        U8 type = b.data.board_0[pos];
        if ((type & (ROOK | BISHOP | WHITE)) == WHITE){
            if((gety(pos) != 0)&&(getx(pos) != 0)){
                w_pawns_in++;
            }
            else{
                w_pawns_out++;
            }
        }
        else if((type & (WHITE | ROOK)) == WHITE){
            w_bishops++;
        }
        else{
            w_rooks++;
        }
    }
    if (b.data.w_pawn_ws != pos(7,7)){
        U8 pos = b.data.w_pawn_ws;
        U8 type = b.data.board_0[pos];
        if ((type & (ROOK | BISHOP | WHITE)) == WHITE){
            if((gety(pos) != 0)&&(getx(pos) != 0)){
                w_pawns_in++;
            }
            else{
                w_pawns_out++;
            }
        }
        else if((type & (WHITE | ROOK)) == WHITE){
            w_bishops++;
        }
        else{
            w_rooks++;
        }
    }
    if (b.data.w_rook_bs != pos(7,7)){
        w_rooks++;
    }
    if (b.data.w_rook_ws != pos(7,7)){
        w_rooks++;
    }
    //calculating out
    if (Move.size() != 0){
        if (b.data.player_to_play == WHITE){
            if (color){
                out += 12*(Move.size());
                out += 275*(w_bishops) + 408*(w_rooks) + 178*(w_pawns_in) + 107*(w_pawns_out);
            }
            else{
                out -= 12*(Move.size());
                out -= 100*(w_bishops) + 150*(w_rooks) + 30*(w_pawns_in + w_pawns_out);
            }
        }
        else {
            if (!color){
                out += 12*(Move.size());
                out += 275*(b_bishops) + 408*(b_rooks) + 178*(b_pawns_in) + 107*(b_pawns_out);
            }
            else{
                out -= 12*(Move.size());
                out -= 100*(b_bishops) + 150*(b_rooks) + 30*(b_pawns_in + b_pawns_out);
            }
        }
    }
    else{
        if (color){
            if (b.data.player_to_play == WHITE){
                out += 275*(w_bishops) + 408*(w_rooks) + 178*(w_pawns_in) + 107*(w_pawns_out);
            }
            else{
                out -= 100*(b_bishops) +150*(b_rooks) +30*(b_pawns_in + b_pawns_out);
            }
            if ((b.data.player_to_play == WHITE)&&(b.in_check())){
                out -= 15000;
            }
            else{
                if (b.in_check()){
                    out += 20000;
                }
            }
        }
        else{
            if (b.data.player_to_play == WHITE){
                out -= 100*(w_bishops) +150*(w_rooks) +30*(w_pawns_in + w_pawns_out);
            }
            else{
                out += 275*(b_bishops) + 408*(b_rooks) + 178*(b_pawns_in) + 107*(b_pawns_out);
            }
            if ((b.data.player_to_play == BLACK)&&(b.in_check())){
                out -= 15000;
            }
            else{
                if (b.in_check()){
                    out += 20000;
                }
            }
        }
    }
    return out;
}

int eval(const Board& b,const bool& color){
    int out = evalHalf(b,color,0);
    std::unordered_set<U16> Move = b.get_legal_moves();
    if (Move.size() != 0){
        if (b.data.player_to_play == WHITE){
            if (color){
                ITAMax(b,color,Move,out);
            }
            else{
                ITAMin(b,color,Move,out);
            }
        }
        else{
            if (color){
                ITAMin(b,color,Move,out);
            }
            else{
                ITAMax(b,color,Move,out);
            }
        }
    }
    return out;
}

int ITMaxA(const Board& b, int alpha,int beta,int d,const bool& color){
    auto moveset = b.get_legal_moves();
    if ((d == 0)||(moveset.size() == 0)){
        return eval(b,color);   
    }
    for (auto m:moveset){
        Board* c = b.copy();
        c->do_move(m);
        int temp = ITMinA(*c,alpha,beta,d-1,color);
        delete c;
        if (std::chrono::high_resolution_clock::now() > end){
            break;
        }
        if (temp > alpha){
            alpha = temp;
        }
        if (alpha >= beta){
            break;
        }
    }
    return alpha;
}

void ITAMax(const Board& b,const bool& color,std::unordered_set<U16>& moveset,int& out){
    int alpha = INT32_MIN;
    for (auto m:moveset){
        Board* c = b.copy();
        c->do_move(m);
        int temp = evalHalf(*c,color,out);
        delete c;
        if (temp > alpha){
            alpha = temp;
        }
    }
    out = alpha;
}

void ITAMin(const Board& b,const bool& color,std::unordered_set<U16>& moveset,int& out){
    int beta = INT32_MAX;
    for (auto m:moveset){
        Board* c = b.copy();
        c->do_move(m);
        int temp = evalHalf(*c,color,out);
        delete c;
        if (temp < beta){
            beta = temp;
        }
    }
    out = beta;
}

int ITMinA(const Board& b, int alpha,int beta,int d,const bool& color){
    auto moveset = b.get_legal_moves();
    if ((d == 0)||(moveset.size() == 0)){
        return eval(b,color);
    }
    for (auto m:moveset){
        Board* c = b.copy();
        c->do_move(m);
        int temp = ITMaxA(*c,alpha,beta,d-1,color);
        delete c;
        if (std::chrono::high_resolution_clock::now() > end){
            break;
        }
        if (temp < beta){
            beta = temp;
        }
        if (alpha >= beta){ 
            break;
        }
    }
    return beta;
}

std::string board_to_string(Board* b) {

    U8 *boards = b->data.board_0;

    std::string board_str = ".......\n.......\n..   ..\n..   ..\n..   ..\n.......\n.......\n";

    for (int i=0; i<56; i++) {
        U8 piece = boards[i];
        if (board_str[i] == '\n' || board_str[i] == ' ') continue;
        board_str[(48-(i/8)*8) + i%8] = piece_to_char(piece);
    }

    return board_str;
}

void Engine::find_best_move(const Board& b) {
    start = std::chrono::high_resolution_clock::now();
    end = start + std::chrono::seconds((long int)(2));
    Board* B = b.copy();
    std::string board = board_to_string(B);
    if (boardrepeats.find(board) != boardrepeats.end()){
        boardrepeats[board]++;
    }
    else{
        boardrepeats[board] = 1;
    }
    auto moveset = b.get_legal_moves();
    const bool color = b.data.player_to_play == WHITE;
    int n = moveset.size();
    if (n == 0){
        this->best_move = 0;
    }
    else{
        std::vector<U16> moveList(moveset.begin(),moveset.end());
        std::vector<int> moveVals(n,INT32_MIN);
        int temp;
        bool Stop = false;
        for (int d=1;d<=MAX_DEPTH;d++){
            if (Stop){
                break;
            }
            int alpha = INT32_MIN;
            int beta = INT32_MAX;
            for (int i=0;i<n;i++){
                Board* c = b.copy();
                bool our_check = b.in_check();
                c->do_move(moveList[i]);
                bool their_check = c->in_check();
                std::unordered_set<U16> their_moves = c->get_legal_moves();
                temp = ITMinA(*c,alpha,beta,d-1,color);
                delete c;
                if ((!our_check)&&(their_check)&&(their_moves.size() == 0)){
                    this->best_move = moveList[i];
                    Stop = true;
                }
                if (std::chrono::high_resolution_clock::now() > end){
                    Stop = true;
                }
                if (Stop){
                    break;
                }
                if (temp > alpha){
                    Board* C = b.copy();
                    C->do_move(moveList[i]);
                    std::string next_board = board_to_string(C);
                    delete C;
                    if (boardrepeats[next_board] < 2){
                        alpha = temp;
                        this->best_move = moveList[i];
                    }
                }
                if (temp > moveVals[i]){
                    moveVals[i] = temp;
                    Swapper(moveList,moveVals,i);
                }
            }
        }
    }
    B->do_move(this->best_move);
    std::string final_board = board_to_string(B);
    if (boardrepeats.find(final_board) != boardrepeats.end()){
        boardrepeats[final_board]++;
    }
    else{
        boardrepeats[final_board] = 1;
    }
    delete B;
}