#include "GameManager.h"
#include "backend/Exceptions.h"
#include <iostream>

int main() {
    try {
        GameManager::instance().run();
    } catch (const GameException& e) {
        std::cerr << "Game Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Standard Error: " << e.what() << std::endl;
        return 2;
    }
    return 0;
}