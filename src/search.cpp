#include "search.h"

#include <iostream>

#include "movegen.h"
#include "position.h"
#include "pvtable.h"

namespace Xake{

namespace Search{

using namespace Evaluate;

static NodesSize leafCounter;
PVTable::PVLine pvLine;

//Killer heuristic
Move killerMoves[MAX_KILLERMOVES][MAX_DEPTH];

//History Heuristic
MoveScore searchHistory[PIECE_SIZE][SQUARE_SIZE_64];

void perft(Position &position, DepthSize depth);

Score alpha_beta(Position &position, SearchInfo &searchInfo, Score alpha, Score beta, DepthSize depth);
Score quiescence_search(Position &position, SearchInfo &searchInfo, Score alpha, Score beta);
void clean_search_info(SearchInfo &searchInfo);
void check_time(SearchInfo &searchInfo);
void pick_move(int moveIndx, MoveGen::MoveList &moveList);

void search(Position &position, SearchInfo &searchInfo){

    int bestMoveScore = -CHECKMATE_SCORE;
    Move bestMove = 0;

    clean_search_info(searchInfo);

    for(DepthSize currentDepth = 1; currentDepth <= searchInfo.depth; ++currentDepth){
        
        bestMoveScore = alpha_beta(position, searchInfo, -CHECKMATE_SCORE, CHECKMATE_SCORE, currentDepth);
        
        if(searchInfo.timeOver){
            break;
        }
        
        searchInfo.realTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count() - searchInfo.startTime;
        

        PVTable::load_pv_line(pvLine, MAX_DEPTH, position);
        bestMove = pvLine.moves[0];


        std::cout <<
        "info" << 
        " depth " << currentDepth << 
        " score cp " << bestMoveScore << 
        " move " << algebraic_move(bestMove) <<
        " nodes " << searchInfo.nodes <<
        " time " << searchInfo.realTime <<
        " ordering " << searchInfo.FirstHitFirst/searchInfo.FirstHit;

        std::cout << " pv";

        for(DepthSize pvLineDepth = 0; pvLineDepth < pvLine.depth; ++pvLineDepth){
            std::cout << " " << algebraic_move(pvLine.moves[pvLineDepth]);
        }

        std::cout << "\n";
        //std::cout << " score: " << bestMoveScore << " depth: " << currentDepth << "\n";
        //std::cout << "move:" << algebraic_move(veryBestMove) << " score: " << bestMoveScore << " depth: " << currentDepth << "\n";
        
    }

    std::cout << "bestmove " << algebraic_move(bestMove) << "\n\n"; 

}

Score alpha_beta(Position &position, SearchInfo &searchInfo, Score alpha, Score beta, DepthSize depth){


    if(depth==0){
        ++searchInfo.nodes;
        return quiescence_search(position, searchInfo, alpha, beta);
    }

    if((searchInfo.nodes & 2047) == 0){
        check_time(searchInfo);
    }

    ++searchInfo.nodes;

    if(((position.is_repetition() || position.get_fifty_moves_counter() >= 100) && position.get_ply())){
        return DRAW_SOCORE;
    }

    bool isCheck = position.square_is_attacked_bySide( Square64(__builtin_ctzll(position.get_pieceTypes_bitboard(position.get_side_to_move(), KING))), ~position.get_side_to_move());

    if(isCheck){
        depth++;
    }

    MoveGen::MoveList moveList;
    MoveGen::generate_all_moves(position, moveList);

    Move pvMove = PVTable::probe_move(position);

    //Set move scores
    for(int mIndx = 0; mIndx < moveList.size; ++mIndx){
        if(raw_move(moveList.moves[mIndx]) == pvMove){
            moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], PV_SCORE);
        }else if(is_capture(moveList.moves[mIndx])){
            if(move_special(moveList.moves[mIndx]) == ENPASSANT){
                moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], MVVLVAScores[PAWN][PAWN]);
            }
            else{
                moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], MVVLVAScores[piece_type(attacker_piece(moveList.moves[mIndx]))][piece_type(captured_piece(moveList.moves[mIndx]))]);
            }
        }
        else if(raw_move(moveList.moves[mIndx]) == raw_move(killerMoves[0][position.get_ply()])){
            moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], KILLERMOVE_SOCORE_0);
        }else if(raw_move(moveList.moves[mIndx]) == raw_move(killerMoves[1][position.get_ply()])){
            moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], KILLERMOVE_SOCORE_1);
        }else{ 
            moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], searchHistory[position.get_mailbox_piece(move_from(moveList.moves[mIndx]))][move_to(moveList.moves[mIndx])]);
        }
    }

    Score score = -CHECKMATE_SCORE;
    Score oldAlpha = alpha;
    Move bestMove = 0;
    int legalMoves = 0;

    for(int mIndx = 0; mIndx < moveList.size; ++mIndx){

        pick_move(mIndx, moveList);
        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }

        ++legalMoves;
        
        score = -alpha_beta(position, searchInfo, -beta, -alpha, depth - 1);
        position.undo_move();

        if(searchInfo.timeOver){
            return DRAW_SOCORE;
        }
        
        if(score>alpha){
            if(score>=beta){

                if(legalMoves == 1){
                    searchInfo.FirstHitFirst++;
                }

                searchInfo.FirstHit++;
                int ply = position.get_ply();
                if(!is_capture(move) && raw_move(move) != raw_move(killerMoves[0][ply])){
                    killerMoves[1][ply] = killerMoves[0][ply];
                    killerMoves[0][ply] = raw_move(move);
                }

                return beta;
            }
            alpha = score;
            bestMove = move;

            if(!is_capture(bestMove)){
                searchHistory[position.get_mailbox_piece(move_from(bestMove))][move_to(bestMove)] += depth;
            }
        }
    }

    if(legalMoves == 0){
        if(isCheck)
            return -CHECKMATE_SCORE + position.get_ply();
        else
            return DRAW_SOCORE;
    }

    if(alpha != oldAlpha){
        PVTable::insert_entry(position, raw_move(bestMove));
    }

    return alpha;
    
}

