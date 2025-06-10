#ifndef UI_CONSTANTS_H
#define UI_CONSTANTS_H

namespace UIConstants {
    // Window Sizes
    namespace WindowSize {
        // Login window dimensions
        static constexpr int LOGIN_WIDTH = 600;
        static constexpr int LOGIN_HEIGHT = 550;
        
        // Game setup window dimensions
        static constexpr int SETUP_WIDTH = 500;
        static constexpr int SETUP_HEIGHT = 350;
        
        // Game board window dimensions
        static constexpr int GAME_WIDTH = 500;
        static constexpr int GAME_HEIGHT = 700;
    }
    
    // Input Field Sizes
    namespace InputField {
        static constexpr int WIDTH = 400;
        static constexpr int HEIGHT = 50;
    }
    
    // Spacing and Margins
    namespace Spacing {
        // Login page spacing
        static constexpr int LOGIN_FIELD_SPACING = 50;
        static constexpr int LOGIN_BUTTON_SPACING = 15;
        static constexpr int LOGIN_VERTICAL_MARGIN = 30;
        static constexpr int LOGIN_FORM_SPACING = 35;
        
        // Game window spacing
        static constexpr int GAME_ELEMENT_SPACING = 20;
        static constexpr int GAME_SETUP_SPACING = 15;
        static constexpr int GAME_BUTTON_SPACING = 10;
        static constexpr int GAME_BOARD_SPACING = 10;
        static constexpr int GAME_BOARD_MARGIN = 15;
        static constexpr int GAME_TOP_MARGIN = 10;
        static constexpr int GAME_BOTTOM_MARGIN = 30;
    }
    
    // Button Sizes
    namespace Button {
        static constexpr int MIN_WIDTH = 100;
        static constexpr int STANDARD_MIN_WIDTH = 120;
        static constexpr int SYMBOL_BUTTON_MIN_WIDTH = 150;
        static constexpr int LOGOUT_BUTTON_WIDTH = 100;
    }
    
    // Game Board Elements
    namespace GameBoard {
        static constexpr int CELL_SIZE = 120;
        static constexpr int BOARD_SPACING = 10;
        static constexpr int BOARD_MARGIN = 15;
    }
    
    // Font Sizes
    namespace Font {
        static constexpr int INPUT_TEXT_SIZE = 20;
        static constexpr int PLACEHOLDER_SIZE = 18;
        static constexpr int BUTTON_SIZE = 16;
        static constexpr int STATUS_LABEL_SIZE = 22;
        static constexpr int CELL_TEXT_SIZE = 52;
        static constexpr int LOGO_SIZE = 32;
        static constexpr int TITLE_SIZE = 18;
    }
    
    // Border and Radius
    namespace Style {
        static constexpr int BORDER_WIDTH = 2;
        static constexpr int BORDER_RADIUS = 8;
        static constexpr int LARGE_BORDER_RADIUS = 10;
        static constexpr int BUTTON_BORDER_BOTTOM = 3;
    }
    
    // Padding
    namespace Padding {
        static constexpr int INPUT_HORIZONTAL = 20;
        static constexpr int INPUT_VERTICAL = 10;
        static constexpr int BUTTON_HORIZONTAL = 25;
        static constexpr int BUTTON_VERTICAL = 12;
        static constexpr int STATUS_HORIZONTAL = 20;
        static constexpr int STATUS_VERTICAL = 12;
    }
}

#endif // UI_CONSTANTS_H