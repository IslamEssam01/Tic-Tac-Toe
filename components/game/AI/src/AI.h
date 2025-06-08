#ifndef AI_H
#define AI_H

#include "Board.h"
#include <utility>

Player otherPlayer(Player p);

int minimax(Board board, Player currentPlayer, Player aiPlayer, int alpha, int beta, int depth);

std::pair<int, int> findBestMove(const Board& board, Player aiPlayer);

#endif