Score quiescence_search(Position &position, SearchInfo &searchInfo, Score alpha, Score beta){

    if((searchInfo.nodes & 2047) == 0){
        check_time(searchInfo);
    }

    ++searchInfo.nodes;

    if(searchInfo.depth > MAX_DEPTH - 1){
        return Evaluate::calc_score(position);
    }

    Score score = Evaluate::calc_score(position);

    if(score >= beta){
        return beta;
    }

    if(score > alpha){
        alpha = score;
    }

    MoveGen::MoveList moveList;
    MoveGen::generate_capture_moves(position, moveList);

    score = -CHECKMATE_SCORE;
    Score oldAlpha = alpha;
    Move bestMove = 0;

    for(int mIndx = 0; mIndx < moveList.size; ++mIndx){

        pick_move(mIndx, moveList);
        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }

        score = -quiescence_search(position, searchInfo, -beta, -alpha);
        position.undo_move();

        if(searchInfo.timeOver){
            return 0;
        }
        
        if(score>alpha){
            if(score>=beta){

                searchInfo.FirstHit++;

                return beta;
            }
            alpha = score;
            bestMove = move;
        }
    }

    if(alpha != oldAlpha){
        PVTable::insert_entry(position, raw_move(bestMove));
    }

    return alpha;

}

void clean_search_info(SearchInfo &searchInfo){
    searchInfo.nodes = 0;
    searchInfo.FirstHit = 0;
    searchInfo.FirstHitFirst = 0;
    
    for(int i = 0; i < MAX_KILLERMOVES; ++i){
        for(int x = 0; x < MAX_DEPTH; ++x){
            killerMoves[i][x] = 0;
        }
    }
    for(int i = 0; i < PIECE_SIZE; ++i){
        for(int x = 0; x < SQUARE_SIZE_64; ++x){
            searchHistory[i][x] = 0;
        }
    }
}

void check_time(SearchInfo &searchInfo){
    if(searchInfo.stopTime != NO_TIME){
        Xake::Time timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        if(timeMs >= searchInfo.stopTime){
            searchInfo.timeOver = true;
        }
    }
}

void pick_move(int moveIndx, MoveGen::MoveList &moveList){

    MoveScore moveScr{0};
    MoveScore bestScr{0};
    int bestIndx = moveIndx;

    for(int i = moveIndx; i < moveList.size; ++i){
        moveScr = move_score(moveList.moves[i]);
        if(moveScr > bestScr){
            bestScr = moveScr;
            bestIndx = i;
        }
    }

    Move moveTemp = moveList.moves[moveIndx];
    moveList.moves[moveIndx] = moveList.moves[bestIndx];
    moveList.moves[bestIndx] = moveTemp;
}


void perft(Position &position, DepthSize depth){

   if(depth == 0){
        leafCounter++;
        return;
    } 

    MoveGen::MoveList moveList;
    MoveGen::generate_all_moves(position, moveList);

    for(int mIndx = 0; mIndx < moveList.size; ++mIndx){
        
        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }
        //std::cout << "\n" << "Sub-move:"<< algebraic_move(move);
        perft(position, depth-1);
        position.undo_move();
    }
    //std::cout << "\n";
}


NodesSize perftTest(Position &position, SearchInfo &searchInfo){

    clean_search_info(searchInfo);
    leafCounter = 0;
    NodesSize allNodesCounter = 0;
    DepthSize actualDepth = searchInfo.depth-1;

    MoveGen::MoveList moveList;
    MoveGen::generate_all_moves(position, moveList);

    std::cout << "\n";
    
    for(int mIndx = 0; mIndx < moveList.size;++mIndx){

        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }

        long cumnodes = leafCounter;
        perft(position, actualDepth);

        position.undo_move();

        long oldNodes = leafCounter - cumnodes;
        allNodesCounter += oldNodes;

        std::cout << algebraic_move(move) << ": " << oldNodes << "\n";

    }

    searchInfo.realTime = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::high_resolution_clock::now().time_since_epoch()).count() - searchInfo.startTime;

    std::cout << "\n" << 
    "total nodes size: " << allNodesCounter << 
    " time ms: " << searchInfo.realTime <<
    "\n\n";

    return allNodesCounter;

}
}
}