#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <optional>

class Player {
private:
    std::optional<int> id;
    std::string name;
    bool is_ai;

public:
    // Constructor for human player
    Player(const std::string& name);
    
    // Constructor for loaded player
    Player(int id, const std::string& name);
    
    // Constructor for AI player
    static Player createAI();
    
    // Getters
    std::optional<int> getId() const;
    std::string getName() const;
    bool isAI() const;
    
    // Setters
    void setId(int new_id);
};

#endif // PLAYER_H