#include "player.h"

Player::Player(const std::string& name) : name(name), is_ai(false) {
    // New player, no ID assigned yet
}

Player::Player(int id, const std::string& name) : id(id), name(name), is_ai(false) {
    // Player loaded from database
}

Player Player::createAI() {
    Player ai_player("AI Player");
    ai_player.is_ai = true;
    return ai_player;
}

std::optional<int> Player::getId() const {
    return id;
}

std::string Player::getName() const {
    return name;
}

bool Player::isAI() const {
    return is_ai;
}

void Player::setId(int new_id) {
    id = new_id;
}