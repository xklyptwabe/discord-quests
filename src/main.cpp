#include <iostream>
#include <string_view>

int main(void) {
    constexpr std::string_view app_name = "Discord Quests & Activity Emulator";
    std::cout << "[INFO] " << app_name << " initialized successfully" << std::endl;

    return 0;
}
