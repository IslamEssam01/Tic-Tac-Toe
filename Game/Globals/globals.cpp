#include "globals.h"

char playerToChar(Player p) {
    switch (p) {
        case Player::X: return 'X';
        case Player::O: return 'O';
        default:        return '-';
    }
}
