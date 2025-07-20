#include "../../include/app_state.h"

AppState* AppState::instance = nullptr;

AppState& AppState::Instance() {
    if (instance == nullptr) {
        instance = new AppState();
    }
    return *instance;
}